/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "Custom/Testing/BlinkTask.hpp"
#include "Custom/Sensors/Thermistor.hpp"
#include "Custom/Sensors/IRBreakSensor.hpp"
#include "Custom/Sensors/RotaryEncoder.hpp"
#include "Custom/IO_Handler.hpp"
#include "Custom/RotatingBlock.hpp"
#include "Custom/ChargingStage.hpp"

#include "Custom/Testing/CoolingSystemTest.hpp"
#include "Custom/Testing/RotatingBlockTest.hpp"
#include "Custom/Testing/ChargingStageTest.hpp"
#include "Custom/Testing/FullSystemTest.hpp"

#include "Custom/Remote GUI/UI_CommandReceiver.hpp"
#include "Custom/Remote GUI/UI_TelemetryTransmitter.hpp"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
BlinkTask statusLed(GPIOA, GPIO_PIN_5, 500);
Thermistor& r = Thermistor::get_instance(&hadc1, ADC_CHANNEL_10);
IRBreakSensor& ir = IRBreakSensor::get_instance();
RotaryEncoder& encoder = RotaryEncoder::get_instance();

CoolingSystem::CoolingSystem& cooling_subsystem = CoolingSystem::CoolingSystem::get_instance();
RotatingBlock::RotatingBlock& rotating_block_subsystem = RotatingBlock::RotatingBlock::get_instance();
ChargingStage::ChargingStage& charging_stage_subsystem = ChargingStage::ChargingStage::get_instance();
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
#ifdef __cplusplus
extern "C" {
#endif

void MX_FREERTOS_Init(void);

#ifdef __cplusplus
}
#endif
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_UART5_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  IO_Handler::init();
  statusLed.start("Blinker", 128, tskIDLE_PRIORITY + 1);

  r.initThermistorThread(256, tskIDLE_PRIORITY + 1);
  ir.init(GPIO_PIN_4, GPIO_PIN_5);
  ir.init_IR_break_thread(128, tskIDLE_PRIORITY + 1);
  encoder.init(&htim4);
  encoder.start_thread(128, tskIDLE_PRIORITY + 2);

  cooling_subsystem.init_fan_gpio(FAN_SW_PIN_GPIO_Port,FAN_SW_PIN_Pin,GPIO_PIN_SET);
  cooling_subsystem.start_cooling_subsystem_thread();
  //CoolingSystemTest::init_cooling_system_test_thread(128, tskIDLE_PRIORITY + 1);


  rotating_block_subsystem.init_feedback_controller();
  rotating_block_subsystem.init_motor_driver(&htim3, TIM_CHANNEL_1, GPIOB, MOTOR_INA_PIN_Pin, GPIOC, MOTOR_INB_PIN_Pin);
  rotating_block_subsystem.start_rotating_block_subsystem_thread();
  //RotatingBlockTest::init_rotating_block_test_thread(256, tskIDLE_PRIORITY + 1);

  charging_stage_subsystem.start_charging_stage_subsystem_thread();
  //ChargingStageTest::init_charging_stage_test_thread(512, tskIDLE_PRIORITY + 1);

  //FullSystemTest::init_manual_testing();
  //FullSystemTest::init_full_system_test_thread(1024, tskIDLE_PRIORITY + 1);

  // Choose UART5 for UI and USART2 for testing
  UI_CommandReceiver::get_instance().init(&huart2);
  UI_CommandReceiver::get_instance().start_thread(512, tskIDLE_PRIORITY + 2);
  UI_TelemetryTransmitter::get_instance().start_thread(256, tskIDLE_PRIORITY + 1);


  // bool ready = cs.get_is_launch_payload_ready();
  // bool aligned = rb.get_is_motor_aligned();
  // bool launch = ready && aligned;

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
