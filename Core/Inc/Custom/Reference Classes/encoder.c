#include "encoder.h"
#include "tim.h"
#include <stdint.h>

/*
 * Pointer to the timer being used for encoder mode.
 * Example: &htim3 if TIM3 is configured for encoder mode.
 */
static TIM_HandleTypeDef *encoder_timer = NULL;

/*
 * Software-extended encoder count.
 *
 * The hardware timer counter is usually 16-bit, so it can wrap around.
 * This 32-bit variable lets us track position over many revolutions.
 */
static int32_t encoder_total_counts = 0;

/*
 * Previous raw timer count.
 *
 * Used to compute the change in encoder count between updates.
 */
static int16_t encoder_last_count = 0;

/*
 * Most recent direction:
 * +1 = forward
 * -1 = reverse
 *  0 = stopped/no change
 */
static int8_t encoder_direction = 0;

void Encoder_Init(TIM_HandleTypeDef *htim){
    encoder_timer = htim;

    encoder_total_counts = 0;
    encoder_last_count = 0;
    encoder_direction = 0;

    __HAL_TIM_SET_COUNTER(encoder_timer, 0);

    HAL_TIM_Encoder_Start(encoder_timer, TIM_CHANNEL_ALL);
}

void Encoder_Update(void){
    if (encoder_timer == NULL)
    {
        return;
    }

    int16_t current_count = (int16_t)__HAL_TIM_GET_COUNTER(encoder_timer);
    int16_t delta = current_count - encoder_last_count;

    encoder_total_counts += delta;
    encoder_last_count = current_count;

    if (delta > 0){
        encoder_direction = 1;
    }
    else if (delta < 0){
        encoder_direction = -1;
    }
    else{
        encoder_direction = 0;
    }
}

int32_t Encoder_GetCounts(void){
    return encoder_total_counts;
}


float Encoder_GetAngleDegrees(void){
    int32_t count_mod = encoder_total_counts % ENCODER_COUNTS_PER_REV;

    if (count_mod < 0)
    {
        count_mod += ENCODER_COUNTS_PER_REV;
    }

    return ((float)count_mod * 360.0f) / (float)ENCODER_COUNTS_PER_REV;
}


float Encoder_GetRevolutions(void){
    return (float)encoder_total_counts / (float)ENCODER_COUNTS_PER_REV;
}


int8_t Encoder_GetDirection(void){
    return encoder_direction;
}


void Encoder_Reset(void){
    if (encoder_timer == NULL)
    {
        return;
    }

    __HAL_TIM_SET_COUNTER(encoder_timer, 0);

    encoder_total_counts = 0;
    encoder_last_count = 0;
    encoder_direction = 0;
}


int32_t Encoder_GetAngleCentiDegrees(void){
    int32_t count_mod = encoder_total_counts % ENCODER_COUNTS_PER_REV;

    if (count_mod < 0)
    {
        count_mod += ENCODER_COUNTS_PER_REV;
    }

    return (count_mod * 36000L) / ENCODER_COUNTS_PER_REV;
}


uint8_t Encoder_GetRailIndexZeroBased(void){
    int32_t angle_cdeg = Encoder_GetAngleCentiDegrees();

    /*
     * For 6 rail positions:
     * 360.00 degrees / 6 = 60.00 degrees per rail
     * In centi-degrees: 60.00 deg = 6000 centi-degrees
     */
    int32_t sector_size_cdeg = 36000L / NUM_RAIL_POSITIONS;

    return (uint8_t)(angle_cdeg / sector_size_cdeg);
}


uint8_t Encoder_GetRailIndexOneBased(void){
    return Encoder_GetRailIndexZeroBased() + 1;
}


uint8_t Encoder_IsRailAligned(uint8_t rail_index_one_based, int32_t tolerance_cdeg){
    if (rail_index_one_based < 1 || rail_index_one_based > NUM_RAIL_POSITIONS)
    {
        return 0;
    }

    if (tolerance_cdeg < 0)
    {
        tolerance_cdeg = -tolerance_cdeg;
    }

    /*
     * Total circle = 360.00 degrees = 36000 centi-degrees.
     */
    const int32_t full_rotation_cdeg = 36000L;

    /*
     * For 6 rails:
     *      sector_size = 36000 / 6 = 6000 centi-degrees = 60.00 degrees
     */
    const int32_t sector_size_cdeg = full_rotation_cdeg / NUM_RAIL_POSITIONS;

    /*
     * Convert one-based rail number to zero-based index.
     *
     * Rail 1 -> index 0 -> center 0 degrees
     * Rail 2 -> index 1 -> center 60 degrees
     * Rail 3 -> index 2 -> center 120 degrees
     */
    int32_t rail_index_zero_based = (int32_t)rail_index_one_based - 1;

    int32_t target_angle_cdeg = rail_index_zero_based * sector_size_cdeg;
    int32_t current_angle_cdeg = Encoder_GetAngleCentiDegrees();

    /*
     * Compute absolute angular error.
     */
    int32_t error_cdeg = current_angle_cdeg - target_angle_cdeg;

    if (error_cdeg < 0)
    {
        error_cdeg = -error_cdeg;
    }

    /*
     * Handle wraparound near 0/360 degrees.
     *
     * Example:
     *      current = 359.00 deg
     *      target  = 0.00 deg
     *
     * Naive error = 359.00 deg
     * Real angular error = 1.00 deg
     */
    if (error_cdeg > (full_rotation_cdeg / 2))
    {
        error_cdeg = full_rotation_cdeg - error_cdeg;
    }

    return (error_cdeg <= tolerance_cdeg) ? 1 : 0;
}


void Encoder_SetZero(void){
    Encoder_Reset();
}

