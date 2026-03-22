/*
 * CoolingSystem.cpp
 *
 *  Created on: Mar 12, 2026
 *      Author: devhu
 */

#include "CoolingSystem.hpp"
#include "ThreadPriorities.hpp"
#define STACK_SIZE 512 //bytes
#define COOLING_SYS_THRESHOLD 40 // Deg. Celsius
#define STATE_FAN_OFF false
#define STATE_FAN_ON true
#define THREAD_BLOCK_DURATION_MS 50

namespace CoolingSystem {
	struct CoolingSystemInputs {
		float railTemp;
		bool uiFanToggle;
		bool reset;
	};

	struct CoolingSystemGPIO {
		int pin_NTC_Thermistor; // ADC Channel
		int pin_HC_05_UART_RX;
	};

	// Store thread in cpp file
	static TaskHandle_t csTaskHandle = nullptr;
	static SemaphoreHandle_t uiFanToggleSemaphore = nullptr; // Semaphore given by ISR


	// Cooling System FSM
	// Inputs: Rail Temp Sensor (float), UI Fan Toggle (bool), Reset (bool)
	// Outputs: Is_Fan_On (bool)

	// Helper read functions
	static float readSensor(int gpioPin){ // TODO
		// 12-bit ADC, mask with & 0xFFF
		return 1.0;
	};

	static bool readUI(int gpioPin){ // TODO
		return false;
	}

	// Thread instructions
	static void coolingSystemTask(void* pvParameters){

		// Cooling System FSM Implementation:
		bool state = STATE_FAN_OFF; // Initial state

		while(true){
			if(xSemaphoreTake(uiFanToggleSemaphore, pdMS_TO_TICKS(THREAD_BLOCK_DURATION_MS))==pdFALSE) return;

			CoolingSystemInputs* coolingSystemInputs = {readSensor(0),readUI(0),false};

			// State transition boolean functions
			bool y = ((int)(coolingSystemInputs->railTemp) >= COOLING_SYSTEM_THRESHOLD) ||
					coolingSystem->uiFanToggle;

			bool z = ((int)(coolingSystemInputs->railTemp) < COOLING_SYSTEM_THRESHOLD) ||
								coolingSystem->uiFanToggle;

			// Mutually exclusive state transitions from both states. No need for else statement
			if(state == STATE_FAN_OFF && y) state = STATE_FAN_ON;
			if(state == STATE_FAN_OFF && !y) state = STATE_FAN_OFF;
			if(state == STATE_FAN_ON && z) state = STATE_FAN_OFF;
			if(state == STATE_FAN_ON && !z) state = STATE_FAN_ON;

			// Thread from Running state -> Blocked state for 50 ms
			vTaskDelay(pdMS_TO_TICKS(THREAD_BLOCK_DURATION_MS));

		}
	}

	// UI Fan Button ISR
	static void UI_FanToggleISR(void* pvParameters){ // TODO: Still need to get the interrupt condition and clear it

		BaseType_t xHigherPriorityTaskWoken = pdFalse;
		xSemaphoreGiveFromISR(uiFanToggleSemaphore, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	void startCoolingSubsystemThread(void) {
		uiFanToggleSemaphore = xSemaphoreCreateBinary();
		xTaskCreate(
				coolingSystemTask, // Thread function
				"Cooling System Thread",
				STACK_SIZE, //
				nullptr, // pvParameters
				ThreadPriorities::COOLING_SYS_PRIORITY, //Thread Priority
				&csTaskHandle
		);
	}
}


