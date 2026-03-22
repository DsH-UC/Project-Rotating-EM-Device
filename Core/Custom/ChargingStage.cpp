/*
 * ChargingStage.cpp
 *
 *  Created on: Mar 13, 2026
 *      Author: devhu
 */

#include "ChargingStage.hpp"
#include "ThreadPriorities.hpp"

#define CAP_VOLTAGE_THRESHOLD_MIN 8// Voltage for a single shot
#define CAP_VOLTAGE_THRESHOLD_MAX 48 // Fully charged cap voltage

#define STATE_DISCHARGED 0
#define STATE_CHARGING 1
#define STATE_CHARGED 2

#define STACK SIZE 256
#define THREAD_BLOCK_DURATION_MS 50

#define NUM_RAIL_PAIRS 6

namespace ChargingStage {
	struct ChargingStageInputs {
		float capVoltage;
		bool uiLaunchButton;
		bool isMotorAligned;
		int gate_launch; // Consumed rail count due to high temperature
		bool reset;
	};

	struct ChargingStageGPIO {
		int pinCapVoltageDivider; //ADC Channel
	};

	// Store thread in cpp file
	static TaskHandle_t csTaskHandle = nullptr;
	static SemaphoreHandle_t uiLaunchButtonSemaphore = nullptr;

	// Charging Stage FSM
	// Inputs: Capacitor Bank Voltage Sensor (float), UI Launch Button (bool), Is Motor Aligned (bool),
	// 		   Reset / Discharge (bool), Gate Launch / Consumed Rail Count (int)

	// Outputs: Ready to Launch Payload (bool)

	// Helper read functions
	static float readSensor(int gpioPin){ //TODO
		// 12-bit ADC, mask w/ & 0xFFF
		return 1.0;
	}

	static bool readUI(int gpioPin){ //TODO
		return false;
	}

	static bool readRotatingBlock_isMotorAligned(void){ //TODO
		return false;
	}

	static int readRotatingBlock_numConsumedRails(void) { //TODO
		return 0;
	}

	// Thread instructions
	static void chargingStageTask(void* pvParameters){
		// Charging Stage FSM Implementation:
		int state = STATE_DISCHARGED; // Initial state

		while(true){

			if(xSemaphoreTake(uiLaunchButtonSemaphore,pdMS_TO_TICKS(THREAD_BLOCK_DURATION_MS)) == pdFALSE) return;

			ChargingStageInputs* chargingStageInputs = {
					readSensor(0),
					readUI(0),
					readRotatingBlock_isMotorAligned(),
					readRotatingBlock_numConsumedRails() ,
					false
			};

			// State transition boolean functions
			bool x0 = ((int)(chargingStageInputs->capVoltage)) >= CAP_VOLTAGE_THRESHOLD_MAX;
			bool x1 = ((int)chargingStageInputs->capVoltage) >= CAP_VOLTAGE_THRESHOLD_MIN;
			bool y = chargingStageInputs->uiLaunchButton && chargingStageInputs->gate_launch < NUM_RAIL_PAIRS;

			// Mutually exclusive state transitions from each state. No need for else statement
			if(state == STATE_DISCHARGED && !y) state = STATE_DISCHARGED;
			if(state == STATE_DISCHARGED && y) state = STATE_CHARGING;

			if(state == STATE_CHARGING && !x0) state = STATE_CHARGING;
			if(state == STATE_CHARGING && x0) state = STATE_CHARGED;

			if(state == STATE_CHARGED && x1) state = STATE_CHARGED;
			if(state == STATE_CHARGED && !x1) state = STATE_DISCHARGED;

			// Thread from Running state -> Blocked state
			vTaskDelay(pdMS_TO_TICKS(THREAD_BLOCK_DURATION_MS));

		}

	}

	static void UI_LaunchButtonISR(void* pvParameters){ // TODO: Still need to get the interrupt condition and clear it
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(uiLaunchButtonSemaphore, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	void startChargingStageSubsytemThread(void){

		uiLaunchButtonSemaphore = xSemaphoreCreateBinary();
		xTaskCreate(
				chargingStageTask, // Thread function
				"Charging Stage Thread",
				STACK_SIZE,
				nullptr, // pvParameters
				ThreadPriorites::CHARGING_STAGE_PRIORITY, // Thread Priority
				&csTaskHandle
		);
	}

}


