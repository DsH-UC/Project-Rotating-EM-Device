/*
 * Thermistor2.hpp
 *
 *  Created on: Apr 25, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_THERMISTOR_HPP_
#define CUSTOM_THERMISTOR_HPP_
// Note: This file is now used for capacitor voltage reading because thermistor reads were moved to the ESP32
// Constants for thermistor calculations
#define THERM_VREF        3.3f // ADC reference voltage used by the MCU
#define THERM_ADC_MAX     4095.0f // Maximum value returned by 12-bit ADC
#define THERM_R_FIXED     10000.0f // 10kΩ // Fixed resistor used in voltage divider with thermistor
#define THERM_R25         10000.0f // 10kΩ // Thermistor resistance at 25°C
#define THERM_BETA 		  3950.0f // Thermistor beta constant (from datasheet)
#define THERM_T0_K 		  298.15f // Reference temperature (25°C) in Kelvin

#define CAP_VDIV_R_TOP 	  100000.0f   // TODO: replace w/ actual top resistor
#define CAP_VDIV_R_BOTTOM 10000.0f    // TODO: replace w/ actual bottom resistor

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "semphr.h"
#include <cstdint> // Standard integer types
#include <cstddef> // For NULL definition
#include <cmath>
#include "adc.h"

class Thermistor {
	// TODO 4/28/26: Clean up code for DMA; OOP, DMA use only, boris math functions, arm_math?
	// Bluetooth GUI TX and RX with button ISR's
	// State machine mock input testing using timed mock SM input variables and printing transitions
		// Interrupt simulation, timed test task, SW timers, testing queue, eTaskGetState
public:
    // Access function for singleton
	static Thermistor& get_instance(ADC_HandleTypeDef* hadc, uint32_t channel);

	// Disable copying
	Thermistor(const Thermistor&) = delete;
	Thermistor& operator = (const Thermistor&) = delete;
	Thermistor(Thermistor&&) = delete;
	Thermistor& operator = (Thermistor&&) = delete;

    void initThermistorThread(uint32_t stackSize, UBaseType_t priority);

    // Thread-safe way for other tasks to get the value
    uint16_t get_avg_sensor_reading(int32_t index);

    uint16_t get_raw_average(uint32_t adc_channel_index);
	float get_rail_temp_c();
	float get_cap_adc_voltage();
	float get_cap_bank_voltage();

    static TaskHandle_t cs_task_handle;


private:
    ADC_HandleTypeDef* _hadc;
    uint32_t _channel;
    float _lastTemp;
    SemaphoreHandle_t _mutex;

    uint16_t therm_ch_avg, cap_ch_avg;

    Thermistor(ADC_HandleTypeDef* hadc, uint32_t channel);

    // Internal task logic
    void run();

    // Static bridge function (MUST be static)
    static void taskEntry(void* obj);

    // Divider voltage in raw ADC count form to float temp in Celcius
    // intermediate conversion functions
    float Thermistor_CountsToVoltage(uint16_t counts);
    float Thermistor_VoltageToResistance(float v_adc);
    float Thermistor_ResistanceToTempC(float r_therm);
    //uint16_t Thermistor_ReadRaw(uint32_t adc_channel);
    uint16_t Thermistor_ReadRawAverage(uint32_t adc_channel_index);

    // Temp to be read by sensor
    float Thermistor_ReadTempC(uint32_t adc_channel_index);

    void Update_All_Channels();

};

#endif /* CUSTOM_THERMISTOR_HPP_ */
