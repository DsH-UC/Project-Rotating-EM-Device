/*
 * FaultDetector.hpp
 *
 *  Created on: Mar 21, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_FAULTDETECTOR_HPP_
#define CUSTOM_FAULTDETECTOR_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ThreadPriorityTable.hpp"

namespace FaultDetector { // TODO
	class FaultDetector {
	public:
		// Access function for singleton
		static FaultDetector& get_instance();

		// Disable copying
		FaultDetector(const FaultDetector&) = delete;
		FaultDetector& operator = (const FaultDetector&) = delete;
		FaultDetector(FaultDetector&&) = delete;
		FaultDetector& operator = (FaultDetector&&) = delete;

		// Input capture event ISRs based on fault detection circuit under voltage
		void chr_stg_fault_ISR(void* pv_parameters);
		void rb_fault_ISR(void* pv_parameters);
		void cool_sys_fault_ISR(void* pv_parameters);

		bool is_reset_all_subsystem_state_machines(bool* is_fault);
		void start_fault_detector_subsystem_thread(void);
	private:
		// Power pins voltage divider inputs
		struct fault_detector_inputs_t {
			float charging_stage_voltage;
			float rotating_block_voltage;
			float cooling_system_voltage;
			bool reset;
		};

		struct fault_detector_GPIO_t {
			int pin_charging_stage_PWR_voltage;
			int pin_rotating_block_PWR_voltage;
			int pin_cooling_system_PWR_voltage;
		}

		fault_detector_inputs_t fd_inputs;
		fault_detector_GPIO_t fd_GPIO;

		bool is_fault;

		// Store thread in cpp file
		TaskHandle_t fd_task_handle;
		SemaphoreHandle_t fd_semaphores[3];

		float* read_fault_sensors(int* sensor_pins, int num_sensors);
		void fault_detector_task(void* pv_parameters);

	};
}

#endif /* CUSTOM_FAULTDETECTOR_HPP_ */
