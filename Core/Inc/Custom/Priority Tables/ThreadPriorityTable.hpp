/*
 * ThreadPriorities.hpp
 *
 *  Created on: Mar 13, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_THREADPRIORITYTABLE_HPP_
#define CUSTOM_THREADPRIORITYTABLE_HPP_

namespace ThreadPriorityTable {

	constexpr int FAULT_DETECTOR_PRIORITY = 0;
	constexpr int COOLING_SYS_PRIORITY = 3;
	constexpr int CHARGING_STAGE_PRIORITY = 1;
	constexpr int ROTATING_BLOCK_PRIORITY = 2;
	constexpr int SENS_STREAM_RD_PRIORITY = 4;
	constexpr int SENS_STREAM_WR_PRIORITY = 4;

}

#endif /* CUSTOM_THREADPRIORITYTABLE_HPP_ */
