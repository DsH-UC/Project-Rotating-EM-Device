#include "adc.h"
#include "stm32f4xx_hal.h"
#include <cmath>
#include <thermistor.hpp>

using namespace std;

// Debug Log: Replaced include "adc.h" with "main.h" which contains stm32xxxx_hal_conf.h, replaced c headers w/ cpp ones.
void Thermistor_Init(ThermistorSensor *sensor, uint32_t adc_channel, uint32_t sample_period_ms){
    if (sensor == NULL)
    {
        return; // handle null pointer gracefully
    }

    sensor->adc_channel = adc_channel;
    sensor->raw_counts = 0;
    sensor->node_voltage = 0.0f;
    sensor->r_ohms = 0.0f;
    sensor->temp_c = 0.0f;
    sensor->sample_period_ms = sample_period_ms;
    sensor->last_sample_ms = 0;
    sensor->valid = 0;
}

void Thermistor_Update(ThermistorSensor *sensor, uint16_t samples)
{
    if (sensor == NULL || samples == 0)
    {
        if (sensor != NULL)
        {
            sensor->valid = 0; // mark as invalid if parameters are not correct
        }
        return; // handle null pointer and zero samples gracefully
    }

    // Check if it's time to sample
    uint32_t now = HAL_GetTick();

    // Respect requested sample period
    if ((now - sensor->last_sample_ms) < sensor->sample_period_ms)
    {
        return;
    }

    sensor->raw_counts = Thermistor_ReadRawAverage(sensor->adc_channel, samples);
    sensor->node_voltage = Thermistor_CountsToVoltage(sensor->raw_counts);
    sensor->r_ohms = Thermistor_VoltageToResistance(sensor->node_voltage);

    if (!isfinite(sensor->r_ohms) || sensor->r_ohms <= 0.0f)
    {
        sensor->valid = 0;
        sensor->last_sample_ms = now;
        return;
    }

    sensor->temp_c = Thermistor_ResistanceToTempC(sensor->r_ohms);

    if (!isfinite(sensor->temp_c))
    {
        sensor->valid = 0;
        sensor->last_sample_ms = now;
        return;
    }

    sensor->last_sample_ms = now;
    sensor->valid = 1;
}

float Thermistor_CountsToVoltage(uint16_t counts)
{
    return ((float)counts / THERM_ADC_MAX ) * THERM_VREF; // Scale ADC counts to voltage
}

float Thermistor_VoltageToResistance(float v_adc)
{
    // Divider form:
    // 3.3V -- THERM_R_FIXED -- node -- Thermistor -- GND
    // Vnode = THERM_VREF * Rtherm / (THERM_R_FIXED + Rtherm)

    if (v_adc <= 0.001f || v_adc >= (THERM_VREF - 0.001f))
    {
        return NAN; // prevent invalid calculation near 0 V
    }

    return THERM_R_FIXED * (v_adc / (THERM_VREF - v_adc));
}

float Thermistor_ResistanceToTempC(float r_therm){
    if (r_therm <= 0.0f || !isfinite(r_therm))
    {
        return NAN; // prevent log of non-positive number
    }

    float ln_ratio = logf(r_therm / THERM_R25); // natural log of resistance ratio
    float inv_T = (1.0f / THERM_T0_K) + (ln_ratio / THERM_BETA);

    float temp_K = 1.0f / inv_T; // temperature in Kelvin
    return temp_K - 273.15f; // convert Kelvin to Celsius
}

uint16_t Thermistor_ReadRaw(uint32_t adc_channel){
    return ADC_Read(adc_channel);
}

uint16_t Thermistor_ReadRawAverage(uint32_t adc_channel, uint16_t samples){
    uint32_t sum = 0;

    if (samples == 0)
    {
        return 0; // prevent division by zero
    }

    for (uint16_t i = 0; i < samples; i++)
    {
        sum += ADC_Read(adc_channel); // collect one ADC sample
    }

    return (uint16_t)(sum / samples); // return average ADC count
}

float Thermistor_ReadTempC(uint32_t adc_channel, uint16_t samples)
{
    uint16_t counts = Thermistor_ReadRawAverage(adc_channel, samples);
    float v_adc = Thermistor_CountsToVoltage(counts);
    float r_therm = Thermistor_VoltageToResistance(v_adc);

    if (!isfinite(r_therm) || r_therm <= 0.0f){
        return NAN;
    }

    return Thermistor_ResistanceToTempC(r_therm);
}
