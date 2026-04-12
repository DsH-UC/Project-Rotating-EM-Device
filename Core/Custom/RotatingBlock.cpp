/*
 * RotatingBlock.cpp
 *
 *  Created on: Mar 21, 2026
 *      Author: devhu
 */

#include "RotatingBlock.hpp"
#include <cstdio>
#include <cmath>

extern "C" {
	extern UART_HandleTypeDef huart4; // Launch button UART handle
}

namespace RotatingBlock {

	// Internal namespace for compile time constants
	namespace Constants {
		constexpr int STACK_SIZE = 512; // bytes
		constexpr int THREAD_BLOCK_DURATION_MS = 50;
		constexpr bool STATE_IDLE = false;
		constexpr bool STATE_ROTATING = true;
		constexpr int COOLING_SYS_THRESHOLD = 40; // Deg. Celsius
		constexpr int NUM_TOTAL_RAILS = 6;
		constexpr float ROTARY_ENC_REF_ANGLE = 90.0f;
		constexpr float EPSILON = 1e-4f;

		// Motor FB PI Controller constants
		constexpr float32_t FB_CONTROLLER_KP = 0.5f;
		constexpr float32_t FB_CONTROLLER_KI = 0.1f;
	}

	static RotatingBlock& RotatingBlock::get_instance(){
		static RotatingBlock instance;
		return instance;
	}

	RotatingBlock::RotatingBlock() { printf("RB Subsystem online.\n"); }
	RotatingBlock::~RotatingBlock() { printf("RB Subsystem offline.\n"); }

	float RotatingBlock::read_temp_sensor(int gpio_pin){ // TODO
		return 0.0;
	}

	float RotatingBlock::read_rotary_encoder(int gpio_pin) { // TODO
		return 0.0;
	}

	bool RotatingBlock::read_ui(int gpio_pin) { // TODO
		return false;
	}

	void RotatingBlock::rotating_block_task(void* pv_parameters) {
		// Rotating Block FSM Implementation
		bool state = Constants::STATE_IDLE; // Initial state

		while(true) {
			if(xSemaphoreTake(this->m_ui_launch_button_semaphore, pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS)) == pdFALSE) return;

			this->m_rb_inputs = {read_temp_sensor(0), read_rotary_encoder(0), read_ui(0), false};

			// State transition boolean functions
			bool rotate_motor = this->m_rb_inputs.ui_launch_button &&
							   ((int) (this->m_rb_inputs.rail_temp) >= Constants::COOLING_SYS_THRESHOLD) &&
							   this->m_num_used_rails < Constants::NUM_TOTAL_RAILS;

			bool is_motor_aligned = std::abs(m_rb_inputs.rotary_encoder_angle - Constants::ROTARY_ENC_REF_ANGLE) <= Constants::EPSILON;

			// Mutually exclusive state transitions between both states. No need for else statements.
			if(state == Constants::STATE_IDLE && !rotate_motor) state = Constants::STATE_IDLE;
			if(state == Constants::STATE_IDLE && rotate_motor) state = Constants::STATE_ROTATING;
			if(state == Constants::STATE_ROTATING && !is_motor_aligned) state = Constants::STATE_ROTATING;
			if(state == Constants::STATE_ROTATING && is_motor_aligned) state = Constants::STATE_IDLE;

			// Thread from Running state -> Blocked state for 50 ms
			vTaskDelay(pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS));
		}

	}

	void RotatingBlock::ui_launch_button_ISR(void* pv_parameters){ // TODO: Still need to get the interrupt conditional/flag wrapped around this block

		BaseType_t xHigherPriorityTaskwoken = pdFALSE;

		// Preepmt thread from Running State -> Ready state
		xSemaphoreGiveFromISR(this->m_ui_launch_button_semaphore, &xHigherPriorityTaskwoken);

		// Listen for a byte, then trigger interrupt for non-blocking read
		HAL_UART_Receive_IT(&huart4, this->m_is_launch_button_pressed_rx, sizeof(uint8_t));

		portYIELD_FROM_ISR(xHigherPriorityTaskwoken);
	}

	void RotatingBlock::init_feedback_controller(){
			// Set Kp and Ki for PID constants
		 	this->m_motor_fb_controller.Kp = Constants::FB_CONTROLLER_KP;
		 	this->m_motor_fb_controller.Ki = Constants::FB_CONTROLLER_KI;
		 	this->m_motor_fb_controller.Kd= 0.0f;

		 	int32_t reset_state_flag = 1;
		    arm_pid_init_f32(&(this->m_motor_fb_controller), reset_state_flag); // 1 resets the state variables to zero
	}

	float32_t RotatingBlock::get_feedback_controller(float32_t setpoint, float32_t fb_output){ // TODO
		float32_t error = setpoint - fb_output; // setpoint - i/p from fb output
		return arm_pid_f32(&(this->m_motor_fb_controller), error);
	}


	bool RotatingBlock::toggle_motor_PWM(bool en_PWM) { // TODO
m		int pwm = this->m_rb_GPIO.pin_Motor_IGBT_Gate;
		return false;
	}
	void RotatingBlock::increment_rail_counter(bool en_rail_counter) {

		if(en_rail_counter) this->m_num_used_rails++;
		return en_rail_counter;
	}

	void RotatingBlock::start_rotating_block_subsystem_thread(void){
		this->m_ui_launch_button_semaphore = xSemaphoreCreateBinary();

		xTaskCreate(
				RotatingBlock::rotating_block_task,
				"Rotating Block Thread",
				Constants::STACK_SIZE,
				nullptr,
				ThreadPriorityTable::ROTATING_BLOCK_PRIORITY,
				&(this->m_rb_task_handle)
		);
	}
}


