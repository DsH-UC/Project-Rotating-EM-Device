/**
  ******************************************************************************
  * @file    Controller_PnPL.c
  * @author  SRA
  * @brief   Controller PnPL Component Manager
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

#include "Controller_PnPL.h"

static const IPnPLComponent_vtbl sController_PnPL_CompIF_vtbl =
{
  Controller_PnPL_vtblGetKey,
  Controller_PnPL_vtblGetNCommands,
  Controller_PnPL_vtblGetCommandKey,
  Controller_PnPL_vtblGetStatus,
  Controller_PnPL_vtblSetProperty,
  Controller_PnPL_vtblExecuteFunction
};

/**
  *  Controller_PnPL internal structure.
  */
struct _Controller_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Controller_PnPL sController_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Controller_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sController_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sController_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Controller_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  controller_comp_init();
  return PNPL_NO_ERROR_CODE;
}

/* IPnPLComponent virtual functions definition -------------------------------*/
char *Controller_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return controller_get_key();
}

uint8_t Controller_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 2;
}

char *Controller_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  switch (id)
  {
    case 0:
      return "controller*set_dfu_mode";
      break;
    case 1:
      return "controller*switch_bank";
      break;
  }
  return (char*)PNPL_NO_ERROR_CODE;
}

uint8_t Controller_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;

  tempJSON = json_value_init_object();

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

uint8_t Controller_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON)
{
  return PNPL_NO_ERROR_CODE;
}

uint8_t Controller_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (json_object_dothas_value(tempJSONObject, "controller*set_dfu_mode"))
  {
    ret = controller_set_dfu_mode();
  }
  if (json_object_dothas_value(tempJSONObject, "controller*switch_bank"))
  {
    ret = controller_switch_bank();
  }
  json_value_free(tempJSON);
  return ret;
}

