/*
 * CoolingSystem.hpp
 *
 *  Created on: Mar 12, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_COOLINGSYSTEM_HPP_
#define CUSTOM_COOLINGSYSTEM_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// Interface file for the Cooling System Thread from the
// SW Architecture Block Diagram
namespace CoolingSystem {

	// Create Cooling System Thread in FreeRTOS
	void startCoolingSubsystemThread(void);
}

#endif /* CUSTOM_COOLINGSYSTEM_HPP_ */
