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

	// Interrupt callback function overrides
	// UI input CPU interrupt callback
   void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
		if (huart->Instance == UART4) { // HC-05 UART
			RotatingBlock::RotatingBlock::getInstance().UI_LaunchButtonISR();
			//ChargingStage::ChargingStage::get_instance().UI_LaunchButtonISR();
			CoolingSystem::CoolingSystem::get_instance().ui_fan_toggle_ISR();
		}
	}

   // Bluetooth UART packet transmission DMA interrupt callback on a complete transmission
   void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	   if (huart->Instance == UART4) {
		   printf("Sent Bluetooth packet.\n");
	   }
   }

   // ADC input callback. Set to DMA interrupt only when the buffer is half full and completely full; Half transfer interrupt
   // These callbacks are for the capacitor voltage, fault detector voltages, and thermistor voltages.
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

   // GPIO read callback to CPU interrupt
   void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	   if(GPIO_pin == IO_Handler::Constants::DUMMY_INT){

	   }
   }

}

namespace IO_Handler {

	namespace Constants {
		constexpr int DUMMY_INT = 36;
		constexpr int ADC_BUF_SIZE = 256; // Allocate some DMA buffer memory to the ADC.
		constexpr uint16_t BLUETOOTH_PACKET_SIZE = 512; //bytes
		constexpr int STREAMING_PIPELINE_QUEUE_SIZE = 512;
		constexpr int STREAMING_PIPELINE_STACK_SIZE = 256;
	}

	IO_Handler& IO_Handler::get_instance(){
			static IO_Handler instance;
			return instance;
		}

	IO_Handler::IO_Handler() {
		init_adc();
		printf("IO Handler interface online.\n");
	}

	IO_Handler::~IO_Handler() { printf("IO Handler interface offline.\n"); }

	void IO_Handler::send_bluetooth_packet(UART_HandleTypeDef* huart) {

		uint8_t* packet;
		uint8_t header[] = "TODO";

		for(int i = 0; i < (int) Constants::BLUETOOTH_PACKET_SIZE; i++){

		}

		// Send packet to HC-05 Bluetooth UART module
		//HAL_UART_Transmit_DMA(huart, packet, Constants::BLUETOOTH_PACKET_SIZE);

		// Enqueue the packet queue and transmit on deqeue

	}

	void IO_Handler::init_adc(void) {
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)(this->m_adc_buffer), Constants::ADC_BUF_SIZE);
	}

	// Sensor Streaming Pipeline functions
	void IO_Handler::start_packet_rdwr_threads(void) {
		this->m_packet_queue = xQueueCreate(Constants::STREAMING_PIPELINE_QUEUE_SIZE,sizeof(uint8_t));

		xTaskCreate(
				packet_queue_write_task,
				"Streaming Pipeline Writer",
				Constants::STREAMING_PIPELINE_STACK_SIZE,
				&(this->m_packet_queue),
				ThreadPriorityTable::SENS_STREAM_WR_PRIORITY,
				&(this->packet_queue_write_task_handle)
		);

		xTaskCreate(
				packet_queue_read_task,
				"Streaming Pipeline Reader",
				Constants::STREAMING_PIPELINE_STACK_SIZE,
				&(this->m_packet_queue),
				ThreadPriorityTable::SENS_STREAM_RD_PRIORITY,
				&(this->packet_queue_read_task_handle)
		);

	}

	void IO_Handler::packet_queue_write_task(void* pv_parameters){
		QueueHandle_t* packet_queue = (QueueHandle_t*) pv_parameters;

	}

	void IO_Handler::packet_queue_read_task(void* pv_parameters){
		QueueHandle_t* packet_queue = (QueueHandle_t*) pv_parameters;

	}

	void compress_packet(void* packet_data){

	}
}
