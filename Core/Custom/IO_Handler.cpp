/*
 * IO_Handler.cpp
 *
 *  Created on: Apr 6, 2026
 *      Author: devhu
 */

#include "IO_Handler.hpp"

extern "C" {
	extern UART_HandleTypeDef huart4; // Launch button UART handle
	extern UART_HandleTypeDef huart1; // Fan button UART handle
	extern UART_HandleTypeDef huart2; //

	// UI input interrupt callback override
   void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
		if (huart->Instance == UART4) {


			// Restart interrupt
			HAL_UART_Receive_IT(&huart2, &global_rx_byte, 1);



		}
	}

}

namespace IO_Handler {

	namespace Constants {

	}

	IO_Handler& IO_Handler::get_instance(){
			static IO_Handler instance;
			return instance;
		}

	IO_Handler::IO_Handler() { printf("IO Handler interface online.\n"); }
	IO_Handler::~IO_Handler() { printf("IO Handler interface offline.\n"); }

	bool IO_Handler::read_UI_launch_button(){
		// Listen for a byte, then trigger interrupt for non-blocking read
		HAL_UART_Receive_IT(&huart4, this->m_is__buttons_pressed_rx, sizeof(uint8_t));
	}

	bool IO_Handler::read_UI_fan_toggle_button(){
		// Listen for a byte, then trigger interrupt for non-blocking read
		HAL_UART_Receive_IT(&huart4, this->m_is_buttons_pressed_rx, sizeof(uint8_t));
	}



}
