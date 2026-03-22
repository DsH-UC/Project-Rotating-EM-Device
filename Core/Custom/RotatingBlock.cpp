/*
 * RotatingBlock.cpp
 *
 *  Created on: Mar 21, 2026
 *      Author: devhu
 */

#include "RotatingBlock.hpp"
#include <cstdio>
#include <cmath>

#define STACK_SIZE 512 // bytes
#define THREAD_BLOCK_DURATION_MS 50
#define STATE_IDLE false
#define STATE_ROTATING true
#define COOLING_SYS_THRESHOLD 40 // Deg. Celsius
#define NUM_TOTAL_RAILS 6
#define ROTARY_ENC_REF_ANGLE 90.0f
#define EPSILON 1e-4f

namespace RotatingBlock {

	// Internal namespace for compile time constants
	namespace Constants {

	}

	RotatingBlock& RotatingBlock::getInstance(){
		static RotatingBlock instance;
		return instance;
	}

	RotatingBlock::RotatingBlock() { printf("RB Subsystem online.\n"); }
	RotatingBlock::~RotatingBlock() { printf("RB Subsystem offline.\n"); }

	float RotatingBlock::readTempSensor(int gpioPin){ // TODO
		return 0.0;
	}

	float RotatingBlock::readRotaryEncoder(int gpioPin) { // TODO
		return 0.0;
	}

	bool RotatingBlock::readUI(int gpioPin) { // TODO
		return false;
	}

	void RotatingBlock::rotatingBlockTask(void* pvParameters) {
		// Rotating Block FSM Implementation
		bool state = STATE_IDLE; // Initial state

		while(true) {
			if(xSemaphoreTake(this->uiLaunchButtonSemaphore, pdMS_TO_TICKS(THREAD_BLOCK_DURATION_MS)) == pdFALSE) return;

			this->rbInputs = {readTempSensor(0), readRotaryEncoder(0), readUI(0), false};

			// State transition boolean functions
			bool rotateMotor = this->rbInputs.uiLaunchButton &&
							   ((int) (this->rbInputs.railTemp) >= COOLING_SYS_THRESHOLD) &&
							   this->numUsedRails < NUM_TOTAL_RAILS;

			bool isMotorAligned = std::abs(rbInputs.rotaryEncoderAngle - ROTARY_ENC_REF_ANGLE) <= EPSILON;

			// Mutually exclusive state transitions between both states. No need for else statements.
			if(state == STATE_IDLE && !rotateMotor) state = STATE_IDLE;
			if(state == STATE_IDLE && rotateMotor) state = STATE_ROTATING;
			if(state == STATE_ROTATING && !isMotorAligned) state = STATE_ROTATING;
			if(state == STATE_ROTATING && isMotorAligned) state = STATE_IDLE;

			// Thread from Running state -> Blocked state for 50 ms
			vTaskDelay(pdMS_TO_TICKS(THREAD_BLOCK_DURATION_MS));
		}

	}

	void RotatingBlock::UI_LaunchButtonISR(void* pvParameters){ // TODO: Still need to get the interrupt conditional/flag wrapped around this block

		BaseType_t xHigherPriorityTaskwoken = pdFALSE;

		// Preepmt thread from Running State -> Ready state
		xSemaphoreGiveFromISR(this->uiLaunchButtonSemaphore, &xHigherPriorityTaskwoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskwoken);
	}

	bool RotatingBlock::toggleMotorPWM(bool enPWM) { // TODO
m		int pwm = this->rbGPIO.pin_Motor_IGBT_Gate;
		return false;
	}
	void RotatingBlock::incrementRailCounter(bool enRailCounter) {

		if(enRailCounter) this->numUsedRails++;
		return enRailCounter;
	}

	void RotatingBlock::startRotatingBlockSubsystemThread(void){
		uiLaunchButtonSemaphore = xSemaphoreCreateBinary();

		xTaskCreate(
				RotatingBlock::rotatingBlockTask,
				"Rotating Block Thread",
				STACK_SIZE,
				nullptr,
				ThreadPriorities::ROTATING_BLOCK_PRIORITY,
				&(this->rbTaskHandle)
		);
	}
}


