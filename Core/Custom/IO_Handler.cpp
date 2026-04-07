/*
 * IO_Handler.cpp
 *
 *  Created on: Apr 6, 2026
 *      Author: devhu
 */

#include "IO_Handler.hpp"
#include "RotatingBlock.hpp"
#include "FaultDetector.hpp"
#include "ChargingStage.hpp"
#include "CoolingSystem.hpp"

extern "C" {
	extern UART_HandleTypeDef huart4; // UI buttons UART handle

	// UI input interrupt callback override
   void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
		if (huart->Instance == UART4) { // HC-05 UART
			RotatingBlock::RotatingBlock::getInstance().UI_LaunchButtonISR();
			//ChargingStage::ChargingStage::get_instance().UI_LaunchButtonISR();
			CoolingSystem::CoolingSystem::get_instance().ui_fan_toggle_ISR();

		}
	}

   void HAL

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

}
