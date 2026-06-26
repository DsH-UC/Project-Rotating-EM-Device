/*
 * IRBreakSensor.hpp
 *
 *  Created on: Apr 29, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_IRBREAKSENSOR_HPP_
#define CUSTOM_IRBREAKSENSOR_HPP_

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/*
 * Class: IRBreakSensor
 *
 * Description: The IRBreakSensor class is the software interface for two sets of IR break sensors used to sense
 * 				payload velocity concurrently. This class would implement software functionality for the Velocity Sensor
 * 				on the Master Block Diagram.
 */
class IRBreakSensor {
public:

	/*
	 * Function: get_instance
	 *
	 * Description: The get_instance function is the sole singleton accessor to the IRBreakSensor class.
	 *
	 * Outputs: The reference to an anonymous sole object instance of the IRBreakSensor class.
	 */
    static IRBreakSensor& get_instance();

    // Disable copying
    IRBreakSensor(const IRBreakSensor&) = delete;
    IRBreakSensor& operator=(const IRBreakSensor&) = delete;

    /*
     * Function: init_IR_break_thread
     *
     * Description: Initializes a single thread that executes the reading of and processing of IR break sensor beam
     * 				blocked events into a velocity.
     *
     * Parameters: stackSize - thread stack memory size allocation in number of words using heap memory.
     * 		   priority - thread execution priority.
     */
    void init_IR_break_thread(uint16_t stackSize, UBaseType_t priority);

    /*
     * Function: init
     *
     * Description: Initializes the IRBreakSensor class by assigning its GPIO pins and internal mutex and
     * producer/consumer queue
     *
     * Parameters: startPin - GPIO pin number for the first set of IR Break sensors the payload passes
     * 		   endPin - GPIO pin number for the second set of IR Break sensors the payload passes
     */
    void init(uint16_t startPin, uint16_t endPin);

    /*
     * Function: on_exti_callback
     *
     * Description: External interrupt handler function inserted into external interrupt callback for when a
     * 				GPIO pin for an IR Break sensor set detects an edge.
     *
     * Parameters: GPIO_pin - GPIO pin number for a single set of IR Break sensors.
     */
    void on_exti_callback(uint16_t GPIO_Pin);

    /*
     * Function: task_loop
     *
     * Description: Wrapper for the block of code to be executed by the class thread within a while loop. Every
     * 				loop execution ends with the blocking of the thread for a limited duration. The thread loop for
     * 				this class dequeues a producer/consumer event queue where blocked beam events come with an
     * 				event type (start or end beam type) and timestamp to track when both beams have been blocked.
     */
    void task_loop();

    /*
     * Function: get_elapsed_ms
     *
     * Description: Computes the elapsed time between the blocking of the start beam from one set of IR break sensors
     * 				and the end beam from the other IT break sensor.
     *
     * Outputs: 0 ms when both beams have not been blocked. Otherwise returns the timestamp difference between when
     * 			the start and the end beams were blocked.
     */
    uint32_t get_elapsed_ms() const;

    /*
     * Function: get_velocity
     *
     * Description: Uses the length of the payload track with the elapsed time between both blocked beam events to
     * 				compute and return a velocity.
     *
     * Outputs: 0 m/s when both beams have not been blocked yet. Otherwise returns the velocity of the payload.
     */
    float get_velocity();

    /*
     * Function: reset
     *
     * Description: Resets the tracking logic determining when the start and end beams were both blocked within the
     * thread loop in that order.
     */
    void reset();

private:
    enum class IRBreakEventType { // Classification categories representing which sensor beam was broken.
        START,
        END
    };

    struct IRBreakEvent { // Event and time stamp
        IRBreakEventType event_type;
        TickType_t tick;
    };

    IRBreakSensor();
    // Thread handle
    TaskHandle_t m_ir_break_task_handle;
    SemaphoreHandle_t m_mutex;

    // GPIO ports and pins
    uint16_t m_start_pin;
    uint16_t m_end_pin;

    // Event queue
    QueueHandle_t m_event_queue;

    // Start and end times
    TickType_t m_start_tick;
    TickType_t m_end_tick;

    // Start and end flags
    bool m_is_started;
    bool m_is_finished;

    // C wrapper around C++ task_loop function
    static void taskEntry(void* argument);
};

#endif /* CUSTOM_IRBREAKSENSOR_HPP_ */
