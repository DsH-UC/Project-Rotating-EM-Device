/*
 * MotorDriver.hpp
 *
 *  Created on: May 2, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_MOTOR_MOTORDRIVER_HPP_
#define CUSTOM_MOTOR_MOTORDRIVER_HPP_

#include "stm32f4xx_hal.h"

/*
 * Class: MotorDriver
 * Description: This class serves as the software interface between a single motor driver board to
 * the STM32F446RE. It signals the motor driver board to rotate in both directions at a specified PWM duty cycle
 * or to stop.
 */
class MotorDriver { // Modified OOP version of Boris' pwm code w/out the setSpeed function
public:

	/*
	 * Function: init
	 *
	 * Description: This function initializes the MotorDriver class by setting the PWM timer and
	 * 				timer channel in addition to GPIO pins for a single motor driver board.
	 *
	 * Parameters: htim - Hardware timer object used to generate motor driver PWM.
	 * 			   channel - PWM timer channel number.
	 * 			   ina_port - GPIO port for the motor driver board's A side pin.
	 * 			   inb_port - GPIO port for the motor driver board's B side pin.
	 * 			   ina_pin - GPIO pin number for the motor driver board's A side pin.
	 * 			   inb_pin - GPIO pin number for the motor driver board's B side pin.
	 */
    void init(TIM_HandleTypeDef* htim, uint32_t channel,
    		  GPIO_TypeDef* ina_port, uint16_t ina_pin,
              GPIO_TypeDef* inb_port,uint16_t inb_pin);

    /*
     * Function: forward
     *
     * Description: This function signals the motor driver board to drive the motor in the forward
     * 				direction clockwise.
     *
     * Parameters: duty - Duty cycle value to set the PWM pulse width in timer ticks.
     */
    void forward(uint16_t duty);

    /*
     * Function: reverse
     *
     * Description: This function signals the motor driver board to drive the motor in the reverse
     * 				direction counterclockwise.
     *
     * Parameters: duty - Duty cycle value to set the PWM pulse width in timer ticks.
     */
    void reverse(uint16_t duty);

    /*
     * Function: stop
     *
     * Description: This function signals the motor driver board to stop motor rotation.
     */
    void stop();

private:
    TIM_HandleTypeDef* m_htim; // PWM timer
    uint32_t m_channel; // PWM timer channel

    // GPIO Ports and pins
    GPIO_TypeDef* m_ina_port;
    uint16_t m_ina_pin;

    GPIO_TypeDef* m_inb_port;
    uint16_t m_inb_pin;

    uint16_t m_max_duty; // Max duty cycle threshold

};

#endif /* CUSTOM_MOTOR_MOTORDRIVER_HPP_ */
