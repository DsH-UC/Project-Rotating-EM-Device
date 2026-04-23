/*
 * NVIC_PriorityTable.hpp
 *
 *  Created on: Mar 22, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_NVIC_PRIORITYTABLE_HPP_
#define CUSTOM_NVIC_PRIORITYTABLE_HPP_

namespace NVIC_PriorityTable {

	// Fault Detector main subsystem interrupt sources
	inline constexpr int FAULT_DETECTOR_CHR_STG_GPRIORITY = 0;
	inline constexpr int FAULT_DETECTOR_CHR_STG_SPRIORITY = 0;

	inline constexpr int FAULT_DETECTOR_RB_GPRIORITY = 0;
	inline constexpr int FAULT_DETECTOR_RB_SPRIORITY = 1;

	inline constexpr int FAULT_DETECTOR_COOL_SYS_GPRIORITY = 0;
	inline constexpr int FAULT_DETECTOR_COOL_SYS_SPRIORITY = 2;

	// UI input interrupt sources
	inline constexpr int UI_FAN_BTN_PRESSED_GPRIORITY = 2;
	inline constexpr int UI_FAN_BTN_PRESSED_SPRIORITY = 0;

	inline constexpr int UI_LAUNCH_BTN_PRESSED_GPRIORITY = 1;
	inline constexpr int UI_LAUNCH_BTN_PRESSED_SPRIORITY = 0;


}

#endif /* CUSTOM_NVIC_PRIORITYTABLE_HPP_ */
