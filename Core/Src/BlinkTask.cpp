/*
 * BlinkTask.cpp
 *
 *  Created on: Apr 24, 2026
 *      Author: devhu
 */

#include <Custom/Testing/BlinkTask.hpp>
#include "Custom/IO_Handler.hpp"

BlinkTask::~BlinkTask() {
	// TODO Auto-generated destructor stub
}

BlinkTask::BlinkTask(GPIO_TypeDef* port, uint16_t pin, uint32_t periodMs)
    : m_port(port), m_pin(pin), m_periodMs(periodMs) {}

void BlinkTask::start(const char* name, uint16_t stackDepth, UBaseType_t priority) {
    xTaskCreate(runStatic, name, stackDepth, this, priority, NULL);
}

void BlinkTask::runStatic(void* params) {
    // Bridge back to the class instance
    static_cast<BlinkTask*>(params)->run();
}

void BlinkTask::run() {
    for (;;) {
        HAL_GPIO_TogglePin(m_port, m_pin);
        //print Thermistor2 getTemperature w/ print_data_from_ext_thread
        // Non-blocking delay: allows other tasks to run
        vTaskDelay(pdMS_TO_TICKS(m_periodMs));
    }
}

void print_data_from_ext_thread(char* msg, int n, uint16_t raw_data){
	snprintf(msg,n,"BlinkTask raw: %u\r\n", raw_data);
	IO_Handler::uart_print(msg);
}
