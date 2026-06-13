/*
 * Testing.cpp
 *
 *  Created on: Apr 30, 2026
 *      Author: devhu
 */

#include <Custom/Testing/CoolingSystemTest.hpp>
#include <Custom/Testing/FullSystemTest.hpp>

static TaskHandle_t cooling_test_task_handle = nullptr;

CoolingSystemTest::CoolingSystemTest() {}
CoolingSystemTest::~CoolingSystemTest() {}

void CoolingSystemTest::run_cooling_system_test_task(void* argument){
    run_cooling_system_test_sequence();

    vTaskDelete(nullptr);
}

void CoolingSystemTest::run_cooling_system_test_sequence() {
	 auto& cs = CoolingSystem::CoolingSystem::get_instance();

	    char msg[100];

	    cs.set_test_mode(true);

	    cs.set_mock_inputs(25.0f, false);
	    //vTaskDelay(pdMS_TO_TICKS(100));
	    FullSystemTest::wait_for_next_test_step(nullptr);
	    snprintf(msg, sizeof(msg),
	             "Test 1: temp=25, fan=%d, expected=0\r\n",
	             cs.get_fan_state());
	    IO_Handler::uart_print(msg);

	    cs.set_mock_inputs(55.0f, false);
	    //vTaskDelay(pdMS_TO_TICKS(100));
	    FullSystemTest::wait_for_next_test_step(nullptr);
	    snprintf(msg, sizeof(msg),
	             "Test 2: temp=55, fan=%d, expected=1\r\n",
	             cs.get_fan_state());
	    IO_Handler::uart_print(msg);

	    cs.set_mock_inputs(25.0f, false);
	    //vTaskDelay(pdMS_TO_TICKS(100));
	    FullSystemTest::wait_for_next_test_step(nullptr);
	    snprintf(msg, sizeof(msg),
	             "Test 3: temp=25, fan=%d, expected=0\r\n",
	             cs.get_fan_state());
	    IO_Handler::uart_print(msg);

	    cs.simulate_button_press();
	    //vTaskDelay(pdMS_TO_TICKS(100));
	    FullSystemTest::wait_for_next_test_step(nullptr);
	    snprintf(msg, sizeof(msg),
	             "Test 4: button press, fan=%d, expected=1\r\n",
	             cs.get_fan_state());
	    IO_Handler::uart_print(msg);

	    cs.simulate_button_press();
	    //vTaskDelay(pdMS_TO_TICKS(100));
	    FullSystemTest::wait_for_next_test_step(nullptr);
	    snprintf(msg, sizeof(msg),
	             "Test 5: button press, fan=%d, expected=0\r\n",
	             cs.get_fan_state());
	    IO_Handler::uart_print(msg);

	    cs.simulate_button_press();
		//vTaskDelay(pdMS_TO_TICKS(100));
	    FullSystemTest::wait_for_next_test_step(nullptr);
		snprintf(msg, sizeof(msg),
				 "Test 6: button press, fan=%d, expected=1\r\n",
				 cs.get_fan_state());
		IO_Handler::uart_print(msg);

		cs.set_mock_inputs(51.0f, false);
		//vTaskDelay(pdMS_TO_TICKS(100));
		FullSystemTest::wait_for_next_test_step(nullptr);
		snprintf(msg, sizeof(msg),
				 "Test 7: temp=51, fan=%d, expected=1\r\n",
				 cs.get_fan_state());
		IO_Handler::uart_print(msg);

		cs.simulate_button_press();
		//vTaskDelay(pdMS_TO_TICKS(100));
		FullSystemTest::wait_for_next_test_step(nullptr);
		snprintf(msg, sizeof(msg),
				 "Test 8: button press, fan=%d, expected=1\r\n",
				 cs.get_fan_state());
		IO_Handler::uart_print(msg);

		// Cleanup and reset before going back to live mode
		IO_Handler::uart_print("Cooling test complete. Returning to live mode.\r\n");
		cs.set_mock_inputs(25.0f, false);
		cs.set_test_mode(false);
}

void CoolingSystemTest::init_cooling_system_test_thread(uint16_t stackSize, UBaseType_t priority) {
    xTaskCreate(
        run_cooling_system_test_task,
        "CoolingTest",
        stackSize,
        nullptr,
        priority,
        &cooling_test_task_handle
    );
}


