/*
 * RotatingBlock.hpp
 *
 *  Created on: Mar 13, 2026
 *      Author: devhu
 */

/**
 * @file RotatingBlock.hpp
 *
 * @brief Interface file for the Rotating Block Subsystem Thread from the Software Architecture
 * Block Diagram
 *
 * This module provides an interface for the Charging Stage Subsystem's state machine,
 * providing access functions to its inputs(thermistor temp, rotary encoder motor angle,
 * and launch GUI button), outputs (is_motor_rotating, GPIO PWM, heated rail count),
 * and states (Rotating/Idle). It can be ran in either test mode for mock testing isolated
 * from the thermistor, rotary encoder, and button input or in live mode for real time
 * operation with them.
 *
 * The state machine activates PWM for rotation from the Idle to the Rotating state when
 * the temperature of the current is too hot and the GUI launch button is pressed given that all
 * rails are not too hot. It stays in the Rotating State until the topmost rail is aligned before
 * going back to the Idle state. Otherwise, the state machine stays in the Idle state.
 *
 * In live mode, this module is to be run with the microcontroller connected to the thermistor,
 * physical Rotating Block Subsystem's motor driver board, and rotary encoder.
 *
 */

#ifndef CUSTOM_ROTATINGBLOCK_HPP_
#define CUSTOM_ROTATINGBLOCK_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "./Priority Tables/ThreadPriorityTable.hpp"
#include "Motor/MotorDriver.hpp"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f446xx.h"
#include "arm_math.h"
#include <cstdint>

namespace RotatingBlock {

	class RotatingBlock {
	public:
		// Access function for singleton
		static RotatingBlock& get_instance();

		// Create Cooling System Thread in FreeRTOS
		void start_rotating_block_subsystem_thread(void);

		void ui_launch_button_ISR(uint16_t gpio_pin);

		// Testing / outputs
		bool get_motor_pwm_enabled();
		int get_num_used_rails();
		bool get_is_motor_aligned();

		// Testing / inputs
		void set_test_mode(bool enable);
		void set_mock_inputs(float temp, float angle, bool launch);
		void simulate_button_press();

		// Motor actuation functions
		void init_motor_driver(TIM_HandleTypeDef* htim, uint32_t channel,
				                       GPIO_TypeDef* ina_port, uint16_t ina_pin,
				                       GPIO_TypeDef* inb_port, uint16_t inb_pin);

		void init_feedback_controller(); // Motor feedback controller

		// Armature loader
		void init_placer_motor_driver(TIM_HandleTypeDef* htim, uint32_t channel, GPIO_TypeDef* ina_port, uint16_t ina_pin, GPIO_TypeDef* inb_port, uint16_t inb_pin);

	private:
		RotatingBlock();
		~RotatingBlock();

		struct rotating_block_inputs_t {
			float rail_temp;
			float rotary_encoder_angle;
			bool ui_launch_button;
			bool reset;
		};

		struct rotating_block_GPIO_t { // To be removed
			int pin_NTC_Thermistor; // ADC Channel
			int pin_Rotary_Encoder; // May need multiple depending on serial comms protocol
			int pin_HC_05_UART_RX;
			int pin_Motor_IGBT_Gate;
		};

		int m_num_used_rails; // Number of currently hot rails. Dependent input
		rotating_block_inputs_t m_rb_inputs;
		rotating_block_GPIO_t m_rb_GPIO;
		uint8_t m_is_launch_button_pressed_rx;
		bool m_is_motor_pwm_enabled;

		// Concurrency member variables
		TaskHandle_t m_rb_task_handle;
		SemaphoreHandle_t m_ui_launch_button_semaphore;

		// Main SM block
		float read_temp_sensor(int gpio_pin); // TODO
		float read_rotary_encoder(int gpio_pin);
		void rotating_block_task(void* pv_parameters);

		// Helper blocks
		void increment_rail_counter(bool en_rail_counter);

		static void runStatic(void* params); // C Wrapper for task loop function

		// Testing member variables
		bool m_test_mode;
		float m_mock_temp;
		float m_mock_encoder_angle;
		bool m_mock_launch_button;
		SemaphoreHandle_t m_testing_mutex;

		// Motor actuation member variables
		bool m_is_motor_aligned;
		MotorDriver m_motor;
		TickType_t m_rail_cool_timestamps[6]; // 6 rails total

		// Motor feedback controller
		float32_t get_feedback_controller(float32_t setpoint, float32_t fb_output);
		uint16_t compute_motor_duty(float current_angle);
		arm_pid_instance_f32 m_motor_fb_controller;

		// Placer motor / armature loader
		MotorDriver m_placer_motor;
		TickType_t m_placer_start_tick;
		bool m_is_placer_motor_enabled;

		// Disable copying
		RotatingBlock(const RotatingBlock&) = delete;
		RotatingBlock& operator = (const RotatingBlock&) = delete;
		RotatingBlock(RotatingBlock&&) = delete;
		RotatingBlock& operator = (RotatingBlock&&) = delete;

		bool m_has_left_alignment_zone;

		float get_next_rail_target_angle(float current_angle);
	};
}

#endif /* CUSTOM_ROTATINGBLOCK_HPP_ */
