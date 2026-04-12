/*
 * IO_Handler.hpp
 *
 *  Created on: Mar 22, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_IO_HANDLER_HPP_
#define CUSTOM_IO_HANDLER_HPP_

#include "main.h"
#include <cstdint>
#include "stm32f4xx_hal.h"
#include <cstdio>
#include <cstdint>


// File containing all functions for reading input signals.

namespace IO_Handler {
	// TODO 4/6:
	// Reading in UI + sensors into interrupt semaphores + DMA buffer -> route to SM's + BT stream TX
		// Prototype BT stream and packet structure
	// Route reset inputs for SMs from FaultDetector
	// Select and program digital filters + FB controller using DSP

	// Charging Stage + Cooling System implementations to use C++ classes
		// Add m_ prefix for member variable to classes
		// Arrange Custom cpp/hpp files into organized subfolders w/ good naming
	// Comment/Document/Codedocs for C++ files in the Custom folder

	// PyQt + numpy GUI for BT stream receiving and UI button signal transmission for fans+launch
	// PyQt sensor logging, move this block from the SW arch block diagram MCU to the UI.

	class IO_Handler {
	public:
		// Access function for singleton
		static IO_Handler& get_instance();

		// Disable copying
		IO_Handler(const IO_Handler&) = delete;
		IO_Handler& operator = (const IO_Handler&) = delete;
		IO_Handler(IO_Handler&&) = delete;
		IO_Handler& operator = (IO_Handler&&) = delete;

		// Input signal sources from the UI. Functions reading from the UI are placed here.
		bool read_UI_launch_button(); // These implementations are used to finish the ISR functions' conditionals in the SM classes.
		bool read_UI_fan_toggle_button();

		int* read_UI_stream_selection();
		// Output signal sources to the UI. It contains a Bluetooth receiver to receive packets of sensor data.
		// Sensor reading functions are placed here.

		// Bluetooth functions
		void send_bluetooth_packet();

	private:
		// Member variables
		QueueHandle_t m_selected_UI_display_sensors;
		uint64_t* m_bluetooth_stream_packet; // Turn into function to create the packet instead of defining this variable.

		uint8_t m_is_launch_button_pressed_rx;
		//uint8_t m_is_fan_toggle_pressed_rx;

		uint16_t* m_adc_buffer;

		void init_adc(void);

		// Sensor Streaming Pipeline Block member variables and functions
		QueueHandle_t m_packet_queue; // Thread safe queue
		TaskHandle_t packet_queue_write_task_handle; // Enqueue thread
		TaskHandle_t packet_queue_read_task_handle; // Dequeue thread

		void start_packet_rdwr_threads(void);
		void packet_queue_write_task(void* pv_parameters);
		void packet_queue_read_task(void* pv_parameters);
		void compress_packet(void* packet_data);

	};

}


#endif /* CUSTOM_IO_HANDLER_HPP_ */
