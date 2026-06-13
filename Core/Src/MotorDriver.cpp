/*
 * MotorDriver.cpp
 *
 *  Created on: May 2, 2026
 *      Author: devhu
 */

#include "MotorDriver.hpp"

void MotorDriver::init(TIM_HandleTypeDef* htim, uint32_t channel,
                       GPIO_TypeDef* ina_port, uint16_t ina_pin,
                       GPIO_TypeDef* inb_port, uint16_t inb_pin) {
    m_htim = htim;
    m_channel = channel;
    m_ina_port = ina_port;
    m_ina_pin = ina_pin;
    m_inb_port = inb_port;
    m_inb_pin = inb_pin;

    m_max_duty = __HAL_TIM_GET_AUTORELOAD(htim);

    HAL_TIM_PWM_Start(m_htim, m_channel);

    this->stop();
}

void MotorDriver::forward(uint16_t duty) {
    if (duty > m_max_duty) duty = m_max_duty; // Guard against above max duty cycle

    HAL_GPIO_WritePin(m_ina_port, m_ina_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(m_inb_port, m_inb_pin, GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(m_htim, m_channel, duty); // Set speed
}

void MotorDriver::reverse(uint16_t duty) {
    if (duty > m_max_duty) duty = m_max_duty; // Guard against above max duty cycle

    HAL_GPIO_WritePin(m_ina_port, m_ina_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(m_inb_port, m_inb_pin, GPIO_PIN_SET);

    __HAL_TIM_SET_COMPARE(m_htim, m_channel, duty); // Set speed
}

void MotorDriver::stop() { __HAL_TIM_SET_COMPARE(m_htim, m_channel, 0); } // Speed 0
