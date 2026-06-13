/*
 * UITelemetryTransmitter.hpp
 *
 *  Created on: May 17, 2026
 *      Author: devhu
 */

#ifndef INC_CUSTOM_REMOTE_GUI_UI_TELEMETRYTRANSMITTER_HPP_
#define INC_CUSTOM_REMOTE_GUI_UI_TELEMETRYTRANSMITTER_HPP_

#include "FreeRTOS.h"
#include "task.h"
#include <cstdint>

class UI_TelemetryTransmitter {
public:
    static UI_TelemetryTransmitter& get_instance();

    void start_thread(uint16_t stackSize, UBaseType_t priority);

private:
    UI_TelemetryTransmitter();

    static void runStatic(void* params);
    void task_loop();

    TaskHandle_t m_task_handle;
};

#endif /* INC_CUSTOM_REMOTE_GUI_UI_TELEMETRYTRANSMITTER_HPP_ */
