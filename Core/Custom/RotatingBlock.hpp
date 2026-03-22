/*
 * RotatingBlock.hpp
 *
 *  Created on: Mar 13, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_ROTATINGBLOCK_HPP_
#define CUSTOM_ROTATINGBLOCK_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ThreadPriorities.hpp"

// Interface file for the Rotating Block Thread from the
// SW Architecture Block Diagram
namespace RotatingBlock {

	class RotatingBlock {
	public:
		// Access function for singleton
		static RotatingBlock& getInstance();

		// Disable copying
		RotatingBlock(const Singleton&) = delete;
		RotatingBlock& operator = (const RotatingBlock&) = delete;
		RotatingBlock(RotatingBlock&&) = delete;
		RotatingBlock& operator = (RotatingBlock&&) = delete;

		// Create Cooling System Thread in FreeRTOS
		void startRotatingBlockSubsystemThread(void);

	private:
		struct RotatingBlockInputs {
			float railTemp;
			float rotaryEncoderAngle;
			bool uiLaunchButton;
			bool reset;
		};

		struct RotatingBlockGPIO {
			int pin_NTC_Thermistor; // ADC Channel
			int pin_Rotary_Encoder; // May need multiple depending on serial comms protocol
			int pin_HC_05_UART_RX;
			int pin_Motor_IGBT_Gate;
		};

		int numUsedRails; // Number of currently hot rails. Dependent input
		RotatingBlockInputs rbInputs;
		RotatingBlockGPIO rbGPIO;

		// Store thread in cpp file
		TaskHandle_t rbTaskHandle;
		SemaphoreHandle_t uiLaunchButtonSemaphore;

		RotatingBlock();
		~RotatingBlock();

		// Main SM block
		float readTempSensor(int gpioPin); // TODO
		float readRotaryEncoder(int gpioPin);
		bool readUI(int gpioPin);
		void rotatingBlockTask(void* pvParameters);
		void UI_LaunchButtonISR(void* pvParameters);

		// Helper blocks
		bool toggleMotorPWM(bool enPWM); // TODO
		void incrementRailCounter(bool enRailCounter); // TODO

	};
}

#endif /* CUSTOM_ROTATINGBLOCK_HPP_ */
