/*
 * ChargingStageTest.hpp
 *
 *  Created on: May 1, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_TESTING_CHARGINGSTAGETEST_HPP_
#define CUSTOM_TESTING_CHARGINGSTAGETEST_HPP_

#include "../ChargingStage.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include <cstdio>
#include "../IO_Handler.hpp"

class ChargingStageTest {
public:
	ChargingStageTest();
	~ChargingStageTest();
	static void init_charging_stage_test_thread(uint16_t stackSize, UBaseType_t priority);
	static void run_charging_stage_test_sequence();
private:
	static void run_charging_stage_test_task(void* argument);
};

#endif /* CUSTOM_TESTING_CHARGINGSTAGETEST_HPP_ */
