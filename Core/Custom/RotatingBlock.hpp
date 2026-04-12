/*
 * RotatingBlock.hpp
 *
 *  Created on: Mar 13, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_ROTATINGBLOCK_HPP_
#define CUSTOM_ROTATINGBLOCK_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ThreadPriorityTable.hpp"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f446xx.h"
#include "arm_math.h"
#include <cstdint>

// Interface file for the Rotating Block Thread from the
// SW Architecture Block Diagram

namespace RotatingBlock {

	class RotatingBlock {
	public:
		// Access function for singleton
		static RotatingBlock& get_instance();

		// Disable copying
		RotatingBlock(const RotatingBlock&) = delete;
		RotatingBlock& operator = (const RotatingBlock&) = delete;
		RotatingBlock(RotatingBlock&&) = delete;
		RotatingBlock& operator = (RotatingBlock&&) = delete;

		// Create Cooling System Thread in FreeRTOS
		void start_rotating_block_subsystem_thread(void);

		void ui_launch_button_ISR(void* pv_parameters);

	private:
		struct RotatingBlockInputs {
			float rail_temp;
			float rotary_encoder_angle;
			bool ui_launch_button;
			bool reset;
		};

		struct RotatingBlockGPIO {
			int pin_NTC_Thermistor; // ADC Channel
			int pin_Rotary_Encoder; // May need multiple depending on serial comms protocol
			int pin_HC_05_UART_RX;
			int pin_Motor_IGBT_Gate;
		};

		int m_num_used_rails; // Number of currently hot rails. Dependent input
		RotatingBlockInputs m_rb_inputs;
		RotatingBlockGPIO m_rb_GPIO;
		uint8_t m_is_launch_button_pressed_rx;

		// FB Controller
		arm_pid_instance_f32 m_motor_fb_controller;

		// Store thread in cpp file
		TaskHandle_t m_rb_task_handle;
		SemaphoreHandle_t m_ui_launch_button_semaphore;

		RotatingBlock();
		~RotatingBlock();

		// Main SM block
		float read_temp_sensor(int gpio_pin); // TODO
		float read_rotary_encoder(int gpio_pin);
		bool read_ui(int gpio_pin);
		void rotating_block_task(void* pv_parameters);

		// Helper blocks
		bool toggle_motor_PWM(bool en_PWM); // TODO
		float32_t get_feedback_controller(float* rotatary_enc_buf, int n);
		void increment_rail_counter(bool en_rail_counter);

	};
}

#endif /* CUSTOM_ROTATINGBLOCK_HPP_ */
