/*
 * UICommandReceiver.hpp
 *
 *  Created on: May 3, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_REMOTE_GUI_UI_COMMANDRECEIVER_HPP_
#define CUSTOM_REMOTE_GUI_UI_COMMANDRECEIVER_HPP_

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <cstdint>
#include <cstdio>
#include <cstring>

/*
 * Class: UI_CommandReceiver
 *
 * Description: This class reads from the UART connection with the Bluetooth network co-processor ESP-32 for UI button press
 * 				commands and for thermistor data from a second rotating ESP-32 streamed to the network co-processor.
 * 				Then, the read button and thermistor data is sent as inputs into subsystem state machine threads to drive
 * 				them. This class represents the sensor pipeline streaming writer block in the software architecture block
 * 				diagram.
 */
class UI_CommandReceiver {
public:
	/*
	 * Function: get_instance
	 *
	 * Description: The get_instance function is the sole singleton accessor to the UI_CommandReceiver class.
	 *
	 * Outputs: The reference to an anonymous sole object instance of the UI_CommandReceiver class.
	 */
    static UI_CommandReceiver& get_instance();

    /*
     * Function: send_to_gui
     *
     * Description: Mutex protected UART transmit into the network co-processor ESP-32 that sends the UART bytes
     * 				to a remote laptop UI over Bluetooth.
     *
     * Parameters: msg - byte string to send over the UART connection.
     */
    void send_to_gui(const char* msg);

    /*
	 * Function: init
	 *
	 * Description: Initializes the UI_CommandReceiver class by assigning its internal UART handle, mutexes, and async
	 * 				producer/consumer queue.
	 *
	 * Parameters: huart - UART handle to stream bytes to.
	 */
    void init(UART_HandleTypeDef* huart);

    /*
   	* Function: start_thread
   	*
   	* Description: Initializes a single thread that executes the reading of UI button presses and remote thermistor data,
   	* 			   feeding them into the subsystem state machine threads, and acknowledging the button press command
   	* 			   requests.
   	*
   	* Parameters: stackSize - thread stack memory size allocation in number of words using heap memory.
   	* 		   	  priority - thread execution priority.
   	*/
    void start_thread(uint16_t stackSize, UBaseType_t priority);

    /*
     * Function: on_uart_rx_callback
     *
     * Description: Interrupt handler executed for every byte received over the UART connection with the network
     * 				co-processor. The received byte is first written into a producer consumer queue and then the
     * 				UART handle is called to wait for the next byte to receive.
     *
     * Parameters: huart - UART handle to read bytes from.
     */
    void on_uart_rx_callback(UART_HandleTypeDef* huart);

    // Rotary thermistor getters
    /*
     * Function: on_get_remote_temp_c
     *
     * Description: Accessor function for reading the temperature of a specific rotating thermistor based on zero
     * 				rail index.
     *
     * Outputs: Returns the thermistor temperature in Celsius for the thermistor connected to an indexed pair of
     * 			rails.
     *
     * Parameters: index - zero based rail index for the rail pair connected to the specified thermistor.
     *
     */
    float get_remote_temp_c(uint8_t index);

    /*
     * Function: get_remote_temp_sequence
     *
     * Description: Accessor function for reading the packet number of the current thermistor data read.
     *
     * Outputs: Returns the integer count of the curernt thermistor data packet count.
     */
    uint32_t get_remote_temp_sequence();

private:
    UI_CommandReceiver();

    static void runStatic(void* params); // C wrapper for C++ task_loop function.
    void task_loop(); // Thread loop code block for periodic execution.

    /*
     * Function: process_command
     *
     * Description: Multiplexes commands using a character mapped to UI button presses as a selector and
     * 				executes those commands.
     * 				'F' - Encodes the Cooling Subsystem's fan toggling command
     * 				'L' - Encodes the payload launch command between the Rotating Block and Charging Stage
     * 					  subsystems
     * 				'R' - Encodes the system reset command. TODO
     * 				'S' - Encodes the status command to view the current subsystem state machine outputs and
     * 					  sensor data.
     * 				'N' - Encodes the testing mode's step command to continue to the next unit test.
     *
     * Parameters: cmd - alphabetic character that encodes commands corresponding to UI button presses.
     *
     */
    void process_command(char cmd);
    void send_response(const char* msg); // Wrapper function for the public send_to_gui function

    UART_HandleTypeDef* m_huart;

    QueueHandle_t m_rx_queue;
    TaskHandle_t m_task_handle;
    uint8_t m_rx_byte;

    static constexpr uint16_t RX_QUEUE_LENGTH = 32;

    SemaphoreHandle_t m_tx_mutex;

    // Rotary thermistor parsing
    static constexpr uint16_t LINE_BUFFER_SIZE = 128;
    char m_line_buffer[LINE_BUFFER_SIZE];
    uint16_t m_line_index;

    static constexpr uint8_t NUM_REMOTE_THERMISTORS = 6; // More rotary thermistor parsing
    float m_remote_temp_c[NUM_REMOTE_THERMISTORS];
    uint32_t m_remote_temp_sequence;
    SemaphoreHandle_t m_remote_temp_mutex;

    /*
	 * Function: process_rtemp_line
	 *
	 * Description: Parses the thermistor data packet into a formatted string such that format specifiers
	 * 				corresponding to temperatures are stored into internal class variables.
	 *
	 * Parameters: line - byte string corresponding to a sequence containing all six thermistor temperatures.
	 *
	 */
    void process_rtemp_line(const char* line);

    /*
	 * Function: process_line
	 *
	 * Description: Binary classifier for when the incoming bytes are either single character UI commands
	 * 				or string based thermistor data packets.
	 *
	 * Parameters: line - the incoming data sent from the network co-processor either in the form of a single
	 * 					  byte character or a string.
	 *
	 */
    void process_line(const char* line);

    // Prevent copying
    UI_CommandReceiver(const UI_CommandReceiver&) = delete;
    UI_CommandReceiver& operator=(const UI_CommandReceiver&) = delete;

};


#endif /* CUSTOM_REMOTE_GUI_UI_COMMANDRECEIVER_HPP_ */
