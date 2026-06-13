/*
 * IO_Handler.cpp
 *
 *  Created on: Apr 25, 2026
 *      Author: devhu
 */

#include "Custom/IO_Handler.hpp"
#include "Custom/Sensors/IRBreakSensor.hpp"
#include "Custom/CoolingSystem.hpp"
#include "Custom/Remote GUI/UI_CommandReceiver.hpp"


extern "C" {
	void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
		IRBreakSensor::get_instance().on_exti_callback(GPIO_Pin);
		CoolingSystem::CoolingSystem::get_instance().ui_fan_toggle_ISR(GPIO_Pin);

	}

	void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
	    UI_CommandReceiver::get_instance().on_uart_rx_callback(huart);
	}
}

static SemaphoreHandle_t uart_mutex = nullptr;

void IO_Handler::init() {
	uart_mutex = xSemaphoreCreateMutex();
}

void IO_Handler::uart_print(const char* msg) {
	    if (uart_mutex == nullptr) return;

	    if (xSemaphoreTake(uart_mutex, portMAX_DELAY) == pdTRUE) {
	        HAL_UART_Transmit(&huart2,
	                          reinterpret_cast<uint8_t*>(const_cast<char*>(msg)),
	                          strlen(msg),
	                          HAL_MAX_DELAY);

	        xSemaphoreGive(uart_mutex);
	    }
	}


