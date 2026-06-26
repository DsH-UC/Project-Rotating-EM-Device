/*
 * IO_Handler.h
 *
 *  Created on: Apr 25, 2026
 *      Author: devhu
 */

/**
 * @file IO_Handler.hpp
 *
 * @brief Interface file for printing messages through USART2 for logging and debug. It also
 * contains all interrupt callback functions to wake threads into the Running state.
 *
 */

#ifndef CUSTOM_IO_HANDLER_HPP_
#define CUSTOM_IO_HANDLER_HPP_

#include "main.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

using namespace std;
// Global main.c variables
#ifdef __cplusplus
extern "C" {
#endif

extern UART_HandleTypeDef huart2;

#ifdef __cplusplus
}
#endif

/*
 * Class: IO_Handler
 *
 * Description: The IO_Handler class manages program and system I/O. It implements a serial monitor print function
 * 				for debugging and testing. Additionally, the class houses two callback functions pertaining to
 * 				external interrupts for UI command buttons and the sending of telemetry from external
 * 				microcontrollers respectively.
 */
class IO_Handler {
private:
	// ThermistorSensor rail_therm;

public:
	// Empty constructor/destructor
	IO_Handler() {}
	~IO_Handler(){}

	// Helper function to redirect printf to UART
	//static void uart_print(const char *msg){ HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY); }

	/*
	 * Function: init
	 *
	 * Description: The init function initializes the IO_Handler class by simply assigning its internal mutex.
	 */
	static void init();

	/*
	 * Function: uart_print
	 *
	 * Description: The uart_print function prints strings to the serial monitor through wrapping the HAL_UART_Transmit
	 * 				function within a mutex critical section for concurrent function calls.
	 *
	 * Inputs: msg - the string to print to the serial monitor.
	 */
	static void uart_print(const char* msg);

//	void init_thermistor() {}
//	void thermistor_while_loop(){}
};



#endif /* CUSTOM_IO_HANDLER_HPP_ */
