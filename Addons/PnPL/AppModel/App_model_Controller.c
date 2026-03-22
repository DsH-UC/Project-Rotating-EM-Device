/**
  ******************************************************************************
  * @file    App_model_Controller.c
  * @author  SRA
  * @brief   Controller PnPL Components APIs
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

/* Controller PnPL Component -------------------------------------------------*/
uint8_t controller_comp_init(void)
{

  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *controller_get_key(void)
{
  return "controller";
}

uint8_t controller_set_dfu_mode(void)
{
  jump_to_bootloader();
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t controller_switch_bank(void)
{
  ToggleFlashBank();
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

