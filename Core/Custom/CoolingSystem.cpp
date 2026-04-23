/*
 * CoolingSystem.cpp
 *
 *  Created on: Mar 12, 2026
 *      Author: devhu
 */

#include "CoolingSystem.hpp"
#include "ThreadPriorityTable.hpp"

namespace CoolingSystem {

	// Internal namespace for compile time constants
	namespace Constants {
		constexpr int STACK_SIZE = 512; // bytes
		constexpr int COOLING_SYS_THRESHOLD = 40; // Deg. Celsius
		constexpr bool STATE_FAN_OFF = false;
		constexpr bool STATE_FAN_ON = true;
		constexpr int THREAD_BLOCK_DURATION_MS = 50;
	}

	static CoolingSystem& CoolingSystem::get_instance(){
		static CoolingSystem instance;
		return instance;
	}

	CoolingSystem::CoolingSystem() { printf("Cooling System Subsystem online.\n"); }
	CoolingSystem::~CoolingSystem() { printf("Cooling System Subsystem offline.\n"); }

	// Helper read functions
	float CoolingSystem::read_sensor(int gpio_pin){ // TODO
		// 12-bit ADC, mask with & 0xFFF
		return Thermistor_ReadTempC(uint32_t adc_channel, uint16_t samples);
		//return 1.0;
	};

	bool CoolingSystem::read_ui(int gpio_pin){ // TODO
		return false;
	}

	// Thread instructions
	void CoolingSystem::cooling_system_task(void* pv_parameters){

		// Cooling System FSM Implementation:
		bool state = Constants::STATE_FAN_OFF; // Initial state

		while(true){
			if(xSemaphoreTake(this->ui_fan_toggle_semaphore, pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS))==pdFALSE) return;

			this->m_cooling_system_inputs = {read_sensor(0),read_ui(0),false};

			// State transition boolean functions
			bool y = ((int)(this->m_cooling_system_inputs->rail_temp) >= Constants::COOLING_SYSTEM_THRESHOLD) ||
					this->m_cooling_system_inputs->ui_fan_toggle;

			bool z = ((int)(this->m_cooling_system_inputs->rail_temp) < Constants::COOLING_SYSTEM_THRESHOLD) ||
								this->m_cooling_system_inputs->ui_fan_toggle;

			// Mutually exclusive state transitions from both states. No need for else statement
			if(state == Constants::STATE_FAN_OFF && y) state = Constants::STATE_FAN_ON;
			if(state == Constants::STATE_FAN_OFF && !y) state = Constants::STATE_FAN_OFF;
			if(state == Constants::STATE_FAN_ON && z) state = Constants::STATE_FAN_OFF;
			if(state == Constants::STATE_FAN_ON && !z) state = Constants::STATE_FAN_ON;

			// Thread from Running state -> Blocked state for 50 ms
			vTaskDelay(pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS));

		}
	}

	// UI Fan Button ISR
	void CoolingSystem::ui_fan_toggle_ISR(void* pv_parameters){ // TODO: Still need to get the interrupt condition and clear it

		BaseType_t xHigherPriorityTaskWoken = pdFalse;

		// Preepmt thread from Running State -> Ready state
		xSemaphoreGiveFromISR(ui_fan_toggle_semaphore, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	void CoolingSystem::start_cooling_subsystem_thread(void) {
		this->ui_fan_toggle_semaphore = xSemaphoreCreateBinary();
		xTaskCreate(
				cooling_system_task, // Thread function
				"Cooling System Thread",
				Constants::STACK_SIZE, //
				nullptr, // pvParameters
				ThreadPriorityTable::COOLING_SYS_PRIORITY, //Thread Priority
				&(this->cs_task_handle)
		);
	}
}


