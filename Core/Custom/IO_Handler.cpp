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
	extern ADC_HandleTypeDef hadc1; // ADC handle

	// UI input interrupt callback override
   void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
		if (huart->Instance == UART4) { // HC-05 UART
			RotatingBlock::RotatingBlock::getInstance().UI_LaunchButtonISR();
			//ChargingStage::ChargingStage::get_instance().UI_LaunchButtonISR();
			CoolingSystem::CoolingSystem::get_instance().ui_fan_toggle_ISR();
		}
	}

   // ADC input callback. Set to interrupt only when the buffer is half full and completely full
   void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	   if(hadc->Instance == ADC1){

		   	 FaultDetector::FaultDetector::get_instance().chr_stg_fault_ISR();
		   	 FaultDetector::FaultDetector::get_instance().rb_fault_ISR();
		   	 FaultDetector::FaultDetector::get_instance().cool_sys_fault_ISR();
	   }
   }

   void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
   	   if(hadc->Instance == ADC1){


   	   }
   }

}

namespace IO_Handler {

	namespace Constants {
		constexpr int ADC_BUF_SIZE = 256; // Allocate some DMA buffer memory to the ADC.
	}

	IO_Handler& IO_Handler::get_instance(){
			static IO_Handler instance;
			return instance;
		}

	IO_Handler::IO_Handler() { printf("IO Handler interface online.\n"); }
	IO_Handler::~IO_Handler() { printf("IO Handler interface offline.\n"); }

}
