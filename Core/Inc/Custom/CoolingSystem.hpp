/*
 * CoolingSystem.hpp
 *
 *  Created on: Mar 12, 2026
 *      Author: devhu
 */

/**
 * @file CoolingSystem.hpp
 *
 * @brief Interface file for the Cooling Subsystem Thread from the Software Architecture
 * Block Diagram
 *
 * This module provides an interface for the Cooling Subsystem's state machine,
 * providing access functions to its inputs(thermistor temp and fan toggle GUI button),
 * outputs (GPIO toggle), and states (ON/OFF). It can be ran in either test mode for mock testing isolated
 * from the thermistor and button input or in live mode for real time operation with
 * them.
 *
 * The state machine sets GPIO high(ON) when the temperature is too hot, exceeding a
 * threshold value, or when the GUI fan toggle button is pressed. Otherwise, the fans
 * stay OFF. However, the temperature exceeding a threshold condition always overrides
 * the toggle button.
 *
 * In live mode, this module is to be run with the microcontroller connected to the
 * physical Cooling Subsystem's fans' transistor switch gate terminal.
 *
 */

#ifndef CUSTOM_COOLINGSYSTEM_HPP_
#define CUSTOM_COOLINGSYSTEM_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <cstdio>
#include "stm32f4xx_hal.h"

namespace CoolingSystem {

	class CoolingSystem {
	public:
		static CoolingSystem& get_instance(); // Access function for singleton object

		// Create Cooling System Thread in FreeRTOS
		void start_cooling_subsystem_thread(void);

		// ISR handler for reading UI Button input used for enabling and disabling the fans
		void ui_fan_toggle_ISR(uint16_t gpio_pin);

		// Testing mode member functions
		void set_test_mode(bool is_enabled);
		void set_mock_inputs(float temp_c, bool ui_toggle);
		void simulate_button_press();
		bool get_fan_state(); // State machine output access getter function

		// Fan actuation: set GPIO pin configuration
		void init_fan_gpio(GPIO_TypeDef* port, uint16_t gpio_pin, GPIO_PinState active_state);

	private:
		 CoolingSystem();
		 ~CoolingSystem();

		// Concurrency member variables
		TaskHandle_t m_cs_task_handle = nullptr;
		SemaphoreHandle_t m_ui_fan_toggle_semaphore = nullptr; // Semaphore given by ISR

		// State machine output member variable
		bool m_is_fan_on;

		// Testing member variables for running this state machine thread in testing mode
		bool m_test_mode = false;
		float m_mock_temp = 25.0f;
		bool m_mock_ui_toggle = false;
		bool m_fan_on = false;
		SemaphoreHandle_t m_testing_mutex = nullptr;


		// Cooling System FSM
		// Inputs: Rail Temp (float), UI Fan Toggle (bool), Reset (bool)
		// Outputs: m_is_fan_on (bool)
		float read_sensor(int gpio_pin); // Access Thermistor thread temperature getter function
		//bool read_ui(int gpio_pin);
		void cooling_system_task(void* pv_parameters); // Isolated thread loop executing the FSM
		static void runStatic(void* params); // C wrapper for the thread's loop function

		// The following functions and member variables pertain to fan actuation
		GPIO_TypeDef* m_fan_gpio_port = nullptr;
		uint16_t m_fan_gpio_pin = 0;
		GPIO_PinState m_fan_active_state = GPIO_PIN_SET;
		void write_fan_output(bool fan_on);

		// Disable object copying
		CoolingSystem(const CoolingSystem&) = delete;
		CoolingSystem& operator = (const CoolingSystem&) = delete;
		CoolingSystem(CoolingSystem&&) = delete;
		CoolingSystem& operator = (CoolingSystem&&) = delete;

		bool is_any_rail_hot();
	};
}

#endif /* CUSTOM_COOLINGSYSTEM_HPP_ */
