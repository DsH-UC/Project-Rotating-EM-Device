/*
 * RotatingBlockTest.hpp
 *
 *  Created on: May 1, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_TESTING_ROTATINGBLOCKTEST_HPP_
#define CUSTOM_TESTING_ROTATINGBLOCKTEST_HPP_

#include "../RotatingBlock.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include <cstdio>
#include "../IO_Handler.hpp"

class RotatingBlockTest {
public:
	RotatingBlockTest();
	~RotatingBlockTest();
	static void init_rotating_block_test_thread(uint16_t stackSize, UBaseType_t priority);
	static void run_rotating_block_test_sequence();
private:
	static void run_rotating_block_test_task(void* argument);
};

#endif /* CUSTOM_TESTING_ROTATINGBLOCKTEST_HPP_ */
