/*
 * ChargingStage.cpp
 *
 *  Created on: Mar 13, 2026
 *      Author: devhu
 */

#include "ChargingStage.hpp"

#include "ThreadPriorityTable.hpp"

namespace ChargingStage {

	// Internal namespace for compile time constants
	namespace Constants {
		constexpr int CAP_VOLTAGE_THRESHOLD_MIN = 8; // Voltage for a single shot
		constexpr int CAP_VOLTAGE_THRESHOLD_MAX = 48; // Fully charged cap voltage

		constexpr int STATE_DISCHARGED = 0;
		constexpr int STATE_CHARGING = 1;
		constexpr int STATE_CHARGED = 2;

		constexpr int STACK_SIZE = 256;
		constexpr int THREAD_BLOCK_DURATION_MS = 50;

		constexpr int NUM_RAIL_PAIRS = 6;
	}

	static ChargingStage& ChargingStage::get_instance(){
		static ChargingStage instance;
		return instance;
	}

	ChargingStage::ChargingStage() { printf("Charging Stage Subsystem online.\n"); }
	ChargingStage::~ChargingStage() { printf("Charging Stage Subsystem offline.\n"); }

	// Charging Stage FSM
	// Inputs: Capacitor Bank Voltage Sensor (float), UI Launch Button (bool), Is Motor Aligned (bool),
	// 		   Reset / Discharge (bool), Gate Launch / Consumed Rail Count (int)

	// Outputs: Ready to Launch Payload (bool)

	// Helper read functions
	float ChargingStage::read_sensor(int gpio_pin){ //TODO
		// 12-bit ADC, mask w/ & 0xFFF
		return VoltageSense_DividerToSourceVoltage(
				VoltageSense_ReadVoltage(uint32_t adc_channel, uint16_t samples),
				float r_top,
				float r_bottom
				);
		//return 1.0;
	}

	bool ChargingStage::read_ui(int gpio_pin){ //TODO
		return false;
	}

	bool ChargingStage::read_rot_block_is_motor_aligned(void){ //TODO
		return false;
	}

	int ChargingStage::read_rot_block_num_consumed_rails(void){ //TODO
		return 0;
	}

	// Thread instructions
	void ChargingStage::charging_stage_task(void* pv_parameters){
		// Charging Stage FSM Implementation:
		int state = Constants::STATE_DISCHARGED; // Initial state

		while(true){

			if(xSemaphoreTake(this->m_ui_launch_button_semaphore,pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS)) == pdFALSE) return;

			this->m_charging_stage_inputs = {
					read_sensor(0),
					read_ui(0),
					read_rot_block_is_motor_aligned(),
					read_rot_block_num_consumed_rails(),
					false
			};

			// State transition boolean functions
			bool x0 = ((int)(this->m_charging_stage_inputs->cap_voltage)) >= Constants::CAP_VOLTAGE_THRESHOLD_MAX;
			bool x1 = ((int)this->m_charging_stage_inputs->cap_voltage) >= Constants::CAP_VOLTAGE_THRESHOLD_MIN;
			bool y = this->m_charging_stage_inputs->ui_launch_button && this->m_charging_stage_inputs->gate_launch < Constants::NUM_RAIL_PAIRS;

			// Mutually exclusive state transitions from each state. No need for else statement
			if(state == Constants::STATE_DISCHARGED && !y) state = Constants::STATE_DISCHARGED;
			if(state == Constants::STATE_DISCHARGED && y) state = Constants::STATE_CHARGING;

			if(state == Constants::STATE_CHARGING && !x0) state = Constants::STATE_CHARGING;
			if(state == Constants::STATE_CHARGING && x0) state = Constants::STATE_CHARGED;

			if(state == Constants::STATE_CHARGED && x1) state = Constants::STATE_CHARGED;
			if(state == Constants::STATE_CHARGED && !x1) state = Constants::STATE_DISCHARGED;

			// Thread from Running state -> Blocked state
			vTaskDelay(pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS));

		}

	}

	void ChargingStage::ui_launch_button_ISR(void* pv_parameters){ // TODO: Still need to get the interrupt condition and clear it
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;

		// Preepmt thread from Running State -> Ready state
		xSemaphoreGiveFromISR(this->m_ui_launch_button_semaphore, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	void ChargingStage::start_charging_stage_subsystem_thread(void){

		this->m_ui_launch_button_semaphore = xSemaphoreCreateBinary();
		xTaskCreate(
				charging_stage_task, // Thread function
				"Charging Stage Thread",
				Constants::STACK_SIZE,
				nullptr, // pvParameters
				ThreadPriorityTable::CHARGING_STAGE_PRIORITY, // Thread Priority
				&(this->m_cs_task_handle)
		);
	}

}


