/*
 * IRBreakSensor.hpp
 *
 *  Created on: Apr 29, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_IRBREAKSENSOR_HPP_
#define CUSTOM_IRBREAKSENSOR_HPP_

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

class IRBreakSensor {
public:
	// TODO 4/29/26 4:49am
	// Configure GPIO pins for both GPIO_MODE_IT_FALLING and GPIO_PULLUP
	// -> Done, but need to generate code from CubeMX and redo main file

	// TODO 4/30/26 12:11am
	// Might be able to remove port parameters since they're unused

	// TODO 4/30/26 2:44am
	// Finish subsystem SM testing tmr for ChrStg and RotBlock
	// Revise Cooling Sys state machine in notebook
	// Bluetooth streaming stuff(setup ESP32 onto STM32 + read GUI buttons) + meet Hudson
	// Cooling System: write fan GPIO/PWM here based on m_fan_on
	// Rotary encoder code integration, waiting on Boris
	// Sensor Logging pipeline and Streaming pipeline
	// physical voltage testing

    static IRBreakSensor& get_instance();

    // Disable copying
    IRBreakSensor(const IRBreakSensor&) = delete;
    IRBreakSensor& operator=(const IRBreakSensor&) = delete;

    void init_IR_break_thread(uint16_t stackSize, UBaseType_t priority);

    void init(uint16_t startPin, uint16_t endPin);

    void on_exti_callback(uint16_t GPIO_Pin);

    void task_loop();

    uint32_t get_elapsed_ms() const;

    float get_velocity();

    void reset();

private:
    enum class IRBreakEventType {
        START,
        END
    };

    struct IRBreakEvent { // Event and time stamp
        IRBreakEventType event_type;
        TickType_t tick;
    };

    IRBreakSensor();
    // Thread handle
    TaskHandle_t m_ir_break_task_handle;
    SemaphoreHandle_t m_mutex;

    // GPIO ports and pins
    uint16_t m_start_pin;
    uint16_t m_end_pin;

    // Event queue
    QueueHandle_t m_event_queue;

    // Start and end times
    TickType_t m_start_tick;
    TickType_t m_end_tick;

    // Start and end flags
    bool m_is_started;
    bool m_is_finished;

    static void taskEntry(void* argument);
};

#endif /* CUSTOM_IRBREAKSENSOR_HPP_ */
