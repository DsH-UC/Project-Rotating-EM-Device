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

class IO_Handler {
private:
	// ThermistorSensor rail_therm;

public:
	IO_Handler() {}
	~IO_Handler(){}

	// Helper function to redirect printf to UART
	//static void uart_print(const char *msg){ HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY); }

	static void init();
	static void uart_print(const char* msg);
//	void init_thermistor() {}
//	void thermistor_while_loop(){}
};



#endif /* CUSTOM_IO_HANDLER_HPP_ */
