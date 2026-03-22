/**
  ******************************************************************************
  * @file    App_model_Inference_Mcu.c
  * @author  SRA
  * @brief   Inference_Mcu PnPL Components APIs
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

/**
  ******************************************************************************
  * This file has been auto generated from the following DTDL Component:
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "App_model.h"

/* USER includes -------------------------------------------------------------*/

/* USER private function prototypes ------------------------------------------*/

/* USER defines --------------------------------------------------------------*/

/* Inference_MCU PnPL Component ----------------------------------------------*/
uint8_t inference_mcu_comp_init(void)
{

  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *inference_mcu_get_key(void)
{
  return "inference_mcu";
}

uint8_t inference_mcu_get_enable(bool *value)
{
  if(mcu_inference){
		*value = true;
	}
	else{
		*value = false;
	}
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t inference_mcu_get_model_filename(char **value)
{
  *value = AI_NETWORK_ORIGIN_MODEL_NAME;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t inference_mcu_start_inference(void)
{
  default_ai = MCU;
	sensor_init();
	/* Set the FIFO to Continuous/Stream Mode. */
	MY_CUSTOM_MOTION_SENSOR_FIFO_Set_Mode(SENSOR_0,  SENSOR_0_STREAM_MODE); /* Set the FIFO to Continuous/Stream Mode */
	mcu_inference = 1;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t inference_mcu_stop_inference(void)
{
  /* Set the FIFO to Bypass Mode. */
	MY_CUSTOM_MOTION_SENSOR_FIFO_Set_Mode(SENSOR_0,  SENSOR_0_BYPASS_MODE); /* Set the FIFO to Bypass Mode */
	mcu_inference = 0;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t inference_mcu_create_telemetry(int label_id, float accuracy, char **telemetry, uint32_t *size)
{
  PnPLTelemetry_t telemetries[2];
  strcpy(telemetries[0].telemetry_name, "label_id");
  telemetries[0].telemetry_value = (void *)& label_id;
  telemetries[0].telemetry_type = PNPL_INT;
  telemetries[0].n_sub_telemetries = 0;
  strcpy(telemetries[1].telemetry_name, "accuracy");
  telemetries[1].telemetry_value = (void *)& accuracy;
  telemetries[1].telemetry_type = PNPL_FLOAT;
  telemetries[1].n_sub_telemetries = 0;

  PnPLSerializeTelemetry("inference_mcu", telemetries, 2, telemetry, size, 0);
  return PNPL_NO_ERROR_CODE;
}
