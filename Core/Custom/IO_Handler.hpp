/*
 * IO_Handler.hpp
 *
 *  Created on: Mar 22, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_IO_HANDLER_HPP_
#define CUSTOM_IO_HANDLER_HPP_

#include <cstdint>
// File containing all functions for reading input signals.

namespace IO_Handler {

	class IO_Handler {
	public:
		// Input signal sources from the UI. Functions reading from the UI are placed here.
		bool read_UI_launch_button(); // These implementations are used to finish the ISR functions' conditionals in the SM classes.
		bool read_UI_fan_toggle_button();

		int* read_UI_stream_selection();
		// Output signal sources to the UI. It contains a Bluetooth receiver to receive packets of sensor data.
		// Sensor reading functions are placed here.

	private:
		QueueHandle_t selected_UI_display_sensors;
		uint64_t* bluetooth_stream_packet;
	};

}


#endif /* CUSTOM_IO_HANDLER_HPP_ */
