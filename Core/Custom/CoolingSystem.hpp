/*
 * CoolingSystem.hpp
 *
 *  Created on: Mar 12, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_COOLINGSYSTEM_HPP_
#define CUSTOM_COOLINGSYSTEM_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// Interface file for the Cooling System Thread from the
// SW Architecture Block Diagram
namespace CoolingSystem {

	class CoolingSystem {
	public:
		// Access function for singleton
		static CoolingSystem& get_instance();

		// Disable copying
		CoolingSystem(const CoolingSystem&) = delete;
		CoolingSystem& operator = (const CoolingSystem&) = delete;
		CoolingSystem(CoolingSystem&&) = delete;
		CoolingSystem& operator = (CoolingSystem&&) = delete;

		// Create Cooling System Thread in FreeRTOS
		void start_cooling_subsystem_thread(void);

		void ui_fan_toggle_ISR(void* pv_parameters);

	private:
		struct cooling_system_inputs_t {
			float rail_temp;
			bool ui_fan_toggle;
			bool reset;
		};

		struct cooling_system_GPIO_t {
			int pin_NTC_thermistor; // ADC Channel
			int pin_HC_05_UART_RX;
		};

		// Store thread in cpp file
		TaskHandle_t cs_task_handle = nullptr;
		SemaphoreHandle_t ui_fan_toggle_semaphore = nullptr; // Semaphore given by ISR

		// Cooling System FSM
		// Inputs: Rail Temp Sensor (float), UI Fan Toggle (bool), Reset (bool)
		// Outputs: Is_Fan_On (bool)
		float read_sensor(int gpio_pin);
		bool read_ui(int gpio_pin);
		void cooling_system_task(void* pv_parameters);
	};
}

#endif /* CUSTOM_COOLINGSYSTEM_HPP_ */
