/*
 * ChargingStage.hpp
 *
 *  Created on: Mar 13, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_CHARGINGSTAGE_HPP_
#define CUSTOM_CHARGINGSTAGE_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// Interface file for the Charging Stage Thread from the
// SW Architecture Block Diagram
namespace ChargingStage {

	// Create Charging Stage Thread in FreeRTOS
	void startChargingStageSubsytemThread(void);
}


#endif /* CUSTOM_CHARGINGSTAGE_HPP_ */
