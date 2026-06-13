/*
 * UITelemetryTransmitter.cpp
 *
 *  Created on: May 17, 2026
 *      Author: devhu
 */

#include <Custom/Remote GUI/UI_TelemetryTransmitter.hpp>
#include "Custom/Remote GUI/UI_CommandReceiver.hpp"
#include "Custom/Sensors/Thermistor.hpp"
#include "Custom/CoolingSystem.hpp"
#include "Custom/ChargingStage.hpp"
#include "Custom/RotatingBlock.hpp"
#include "Custom/IO_Handler.hpp"
#include "Custom/Sensors/RotaryEncoder.hpp"
#include "Custom/Sensors/IRBreakSensor.hpp"

#include "adc.h"
#include <cstdio>
#include <cmath>

namespace {
    constexpr uint32_t TELEMETRY_PERIOD_MS = 1000;

    void split_float(float value, int32_t& whole, int32_t& frac) {
        if (!isfinite(value)) {
            whole = 0;
            frac = 0;
            return;
        }

        whole = static_cast<int32_t>(value);
        frac = static_cast<int32_t>((value - whole) * 100.0f);

        if (frac < 0) frac = -frac;
    }
}

UI_TelemetryTransmitter::UI_TelemetryTransmitter() : m_task_handle(nullptr) {}

UI_TelemetryTransmitter& UI_TelemetryTransmitter::get_instance() {
    static UI_TelemetryTransmitter instance;
    return instance;
}

void UI_TelemetryTransmitter::start_thread(uint16_t stackSize, UBaseType_t priority) {
    BaseType_t ok = xTaskCreate(
            UI_TelemetryTransmitter::runStatic,
            "Telemetry",
            stackSize,
            this,
            priority,
            &m_task_handle
        );

        configASSERT(ok == pdPASS);
}

void UI_TelemetryTransmitter::runStatic(void* params) { static_cast<UI_TelemetryTransmitter*>(params)->task_loop(); }

void UI_TelemetryTransmitter::task_loop() {
    char msg[192];
    auto& analog = Thermistor::get_instance(&hadc1, ADC_CHANNEL_10);

    while (true) {

        float rail_temp_c = analog.get_rail_temp_c();
        float cap_v = analog.get_cap_bank_voltage();

        bool fan_on = CoolingSystem::CoolingSystem::get_instance().get_fan_state();
        bool payload_ready = ChargingStage::ChargingStage::get_instance().get_is_launch_payload_ready();
        bool motor_aligned = RotatingBlock::RotatingBlock::get_instance().get_is_motor_aligned();
        int used_rails = RotatingBlock::RotatingBlock::get_instance().get_num_used_rails();

        int32_t temp_w, temp_f;
        int32_t cap_w, cap_f;

        split_float(rail_temp_c, temp_w, temp_f);
        split_float(cap_v, cap_w, cap_f);

        float enc_angle = RotaryEncoder::get_instance().get_angle_degrees();

        uint8_t active_rail = RotaryEncoder::get_instance().get_rail_index_zero_based();
        float active_temp = UI_CommandReceiver::get_instance().get_remote_temp_c(active_rail);


        int32_t enc_w, enc_f;
        split_float(enc_angle, enc_w, enc_f);

        //int32_t active_temp_w, active_temp_f;
        //split_float(active_temp, active_temp_w, active_temp_f);

        float ir_velocity = IRBreakSensor::get_instance().get_velocity();

        int32_t vel_w, vel_f;
        split_float(ir_velocity, vel_w, vel_f);

        snprintf(msg, sizeof(msg),
                 "TEL temp=%ld.%02ld cap=%ld.%02ld fan=%d ready=%d aligned=%d rails=%d enc=%ld.%02ld active=%d vel=%ld.%02ld\r\n",
                 (long)temp_w, (long)temp_f,
                 (long)cap_w, (long)cap_f,
                 fan_on, payload_ready, motor_aligned, used_rails,
                 (long)enc_w, (long)enc_f, active_rail,
				 (long)vel_w, (long)vel_f);

        UI_CommandReceiver::get_instance().send_to_gui(msg);

        vTaskDelay(pdMS_TO_TICKS(TELEMETRY_PERIOD_MS));
    }
}
