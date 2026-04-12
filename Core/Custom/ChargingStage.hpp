/*
 * ChargingStage.hpp
 *
 *  Created on: Mar 13, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_CHARGINGSTAGE_HPP_
#define CUSTOM_CHARGINGSTAGE_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <cstdio>


// Interface file for the Charging Stage Thread from the
// SW Architecture Block Diagram
namespace ChargingStage {
	class ChargingStage {
		public:
			// Access function for singleton
			static ChargingStage& get_instance();

			// Disable copying
			ChargingStage(const ChargingStage&) = delete;
			ChargingStage& operator = (const ChargingStage&) = delete;
			ChargingStage(ChargingStage&&) = delete;
			ChargingStage& operator = (ChargingStage&&) = delete;

			// Create Charging Stage Thread in FreeRTOS
			void start_charging_stage_subsystem_thread(void);

			void ui_launch_button_ISR(void* pv_parameters);

			bool read_ui(int gpio_pin);
			float read_sensor(int gpio_pin);
			int read_rot_block_num_consumed_rails(void);
			bool read_rot_block_is_motor_aligned(void);

		private:
			struct charging_stage_inputs_t {
				float cap_voltage;
				bool ui_launch_button;
				bool is_motor_aligned;
				int gate_launch; // Consumed rail count due to high temperature
				bool reset;
			};

			struct charging_stage_GPIO_t {
				int pin_cap_voltage_divider; //ADC Channel
			};

			// Store thread in cpp file
			TaskHandle_t m_cs_task_handle = nullptr;
			SemaphoreHandle_t m_ui_launch_button_semaphore = nullptr;

			charging_stage_inputs_t* m_charging_stage_inputs;

			void charging_stage_task(void* pv_parameters);

	};
}


#endif /* CUSTOM_CHARGINGSTAGE_HPP_ */
