/*
 * RotaryEncoder.cpp
 *
 *  Created on: May 2, 2026
 *      Author: devhu
 */

#include "Custom/Sensors/RotaryEncoder.hpp"
#include "Custom/IO_Handler.hpp"

namespace {
static constexpr int32_t COUNTS_PER_REV = 4000; // 1000 PPR * 4
static constexpr uint8_t NUM_RAILS = 6;
constexpr uint32_t UPDATE_PERIOD_MS = 5;
}

RotaryEncoder::RotaryEncoder()
    : m_timer(nullptr),
      m_task_handle(nullptr),
      m_mutex(nullptr),
      m_total_counts(0),
      m_last_count(0) {
}

RotaryEncoder& RotaryEncoder::get_instance() {
    static RotaryEncoder instance;
    return instance;
}

void RotaryEncoder::init(TIM_HandleTypeDef* htim) {
    m_timer = htim;
    m_mutex = xSemaphoreCreateMutex();

    __HAL_TIM_SET_COUNTER(m_timer, 0);

    m_total_counts = 0;
    m_last_count = 0;

    HAL_TIM_Encoder_Start(m_timer, TIM_CHANNEL_ALL);
}

void RotaryEncoder::start_thread(uint16_t stackSize, UBaseType_t priority) {
    xTaskCreate(runStatic, "EncoderTask", stackSize, this, priority, &m_task_handle);
}

void RotaryEncoder::runStatic(void* params) { static_cast<RotaryEncoder*>(params)->task_loop(); }

void RotaryEncoder::task_loop() {
    while (true) {
    	int16_t current = (int16_t)__HAL_TIM_GET_COUNTER(m_timer);
		int16_t delta = current - m_last_count;

		if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
			m_total_counts += delta;
			m_last_count = current;
			xSemaphoreGive(m_mutex);
		}
		// Debug printing outside of mutex block
//		static uint32_t print_div = 0;
//		if (++print_div >= 50) {
//		    print_div = 0;
//
//		    int32_t angle_cdeg = get_angle_centi_degrees();
//		    char msg[48];
//		    snprintf(msg, sizeof(msg), "ENC cnt=%ld raw=%d angle=%ld.%02ld\r\n",
//		             (long)m_total_counts, current, (long)(angle_cdeg / 100),
//					 (long)(angle_cdeg % 100));
//
//		    IO_Handler::uart_print(msg);
//		}

        vTaskDelay(pdMS_TO_TICKS(UPDATE_PERIOD_MS)); // Put thread in Blocked state for other threads to run
    }
}

float RotaryEncoder::get_angle_degrees() {
    int32_t count_mod;

    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        count_mod = m_total_counts % COUNTS_PER_REV;
        xSemaphoreGive(m_mutex);
    }

    if (count_mod < 0) count_mod += COUNTS_PER_REV;

    return (float)count_mod * 360.0f / (float)COUNTS_PER_REV;
}

int32_t RotaryEncoder::get_angle_centi_degrees() {
    int32_t count_mod;

    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        count_mod = m_total_counts % COUNTS_PER_REV;
        xSemaphoreGive(m_mutex);
    }

    if (count_mod < 0) count_mod += COUNTS_PER_REV;

    return (count_mod * 36000L) / COUNTS_PER_REV;
}

bool RotaryEncoder::is_rail_aligned(uint8_t rail_index_zero_based, int32_t tol_cdeg) {
    if (rail_index_zero_based >= NUM_RAILS) return false;

    if (tol_cdeg < 0) tol_cdeg = -tol_cdeg;

    const int32_t full = 36000;
    const int32_t sector = full / NUM_RAILS;

    int32_t target = rail_index_zero_based * sector;
    int32_t current = get_angle_centi_degrees();

    int32_t error = current - target;
    if (error < 0) error = -error;
    if (error > full / 2) error = full - error;

    return error <= tol_cdeg;
}

uint8_t RotaryEncoder::get_rail_index_zero_based(void){
    int32_t angle_cdeg = get_angle_centi_degrees();

    /*
     * For 6 rail positions:
     * 360.00 degrees / 6 = 60.00 degrees per rail
     * In centi-degrees: 60.00 deg = 6000 centi-degrees
     */
    int32_t sector_size_cdeg = 36000L / NUM_RAILS;

    return (uint8_t)(angle_cdeg / sector_size_cdeg);
}

void RotaryEncoder::set_zero() { // Reset encoder counter
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
        __HAL_TIM_SET_COUNTER(m_timer, 0);
        m_total_counts = 0;
        m_last_count = 0;
        xSemaphoreGive(m_mutex);
    }
}
