/*
 * RotatingBlock.cpp
 *
 *  Created on: Mar 21, 2026
 *      Author: devhu
 */

#include "Custom/RotatingBlock.hpp"
#include <cstdio>
#include <cmath>
#include "Custom/Sensors/RotaryEncoder.hpp"
#include "Custom/Remote GUI/UI_CommandReceiver.hpp"

extern "C" {
	extern UART_HandleTypeDef huart5; // Launch button UART handle -> UART 5 now
}

namespace RotatingBlock {

	// Internal namespace for compile time constants
	namespace Constants {
		constexpr int STACK_SIZE = 512;
		constexpr int THREAD_BLOCK_DURATION_MS = 50;
		//constexpr bool STATE_IDLE = false;
		//constexpr bool STATE_ROTATING = true;
		constexpr int COOLING_SYS_THRESHOLD = 23; // Deg. Celsius
		constexpr int NUM_TOTAL_RAILS = 6;
		constexpr float ROTARY_ENC_REF_ANGLE = 90.0f;
		constexpr float EPSILON_DEG = 2.0f;
		constexpr uint16_t MOTOR_RUN_DUTY = 500;
		constexpr TickType_t RAIL_COOLING_TIME = pdMS_TO_TICKS(10000); // 10 sec (tune later)

		// Motor FB PI Controller constants
		constexpr float32_t FB_CONTROLLER_KP = 6.0f;
		constexpr float32_t FB_CONTROLLER_KI = 0.02f;
		constexpr float32_t FB_CONTROLLER_KD = 0.35f;
		constexpr uint16_t MOTOR_MIN_DUTY = 1469; // 10%
		constexpr uint16_t MOTOR_MAX_DUTY = 1889; // 30 %

		// Armature loader
		constexpr uint16_t PLACER_MOTOR_DUTY = 4198;      // tune this
		constexpr TickType_t PLACER_RUN_TIME = pdMS_TO_TICKS(500);
		enum State {
		    STATE_IDLE = 0,
		    STATE_ROTATING,
		    STATE_PLACING_METAL
		};
	}

	RotatingBlock& RotatingBlock::get_instance(){
		static RotatingBlock instance;
		return instance;
	}

	RotatingBlock::RotatingBlock() :
		m_num_used_rails(0),
		m_rb_inputs{0.0f, 0.0f, false, false},
		m_rb_GPIO{0, 0, 0, 0},
		m_is_launch_button_pressed_rx(0),
		m_is_motor_pwm_enabled(false),
		//m_motor_fb_controller{},
		m_rb_task_handle(nullptr),
		m_ui_launch_button_semaphore(nullptr),

		m_test_mode(false),
		m_mock_temp(25.0f),
		m_mock_encoder_angle(0.0f),
		m_mock_launch_button(false),
		m_testing_mutex(nullptr),
		m_has_left_alignment_zone(false),
		m_placer_start_tick(0),
		m_is_placer_motor_enabled(false)
	{
		printf("RB Subsystem online.\n");
	}
	RotatingBlock::~RotatingBlock() { printf("RB Subsystem offline.\n"); }

	float RotatingBlock::read_temp_sensor(int gpio_pin){ // TODO
		//return Thermistor_ReadTempC(uint32_t adc_channel, uint16_t samples);
		return 0.0;
	}

	float RotatingBlock::read_rotary_encoder(int gpio_pin) { // TODO
		return 0.0;
	}

	void RotatingBlock::rotating_block_task(void* pv_parameters) {
		// Rotating Block FSM Implementation
		//bool state = Constants::STATE_IDLE; // Initial state
		Constants::State state = Constants::STATE_IDLE;

		while(true) {
			// Thread from Running state -> Blocked state for up to 50 ms
			bool is_launch_button_pressed = (xSemaphoreTake(m_ui_launch_button_semaphore,
			                    pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS)) == pdTRUE);

			// Begin cooling decrement queue
			TickType_t now = xTaskGetTickCount();
			int write_idx = 0;

			for (int read_idx = 0; read_idx < m_num_used_rails; read_idx++) {
			    TickType_t age = now - m_rail_cool_timestamps[read_idx];
			    if (age < Constants::RAIL_COOLING_TIME) {
			        // still hot → keep it
			        m_rail_cool_timestamps[write_idx] = m_rail_cool_timestamps[read_idx];
			        write_idx++;
			    } // else cooled, so drop it
			}
			m_num_used_rails = write_idx;
			// End cooling decrement queue

			// Temp variables to store either their testing or live sensor/button versions
			float temp;
			float angle;
			bool launch;

			// Write guard only for inside the test mode if block for external testing thread
			if (xSemaphoreTake(m_testing_mutex, portMAX_DELAY) == pdTRUE) {
			    if (m_test_mode) {
			        temp = m_mock_temp;
			        angle = m_mock_encoder_angle;
			        launch = is_launch_button_pressed || m_mock_launch_button;
			        m_mock_launch_button = false; // one shot pulse
			    } else {
//			        temp = read_temp_sensor(0);
//			        angle = read_rotary_encoder(0);
//			        launch = is_launch_button_pressed;
			        uint8_t active_rail = RotaryEncoder::get_instance().get_rail_index_zero_based();
					temp = UI_CommandReceiver::get_instance().get_remote_temp_c(active_rail);

					angle = RotaryEncoder::get_instance().get_angle_degrees();
					launch = is_launch_button_pressed;
			    }
			    xSemaphoreGive(m_testing_mutex);
			}

			m_rb_inputs = {temp, angle, launch, false};

			// State transition boolean functions
			//bool temp_ready = m_rb_inputs.rail_temp >= Constants::COOLING_SYS_THRESHOLD;
			//bool rails_available = m_num_used_rails < Constants::NUM_TOTAL_RAILS;
			//bool rotate_motor = m_rb_inputs.ui_launch_button && temp_ready && rails_available;

//			float error = m_rb_inputs.rotary_encoder_angle - Constants::ROTARY_ENC_REF_ANGLE;
//			if (error > 180.0f) error -= 360.0f;
//			if (error < -180.0f) error += 360.0f; // Prevents error bug near 359 and 0 deg
//			bool is_motor_aligned = std::abs(error) <= Constants::EPSILON_DEG;
			uint8_t active_rail = RotaryEncoder::get_instance().get_rail_index_zero_based();

			bool is_motor_aligned = RotaryEncoder::get_instance().is_rail_aligned(
			        active_rail, static_cast<int32_t>(Constants::EPSILON_DEG * 100.0f));

			m_is_motor_aligned = is_motor_aligned; // Set sensor based subsystem output

			// FSM State transitions
//			if (state == Constants::STATE_IDLE) {
//				if (rotate_motor) {
//					state = Constants::STATE_ROTATING; //replace MOTOR_RUN_DUTY w/ fb controller o/p
//					m_has_left_alignment_zone = false;
//
//					//m_motor.forward(Constants::MOTOR_RUN_DUTY);	//toggle_motor_PWM(true);
//					uint16_t duty = compute_motor_duty(m_rb_inputs.rotary_encoder_angle);
//					m_motor.forward(duty);
//					m_is_motor_pwm_enabled = true;
//				}
//			} else if (state == Constants::STATE_ROTATING) {
////				if (is_motor_aligned) {
////					m_motor.stop();	//toggle_motor_PWM(false);
////					m_is_motor_pwm_enabled = false;
////					increment_rail_counter(true);
////					state = Constants::STATE_IDLE;
////				} else  {
////					m_motor.forward(Constants::MOTOR_RUN_DUTY);//toggle_motor_PWM(true);
////					m_is_motor_pwm_enabled = true;
////				}
//				if (!is_motor_aligned) {
//					m_has_left_alignment_zone = true;
//
//					//m_motor.forward(Constants::MOTOR_RUN_DUTY);
//					uint16_t duty = compute_motor_duty(m_rb_inputs.rotary_encoder_angle);
//					m_motor.forward(duty);
//					m_is_motor_pwm_enabled = true;
//				}
//				else if (m_has_left_alignment_zone && is_motor_aligned) {
//					m_motor.stop();
//					m_is_motor_pwm_enabled = false;
//
//					increment_rail_counter(true);
//
//					m_has_left_alignment_zone = false;
//					state = Constants::STATE_IDLE;
//				}
//				else {
//					// Still in the original alignment zone after starting.
//					// Keep rotating until we leave it first.
//					//m_motor.forward(Constants::MOTOR_RUN_DUTY);
//					uint16_t duty = compute_motor_duty(m_rb_inputs.rotary_encoder_angle);
//					m_motor.forward(duty);
//					m_is_motor_pwm_enabled = true;
//				}
//			}
			bool rail_too_hot = m_rb_inputs.rail_temp >= Constants::COOLING_SYS_THRESHOLD;
			bool rail_cold_enough = !rail_too_hot;
			bool rails_available = m_num_used_rails < Constants::NUM_TOTAL_RAILS;
			bool launch_requested = m_rb_inputs.ui_launch_button;

			// FSM State transitions
			if (state == Constants::STATE_IDLE) {

			    if (launch_requested && rails_available) {

			        if (rail_cold_enough && is_motor_aligned) {
			            // Current active rail is usable and already aligned.
			            // No wheel rotation needed. Place metal immediately.
			            m_placer_motor.forward(Constants::PLACER_MOTOR_DUTY);
			            m_is_placer_motor_enabled = true;
			            m_placer_start_tick = xTaskGetTickCount();

			            state = Constants::STATE_PLACING_METAL;
			        }
			        else {
			            // Current rail is too hot OR not aligned.
			            // Rotate to next aligned rail first.
			            state = Constants::STATE_ROTATING;
			            m_has_left_alignment_zone = false;

			            uint16_t duty = compute_motor_duty(m_rb_inputs.rotary_encoder_angle);
			            m_motor.forward(duty);
			            m_is_motor_pwm_enabled = true;
			        }
			    }
			}
			else if (state == Constants::STATE_ROTATING) {

			    if (!is_motor_aligned) {
			        //Out starting alignment zone.
			        m_has_left_alignment_zone = true;

			        uint16_t duty = compute_motor_duty(m_rb_inputs.rotary_encoder_angle);
			        m_motor.forward(duty);
			        m_is_motor_pwm_enabled = true;
			    }
			    else if (m_has_left_alignment_zone && is_motor_aligned) {
			        //At the next aligned rail.
			        m_motor.stop();
			        m_is_motor_pwm_enabled = false;

			        increment_rail_counter(true);

			        //Place armature after rotation.
			        m_placer_motor.forward(Constants::PLACER_MOTOR_DUTY);
			        m_is_placer_motor_enabled = true;
			        m_placer_start_tick = xTaskGetTickCount();

			        m_has_left_alignment_zone = false;
			        state = Constants::STATE_PLACING_METAL;
			    }
			    else {
			        // Still inside the alignment zone after starting so keep rotating until we leave it first.
			        uint16_t duty = compute_motor_duty(m_rb_inputs.rotary_encoder_angle);
			        m_motor.forward(duty);
			        m_is_motor_pwm_enabled = true;
			    }
			}
			else if (state == Constants::STATE_PLACING_METAL) {
			    TickType_t now = xTaskGetTickCount();

			    if ((now - m_placer_start_tick) >= Constants::PLACER_RUN_TIME) {
			        m_placer_motor.stop();
			        m_is_placer_motor_enabled = false;

			        state = Constants::STATE_IDLE;
			    }
			}

			vTaskDelay(pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS)); // Guarantee a block
		}

	}

	void RotatingBlock::ui_launch_button_ISR(uint16_t gpio_pin){ // TODO: Still need to get the interrupt conditional/flag wrapped around this block

		if (gpio_pin == GPIO_PIN_7) {
			BaseType_t xHigherPriorityTaskwoken = pdFALSE;

			// Preepmt thread from Running State -> Ready state
			xSemaphoreGiveFromISR(m_ui_launch_button_semaphore, &xHigherPriorityTaskwoken);

			// Listen for a byte, then trigger interrupt for non-blocking read
			//HAL_UART_Receive_IT(&huart4, &m_is_launch_button_pressed_rx, sizeof(uint8_t));

			portYIELD_FROM_ISR(xHigherPriorityTaskwoken);
		}
	}

//	void RotatingBlock::init_feedback_controller(){
//			// Set Kp and Ki for PID constants
//		 	this->m_motor_fb_controller.Kp = Constants::FB_CONTROLLER_KP;
//		 	this->m_motor_fb_controller.Ki = Constants::FB_CONTROLLER_KI;
//		 	this->m_motor_fb_controller.Kd= 0.0f;
//
//		 	int32_t reset_state_flag = 1;
//		    arm_pid_init_f32(&(this->m_motor_fb_controller), reset_state_flag); // 1 resets the state variables to zero
//	}

	float32_t RotatingBlock::get_feedback_controller(float32_t setpoint, float32_t fb_output){ // TODO
		float32_t error = setpoint - fb_output; // setpoint - i/p from fb output
		return arm_pid_f32(&(this->m_motor_fb_controller), error);
	}

	void RotatingBlock::increment_rail_counter(bool en_rail_counter) {
		if(en_rail_counter && m_num_used_rails < Constants::NUM_TOTAL_RAILS) {
			m_rail_cool_timestamps[m_num_used_rails] = xTaskGetTickCount();
			m_num_used_rails++;
		}
	}

	void RotatingBlock::start_rotating_block_subsystem_thread(void){
		m_testing_mutex = xSemaphoreCreateMutex();
		m_ui_launch_button_semaphore = xSemaphoreCreateBinary();

		xTaskCreate(
				RotatingBlock::runStatic,
				"Rotating Block Thread",
				Constants::STACK_SIZE,
				this,
				ThreadPriorityTable::ROTATING_BLOCK_PRIORITY,
				&(this->m_rb_task_handle)
		);
	}

	void RotatingBlock::runStatic(void* params) {
	    static_cast<RotatingBlock*>(params)->rotating_block_task(nullptr);
	}

	bool RotatingBlock::get_motor_pwm_enabled(){ return m_is_motor_pwm_enabled; }
	int RotatingBlock::get_num_used_rails(){ return m_num_used_rails; }
	bool RotatingBlock::get_is_motor_aligned() { return m_is_motor_aligned; }

	void RotatingBlock::set_test_mode(bool enable) {
		// Write guard b/c of external testing thread
	    if (xSemaphoreTake(m_testing_mutex, portMAX_DELAY) == pdTRUE) {
	        m_test_mode = enable;
	        xSemaphoreGive(m_testing_mutex);
	    }
	}

	void RotatingBlock::set_mock_inputs(float temp, float angle, bool launch) {
		// Write guard b/c of external testing thread
	    if (xSemaphoreTake(m_testing_mutex, portMAX_DELAY) == pdTRUE) {
	        m_mock_temp = temp;
	        m_mock_encoder_angle = angle;
	        m_mock_launch_button = launch;
	        xSemaphoreGive(m_testing_mutex);
	    }
	}

	void RotatingBlock::simulate_button_press() { xSemaphoreGive(m_ui_launch_button_semaphore); }


	// Motor actuation
	void RotatingBlock::init_motor_driver(TIM_HandleTypeDef* htim, uint32_t channel,
						   GPIO_TypeDef* ina_port, uint16_t ina_pin,
			               GPIO_TypeDef* inb_port, uint16_t inb_pin) {
		m_motor.init(htim, channel, ina_port, ina_pin, inb_port, inb_pin);
	}

	// Armature loader init
	void RotatingBlock::init_placer_motor_driver(TIM_HandleTypeDef* htim, uint32_t channel,
	                              GPIO_TypeDef* ina_port, uint16_t ina_pin,
	                              GPIO_TypeDef* inb_port, uint16_t inb_pin) {
	    m_placer_motor.init(htim, channel, ina_port, ina_pin, inb_port, inb_pin);
	}

	// Motor feedback control
	void RotatingBlock::init_feedback_controller(){
		m_motor_fb_controller.Kp = Constants::FB_CONTROLLER_KP;
		m_motor_fb_controller.Ki = Constants::FB_CONTROLLER_KI;
		m_motor_fb_controller.Kd = Constants::FB_CONTROLLER_KD;

		arm_pid_init_f32(&m_motor_fb_controller, 1); // 1 = reset
	}

	uint16_t RotatingBlock::compute_motor_duty(float current_angle){
		float target_angle = get_next_rail_target_angle(current_angle);

		float32_t error = target_angle - current_angle;

		if (error > 180.0f) error -= 360.0f;
		if (error < -180.0f) error += 360.0f;

		float32_t control = arm_pid_f32(&m_motor_fb_controller, error);

		if (control < 0.0f) control = -control;
		if (control < Constants::MOTOR_MIN_DUTY) control = Constants::MOTOR_MIN_DUTY;
		if (control > Constants::MOTOR_MAX_DUTY) control = Constants::MOTOR_MAX_DUTY;

		return static_cast<uint16_t>(control);
	}

	float RotatingBlock::get_next_rail_target_angle(float current_angle)
	{
	    constexpr float SECTOR_DEG = 360.0f / Constants::NUM_TOTAL_RAILS;

	    while (current_angle < 0.0f) {
	        current_angle += 360.0f;
	    }

	    while (current_angle >= 360.0f) {
	        current_angle -= 360.0f;
	    }

	    int current_sector = static_cast<int>(current_angle / SECTOR_DEG);
	    int next_sector = (current_sector + 1) % Constants::NUM_TOTAL_RAILS;

	    return next_sector * SECTOR_DEG;
	}
}

