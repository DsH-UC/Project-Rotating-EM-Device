/*
 * UICommandReceiver.cpp
 *
 *  Created on: May 3, 2026
 *      Author: devhu
 */

#include <Custom/Remote GUI/UI_CommandReceiver.hpp>
#include "Custom/CoolingSystem.hpp"
#include "Custom/RotatingBlock.hpp"
#include "Custom/ChargingStage.hpp"
#include "Custom/IO_Handler.hpp"
#include "Custom/Testing/FullSystemTest.hpp"
#include "Custom/Sensors/Thermistor.hpp"
#include <cmath>

UI_CommandReceiver::UI_CommandReceiver()
: m_huart(nullptr),
  m_rx_queue(nullptr),
  m_task_handle(nullptr),
  m_rx_byte(0),
  m_tx_mutex(nullptr),
  m_line_index(0),
  m_remote_temp_sequence(0),
  m_remote_temp_mutex(nullptr){

	m_line_buffer[0] = '\0';
	for (uint8_t i = 0; i < NUM_REMOTE_THERMISTORS; i++) m_remote_temp_c[i] = NAN;

}

UI_CommandReceiver& UI_CommandReceiver::get_instance() {
    static UI_CommandReceiver instance;
    return instance;
}

// Thread safe UART Transmit to ESP32 to protect against concurrent write from telemetry
// and other status response getters
void UI_CommandReceiver::send_to_gui(const char* msg) {
    if (m_huart == nullptr || msg == nullptr || m_tx_mutex == nullptr) return;

    if (xSemaphoreTake(m_tx_mutex, portMAX_DELAY) == pdTRUE) {
        HAL_UART_Transmit(
            m_huart,
            reinterpret_cast<uint8_t*>(const_cast<char*>(msg)),
            strlen(msg),
            pdMS_TO_TICKS(50));

        xSemaphoreGive(m_tx_mutex);
    }
}

void UI_CommandReceiver::init(UART_HandleTypeDef* huart) {
    m_huart = huart;

    m_rx_queue = xQueueCreate(RX_QUEUE_LENGTH, sizeof(uint8_t));
    m_tx_mutex = xSemaphoreCreateMutex();
    m_remote_temp_mutex = xSemaphoreCreateMutex();

    HAL_UART_Receive_IT(m_huart, &m_rx_byte, 1); // Check first receive interrupt
}

void UI_CommandReceiver::start_thread(uint16_t stackSize, UBaseType_t priority) {
    xTaskCreate(UI_CommandReceiver::runStatic, "UICommandRX", stackSize, this, priority, &m_task_handle);
}

void UI_CommandReceiver::runStatic(void* params){ static_cast<UI_CommandReceiver*>(params)->task_loop(); }

void UI_CommandReceiver::on_uart_rx_callback(UART_HandleTypeDef* huart) {
    if (huart != m_huart || m_rx_queue == nullptr) return;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    uint8_t byte = m_rx_byte;

    xQueueSendFromISR(m_rx_queue, &byte, &xHigherPriorityTaskWoken);

    HAL_UART_Receive_IT(m_huart, &m_rx_byte, 1);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void UI_CommandReceiver::task_loop() {
    uint8_t byte = 0;
    while (true) {
        if (xQueueReceive(m_rx_queue, &byte, portMAX_DELAY) == pdTRUE) {
            char c = static_cast<char>(byte);

			if (c == '\r') {
				continue;
			}

			if (c == '\n') {
				m_line_buffer[m_line_index] = '\0';

				if (m_line_index > 0) {
					process_line(m_line_buffer);
				}

				m_line_index = 0;
				m_line_buffer[0] = '\0';
				continue;
			}

			if (m_line_index < LINE_BUFFER_SIZE - 1) {
				m_line_buffer[m_line_index++] = c;
			} else {
				// Overflow: reset line
				m_line_index = 0;
				m_line_buffer[0] = '\0';
			}

            // if (c == '\n' || c == '\r') continue; // Ignore line endings; process_command(c);
        }
    }
}

void UI_CommandReceiver::process_command(char cmd) {
    switch (cmd) {
    case 'F':
    case 'f':
        IO_Handler::uart_print("UI CMD: Fan toggle\r\n");
        CoolingSystem::CoolingSystem::get_instance().simulate_button_press();
        send_response("ACK fan_toggle\r\n");
        break;

    case 'L':
    case 'l':
        IO_Handler::uart_print("UI CMD: Launch request\r\n");

        // Launch request goes to both relevant subsystems
        RotatingBlock::RotatingBlock::get_instance().simulate_button_press();
        ChargingStage::ChargingStage::get_instance().simulate_button_press();
        send_response("ACK launch_request\r\n");
        break;

    case 'R':
    case 'r':
        IO_Handler::uart_print("UI CMD: Reset/discharge TODO\r\n");
        // TODO: Add reset behavior later
        send_response("ACK reset_discharge TODO\r\n");
        break;

    case 'S':
    case 's': {

    	IO_Handler::uart_print("UI CMD: Status request\r\n");
    	auto& analog = Thermistor::get_instance(nullptr, 0);

    	float rail_temp = analog.get_rail_temp_c();
    	float cap_v = analog.get_cap_bank_voltage();

    	int32_t temp_whole = (int32_t)rail_temp;
    	int32_t temp_frac  = (int32_t)((rail_temp - temp_whole) * 100);
    	if (temp_frac < 0) temp_frac = -temp_frac;

    	int32_t cap_whole = (int32_t)cap_v;
    	int32_t cap_frac  = (int32_t)((cap_v - cap_whole) * 100);
    	if (cap_frac < 0) cap_frac = -cap_frac;

		bool fan_on = CoolingSystem::CoolingSystem::get_instance().get_fan_state();
		bool payload_ready = ChargingStage::ChargingStage::get_instance().get_is_launch_payload_ready();
		bool motor_aligned = RotatingBlock::RotatingBlock::get_instance().get_is_motor_aligned();
		int used_rails = RotatingBlock::RotatingBlock::get_instance().get_num_used_rails();

		char response[192];
		snprintf(response, sizeof(response),
				 "STATUS fan=%d ready=%d aligned=%d rails=%d temp=%ld.%02ld cap=%ld.%02ld\r\n",
				 fan_on,
				 payload_ready,
				 motor_aligned,
				 used_rails,
				 (long)temp_whole,
				 (long)temp_frac,
				 (long)cap_whole,
				 (long)cap_frac);

		send_response(response);
		break;
    }
    case 'N':
    case 'n':
        IO_Handler::uart_print("UI CMD: Next test step\r\n");
        FullSystemTest::next_test_step_from_ui();
        break;
    default: {
    	char msg[96];
		snprintf(msg, sizeof(msg),
				 "UI CMD: Unknown command hex=0x%02X\r\n",
				 static_cast<unsigned char>(cmd));
		IO_Handler::uart_print(msg);
		break;
    }
    }
}

void UI_CommandReceiver::send_response(const char* msg) { send_to_gui(msg); }

void UI_CommandReceiver::process_line(const char* line) {
    if (line == nullptr || line[0] == '\0') return;

    // Single-character commands: F, L, R, S, N
    if (line[1] == '\0') {
        process_command(line[0]);
        return;
    }

    // Rotary ESP32 thermistor packet
    if (strncmp(line, "RTEMP", 5) == 0) {
//        float temp = 0.0f;
//        float v_adc = 0.0f;
//        unsigned int raw = 0;
//        unsigned long seq = 0;
//
//        int parsed = sscanf(line, "RTEMP seq=%lu temp=%f raw=%u v=%f", &seq, &temp, &raw, &v_adc);
//
//        if (parsed >= 2) {
//            char msg[128];
//            snprintf(msg, sizeof(msg), "STM32 RX RTEMP seq=%lu temp=%ld.%02ld raw=%u\r\n",
//                     seq, (long)temp, (long)((temp - (long)temp) * 100.0f), raw);
//            IO_Handler::uart_print(msg); // TODO later: store temp into RemoteSensorData class / variable
//        }
    	process_rtemp_line(line);
        return;

    }

    char msg[128];
    snprintf(msg, sizeof(msg), "Unknown line: %s\r\n", line);
    IO_Handler::uart_print(msg);
}

void UI_CommandReceiver::process_rtemp_line(const char* line)
{
    unsigned long seq = 0;
    float t0 = NAN, t1 = NAN, t2 = NAN, t3 = NAN, t4 = NAN, t5 = NAN;

    int parsed = sscanf(line, "RTEMP seq=%lu t0=%f t1=%f t2=%f t3=%f t4=%f t5=%f",
    					&seq, &t0, &t1, &t2, &t3, &t4, &t5);
    if (parsed >= 2) {
        if (xSemaphoreTake(m_remote_temp_mutex, portMAX_DELAY) == pdTRUE) {
            m_remote_temp_sequence = static_cast<uint32_t>(seq);

            m_remote_temp_c[0] = t0; m_remote_temp_c[1] = t1;
            m_remote_temp_c[2] = t2; m_remote_temp_c[3] = t3;
            m_remote_temp_c[4] = t4; m_remote_temp_c[5] = t5;

            xSemaphoreGive(m_remote_temp_mutex);
        }

        char msg[128];
        snprintf(msg, sizeof(msg), "STM32 stored RTEMP seq=%lu t0=%ld.%02ld\r\n",
                 seq, (long)t0, (long)((t0 - (long)t0) * 100.0f));

        IO_Handler::uart_print(msg);
    } else {
        IO_Handler::uart_print("Bad RTEMP parse\r\n");
    }
}

float UI_CommandReceiver::get_remote_temp_c(uint8_t index) {
    if (index >= NUM_REMOTE_THERMISTORS || m_remote_temp_mutex == nullptr) return NAN;

    float value = NAN;

    if (xSemaphoreTake(m_remote_temp_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        value = m_remote_temp_c[index];
        xSemaphoreGive(m_remote_temp_mutex);
    }
    return value;
}

uint32_t UI_CommandReceiver::get_remote_temp_sequence() {
    if (m_remote_temp_mutex == nullptr) return 0;

    uint32_t seq = 0;

    if (xSemaphoreTake(m_remote_temp_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        seq = m_remote_temp_sequence;
        xSemaphoreGive(m_remote_temp_mutex);
    }
    return seq;
}
