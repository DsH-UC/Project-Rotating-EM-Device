/**
  ******************************************************************************
  * @file    Inference_Mcu_PnPL.c
  * @author  SRA
  * @brief   Inference_Mcu PnPL Component Manager
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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "App_model.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"
#include "PnPLCompManager.h"

#include "Inference_Mcu_PnPL.h"

static const IPnPLComponent_vtbl sInference_Mcu_PnPL_CompIF_vtbl =
{
  Inference_Mcu_PnPL_vtblGetKey,
  Inference_Mcu_PnPL_vtblGetNCommands,
  Inference_Mcu_PnPL_vtblGetCommandKey,
  Inference_Mcu_PnPL_vtblGetStatus,
  Inference_Mcu_PnPL_vtblSetProperty,
  Inference_Mcu_PnPL_vtblExecuteFunction
};

/**
  *  Inference_Mcu_PnPL internal structure.
  */
struct _Inference_Mcu_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Inference_Mcu_PnPL sInference_Mcu_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Inference_Mcu_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sInference_Mcu_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sInference_Mcu_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Inference_Mcu_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  inference_mcu_comp_init();
  return PNPL_NO_ERROR_CODE;
}

/* IPnPLComponent virtual functions definition -------------------------------*/
char *Inference_Mcu_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return inference_mcu_get_key();
}

uint8_t Inference_Mcu_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 2;
}

char *Inference_Mcu_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  switch (id)
  {
    case 0:
      return "inference_mcu*start_inference";
      break;
    case 1:
      return "inference_mcu*stop_inference";
      break;
  }
  return (char*)PNPL_NO_ERROR_CODE;
}

uint8_t Inference_Mcu_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  bool temp_b = 0;
  inference_mcu_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "inference_mcu.enable", temp_b);
  char *temp_s = "";
  inference_mcu_get_model_filename(&temp_s);
  json_object_dotset_string(JSON_Status, "inference_mcu.model_filename", temp_s);

  if (pretty == 1)
  {
    *serializedJSON = json_serialize_to_string_pretty(tempJSON);
    *size = json_serialization_size_pretty(tempJSON);
  }
  else
  {
    *serializedJSON = json_serialize_to_string(tempJSON);
    *size = json_serialization_size(tempJSON);
  }

  /* No need to free temp_j as it is part of tempJSON */
  json_value_free(tempJSON);

  return PNPL_NO_ERROR_CODE;
}

uint8_t Inference_Mcu_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON)
{
  return PNPL_NO_ERROR_CODE;
}

uint8_t Inference_Mcu_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (json_object_dothas_value(tempJSONObject, "inference_mcu*start_inference"))
  {
    ret = inference_mcu_start_inference();
  }
  if (json_object_dothas_value(tempJSONObject, "inference_mcu*stop_inference"))
  {
    ret = inference_mcu_stop_inference();
  }
  json_value_free(tempJSON);
  return ret;
}

