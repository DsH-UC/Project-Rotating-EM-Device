/*
 * ChargingStageTest.cpp
 *
 *  Created on: May 1, 2026
 *      Author: devhu
 */

#include <Custom/Testing/ChargingStageTest.hpp>
#include "Custom/Testing/FullSystemTest.hpp"

static TaskHandle_t chr_stage_test_task_handle = nullptr;

ChargingStageTest::ChargingStageTest() {}
ChargingStageTest::~ChargingStageTest() {}

void ChargingStageTest::run_charging_stage_test_task(void* argument) {
	run_charging_stage_test_sequence();
    vTaskDelete(nullptr);

}

void ChargingStageTest::run_charging_stage_test_sequence() {
	 auto& cs = ChargingStage::ChargingStage::get_instance();

	    char msg[100];

	    cs.set_test_mode(true);

	    // Test 1: low voltage, not ready
	    cs.set_mock_inputs(5.0f, false, false, 0);
	    //vTaskDelay(pdMS_TO_TICKS(100));
	    FullSystemTest::wait_for_next_test_step(nullptr);

	    snprintf(msg, sizeof(msg), "// Test 1: low voltage, not ready - ready=%d expected=0\r\n", cs.get_is_launch_payload_ready());
	    IO_Handler::uart_print(msg);

	    // Test 2: charging, not ready
	    cs.set_mock_inputs(20.0f, true, false, 0);
	    cs.simulate_button_press();
	    //vTaskDelay(pdMS_TO_TICKS(100));
	    FullSystemTest::wait_for_next_test_step(nullptr);

	    snprintf(msg, sizeof(msg), "Test 2: charging, not ready - ready=%d expected=0\r\n", cs.get_is_launch_payload_ready());
	    IO_Handler::uart_print(msg);

	    // Test 3: fully charged, ready
	    cs.set_mock_inputs(50.0f, false, false, 0);
	    //vTaskDelay(pdMS_TO_TICKS(100));
	    FullSystemTest::wait_for_next_test_step(nullptr);

	    snprintf(msg, sizeof(msg), "Test 3: fully charged, ready - ready=%d expected=1\r\n", cs.get_is_launch_payload_ready());
	    IO_Handler::uart_print(msg);

	    // Test 4: discharge, not ready again
	    cs.set_mock_inputs(5.0f, false, false, 0);
	    //vTaskDelay(pdMS_TO_TICKS(100));
	    FullSystemTest::wait_for_next_test_step(nullptr);

	    snprintf(msg, sizeof(msg), "Test 4: discharge, not ready again - ready=%d expected=0\r\n", cs.get_is_launch_payload_ready());
	    IO_Handler::uart_print(msg);

	    // Cleanup and reset before going back to live mode
		IO_Handler::uart_print("Charging Stage test complete. Returning to live mode.\r\n");
		cs.set_mock_inputs(0.0f, false, false, 0);
	    cs.set_test_mode(false);
}

void ChargingStageTest::init_charging_stage_test_thread(uint16_t stackSize, UBaseType_t priority) {
	xTaskCreate(
		run_charging_stage_test_task,
		"ChargingStageTest",
		stackSize,
		nullptr,
		priority,
		&chr_stage_test_task_handle
	);
}

//void ChargingStageTest::init_charging_stage_test_thread(uint16_t stackSize,
//                                                        UBaseType_t priority)
//{
//    char msg[100];
//
//    snprintf(msg, sizeof(msg),
//             "Creating ChargingStageTest task, old handle=%p tick=%lu\r\n",
//             chr_stage_test_task_handle,
//             (unsigned long)xTaskGetTickCount());
//    IO_Handler::uart_print(msg);
//
//    if (chr_stage_test_task_handle != nullptr) {
//        IO_Handler::uart_print("ChargingStageTest already created, skipping\r\n");
//        return;
//    }
//
//    BaseType_t ok = xTaskCreate(
//        run_charging_stage_test_task,
//        "ChargingStageTest",
//        stackSize,
//        nullptr,
//        priority,
//        &chr_stage_test_task_handle
//    );
//
//    configASSERT(ok == pdPASS);
//
//    snprintf(msg, sizeof(msg),
//             "Created ChargingStageTest task, new handle=%p\r\n",
//             chr_stage_test_task_handle);
//    IO_Handler::uart_print(msg);
//}
