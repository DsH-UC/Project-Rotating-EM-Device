/*
 * FullSystemTest.cpp
 *
 *  Created on: May 3, 2026
 *      Author: devhu
 */

#include "Custom/Testing/FullSystemTest.hpp"

#include "Custom/Testing/CoolingSystemTest.hpp"
#include "Custom/Testing/ChargingStageTest.hpp"
#include "Custom/Testing/RotatingBlockTest.hpp"
#include "Custom/IO_Handler.hpp"

TaskHandle_t FullSystemTest::full_system_test_task_handle = nullptr;
SemaphoreHandle_t FullSystemTest::m_next_test_step_sem = nullptr;

void FullSystemTest::init_full_system_test_thread(uint16_t stackSize, UBaseType_t priority) {
    if (full_system_test_task_handle != nullptr) {
        IO_Handler::uart_print("FullSystemTest already created\r\n");
        return;
    }

    BaseType_t ok = xTaskCreate(
        FullSystemTest::run_full_system_test_task,
        "FullSystemTest",
        stackSize,
        nullptr,
        priority,
        &full_system_test_task_handle
    );

    configASSERT(ok == pdPASS);
}

void FullSystemTest::run_full_system_test_task(void* argument) {
    (void)argument;

    IO_Handler::uart_print("\r\n FULL SYSTEM TEST START \r\n");

    IO_Handler::uart_print("\r\n CoolingSystem Tests \r\n");
    CoolingSystemTest::run_cooling_system_test_sequence();

    vTaskDelay(pdMS_TO_TICKS(250));

    IO_Handler::uart_print("\r\n ChargingStage Tests\r\n");
    ChargingStageTest::run_charging_stage_test_sequence();

    vTaskDelay(pdMS_TO_TICKS(250));

    IO_Handler::uart_print("\r\n RotatingBlock Tests \r\n");
    RotatingBlockTest::run_rotating_block_test_sequence();

    IO_Handler::uart_print("\r\n FULL SYSTEM TEST COMPLETE \r\n");

    full_system_test_task_handle = nullptr;
    vTaskDelete(nullptr);
}

// Manual testing
void FullSystemTest::init_manual_testing() {
	if (m_next_test_step_sem == nullptr) m_next_test_step_sem = xSemaphoreCreateBinary();
}

void FullSystemTest::next_test_step_from_ui() {
	 if (m_next_test_step_sem != nullptr) xSemaphoreGive(m_next_test_step_sem);
}

void FullSystemTest::wait_for_next_test_step(const char* prompt) {
	if (prompt != nullptr) IO_Handler::uart_print(prompt);

	IO_Handler::uart_print("Press N in serial terminal to continue to next test.\r\n");

	if (m_next_test_step_sem != nullptr) xSemaphoreTake(m_next_test_step_sem, portMAX_DELAY);

}


