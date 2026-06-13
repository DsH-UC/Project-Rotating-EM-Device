/*
 * Thermistor2.cpp
 *
 *  Created on: Apr 25, 2026
 *      Author: devhu
 */

#include <Custom/Sensors/Thermistor.hpp>
#include "Custom/IO_Handler.hpp"

using namespace std;
 // Actually the cap voltage sensor now.
TaskHandle_t Thermistor::cs_task_handle = nullptr;
#ifdef __cplusplus
extern "C" {
#endif

extern volatile uint16_t adc_dma_buffer[32];
// extern DMA_HandleTypeDef hdma_adc1;

#ifdef __cplusplus
}
#endif

extern "C" { // To be put into IO_Handler
    void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    	if (hadc->Instance == ADC1) {
			// 1. Stop the ADC so it doesn't overwrite our data while we process it
			HAL_ADC_Stop_DMA(hadc);

			BaseType_t xHigherPriorityTaskWoken = pdFALSE;

			// 2. Notify the task (using the static handle)
			if (Thermistor::cs_task_handle != NULL) {
				vTaskNotifyGiveFromISR(Thermistor::cs_task_handle, &xHigherPriorityTaskWoken);
			}
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    	}
    }
}

Thermistor::Thermistor(ADC_HandleTypeDef* hadc, uint32_t channel)
    : _hadc(hadc), _channel(channel), _lastTemp(0.0f) {

    // Create the mutex before the thread starts
    _mutex = xSemaphoreCreateMutex();
}

Thermistor& Thermistor::get_instance(ADC_HandleTypeDef* hadc, uint32_t channel){
		static Thermistor instance(hadc, channel);
		return instance;
}

void Thermistor::initThermistorThread(uint32_t stackSize, UBaseType_t priority) {
    xTaskCreate(Thermistor::taskEntry, "TempTask", stackSize, this, priority, &(Thermistor::cs_task_handle));
}

void Thermistor::run() {
	Thermistor::cs_task_handle = xTaskGetCurrentTaskHandle();
	for(;;) {
		Update_All_Channels();
		float temp_c = get_rail_temp_c(); // '16' is ignored or handled internally now
		float cap_v = get_cap_bank_voltage(); //Thermistor_ReadTempC(1);

		// Format and print using %ld.%02ld
	    int32_t temp_whole = (int32_t)temp_c;
	    int32_t temp_frac  = (int32_t)((temp_c - temp_whole) * 100);
	    // Use labs() to ensure the fraction is positive (important for negative temps)
	    if (temp_frac < 0) temp_frac = -temp_frac;

	    int32_t cap_whole = (int32_t)cap_v;
	    int32_t cap_frac  = (int32_t)((cap_v -cap_whole) * 100);
	    if (cap_frac < 0) cap_frac = -cap_frac;

	    char msg[80];
	    snprintf(msg, sizeof(msg), "T1: %ld.%02ld | C1: %ld.%02ld\r\n",
	             (long)temp_whole, (long)temp_frac, (long)cap_whole, (long)cap_frac);
	    //snprintf(msg,sizeof(msg),"T1: %u | C1: %u", Thermistor_ReadRawAverage(0), Thermistor_ReadRawAverage(1));

	    //IO_Handler::uart_print(msg);
		//IO_Handler::uart_print("hi\r\n");
	    // 7. Sleep
	    vTaskDelay(pdMS_TO_TICKS(500));
	}

}

void Thermistor::taskEntry(void* obj) { static_cast<Thermistor*>(obj)->run(); }

// Boris

// Private
float Thermistor::Thermistor_CountsToVoltage(uint16_t counts){
    return ((float)counts / THERM_ADC_MAX ) * THERM_VREF; // Scale ADC counts to voltage
}

float Thermistor::Thermistor_VoltageToResistance(float v_adc){
    // Divider form:
    // 3.3V -- THERM_R_FIXED -- node -- Thermistor -- GND
    // Vnode = THERM_VREF * Rtherm / (THERM_R_FIXED + Rtherm)

    if (v_adc <= 0.001f || v_adc >= (THERM_VREF - 0.001f)) return NAN; // prevent invalid calculation near 0 V

    return THERM_R_FIXED * (v_adc / (THERM_VREF - v_adc));
}

float Thermistor::Thermistor_ResistanceToTempC(float r_therm){
    if (r_therm <= 0.0f || !isfinite(r_therm)) return NAN; // prevent log of non-positive number

    float ln_ratio = logf(r_therm / THERM_R25); // natural log of resistance ratio
    float inv_T = (1.0f / THERM_T0_K) + (ln_ratio / THERM_BETA);

    float temp_K = 1.0f / inv_T; // temperature in Kelvin
    return temp_K - 273.15f; // convert Kelvin to Celsius
}

float Thermistor::Thermistor_ReadTempC(uint32_t adc_channel_index){
    uint16_t counts = Thermistor_ReadRawAverage(adc_channel_index);
    float v_adc = Thermistor_CountsToVoltage(counts);
    float r_therm = Thermistor_VoltageToResistance(v_adc);

	if (counts >= 4090 || counts <= 5) {
		return NAN;          // Return "Not a Number"
	}
    if (!isfinite(r_therm) || r_therm <= 0.0f) return NAN;

    return Thermistor_ResistanceToTempC(r_therm);
}

uint16_t Thermistor::Thermistor_ReadRawAverage(uint32_t adc_channel_index) {
    uint16_t val = 0;
    if (_mutex == nullptr) return 0;

    // READ GUARD: Ensures we don't read while Update_All_Channels is writing
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        if (adc_channel_index == 0) val = this->therm_ch_avg;
        else if (adc_channel_index == 1) val = this->cap_ch_avg;
        xSemaphoreGive(_mutex);
    }

    return val; // Returns 0 if mutex was busy or index is wrong
}

void Thermistor::Update_All_Channels() {
    // 1. Trigger the hardware burst
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma_buffer, 32);

    // 2. Wait for hardware to finish (Task is BLOCKED here, saving CPU)
    if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)) == pdTRUE) {

        // 3. Process the raw data on the LOCAL STACK (No mutex needed yet)
        uint32_t s0 = 0, s1 = 0;
        for (int i = 0; i < 32; i += 2) {
            s0 += adc_dma_buffer[i];
            s1 += adc_dma_buffer[i+1];
        }

        // 4. WRITE GUARD
        if (xSemaphoreTake(_mutex, portMAX_DELAY) == pdTRUE) {
            this->therm_ch_avg = (uint16_t)(s0 / 16);
            this->cap_ch_avg = (uint16_t)(s1 / 16);
            xSemaphoreGive(_mutex); // UNLOCK
        }

    }
}

// Public thermistor temp and cap voltage getters
uint16_t Thermistor::get_raw_average(uint32_t adc_channel_index) {
    return Thermistor_ReadRawAverage(adc_channel_index);
}

float Thermistor::get_rail_temp_c() { return Thermistor_ReadTempC(0); }

float Thermistor::get_cap_adc_voltage() {
    uint16_t counts = Thermistor_ReadRawAverage(1);
    return Thermistor_CountsToVoltage(counts);
}

float Thermistor::get_cap_bank_voltage() {
    float v_adc = get_cap_adc_voltage();

    if (!isfinite(v_adc)) return NAN;
    return v_adc * ((CAP_VDIV_R_TOP + CAP_VDIV_R_BOTTOM) / CAP_VDIV_R_BOTTOM);
}







