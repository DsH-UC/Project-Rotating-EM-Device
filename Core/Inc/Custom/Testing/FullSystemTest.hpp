/*
 * FullSystemTest.hpp
 *
 *  Created on: May 3, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_TESTING_FULLSYSTEMTEST_HPP_
#define CUSTOM_TESTING_FULLSYSTEMTEST_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include <cstdint>
#include "semphr.h"

class FullSystemTest {
public:
    static void init_full_system_test_thread(uint16_t stackSize, UBaseType_t priority);

    // Manual testing
    static void init_manual_testing();
    static void next_test_step_from_ui();
    static void wait_for_next_test_step(const char* prompt);
private:
    static TaskHandle_t full_system_test_task_handle;
    static SemaphoreHandle_t m_next_test_step_sem;

    static void run_full_system_test_task(void* argument);
};

#endif /* CUSTOM_TESTING_FULLSYSTEMTEST_HPP_ */
