/*
 * FaultDetector.hpp
 *
 *  Created on: Mar 21, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_FAULTDETECTOR_HPP_
#define CUSTOM_FAULTDETECTOR_HPP_

#include "FreeRTOS.h"
#include "task.h"

namespace FaultDetector { // TODO
	class FaultDetector {
	public:
		// Access function for singleton
		static FaultDetector& getInstance();

		// Disable copying
		FaultDetector(const Singleton&) = delete;
		FaultDetector& operator = (const FaultDetector&) = delete;
		FaultDetector(FaultDetector&&) = delete;
		FaultDetector& operator = (FaultDetector&&) = delete;

		bool resetAllSubsystemStateMachines(bool* isFault);
	private:
		// Power pins voltage divider inputs
		struct FaultDetectorInputs {
			float chargingStageVoltage;
			float rotatingBlockVoltage;
			float coolingSystemVoltage;
		};

		struct FaultDetectorGPIO {
			int pin_ChargingStagePWR_Voltage;
			int pin_RotatingBlockPWR_Voltage;
			int pin_CoolingSystemPWR_Voltage;
		}

		FaultDetectorInputs fdInputs;
		FaultDetectorGPIO fdGPIO;

		bool isFault;

		// Store thread in cpp file
		TaskHandle_t fdTaskHandle;
		SemaphoreHandle_t fdSemaphores[3];

		float readSensors(int* sensorPins, int numSensors);
		void startFaultDetectorSubsystemThread(void);
	};

}

#endif /* CUSTOM_FAULTDETECTOR_HPP_ */
