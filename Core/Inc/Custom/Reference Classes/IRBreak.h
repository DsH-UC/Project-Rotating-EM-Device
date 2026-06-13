/**
 * @file IRBreak.h
 *
 * @brief Interface for IR break-beam sensor modules.
 *
 * This module provides a software interface for reading an IR break sensor
 * using a GPIO input pin on the MCU. It supports:
 * - GPIO hardware initialization
 * - sensor object initialization
 * - periodic sensor state updates
 * - beam-broken status checks
 * - edge detection for beam transitions
 *
 * This module is intended for use with one or more optical gate sensors,
 * such as start/end break sensors used for projectile detection.
 */

#ifndef IRBREAK_H
#define IRBREAK_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include <stddef.h>

typedef struct
{
    GPIO_TypeDef *port;       // GPIO port used by the IR receiver output
    uint16_t pin;             // GPIO pin used by the IR receiver output
    
    uint8_t beam_broken;      // 1 if beam is broken, 0 if intact
    uint8_t last_beam_broken; // previous state, useful for edge detection
    uint8_t valid;            // 1 if sensor is configured and usable

    uint32_t last_update_ms; // last time sensor state was updated
} IRBreakSensor;

/**
 * @brief Timer structure for tracking IR break sensor events.
 */
typedef struct {
    uint32_t start_time;
    uint32_t end_time;
    uint8_t  started;
    uint8_t  finished;
} IRBreakTimer;

/**
 * @brief Initializes the MCU GPIO pin used by an IR break sensor.
 *
 * This function configures the selected pin as a digital input and applies
 * the requested pull mode.
 *
 * @param port      GPIO port for the sensor input pin
 * @param pin       GPIO pin for the sensor input pin
 * @param pull_mode GPIO pull configuration (e.g. GPIO_NOPULL, GPIO_PULLUP, GPIO_PULLDOWN)
 */
void IRBreak_GPIO_Init(GPIO_TypeDef *port, uint16_t pin, uint32_t pull_mode);

/**
 * @brief Initializes an IRBreakSensor object.
 *
 * This function stores the GPIO port and pin used by the sensor and clears
 * the runtime state fields.
 *
 * @param sensor Pointer to the IRBreakSensor object to initialize
 * @param port   GPIO port used by the sensor
 * @param pin    GPIO pin used by the sensor
 */
void IRBreak_Init(IRBreakSensor *sensor, GPIO_TypeDef *port, uint16_t pin);

/**
 * @brief Updates the logical beam state of the sensor.
 *
 * This function reads the GPIO input pin, stores the current beam state,
 * preserves the previous beam state for edge detection, and records the
 * update time.
 *
 * @param sensor Pointer to the IRBreakSensor object to update
 */
void IRBreak_Update(IRBreakSensor *sensor);

/**
 * @brief Checks if the IR beam is currently broken.
 *
 * @param sensor Pointer to the IRBreakSensor object to check
 * @return 1 if the beam is broken, 0 if intact or if sensor is invalid
 */
uint8_t IRBreak_IsBroken(IRBreakSensor *sensor);

/**
 * @brief Checks for a rising edge on the IR beam.
 *
 * @param sensor Pointer to the IRBreakSensor object to check
 * @return 1 if a rising edge is detected, 0 otherwise
 */
uint8_t IRBreak_RisingEdge(IRBreakSensor *sensor);

/**
 * @brief Detects a falling edge in the logical beam-broken state.
 *
 * A falling edge means the sensor changed from:
 *      beam broken (1) -> beam intact (0)
 *
 * @param sensor Pointer to the IRBreakSensor object
 * @return 1 if a falling edge occurred, 0 otherwise
 */
uint8_t IRBreak_FallingEdge(IRBreakSensor *sensor);

// Additional functions for timing events can be added here, such as starting/stopping timers
// based on beam break events, or measuring durations of beam breaks.

/**
 * @brief Initialize timing structure for IR-based measurements.
 */
void IRBreak_TimerInit(IRBreakTimer *t);


/**
 * @brief Update timer based on two IR sensors.
 *
 * Starts timing when the "start" sensor beam is broken,
 * stops timing when the "end" sensor beam is broken.
 *
 * @param t     Timer object
 * @param start Start sensor
 * @param end   End sensor
 */
void IRBreak_UpdateTimer(IRBreakTimer *t,
                         IRBreakSensor *start,
                         IRBreakSensor *end);

/**
 * @brief Get elapsed time in milliseconds between two beam breaks.
 *
 * @return Time difference (ms), or 0 if not complete
 */
uint32_t IRBreak_GetElapsed(IRBreakTimer *t);

/**
 * @brief Reset timer to initial state.
 */
void IRBreak_ResetTimer(IRBreakTimer *t);

#endif