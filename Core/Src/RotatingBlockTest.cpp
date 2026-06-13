/*
 * RotatingBlockTest.cpp
 *
 *  Created on: May 1, 2026
 *      Author: devhu
 */

#include <Custom/Testing/RotatingBlockTest.hpp>
#include "Custom/Testing/FullSystemTest.hpp"

//static TaskHandle_t rot_block_test_task_handle = nullptr;
//
//RotatingBlockTest::RotatingBlockTest() {}
//RotatingBlockTest::~RotatingBlockTest() {}
//
//void RotatingBlockTest::run_rotating_block_test_task(void* argument)
//{
//    run_rotating_block_test_sequence();
//    vTaskDelete(nullptr);
//}
//
//void RotatingBlockTest::run_rotating_block_test_sequence() {
//	auto& rb = RotatingBlock::RotatingBlock::get_instance();
//
//	char msg[120];
//
//	rb.set_test_mode(true);
//
//	IO_Handler::uart_print("RotatingBlock test started\r\n");
//
//	// Test 1: low temp, no button -> should not rotate
//	rb.set_mock_inputs(25.0f, 0.0f, false);
//	//vTaskDelay(pdMS_TO_TICKS(150));
//	FullSystemTest::wait_for_next_test_step(nullptr);
//
//	snprintf(msg, sizeof(msg), "Test 1: low temp no button, pwm=%d expected=0\r\n", rb.get_motor_pwm_enabled());
//	IO_Handler::uart_print(msg);
//
//	// Test 2: low temp + button -> should still not rotate
//	rb.set_mock_inputs(25.0f, 0.0f, false);
//	rb.simulate_button_press();
//	//vTaskDelay(pdMS_TO_TICKS(150));
//	FullSystemTest::wait_for_next_test_step(nullptr);
//
//	snprintf(msg, sizeof(msg), "Test 2: low temp + button, pwm=%d expected=0\r\n", rb.get_motor_pwm_enabled());
//	IO_Handler::uart_print(msg);
//
//	// Test 3: temp OK + button + not aligned -> should start rotating
//	rb.set_mock_inputs(50.0f, 0.0f, false);
//	rb.simulate_button_press();
//	//vTaskDelay(pdMS_TO_TICKS(150));
//	FullSystemTest::wait_for_next_test_step(nullptr);
//
//	snprintf(msg, sizeof(msg), "Test 3: temp OK + button, pwm=%d expected=1\r\n", rb.get_motor_pwm_enabled());
//	IO_Handler::uart_print(msg);
//
//	// Test 4: still not aligned -> should keep rotating
//	rb.set_mock_inputs(50.0f, 30.0f, false);
//	//vTaskDelay(pdMS_TO_TICKS(150));
//	FullSystemTest::wait_for_next_test_step(nullptr);
//
//	snprintf(msg, sizeof(msg), "Test 4: rotating not aligned, pwm=%d expected=1\r\n", rb.get_motor_pwm_enabled());
//	IO_Handler::uart_print(msg);
//
//	// Test 5: aligned -> should stop and increment rail count
//	rb.set_mock_inputs(50.0f, 90.0f, false);
//	//vTaskDelay(pdMS_TO_TICKS(150));
//	FullSystemTest::wait_for_next_test_step(nullptr);
//
//	snprintf(msg, sizeof(msg), "Test 5: aligned, pwm=%d expected=0\r\n", rb.get_motor_pwm_enabled());
//	IO_Handler::uart_print(msg);
//
//	snprintf(msg, sizeof(msg), "Test 5: rails used=%d expected=1\r\n", rb.get_num_used_rails());
//	IO_Handler::uart_print(msg);
//
//	// Test 6: after alignment, another press should rotate again if rails available
//	rb.set_mock_inputs(50.0f, 0.0f, false);
//	rb.simulate_button_press();
//	//vTaskDelay(pdMS_TO_TICKS(150));
//	FullSystemTest::wait_for_next_test_step(nullptr);
//
//	snprintf(msg, sizeof(msg), "Test 6: second rotation, pwm=%d expected=1\r\n", rb.get_motor_pwm_enabled());
//	IO_Handler::uart_print(msg);
//
//	// Stop second rotation by aligning
//	rb.set_mock_inputs(50.0f, 90.0f, false);
//	//vTaskDelay(pdMS_TO_TICKS(150));
//	FullSystemTest::wait_for_next_test_step(nullptr);
//
//	snprintf(msg, sizeof(msg), "Test 6b: second aligned, pwm=%d expected=0\r\n", rb.get_motor_pwm_enabled());
//	IO_Handler::uart_print(msg);
//
//	snprintf(msg, sizeof(msg), "Test 6b: rails used=%d expected=2\r\n", rb.get_num_used_rails());
//	IO_Handler::uart_print(msg);
//
//	IO_Handler::uart_print("RotatingBlock test complete. Returning to live mode.\r\n");
//
//	rb.set_mock_inputs(25.0f, 0.0f, false);
//	rb.set_test_mode(false);
//}
//
//void RotatingBlockTest::init_rotating_block_test_thread(uint16_t stackSize, UBaseType_t priority) {
//	xTaskCreate(
//		run_rotating_block_test_task,
//		"RotatingBlockTest",
//		stackSize,
//		nullptr,
//		priority,
//		&rot_block_test_task_handle
//	);
//}
/*
 * RotatingBlockTest.cpp
 *
 *  Created on: May 1, 2026
 *      Author: devhu
 */

#include <Custom/Testing/RotatingBlockTest.hpp>
#include "Custom/Testing/FullSystemTest.hpp"

static TaskHandle_t rot_block_test_task_handle = nullptr;

RotatingBlockTest::RotatingBlockTest() {}
RotatingBlockTest::~RotatingBlockTest() {}

void RotatingBlockTest::run_rotating_block_test_task(void* argument)
{
    run_rotating_block_test_sequence();
    vTaskDelete(nullptr);
}

static void print_rb_state(const char* label, int expected_pwm, int expected_rails)
{
    auto& rb = RotatingBlock::RotatingBlock::get_instance();

    char msg[160];

    snprintf(msg, sizeof(msg),
             "%s: pwm=%d expected=%d | rails=%d expected=%d\r\n",
             label,
             rb.get_motor_pwm_enabled(),
             expected_pwm,
             rb.get_num_used_rails(),
             expected_rails);

    IO_Handler::uart_print(msg);
}

void RotatingBlockTest::run_rotating_block_test_sequence()
{
    auto& rb = RotatingBlock::RotatingBlock::get_instance();

    IO_Handler::uart_print("\r\n--- RotatingBlock Test START ---\r\n");

    rb.set_test_mode(true);

    /*
     * Test 1:
     * Cold active rail, no launch request.
     * Motor should stay off.
     */
    rb.set_mock_inputs(25.0f, 0.0f, false);
    FullSystemTest::wait_for_next_test_step(
        "RB Test 1: cold rail, no launch. Expect motor OFF.\r\n"
    );
    print_rb_state("RB Test 1", 0, 0);

    /*
     * Test 2:
     * Cold active rail, launch request.
     * Temperature is below threshold, so motor should stay off.
     */
    rb.set_mock_inputs(25.0f, 0.0f, false);
    rb.simulate_button_press();

    FullSystemTest::wait_for_next_test_step(
        "RB Test 2: cold rail + launch request. Expect motor OFF.\r\n"
    );
    print_rb_state("RB Test 2", 0, 0);

    /*
     * Test 3:
     * Hot active rail, launch request, currently aligned at 0 degrees.
     * Motor should start rotating.
     */
    rb.set_mock_inputs(50.0f, 0.0f, false);
    rb.simulate_button_press();

    FullSystemTest::wait_for_next_test_step(
        "RB Test 3: hot rail + launch at 0 deg. Expect motor ON.\r\n"
    );
    print_rb_state("RB Test 3", 1, 0);

    /*
     * Test 4:
     * Still inside the starting alignment zone.
     * Because of m_has_left_alignment_zone, motor should NOT stop yet.
     */
    rb.set_mock_inputs(50.0f, 0.0f, false);

    FullSystemTest::wait_for_next_test_step(
        "RB Test 4: still at starting alignment zone. Expect motor still ON.\r\n"
    );
    print_rb_state("RB Test 4", 1, 0);

    /*
     * Test 5:
     * Move away from alignment zone.
     * This should set m_has_left_alignment_zone internally.
     * Motor should keep rotating.
     */
    rb.set_mock_inputs(50.0f, 30.0f, false);

    FullSystemTest::wait_for_next_test_step(
        "RB Test 5: left alignment zone at 30 deg. Expect motor ON.\r\n"
    );
    print_rb_state("RB Test 5", 1, 0);

    /*
     * Test 6:
     * Reach next rail alignment at 60 degrees.
     * Motor should stop and used rail count should increment.
     */
    rb.set_mock_inputs(50.0f, 60.0f, false);

    FullSystemTest::wait_for_next_test_step(
        "RB Test 6: next rail alignment at 60 deg. Expect motor OFF and rails=1.\r\n"
    );
    print_rb_state("RB Test 6", 0, 1);

    /*
     * Test 7:
     * Start another rotation from the next aligned rail.
     */
    rb.set_mock_inputs(50.0f, 60.0f, false);
    rb.simulate_button_press();

    FullSystemTest::wait_for_next_test_step(
        "RB Test 7: second hot launch at 60 deg. Expect motor ON.\r\n"
    );
    print_rb_state("RB Test 7", 1, 1);

    /*
     * Test 8:
     * Still at 60 degrees. Should keep rotating because it has not left
     * the starting alignment zone yet.
     */
    rb.set_mock_inputs(50.0f, 60.0f, false);

    FullSystemTest::wait_for_next_test_step(
        "RB Test 8: still at second starting zone. Expect motor ON.\r\n"
    );
    print_rb_state("RB Test 8", 1, 1);

    /*
     * Test 9:
     * Leave second alignment zone.
     */
    rb.set_mock_inputs(50.0f, 90.0f, false);

    FullSystemTest::wait_for_next_test_step(
        "RB Test 9: left second alignment zone at 90 deg. Expect motor ON.\r\n"
    );
    print_rb_state("RB Test 9", 1, 1);

    /*
     * Test 10:
     * Reach next alignment at 120 degrees.
     * Motor should stop and used rail count should become 2.
     */
    rb.set_mock_inputs(50.0f, 120.0f, false);

    FullSystemTest::wait_for_next_test_step(
        "RB Test 10: next rail alignment at 120 deg. Expect motor OFF and rails=2.\r\n"
    );
    print_rb_state("RB Test 10", 0, 2);

    IO_Handler::uart_print("RotatingBlock test complete. Returning to live mode.\r\n");

    rb.set_mock_inputs(25.0f, 0.0f, false);
    rb.set_test_mode(false);

    IO_Handler::uart_print("--- RotatingBlock Test END ---\r\n");
}

void RotatingBlockTest::init_rotating_block_test_thread(uint16_t stackSize,
                                                        UBaseType_t priority)
{
    if (rot_block_test_task_handle != nullptr) {
        IO_Handler::uart_print("RotatingBlockTest already created\r\n");
        return;
    }

    BaseType_t ok = xTaskCreate(
        run_rotating_block_test_task,
        "RotatingBlockTest",
        stackSize,
        nullptr,
        priority,
        &rot_block_test_task_handle
    );

    configASSERT(ok == pdPASS);
}
