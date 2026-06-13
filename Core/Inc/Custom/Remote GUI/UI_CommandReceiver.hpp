/*
 * UICommandReceiver.hpp
 *
 *  Created on: May 3, 2026
 *      Author: devhu
 */

#ifndef CUSTOM_REMOTE_GUI_UI_COMMANDRECEIVER_HPP_
#define CUSTOM_REMOTE_GUI_UI_COMMANDRECEIVER_HPP_

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <cstdint>
#include <cstdio>
#include <cstring>

class UI_CommandReceiver {
public:
    static UI_CommandReceiver& get_instance();
    void send_to_gui(const char* msg);

    void init(UART_HandleTypeDef* huart);
    void start_thread(uint16_t stackSize, UBaseType_t priority);

    // Called from HAL_UART_RxCpltCallback
    void on_uart_rx_callback(UART_HandleTypeDef* huart);

    // Rotary thermistor getters
    float get_remote_temp_c(uint8_t index);
    uint32_t get_remote_temp_sequence();

private:
    UI_CommandReceiver();

    static void runStatic(void* params);
    void task_loop();

    void process_command(char cmd);
    void send_response(const char* msg);

    UART_HandleTypeDef* m_huart;

    QueueHandle_t m_rx_queue;
    TaskHandle_t m_task_handle;
    uint8_t m_rx_byte;

    static constexpr uint16_t RX_QUEUE_LENGTH = 32;

    SemaphoreHandle_t m_tx_mutex;

    // Rotary thermistor parsing
    static constexpr uint16_t LINE_BUFFER_SIZE = 128;
    char m_line_buffer[LINE_BUFFER_SIZE];
    uint16_t m_line_index;

    static constexpr uint8_t NUM_REMOTE_THERMISTORS = 6; // More rotary thermistor parsing
    float m_remote_temp_c[NUM_REMOTE_THERMISTORS];
    uint32_t m_remote_temp_sequence;
    SemaphoreHandle_t m_remote_temp_mutex;
    void process_rtemp_line(const char* line);

    void process_line(const char* line);

    // Prevent copying
    UI_CommandReceiver(const UI_CommandReceiver&) = delete;
    UI_CommandReceiver& operator=(const UI_CommandReceiver&) = delete;

};


#endif /* CUSTOM_REMOTE_GUI_UI_COMMANDRECEIVER_HPP_ */
