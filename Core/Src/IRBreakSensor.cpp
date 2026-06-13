/*
 * IRBreakSensor.cpp
 *
 *  Created on: Apr 29, 2026
 *      Author: devhu
 */

#include "Custom/Sensors/IRBreakSensor.hpp"
#include "Custom/IO_Handler.hpp"

#define METERS_BETWEEN_SENSORS 0.43f

// Constructor
IRBreakSensor::IRBreakSensor()
    : m_mutex(nullptr),
      m_start_pin(0),
      m_end_pin(0),
      m_event_queue(nullptr),
      m_start_tick(0),
      m_end_tick(0),
      m_is_started(false),
      m_is_finished(false)
{
}

// Singleton instance
IRBreakSensor& IRBreakSensor::get_instance(){
    static IRBreakSensor instance;
    return instance;
}

void IRBreakSensor::init_IR_break_thread(uint16_t stackSize, UBaseType_t priority){
    xTaskCreate(
    		IRBreakSensor::taskEntry,
			"IRBreakTask",
			stackSize,
			this,
			priority,
			&m_ir_break_task_handle
	);
}

void IRBreakSensor::taskEntry(void* obj){
    static_cast<IRBreakSensor*>(obj)->task_loop();
}

// Initialization
void IRBreakSensor::init(uint16_t startPin, uint16_t endPin) {
    m_start_pin  = startPin;
    m_end_pin  = endPin;

    // Create queue for events
    m_event_queue = xQueueCreate(10, sizeof(IRBreakEvent));
    m_mutex = xSemaphoreCreateMutex();

    reset();
}

// EXTI interrupt handler for HAL_GPIO_EXTI_Callback
void IRBreakSensor::on_exti_callback(uint16_t GPIO_Pin){
	// Interrupt on a detected edge from either the start or ending IR sensor.
	// the callback then determines the type of event from this, records the time stamp,
	// and enqueues the event into

    if (m_event_queue == nullptr) return;

    IRBreakEvent event;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Determine which sensor triggered
    if (GPIO_Pin == m_start_pin) event.event_type = IRBreakEventType::START;
    else if (GPIO_Pin == m_end_pin) event.event_type = IRBreakEventType::END;
    else return; // Neither the start of the end sensor pins


    // Time stamp from ISR-safe tick
    event.tick = xTaskGetTickCountFromISR();

    // Enqueue event to event queue
    xQueueSendFromISR(m_event_queue, &event, &xHigherPriorityTaskWoken);

    // Context switch if needed
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

//Blocking and non polling FreeRTOS task loop
void IRBreakSensor::task_loop(){
    IRBreakEvent event; // Single slot dequeued event storage

    while (true) { // Process the event queue
//    	if (xQueueReceive(m_event_queue, &event, pdMS_TO_TICKS(1000)) == pdTRUE) {
//    	    IO_Handler::uart_print("IR event received\r\n");
//    	} else {
//    	    IO_Handler::uart_print("IR task alive, no event\r\n");
//    	}
    	// On a dequeuing of the event queue, determine conditions for
    	// when object crosses both sensors, and record the time stamps of the object
    	// entering and leaving to later get elapsed time
        if (xQueueReceive(m_event_queue, &event, portMAX_DELAY) == pdTRUE) {

            // Start event
            if (event.event_type == IRBreakEventType::START) {
            	IO_Handler::uart_print("IR START\r\n");
//                if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
//                	if(!m_is_started){
//						m_start_tick = event.tick;
//						m_is_started = true;
//						m_is_finished = false;
//                	}
//                	xSemaphoreGive(m_mutex);
//                }
            	if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {

            	        // If previous measurement finished, start a new measurement
            	        if (m_is_finished) {
            	            m_start_tick = event.tick;
            	            m_end_tick = 0;
            	            m_is_started = true;
            	            m_is_finished = false;
            	        }
            	        else if (!m_is_started) {
            	            m_start_tick = event.tick;
            	            m_is_started = true;
            	            m_is_finished = false;
            	        }

            	        xSemaphoreGive(m_mutex);
            	    }
            }

            // End event
            else if (event.event_type == IRBreakEventType::END){
            	IO_Handler::uart_print("IR END\r\n");
            	if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
            		if(m_is_started && !m_is_finished){
						m_end_tick = event.tick;
						m_is_finished = true;
            		}
            		xSemaphoreGive(m_mutex);
            	}
            }
        }
        //IO_Handler::uart_print("IR Break Sensor loop\r\n");
    }
}

// Get elapsed time in milliseconds
uint32_t IRBreakSensor::get_elapsed_ms() const{
	// Elapsed time can only be recorded when the object leaves both sensors.
	// Then, it needs to be converted from ticks to milliseconds.
    uint32_t elapsed = 0;

    // Mutex read guard
	if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
		if (m_is_started && m_is_finished)
			elapsed = (m_end_tick - m_start_tick) * portTICK_PERIOD_MS;
		xSemaphoreGive(m_mutex);
	}

    return elapsed;
}

float IRBreakSensor::get_velocity(){

    uint32_t elapsed_ms = get_elapsed_ms();

    // Object is still crossing both sensors + cannot divide by 0.
    if (elapsed_ms == 0) return 0.0f;

    float time_seconds = elapsed_ms / 1000.0f;

    return METERS_BETWEEN_SENSORS / time_seconds;
}

// Reset timing state
void IRBreakSensor::reset(){
    if (m_mutex == nullptr) {
		m_start_tick = 0;
		m_end_tick = 0;
		m_is_started = false;
		m_is_finished = false;
		return;
   }

	if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {
		m_start_tick = 0;
		m_end_tick = 0;
		m_is_started = false;
		m_is_finished = false;
		xSemaphoreGive(m_mutex);
	}
}


