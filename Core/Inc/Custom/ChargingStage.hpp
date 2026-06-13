/*
 * ChargingStage.hpp
 *
 *  Created on: Mar 13, 2026
 *      Author: devhu
 */

/**
 * @file ChargingStage.hpp
 *
 * @brief Interface file for the Charging Stage Subsystem Thread from the Software Architecture
 * Block Diagram
 *
 * This module provides an interface for the Charging Stage Subsystem's state machine,
 * providing access functions to its inputs(capacitor bank voltage, heated rail count,
 * and launch GUI button), outputs (payload launch ready), and states (Discharged, Charging,
 * and Charged). It can be ran in either test mode for mock testing isolated
 * from the capacitor bank voltage, Rotating Block, button input. Otherwise, it can be ran in
 * live mode for real time operation with those input sources.
 *
 * The state machine is in the Discharged state when the capacitor voltage is below a min threshold.
 * A GUI launch button press at less than the maximum number of heated rails sets the capacitor
 * in the Charging state until the capacitor bank voltage reaches a max threshold value to where it
 * enters the Charged state. In the Charged State, dropping below a min threshold transitions back
 * to the Discharged State.
 *
 * In live mode, this module is to be run with the microcontroller connected to the
 * physical Charging Stage Subsystem's pulsing circuit transistor switch.
 *
 */

#ifndef CUSTOM_CHARGINGSTAGE_HPP_
#define CUSTOM_CHARGINGSTAGE_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <cstdio>

namespace ChargingStage {
	class ChargingStage {
		public:
			// Access function for singleton
			static ChargingStage& get_instance();

			// Create Charging Stage Thread in FreeRTOS
			void start_charging_stage_subsystem_thread(void);

			void ui_launch_button_ISR(void* pv_parameters);

			float read_sensor(int gpio_pin);
			int read_rot_block_num_consumed_rails(void);

			// Note 5/3/26: Remove this function and its associated member variable
			bool read_rot_block_is_motor_aligned(void);

			// Not part of SM, but is connected to the physical subsystem.
			// The IR break sensor is for velocity telemetry
			float get_IR_velocity();

			bool get_is_launch_payload_ready(); // expose SM output variable

			// Testing mock variable setter member functions
			void set_test_mode(bool enable);
			void set_mock_inputs(float cap_voltage, bool launch_button, bool motor_aligned, int gate_launch);
			void simulate_button_press();

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

			ChargingStage();
			~ChargingStage();

			// Store thread in cpp file
			TaskHandle_t m_cs_task_handle = nullptr;
			SemaphoreHandle_t m_ui_launch_button_semaphore = nullptr;

			charging_stage_inputs_t m_cs_inputs;
			bool m_is_payload_launch_ready; // SM output

			void charging_stage_task(void* pv_parameters);
			static void runStatic(void* params);

			// Testing member variables
			bool m_test_mode;
			float m_mock_cap_voltage;
			bool m_mock_launch_button;
			bool m_mock_motor_aligned;
			int m_mock_gate_launch;
			SemaphoreHandle_t m_testing_mutex;


			// Disable copying
			ChargingStage(const ChargingStage&) = delete;
			ChargingStage& operator = (const ChargingStage&) = delete;
			ChargingStage(ChargingStage&&) = delete;
			ChargingStage& operator = (ChargingStage&&) = delete;
	};
}

#endif /* CUSTOM_CHARGINGSTAGE_HPP_ */
