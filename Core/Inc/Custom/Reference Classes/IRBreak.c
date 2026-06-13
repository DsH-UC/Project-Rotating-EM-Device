#include "IRBreak.h"

/**
 * @brief Initialize GPIO pin for IR break sensor input.
 *
 * Configures the selected GPIO pin as a digital input with the specified
 * pull configuration. Also ensures the corresponding GPIO clock is enabled.
 *
 * @param port GPIO port (GPIOA, GPIOB, etc.)
 * @param pin  GPIO pin number
 * @param pull_mode Pull configuration (GPIO_NOPULL, GPIO_PULLUP, etc.)
 */
void IRBreak_GPIO_Init(GPIO_TypeDef *port, uint16_t pin, uint32_t pull_mode)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable the correct GPIO clock based on selected port
    if (port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
    else if (port == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
    else return;

    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = pull_mode;

    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

/**
 * @brief Initialize IR sensor software structure.
 *
 * Sets GPIO references and clears runtime state variables.
 */
void IRBreak_Init(IRBreakSensor *sensor, GPIO_TypeDef *port, uint16_t pin)
{
    if (sensor == NULL)
    {
        return;
    }

    sensor->port = port;
    sensor->pin = pin;

    // Initialize state tracking
    sensor->beam_broken = 0;
    sensor->last_beam_broken = 0;
    sensor->valid = 1;             // Mark sensor as usable
    sensor->last_update_ms = 0;    // No updates yet
}

/**
 * @brief Poll sensor and update beam state.
 *
 * Reads GPIO input and updates:
 * - current state
 * - previous state (for edge detection)
 * - timestamp
 *
 * Assumes ACTIVE LOW sensor:
 *      LOW  = beam broken
 *      HIGH = beam intact
 */
void IRBreak_Update(IRBreakSensor *sensor)
{
    if (sensor == NULL || sensor->valid == 0)
    {
        return;
    }

    // Save previous state for edge detection
    sensor->last_beam_broken = sensor->beam_broken;
    GPIO_PinState state = HAL_GPIO_ReadPin(sensor->port, sensor->pin);

    // Assuming active low: GPIO_PIN_RESET means beam is broken
    // If active high, simply reverse the logic
    if (state == GPIO_PIN_RESET)
    {
        sensor->beam_broken = 1; // Beam is broken
    }
    else
    {
        sensor->beam_broken = 0; // Beam is intact
    }

    // Timestamp update (ms since system start)
    sensor->last_update_ms = HAL_GetTick(); 
}

/**
 * @brief Get current beam state.
 */
uint8_t IRBreak_IsBroken(IRBreakSensor *sensor)
{
    if (sensor == NULL || sensor->valid == 0)
    {
        return 0;
    }

    return sensor->beam_broken;
}

/**
 * @brief Detect rising edge (beam just broken).
 *
 * Transition:
 *      0 -> 1
 */
uint8_t IRBreak_RisingEdge(IRBreakSensor *sensor)
{
    if (sensor == NULL || sensor->valid == 0)
    {
        return 0;
    }

    return (sensor->last_beam_broken == 0 && sensor->beam_broken == 1);
}

/**
 * @brief Detect falling edge (beam just restored).
 *
 * Transition:
 *      1 -> 0
 */
uint8_t IRBreak_FallingEdge(IRBreakSensor *sensor)
{
    if (sensor == NULL || sensor->valid == 0)
    {
        return 0;
    }

    return (sensor->last_beam_broken == 1 && sensor->beam_broken == 0);
}

/**
 * @brief Initialize timing structure.
 *
 * Used for measuring time between two IR sensors.
 */
void IRBreak_TimerInit(IRBreakTimer *t)
{
    if (t == NULL) {
        return;
    }

    t->start_time = 0;
    t->end_time = 0;
    t->started = 0;
    t->finished = 0;
}

/**
 * @brief Update timing based on two sensors.
 *
 * Start sensor → triggers timer start
 * End sensor   → triggers timer stop
 *
 * Uses rising edge detection (beam break event).
 */
void IRBreak_UpdateTimer(IRBreakTimer *t,
                         IRBreakSensor *start,
                         IRBreakSensor *end){
    if (t == NULL || start == NULL || end == NULL) {
        return;
    }

    if (start->valid == 0 || end->valid == 0) {
        return;
    }

    /*
     * beam_broken = 1 when beam is broken
     * So the beam-break event is a RISING edge:
     * 0 -> 1
     */
    // start timin on first beam break
    if (t->started == 0 && IRBreak_RisingEdge(start)) {
        t->start_time = HAL_GetTick();
        t->started = 1;
        t->finished = 0;
    }

    //  stop timing on second beam break
    if (t->started == 1 && t->finished == 0 && IRBreak_RisingEdge(end)) {
        t->end_time = HAL_GetTick();
        t->finished = 1;
    }
}

/**
 * @brief Get elapsed time between sensors (ms).
 */
uint32_t IRBreak_GetElapsed(IRBreakTimer *t)
{
    if (t == NULL) {
        return 0;
    }

    if (t->started == 1 && t->finished == 1) {
        return t->end_time - t->start_time;
    }

    return 0;
}

/**
 * @brief Reset timer state.
 */
void IRBreak_ResetTimer(IRBreakTimer *t)
{
    if (t == NULL) {
        return;
    }

    t->start_time = 0;
    t->end_time = 0;
    t->started = 0;
    t->finished = 0;
}

