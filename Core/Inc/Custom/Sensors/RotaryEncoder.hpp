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

class RotaryEncoder {
public:
	static RotaryEncoder& get_instance();

	void init(TIM_HandleTypeDef* htim);
	void start_thread(uint16_t stackSize, UBaseType_t priority);

	float get_angle_degrees();
	int32_t get_angle_centi_degrees();
	bool is_rail_aligned(uint8_t rail_index_zero_based, int32_t tolerance_cdeg);

	uint8_t get_rail_index_zero_based();
	int32_t get_counts();
	int8_t get_direction();

	void set_zero();

private:
	RotaryEncoder();

	static void runStatic(void* params);
	void task_loop();

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
