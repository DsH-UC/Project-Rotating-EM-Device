/*
 * MotorDriver.hpp
 *
 *  Created on: May 2, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_MOTOR_MOTORDRIVER_HPP_
#define CUSTOM_MOTOR_MOTORDRIVER_HPP_

#include "stm32f4xx_hal.h"

class MotorDriver { // Modified OOP version of Boris' pwm code w/out the setSpeed function
public:
    void init(TIM_HandleTypeDef* htim, uint32_t channel,
    		  GPIO_TypeDef* ina_port, uint16_t ina_pin,
              GPIO_TypeDef* inb_port,uint16_t inb_pin);

    void forward(uint16_t duty);
    void reverse(uint16_t duty);
    void stop();

private:
    TIM_HandleTypeDef* m_htim; // PWM timer
    uint32_t m_channel;

    GPIO_TypeDef* m_ina_port; // Ports and pins
    uint16_t m_ina_pin;

    GPIO_TypeDef* m_inb_port;
    uint16_t m_inb_pin;

    uint16_t m_max_duty; // Duty cycle threshold

};

#endif /* CUSTOM_MOTOR_MOTORDRIVER_HPP_ */
