/*
 * RotaryEncoder.hpp
 *
 *  Created on: May 2, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_ROTARYENCODER_HPP_
#define CUSTOM_ROTARYENCODER_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32f4xx_hal.h"
#include <cstdint>

/*
 * Class: RotaryEncoder
 *
 * Description: The RotaryEncoder class acts as the software interface to the rotary encoder sensing the
 * 				angle of the Rotating Block Subsystem's primary motor, which is responsible for rotating
 * 				the barrel of rail pairs.
 */
class RotaryEncoder {
public:
	/*
	 * Function: get_instance
	 *
	 * Description: The get_instance function is the sole singleton accessor to the RotaryEncoder class.
	 *
	 * Outputs: The reference to an anonymous sole object instance of the RotaryEncoder class.
	 */
	static RotaryEncoder& get_instance();

	/*
	 * Function: init
	 *
	 * Description: Initializes the RotaryEncoder class by assigning a mutex, a timer for counting angles, starting
	 * the timer, and setting the internal counter variables to zero.
	 *
	 * Parameters: htim - The timer object used to track encoder angle counts in a 16-bit resolution.
	 */
	void init(TIM_HandleTypeDef* htim);

	/*
	 * Function: start_thread
	 *
	 * Description: Initializes a single thread that executes the reading of and processing of rotary encoder angle
	 * 				into a rail alignment status boolean.
	 *
	 * Parameters: stackSize - thread stack memory size allocation in number of words using heap memory.
     * 		   	   priority - thread execution priority.
	 */
	void start_thread(uint16_t stackSize, UBaseType_t priority);

	/*
	 * Function: get_angle_degrees
	 *
	 * Description: Converts the encoder angle from raw counts into degrees.
	 *
	 * Outputs: The encoder timer count number as degrees.
	 */
	float get_angle_degrees();

	/*
	 * Function: get_angle_centi_degrees
	 *
	 * Description: Converts the encoder angle from raw counts into centidegrees.
	 *
	 * Outputs: The encoder timer count number as centidegrees.
	 */
	int32_t get_angle_centi_degrees();

	/*
	 * Function: is_rail_aligned
	 *
	 * Description: Uses the encoder angle to determine whether the rails are aligned against a fixed reference
	 * 				with a degree margin of error to a set tolerance.
	 *
	 * Parameters: rail_index_zero_based - Boolean for setting whether the alignment calculation is based on either
	 * 									   zero indexed or one indexed rails.
	 * 			   tolerance_cdeg - Acceptable margin of error for alignment consideration in centidegrees.
	 *
	 * Outputs: boolean for whether the active rail is aligned for payload launch.
	 */
	bool is_rail_aligned(uint8_t rail_index_zero_based, int32_t tolerance_cdeg);

	/*
	 * Function: get_rail_index_zero_based
	 *
	 * Description: Calculates the zero based position index of the active rail based on rotary encoder angle.
	 *
	 * Outputs: Returns the zero based index of the current rail.
	 */
	uint8_t get_rail_index_zero_based();

	/*
	 * Function: get_counts
	 *
	 * Description: Reads the rotary encoder timer count from the timer's register.
	 *
	 * Outputs: Returns the rotary encoder counter value to a 16-bit resolution.
	 */
	int32_t get_counts();

	/*
	 * Function: set_zero
	 *
	 * Description: Resets the encoder angle to zero it.
	 */
	void set_zero();

private:
	RotaryEncoder();

	static void runStatic(void* params); // C wrapper around C++ task_loop function
	void task_loop(); // Thread loop code block

	TIM_HandleTypeDef* m_timer;
	TaskHandle_t m_task_handle;
	SemaphoreHandle_t m_mutex;

	int32_t m_total_counts;
	int16_t m_last_count;

	// Disable copying
	RotaryEncoder(const RotaryEncoder&) = delete;
	RotaryEncoder& operator=(const RotaryEncoder&) = delete;
};

#endif /* CUSTOM_ROTARYENCODER_HPP_ */
