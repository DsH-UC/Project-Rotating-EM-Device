/*
 * ChargingStage.cpp
 *
 *  Created on: Mar 13, 2026
 *      Author: devhu
 */

#include "Custom/ChargingStage.hpp"
#include "Custom/Priority Tables/ThreadPriorityTable.hpp"

namespace ChargingStage {

	/*
	 * Internal namespace for compile time constants
	 */
	namespace Constants {
		constexpr int CAP_VOLTAGE_THRESHOLD_MIN = 8; // Voltage for a single shot
		constexpr int CAP_VOLTAGE_THRESHOLD_MAX = 48; // Fully charged cap voltage

		constexpr int STATE_DISCHARGED = 0;
		constexpr int STATE_CHARGING = 1;
		constexpr int STATE_CHARGED = 2;

		constexpr int STACK_SIZE = 512;
		constexpr int THREAD_BLOCK_DURATION_MS = 50;

		constexpr int NUM_RAIL_PAIRS = 6;
	}


	/*
	 * Constructor and object instantiation functions
	 */
	ChargingStage& ChargingStage::get_instance(){
		static ChargingStage instance;
		return instance;
	}

	ChargingStage::ChargingStage() :
			m_is_payload_launch_ready(false),
			m_test_mode(false),
			m_mock_cap_voltage(0.0f),
			m_mock_launch_button(false),
			m_mock_motor_aligned(false),
			m_mock_gate_launch(0),
			m_testing_mutex(nullptr)
	{ printf("Charging Stage Subsystem online.\n"); }

	ChargingStage::~ChargingStage() { printf("Charging Stage Subsystem offline.\n"); }


	/*
	 * Helper read function to access the capacitor bank voltage sensor,
	 * IR gate velocity, Rotta from their threads
	 */
	float ChargingStage::read_sensor(int gpio_pin){ //TODO
		// 12-bit ADC, mask w/ & 0xFFF
		return 0.0;
	}

	bool ChargingStage::read_rot_block_is_motor_aligned(void){ // To be removed
		return false;
	}

	int ChargingStage::read_rot_block_num_consumed_rails(void){ //TODO
		return 0;
	}

	float ChargingStage::get_IR_velocity(){ // TODO
		return 0.0f;
	}

	bool ChargingStage::get_is_launch_payload_ready(){ return m_is_payload_launch_ready; }


	/*
	 * Thread functions for initialization and thread loop implementation
	 */
	void ChargingStage::start_charging_stage_subsystem_thread(void){
		m_ui_launch_button_semaphore = xSemaphoreCreateBinary();
		m_testing_mutex = xSemaphoreCreateMutex();

		xTaskCreate(
				runStatic, // Thread function
				"Charging Stage Thread",
				Constants::STACK_SIZE,
				this, // pvParameters
				ThreadPriorityTable::CHARGING_STAGE_PRIORITY, // Thread Priority
				&(this->m_cs_task_handle)
		);
	}
	void ChargingStage::runStatic(void* params) {
		    // Bridge back to the class instance
		    static_cast<ChargingStage*>(params)->charging_stage_task(nullptr);
	}

	void ChargingStage::charging_stage_task(void* pv_parameters){
		// Charging Stage FSM Implementation:
		int state = Constants::STATE_DISCHARGED; // Initial state

		while(true){
			// Thread from Running state -> Blocked state
			bool is_launch_button_pressed = (xSemaphoreTake(m_ui_launch_button_semaphore,
			                    pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS)) == pdTRUE);

			// Temp variables to hold copies of either test or live versions and be written into
			// the state machine input member variables
			float cap_voltage;
			bool launch_button;
			bool motor_aligned;
			int gate_launch;

			if (xSemaphoreTake(m_testing_mutex, portMAX_DELAY) == pdTRUE) {
				if (m_test_mode) {
			    	cap_voltage = m_mock_cap_voltage;
			        launch_button = is_launch_button_pressed || m_mock_launch_button;
			        motor_aligned = m_mock_motor_aligned;
			        gate_launch = m_mock_gate_launch;

			        m_mock_launch_button = false; // one-shot
			    } else {
			        cap_voltage = read_sensor(0);
			        launch_button = is_launch_button_pressed;
			        motor_aligned = read_rot_block_is_motor_aligned();
			        gate_launch = read_rot_block_num_consumed_rails();
			    }

			    xSemaphoreGive(m_testing_mutex);
			}

			m_cs_inputs = { cap_voltage, launch_button, motor_aligned, gate_launch, false };

			// State transition boolean functions
			bool cap_full = m_cs_inputs.cap_voltage >= Constants::CAP_VOLTAGE_THRESHOLD_MAX;

			bool cap_above_min = m_cs_inputs.cap_voltage >= Constants::CAP_VOLTAGE_THRESHOLD_MIN;

			bool launch_request = m_cs_inputs.ui_launch_button && m_cs_inputs.gate_launch < Constants::NUM_RAIL_PAIRS;

			if (state == Constants::STATE_DISCHARGED) {
				if (launch_request) state = Constants::STATE_CHARGING;
			} else if (state == Constants::STATE_CHARGING) {
				if (cap_full) state = Constants::STATE_CHARGED;
			} else if (state == Constants::STATE_CHARGED) {
			    if (!cap_above_min) state = Constants::STATE_DISCHARGED;
			}

			// SM output (changed from Mealy to Moore)
			m_is_payload_launch_ready = (state == Constants::STATE_CHARGED);
		}

	}


	/*
	 * UI Launch Button ISR handler / On EXTI callback:  Block thread loop until GUI button is pressed
	 */
	void ChargingStage::ui_launch_button_ISR(void* pv_parameters){ // TODO: Still need to get the interrupt condition and clear it
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;

		// Preepmt thread from Running State -> Ready state
		xSemaphoreGiveFromISR(this->m_ui_launch_button_semaphore, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}


	/*
	 * Public test mode functions to enable testing mode and setting mock inputs
	 */
	void ChargingStage::set_test_mode(bool enable) {
		// Write guard for testing mock variables due to concurrent read from testing thread
	    if (xSemaphoreTake(m_testing_mutex, portMAX_DELAY) == pdTRUE) {
	        m_test_mode = enable;
	        xSemaphoreGive(m_testing_mutex);
	    }
	}

	void ChargingStage::set_mock_inputs(float cap_voltage, bool launch_button,
										bool motor_aligned, int gate_launch) {
		// Write guard for testing mock variables due to concurrent read from testing thread
	    if (xSemaphoreTake(m_testing_mutex, portMAX_DELAY) == pdTRUE) {

	        m_mock_cap_voltage = cap_voltage;
	        m_mock_launch_button = launch_button;
	        m_mock_motor_aligned = motor_aligned;
	        m_mock_gate_launch = gate_launch;
	        xSemaphoreGive(m_testing_mutex);
	    }
	}

	void ChargingStage::simulate_button_press() { xSemaphoreGive(m_ui_launch_button_semaphore); }
}
