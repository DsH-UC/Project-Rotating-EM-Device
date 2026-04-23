################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Addons/PnPL/AppModel/App_model.c \
../Addons/PnPL/AppModel/App_model_Controller.c \
../Addons/PnPL/AppModel/App_model_Deviceinformation.c \
../Addons/PnPL/AppModel/App_model_Firmware_Info.c \
../Addons/PnPL/AppModel/App_model_Inference_Mcu.c 

C_DEPS += \
./Addons/PnPL/AppModel/App_model.d \
./Addons/PnPL/AppModel/App_model_Controller.d \
./Addons/PnPL/AppModel/App_model_Deviceinformation.d \
./Addons/PnPL/AppModel/App_model_Firmware_Info.d \
./Addons/PnPL/AppModel/App_model_Inference_Mcu.d 

OBJS += \
./Addons/PnPL/AppModel/App_model.o \
./Addons/PnPL/AppModel/App_model_Controller.o \
./Addons/PnPL/AppModel/App_model_Deviceinformation.o \
./Addons/PnPL/AppModel/App_model_Firmware_Info.o \
./Addons/PnPL/AppModel/App_model_Inference_Mcu.o 


# Each subdirectory must supply rules for building sources it contributes
Addons/PnPL/AppModel/%.o Addons/PnPL/AppModel/%.su Addons/PnPL/AppModel/%.cyclo: ../Addons/PnPL/AppModel/%.c Addons/PnPL/AppModel/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -I../Addons/PnPL/Components -I../Addons/PnPL/AppModel -I../Middlewares/ST/PnPLCompManager/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Addons-2f-PnPL-2f-AppModel

clean-Addons-2f-PnPL-2f-AppModel:
	-$(RM) ./Addons/PnPL/AppModel/App_model.cyclo ./Addons/PnPL/AppModel/App_model.d ./Addons/PnPL/AppModel/App_model.o ./Addons/PnPL/AppModel/App_model.su ./Addons/PnPL/AppModel/App_model_Controller.cyclo ./Addons/PnPL/AppModel/App_model_Controller.d ./Addons/PnPL/AppModel/App_model_Controller.o ./Addons/PnPL/AppModel/App_model_Controller.su ./Addons/PnPL/AppModel/App_model_Deviceinformation.cyclo ./Addons/PnPL/AppModel/App_model_Deviceinformation.d ./Addons/PnPL/AppModel/App_model_Deviceinformation.o ./Addons/PnPL/AppModel/App_model_Deviceinformation.su ./Addons/PnPL/AppModel/App_model_Firmware_Info.cyclo ./Addons/PnPL/AppModel/App_model_Firmware_Info.d ./Addons/PnPL/AppModel/App_model_Firmware_Info.o ./Addons/PnPL/AppModel/App_model_Firmware_Info.su ./Addons/PnPL/AppModel/App_model_Inference_Mcu.cyclo ./Addons/PnPL/AppModel/App_model_Inference_Mcu.d ./Addons/PnPL/AppModel/App_model_Inference_Mcu.o ./Addons/PnPL/AppModel/App_model_Inference_Mcu.su

.PHONY: clean-Addons-2f-PnPL-2f-AppModel

