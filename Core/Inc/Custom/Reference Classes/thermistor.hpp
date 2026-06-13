/**
 * @file    thermistor.h
 *
 * @author  Boris Curiel Molina
 *
 * @date    9 March 2026
 *
 * @details
 * This file defines the thermistor sensor interface and state structure.
 * It provides functions for initializing a thermistor sensor instance,
 * reading ADC-based thermistor data, and converting that data into
 * voltage, resistance, and temperature values.
 */


#ifndef THERMISTOR_H
#define THERMISTOR_H

#include <cstdint> // Standard integer types
#include <cstddef> // For NULL definition
#include "stm32f4xx_hal.h"
#include "stm32f446xx.h"
//#include "arm_math.h"

// Constants for thermistor calculations
// ADC reference voltage used by the MCU
#define THERM_VREF        3.3f

// Maximum value returned by 12-bit ADC
#define THERM_ADC_MAX     4095.0f

// Fixed resistor used in voltage divider with thermistor
#define THERM_R_FIXED     10000.0f   // 10kΩ

// Thermistor resistance at 25°C
#define THERM_R25         10000.0f   // 10kΩ

// Thermistor beta constant (from datasheet)
#define THERM_BETA        3950.0f

// Reference temperature (25°C) in Kelvin
#define THERM_T0_K        298.15f

typedef struct
{
    uint32_t adc_channel;      // ADC input used for this thermistor

    uint16_t raw_counts;       // Latest averaged ADC reading
    float node_voltage;        // Voltage measured at ADC pin
    float r_ohms;              // Calculated thermistor resistance
    float temp_c;              // Calculated temperature in Celsius

    uint32_t sample_period_ms; // Desired sampling interval
    uint32_t last_sample_ms;   // Timestamp of previous sample

    uint8_t valid;             // 1 if reading is considered valid
} ThermistorSensor;

void Thermistor_Init(ThermistorSensor *sensor, uint32_t adc_channel, uint32_t sample_period_ms);
void Thermistor_Update(ThermistorSensor *sensor, uint16_t samples);

/**
 * Converts raw ADC counts into an analog voltage.
 * @param counts The raw ADC counts (0 to THERM_ADC_MAX)
 * @return Voltage in volts corresponding to ADC reading.
 */
float Thermistor_CountsToVoltage(uint16_t counts);

/**
 * Converts measured ADC voltage into thermistor resistance.
 * Uses voltage divider equation based on circuit configuration:
 *  Vnode = Vref * Rtherm / (Rfixed + Rtherm)
 *
 * @param v_adc Measured voltage from ADC
 * @return Calculated thermistor resistance (Ohms)
 */
float Thermistor_VoltageToResistance(float v_adc);

/**
 * Converts thermistor resistance into temperature in Celsius.
 * Uses the Beta thermistor model:
 *      1/T = 1/T0 + (1/B) * ln(R/R0)
 *
 * @param r_therm Thermistor resistance (Ohms)
 * @return Temperature in degrees Celsius
 */
float Thermistor_ResistanceToTempC(float r_therm);



// Functions that may be deleted later
/**
 * Reads a raw ADC value from the specified channel.
 *
 * @param adc_channel ADC channel connected to thermistor divider node
 * @return Raw ADC value (0–4095)
 */
uint16_t Thermistor_ReadRaw(uint32_t adc_channel);

/**
 * Reads multiple ADC samples and returns the average value.
 *
 * @param adc_channel ADC channel connected to thermistor divider node
 * @param samples Number of samples to average (must be > 0)
 * @return Average ADC value (0–4095)
 */
uint16_t Thermistor_ReadRawAverage(uint32_t adc_channel, uint16_t samples);

/**
 * Reads the thermistor and returns temperature in Celsius.
 *
 * Performs the full measurement pipeline:
 *      ADC read → voltage → resistance → temperature
 *
 * @param adc_channel ADC channel connected to thermistor circuit
 * @param samples Number of ADC readings used for averaging
 * @return Temperature in degrees Celsius
 */
float Thermistor_ReadTempC(uint32_t adc_channel, uint16_t samples);

#endif
