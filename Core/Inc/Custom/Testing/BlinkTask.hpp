/*
 * BlinkTask.h
 *
 *  Created on: Apr 24, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_BLINKTASK_HPP_
#define CUSTOM_BLINKTASK_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
class BlinkTask {
public:
    // Constructor initializes hardware info and timing
    BlinkTask(GPIO_TypeDef* port, uint16_t pin, uint32_t periodMs);
    ~BlinkTask();

    // Starts the task in the FreeRTOS scheduler
    void start(const char* name, uint16_t stackDepth, UBaseType_t priority);

    void print_data_from_ext_thread(char* msg, int n, uint16_t raw_data);
private:
    // Static wrapper to bridge C-style FreeRTOS to C++
    static void runStatic(void* params);

    // The actual loop logic
    void run();

    GPIO_TypeDef* m_port;
    uint16_t m_pin;
    uint32_t m_periodMs;
};

#endif /* CUSTOM_BLINKTASK_HPP_ */

