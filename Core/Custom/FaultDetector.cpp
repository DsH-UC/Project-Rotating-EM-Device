/*
 * FaultDetector.cpp
 *
 *  Created on: Mar 22, 2026
 *      Author: devhu
 */
#include "FaultDetector.hpp"

namespace FaultDetector {
	// Internal namespace for compile time constants
	namespace Constants {

		// Definition of subsystem semaphore indexes. Add additional subystem indexes here
		constexpr int CHR_STG_SEMPHR_IDX = 0;
		constexpr int RB_SEMPHR_IDX = 1;
		constexpr int COOL_SYS_SEMPHR_IDX = 2;

		constexpr int STACK_SIZE = 256;

		constexpr float FAULT_VOLTAGE_THRESHOLD = 2.0;

		constexpr bool STATE_CLOSED = false;
		constexpr bool STATE_OPEN = true;

		constexpr int NUM_SUBSYSTEMS = 3;

		constexpr int THREAD_BLOCK_DURATION_MS = 50;

		constexpr int SENSOR_PINS[] = {12,13,14};
	}

	FaultDetector& FaultDetector::get_instance(){
		static FaultDetector instance;
		return instance;
	}

	FaultDetector::FaultDetector() { printf("Fault Detector Subsystem online.\n"); }
	FaultDetector::~FaultDetector() { printf("Fault Detector Subsystem offline.\n"); }

	bool FaultDetector::is_reset_all_subsystem_state_machines(bool* is_fault) {
		// TODO: Need to transmit this result to the other three state machines' reset input
		return *is_fault;
	}

	float* FaultDetector::read_fault_sensors(int* sensor_pins, int num_sensors){

		for(int i = 0; i < num_sensors; i++){
			//f(sensor_pins[i])
		}
		float placeholder = 0.0;
		return &placeholder;
	}

	void FaultDetector::fault_detector_task(void* pv_parameters){
		// Fault Detector FSM Implementation
		bool state = STATE_CLOSED; // Initial state

		while(true){
			if(xSemaphoreTake(this->fd_semaphores[Constants::CHR_STG_SEMPHR_IDX], pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS)) == pdFALSE) return;
			if(xSemaphoreTake(this->fd_semaphores[Constants::RB__SEMPHR_IDX], pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS)) == pdFALSE) return;
			if(xSemaphoreTake(this->fd_semaphores[Constants::COOL_SYS_SEMPHR_IDX], pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS)) == pdFALSE) return;

			this->fd_inputs = {
					read_fault_sensors(Constants::SENSOR_PINS, Constants::NUM_SUBSYSTEMS)[0],
					read_fault_sensors(Constants::SENSOR_PINS, Constants::NUM_SUBSYSTEMS)[1],
					read_fault_sensors(Constants::SENSOR_PINS, Constants::NUM_SUBSYSTEMS)[2],
					false
			};

			// State transition boolean function. This is also the fault condition
			this->is_fault = this->fd_inputs.charging_stage_voltage <= Constants::FAULT_VOLTAGE_THRESHOLD ||
							   this->fd_inputs.rotating_block_voltage <= Constants::FAULT_VOLTAGE_THRESHOLD ||
							   this->fd_inputs.cooling_system_voltage <= Constants::FAULT_VOLTAGE_THRESHOLD;

			// Mutually exclusive state transitions between both states. No need for else statements.
			if(state == Constants::STATE_CLOSED && this->is_fault) state == Constants::STATE_OPEN;
			if(state == Constants::STATE_CLOSED && !(this->is_fault)) state == Constants::STATE_CLOSED;

			// Reset check and transition
			if(this->fd_inputs.reset) state = Constants::STATE_CLOSED;

			// Thread from Running state -> Blocked state for 50 ms
			vTaskDelay(pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS));
		}
	}

	void FaultDetector::start_fault_detector_subsystem_thread(void){
		this->fd_semaphores[Constants::CHR_STG_SEMPHR_IDX] = xSemaphoreCreateBinary();
		this->fd_semaphores[Constants::RB_SEMPHR_IDX] = xSemaphoreCreateBinary();
		this->fd_semaphores[Constants::COOL_SYS_SEMPHR_IDX] = xSemaphoreCreateBinary();

		xTaskCreate(
				FaultDetector::fault_detector_task,
				"Fault Detector Thread",
				Constants::STACK_SIZE,
				nullptr,
				ThreadPriorityTable::FAULT_DETECTOR_PRIORITY,
				&(this->fd_task_handle)
		);
	}

	void FaultDetector::chr_stg_fault_ISR(void* pv_parameters){ // TODO: Still need to get the interrupt conditional/flag wrapped around this block
		BaseType_t xHigherPriorityTaskwoken = pdFALSE;

		// Preepmt thread from Running State -> Ready state
		xSemaphoreGiveFromISR(this->fd_semaphores[Constants::CHR_STG_SEMPHR_IDX], &xHigherPriorityTaskwoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskwoken);
	}

	void FaultDetector::rb_fault_ISR(void* pv_parameters){ // TODO: Still need to get the interrupt conditional/flag wrapped around this block
		BaseType_t xHigherPriorityTaskwoken = pdFALSE;

		// Preepmt thread from Running State -> Ready state
		xSemaphoreGiveFromISR(this->fd_semaphores[Constants::RB_SEMPHR_IDX], &xHigherPriorityTaskwoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskwoken);
	}

	void FaultDetector::cool_sys_fault_ISR(void* pv_parameters){ // TODO: Still need to get the interrupt conditional/flag wrapped around this block
		BaseType_t xHigherPriorityTaskwoken = pdFALSE;

		// Preepmt thread from Running State -> Ready state
		xSemaphoreGiveFromISR(this->fd_semaphores[Constants::COOL_SYS_SEMPHR_IDX], &xHigherPriorityTaskwoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskwoken);
	}

}


