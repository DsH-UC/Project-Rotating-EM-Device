/*
 * Testing.hpp
 *
 *  Created on: Apr 30, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_TESTING_HPP_
#define CUSTOM_TESTING_HPP_

#include "../CoolingSystem.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include <cstdio>
#include "../IO_Handler.hpp"

class CoolingSystemTest {
public:
	CoolingSystemTest();
	~CoolingSystemTest();
	static void init_cooling_system_test_thread(uint16_t stackSize, UBaseType_t priority);
	static void run_cooling_system_test_sequence();
private:
	static void run_cooling_system_test_task(void* argument);
};

#endif /* CUSTOM_TESTING_HPP_ */
