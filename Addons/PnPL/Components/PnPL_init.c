/**
  ******************************************************************************
  * @file    PnPL_init.c
  * @author  SRA
  * @brief   PnPL Components initialization functions
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
  * This file has been auto generated from the following Device Template Model:
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "PnPL_init.h"

static IPnPLComponent_t *pInference_Mcu_PnPLObj = NULL;
static IPnPLComponent_t *pController_PnPLObj = NULL;
static IPnPLComponent_t *pFirmware_Info_PnPLObj = NULL;
static IPnPLComponent_t *pDeviceinformation_PnPLObj = NULL;

uint8_t PnPL_Components_Alloc(void)
{
  /* PnPL Components Allocation */
  pInference_Mcu_PnPLObj = Inference_Mcu_PnPLAlloc();
  pController_PnPLObj = Controller_PnPLAlloc();
  pFirmware_Info_PnPLObj = Firmware_Info_PnPLAlloc();
  pDeviceinformation_PnPLObj = Deviceinformation_PnPLAlloc();
  return PNPL_NO_ERROR_CODE;
}

uint8_t PnPL_Components_Init(void)
{
  /* Init&Add PnPL Components */
  Inference_Mcu_PnPLInit(pInference_Mcu_PnPLObj);
  Controller_PnPLInit(pController_PnPLObj);
  Firmware_Info_PnPLInit(pFirmware_Info_PnPLObj);
  Deviceinformation_PnPLInit(pDeviceinformation_PnPLObj);
  return PNPL_NO_ERROR_CODE;
}
