#ifndef ENCODER_H
#define ENCODER_H

/*
 * encoder.h
 *
 * Header file for reading an incremental quadrature encoder using
 * an STM32 hardware timer configured in Encoder Mode.
 *
 * Hardware signal chain:
 *
 * M9 Encoder differential outputs:
 *      A+ / A-
 *      B+ / B-
 *      Index+ / Index-
 *
 * Differential receiver:
 *      AM26C32IN
 *
 * Receiver outputs to STM32:
 *      1Y -> PA6 / TIM3_CH1  // Encoder A signal
 *      2Y -> PA7 / TIM3_CH2  // Encoder B signal
 *      3Y -> PB6 / EXTI      // Optional index pulse
 *
 * The AM26C32 converts the encoder's differential signals into normal
 * single-ended logic signals that the STM32 can read.
 *
 * The STM32 timer handles the quadrature decoding in hardware.
 */

#include "main.h"     // Provides STM32 HAL types such as TIM_HandleTypeDef
#include <stdint.h>   // Provides fixed-width integer types: int32_t, int8_t, etc.

/*
 * ENCODER_PPR
 *
 * Pulses per revolution from the encoder datasheet.
 *
 * The Timken M9 encoder version we are using is 1000 PPR.
 *
 * PPR means the encoder produces 1000 cycles of A/B signals per full
 * mechanical revolution.
 */
#define ENCODER_PPR 1000

/*
 * ENCODER_COUNTS_PER_REV
 *
 * In quadrature encoder mode, the STM32 timer can count 4 edges per pulse:
 *
 *      A rising edge
 *      A falling edge
 *      B rising edge
 *      B falling edge
 *
 * Therefore:
 *
 *      counts per revolution = PPR * 4
 *
 * For a 1000 PPR encoder:
 *
 *      1000 * 4 = 4000 counts/revolution
 *
 * This gives an angular resolution of:
 *
 *      360 degrees / 4000 counts = 0.09 degrees/count
 */
#define ENCODER_COUNTS_PER_REV (ENCODER_PPR * 4)

#define NUM_RAIL_POSITIONS 6


//-----------------------------------------------------------------------------------------------
/*
 * Encoder_Init
 *
 * Initializes the encoder software module.
 *
 * This function:
 *      - Stores a pointer to the timer used for encoder mode
 *      - Resets the timer counter to 0
 *      - Clears the software count accumulator
 *      - Starts the STM32 hardware timer in encoder mode
 *
 * Parameter:
 *      htim - pointer to the timer handle configured in encoder mode
 *
 * Example:
 *      Encoder_Init(&htim3);
 *
 * Important:
 *      The timer must already be configured in CubeMX as Encoder Mode.
 *      For our selected pins:
 *          PA6 -> TIM3_CH1
 *          PA7 -> TIM3_CH2
 */
void Encoder_Init(TIM_HandleTypeDef *htim);

/*
 * Encoder_Update
 *
 * Updates the software-tracked encoder count.
 *
 * The STM32 timer counter is usually 16-bit, meaning it can overflow
 * or underflow. This function reads the current timer count, compares it
 * to the previous count, and adds the difference to a larger 32-bit
 * software counter.
 *
 * This allows the encoder count to track rotation beyond the hardware
 * timer's limited counter range.
 *
 * Call this function repeatedly in the main loop or from a periodic task.
 *
 * Example:
 *      while (1)
 *      {
 *          Encoder_Update();
 *          HAL_Delay(10);
 *      }
 */
void Encoder_Update(void);

/*
 * Encoder_GetCounts
 *
 * Returns the total accumulated encoder count.
 *
 * The value can be:
 *      positive -> rotation in one direction
 *      negative -> rotation in the opposite direction
 *
 * Return:
 *      signed 32-bit total encoder count
 *
 * Example:
 *      int32_t counts = Encoder_GetCounts();
 */
int32_t Encoder_GetCounts(void);

/*
 * Encoder_GetAngleDegrees
 *
 * Converts the current encoder count into an angle from 0 to 360 degrees.
 *
 * This function wraps the count so the returned angle always represents
 * one mechanical revolution.
 *
 * Example:
 *      0 counts       -> 0 degrees
 *      1000 counts    -> 90 degrees
 *      2000 counts    -> 180 degrees
 *      3000 counts    -> 270 degrees
 *      4000 counts    -> 0 degrees again
 *
 * Return:
 *      angle in degrees as a floating-point value
 */
float Encoder_GetAngleDegrees(void);

/*
 * Encoder_GetRevolutions
 *
 * Converts the total encoder count into total revolutions.
 *
 * Unlike Encoder_GetAngleDegrees(), this does not wrap back to zero.
 * It tells you how many full rotations have occurred, including partial
 * rotations.
 *
 * Example:
 *      4000 counts -> 1.0 revolution
 *      8000 counts -> 2.0 revolutions
 *      -4000 counts -> -1.0 revolution
 *
 * Return:
 *      signed revolution count as a floating-point value
 */
float Encoder_GetRevolutions(void);

/*
 * Encoder_GetDirection
 *
 * Returns the most recently detected direction of motion.
 *
 * Return:
 *      +1 -> encoder moved forward during the last update
 *      -1 -> encoder moved backward during the last update
 *       0 -> no movement detected during the last update
 *
 * Important:
 *      "Forward" depends on wiring. If the direction is reversed from
 *      what you expect, you can either swap A/B signals or invert the
 *      direction in software.
 */
int8_t Encoder_GetDirection(void);

/*
 * Encoder_Reset
 *
 * Resets the encoder count back to zero.
 *
 * This function:
 *      - Clears the STM32 timer counter
 *      - Clears the software accumulated count
 *      - Clears the stored direction
 *
 * This can be used:
 *      - during startup
 *      - during calibration
 *      - when the index pulse is detected
 *      - when manually setting a new zero position
 *
 * Example:
 *      Encoder_Reset();
 */
void Encoder_Reset(void);

/*
 * Encoder_SetZero
 *
 * Sets the current encoder position as the software zero reference.
 *
 * This function does not physically move the encoder or motor. It simply
 * resets the software count reference so the current mechanical position
 * becomes 0 degrees.
 *
 * This is useful when:
 *      - the rotating assembly is manually placed at the desired home position
 *      - the system completes a homing routine
 *      - the index pulse is detected and should define zero
 *
 * Example:
 *      Encoder_SetZero();
 */
void Encoder_SetZero(void);


/*
* Encoder_GetAngleCentiDegrees
 *
 * returns the current encoder angle in centi-degrees (1/100th of a degree).
 *
 * Example:
 *      0 counts       -> 0 centi-degrees
 *      1000 counts    -> 9000 centi-degrees (90.00 degrees)
 *      2000 counts    -> 18000 centi-degrees (180.00 degrees)
 *      3000 counts    -> 27000 centi-degrees (270.00 degrees)
 *      4000 counts    -> 0 centi-degrees again
 *
 * This function is useful because STM32 printf can be annoying with floats.
 * Returning an integer angle avoids needing floating point UART printing.
 * 
 * Return:
 *      angle from 0 to 35999 centi-degrees 
 */
int32_t Encoder_GetAngleCentiDegrees(void);


/*
 * Encoder_GetRailIndexZeroBased
 *
 * Converts the current encoder angle into a rail sector index.
 *
 * Zero-based indexing means the rails are numbered:
 *
 *      0, 1, 2, ..., NUM_RAIL_POSITIONS - 1
 * 
 * For a 6-rail system:
 *
 *      Rail index 0: 0.00   to 59.99 degrees
 *      Rail index 1: 60.00  to 119.99 degrees
 *      Rail index 2: 120.00 to 179.99 degrees
 *      Rail index 3: 180.00 to 239.99 degrees
 *      Rail index 4: 240.00 to 299.99 degrees
 *      Rail index 5: 300.00 to 359.99 degrees
 *
 * This is useful for array indexing in C, for example:
 *
 *      rail_temperature[rail_index]
 *
 * Return:
 *      current rail sector using zero-based indexing
 */
uint8_t Encoder_GetRailIndexZeroBased(void);


/*
 * Encoder_GetRailIndexOneBased
 * Converts the current encoder angle into a human-readable rail number.
 *
 * One-based indexing means the rails are numbered:
 *
 *      1, 2, 3, ..., NUM_RAIL_POSITIONS
 *
 * For a 6-rail system:
 *
 *      Rail 1: 0.00   to 59.99 degrees
 *      Rail 2: 60.00  to 119.99 degrees
 *      Rail 3: 120.00 to 179.99 degrees
 *      Rail 4: 180.00 to 239.99 degrees
 *      Rail 5: 240.00 to 299.99 degrees
 *      Rail 6: 300.00 to 359.99 degrees
 *
 * This is easier to print to UART or show in a UI because humans usually
 * expect rail numbering to start at 1.
 *
 * Return:
 *      current rail number using one-based indexing
 */
uint8_t Encoder_GetRailIndexOneBased(void);


/*
 * Encoder_IsRailAligned
 *
 * Checks whether the current encoder angle is within a tolerance window
 * around the center angle of a selected rail.
 *
 * Parameters:
 *      rail_index_one_based:
 *          Rail number using human-readable indexing.
 *          Valid range: 1 to NUM_RAIL_POSITIONS.
 *
 *      tolerance_cdeg:
 *          Alignment tolerance in centi-degrees.
 *
 *          Example:
 *              100  = 1.00 degree
 *              200  = 2.00 degrees
 *              500  = 5.00 degrees
 *
 * Return:
 *      1 -> selected rail is aligned
 *      0 -> selected rail is not aligned
 *
 * Example:
 *      Encoder_IsRailAligned(3, 200);
 *
 *      Checks whether Rail 3 is within ±2.00 degrees of its center angle.
 */
uint8_t Encoder_IsRailAligned(uint8_t rail_index_one_based, int32_t tolerance_cdeg);

#endif /* ENCODER_H */