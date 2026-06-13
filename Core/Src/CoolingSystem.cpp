/*
 * CoolingSystem.cpp
 *
 *  Created on: Mar 12, 2026
 *      Author: devhu
 */

#include "Custom/CoolingSystem.hpp"
#include "Custom/Priority Tables/ThreadPriorityTable.hpp"
#include "Custom/IO_Handler.hpp"
#include "Custom/Remote GUI/UI_CommandReceiver.hpp"
#include <cmath>

namespace CoolingSystem {

	/*
	 * Internal namespace for compile time constants
	 */
	namespace Constants {
		constexpr int STACK_SIZE = 512;
		constexpr int COOLING_SYS_THRESHOLD = 23; // Deg. Celsius
		constexpr bool STATE_FAN_OFF = false;
		constexpr bool STATE_FAN_ON = true;
		constexpr int THREAD_BLOCK_DURATION_MS = 50;

		GPIO_TypeDef* FAN_GPIO_PORT = FAN_SW_PIN_GPIO_Port;
		constexpr uint16_t FAN_GPIO_PIN = FAN_SW_PIN_Pin;
	}

	/*
	 * Constructor and object instantiation functions
	 */
	CoolingSystem& CoolingSystem::get_instance(){
		static CoolingSystem instance;
		return instance;
	}

	CoolingSystem::CoolingSystem()
	: m_cs_task_handle(nullptr),
	  m_ui_fan_toggle_semaphore(nullptr),
	  m_testing_mutex(nullptr),
	  m_test_mode(false),
	  m_mock_temp(25.0f),
	  m_mock_ui_toggle(false),
	  m_fan_on(false),
	  m_fan_gpio_port(nullptr),
	  m_fan_gpio_pin(0),
	  m_fan_active_state(GPIO_PIN_SET)
	{
		printf("Cooling System Subsystem online.\n");
	}
	CoolingSystem::~CoolingSystem() { printf("Cooling System Subsystem offline.\n"); }

	/*
	 * Helper read function to access thermistor temperature from its thread
	 */
	float CoolingSystem::read_sensor(int gpio_pin){ // TODO
		// 12-bit ADC, mask with & 0xFFF
		//return Thermistor_ReadTempC(uint32_t adc_channel, uint16_t samples);
		// execute getTemperature() from Thermistor2 here
		return 25.0;
	};

	/*
	 * Thread functions for initialization and thread loop implementation
	 */
	void CoolingSystem::runStatic(void* params) {
	    // Bridge back to the class instance
	    static_cast<CoolingSystem*>(params)->cooling_system_task(nullptr);
	}

	void CoolingSystem::start_cooling_subsystem_thread(void) {
		m_ui_fan_toggle_semaphore = xSemaphoreCreateBinary();
		m_testing_mutex = xSemaphoreCreateMutex();

		xTaskCreate(
				runStatic, // Thread function
				"Cooling System Thread",
				Constants::STACK_SIZE, //
				this, // pvParameters
				ThreadPriorityTable::COOLING_SYS_PRIORITY, //Thread Priority
				&(this->m_cs_task_handle)
		);
	}

	/* Charging Stage FSM
	   Inputs: Capacitor Bank Voltage Sensor (float), UI Launch Button (bool), Is Motor Aligned (bool),
	 		   Reset / Discharge (bool), Gate Launch / Consumed Rail Count (int)
	   Outputs: Ready to Launch Payload (bool)
	*/
	void CoolingSystem::cooling_system_task(void* pv_parameters) {

		// Cooling System FSM Implementation:
		bool state = Constants::STATE_FAN_OFF; // Initial state
		bool manual_fan_enabled = false;

		while(true){
			// Block thread for up to 50ms while waiting for ui fan button press
			bool is_button_pressed = (xSemaphoreTake(
				m_ui_fan_toggle_semaphore,
				pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS)) == pdTRUE);

			float temp = 0.0f;
			bool ui_toggle = false;
			bool test_mode_snapshot = false;

			if (xSemaphoreTake(m_testing_mutex, portMAX_DELAY) == pdTRUE) {
			    test_mode_snapshot = m_test_mode;

			    if (m_test_mode) {
			        temp = m_mock_temp;
			        ui_toggle = is_button_pressed || m_mock_ui_toggle;
			        m_mock_ui_toggle = false;
			    } else {
			        temp = 0.0f;
			        ui_toggle = is_button_pressed;
			    }

			    xSemaphoreGive(m_testing_mutex);
			}

			bool is_too_hot = false;

			if (test_mode_snapshot) {
			    is_too_hot = temp >= Constants::COOLING_SYS_THRESHOLD;
			} else {
			    is_too_hot = is_any_rail_hot();
			}

//			if (ui_toggle) state = !state;
//			else if (is_too_hot) state = Constants::STATE_FAN_ON;
//			else state = Constants::STATE_FAN_OFF;

			// Latch logic to make fans manual unless its too hot
			if (ui_toggle) manual_fan_enabled = !manual_fan_enabled;
			state = (manual_fan_enabled || is_too_hot)
					? Constants::STATE_FAN_ON : Constants::STATE_FAN_OFF;

			// Write guard for o/p variable
			if (xSemaphoreTake(m_testing_mutex, portMAX_DELAY) == pdTRUE) {
				m_fan_on = state;
				xSemaphoreGive(m_testing_mutex);
			}

			// Fan actuation: Write fan GPIO/PWM here based on SM on/off state
			write_fan_output(state);
			vTaskDelay(pdMS_TO_TICKS(Constants::THREAD_BLOCK_DURATION_MS));
		}
	}

	/*
	 * UI Fan Button ISR handler / On EXTI callback:  Block thread loop until GUI button is pressed
	 */
	void CoolingSystem::ui_fan_toggle_ISR(uint16_t gpio_pin){ // TODO: Still need to get the interrupt condition and clear it
		if(gpio_pin == GPIO_PIN_8){
			BaseType_t xHigherPriorityTaskWoken = pdFALSE;

			// Preepmt thread from Running State -> Ready state
			xSemaphoreGiveFromISR(m_ui_fan_toggle_semaphore, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}


	/*
	 * Public read for SM output w/ read guard
	 */
	bool CoolingSystem::get_fan_state(){

	    bool state = false; // temp variable for read guard

	    if (xSemaphoreTake(m_testing_mutex, portMAX_DELAY) == pdTRUE) {
	        state = m_fan_on;
	        xSemaphoreGive(m_testing_mutex);
	    }

	    return state;
	}


	/*
	 * Public test mode functions to enable testing mode and setting mock inputs
	 */
	void CoolingSystem::set_test_mode(bool enable) {

		// Write guard into m_test_mode
	    if (xSemaphoreTake(m_testing_mutex, portMAX_DELAY) == pdTRUE) {
	        m_test_mode = enable;
	        xSemaphoreGive(m_testing_mutex);
	    }
	}

	void CoolingSystem::set_mock_inputs(float temp_c, bool ui_toggle) {

		// Write guard into the other two testing variables
	    if (xSemaphoreTake(m_testing_mutex, portMAX_DELAY) == pdTRUE) {
	        m_mock_temp = temp_c;
	        m_mock_ui_toggle = ui_toggle;
	        xSemaphoreGive(m_testing_mutex);
	    }
	}

	void CoolingSystem::simulate_button_press() {
		// Give to button press semaphore to simulate a button press
		xSemaphoreGive(m_ui_fan_toggle_semaphore);
	}


	/*
	 * Fan actuation functions to set its output GPIO pin and write to it
	 */
	void CoolingSystem::init_fan_gpio(GPIO_TypeDef* port, uint16_t pin, GPIO_PinState active_state) {
	    m_fan_gpio_port = port;
	    m_fan_gpio_pin = pin;
	    m_fan_active_state = active_state;

	    write_fan_output(false); // fan starts OFF
	}

	void CoolingSystem::write_fan_output(bool fan_on) {

		if (m_fan_gpio_port == nullptr) return;
		// Set ON condition depending on active low or active high requirement
	    GPIO_PinState inactive_state = (m_fan_active_state == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET;
	    HAL_GPIO_WritePin(m_fan_gpio_port, m_fan_gpio_pin, fan_on ? m_fan_active_state : inactive_state);
	}

	bool CoolingSystem::is_any_rail_hot() {
	    for (uint8_t i = 0; i < 6; i++) {
	        float temp = UI_CommandReceiver::get_instance().get_remote_temp_c(i);

	        // Ignore invalid/not-yet-received thermistor values
	        if (std::isnan(temp)) {
	            continue;
	        }

	        if (temp >= Constants::COOLING_SYS_THRESHOLD) {
	            return true;
	        }
	    }

	    return false;
	}

}




