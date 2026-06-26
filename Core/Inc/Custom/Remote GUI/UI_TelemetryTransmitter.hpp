/*
 * UITelemetryTransmitter.hpp
 *
 *  Created on: May 17, 2026
 *      Author: devhu
 */

#ifndef INC_CUSTOM_REMOTE_GUI_UI_TELEMETRYTRANSMITTER_HPP_
#define INC_CUSTOM_REMOTE_GUI_UI_TELEMETRYTRANSMITTER_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include <cstdint>

/*
 * Class: UI_TelemetryTransmitter
 *
 * Description: This class periodically reads from all sensors and subsystem state machines to send as a packet into a Bluetooth
 * 				network co-processor ESP-32 over UART within a single thread. This acts as the streaming pipeline reader
 * 				thread block within the software architecture block diagram.
 */
class UI_TelemetryTransmitter {
public:
	/*
	 * Function: get_instance
	 *
	 * Description: The get_instance function is the sole singleton accessor to the UI_TelemetryTransmitter class.
	 *
	 * Outputs: The reference to an anonymous sole object instance of the UI_TelemetryTransmitter class.
	 */
    static UI_TelemetryTransmitter& get_instance();

    /*
	* Function: start_thread
	*
	* Description: Initializes a single thread that executes the reading of sensors and state machine outputs,
	* 			   assembling them into a packet, and sending the packet bytes to an external network co-processor
	* 			   ESP-32 over UART.
	*
	* Parameters: stackSize - thread stack memory size allocation in number of words using heap memory.
	* 		   	  priority - thread execution priority.
	*/
    void start_thread(uint16_t stackSize, UBaseType_t priority);

private:
    UI_TelemetryTransmitter();

    static void runStatic(void* params); // C wrapper for C++ task_loop function.
    void task_loop(); // Thread loop code block for periodic execution.

    TaskHandle_t m_task_handle;
};

#endif /* INC_CUSTOM_REMOTE_GUI_UI_TELEMETRYTRANSMITTER_HPP_ */
