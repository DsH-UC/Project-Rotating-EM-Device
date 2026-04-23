################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Addons/PnPL/Components/Controller_PnPL.c \
../Addons/PnPL/Components/Deviceinformation_PnPL.c \
../Addons/PnPL/Components/Firmware_Info_PnPL.c \
../Addons/PnPL/Components/Inference_Mcu_PnPL.c \
../Addons/PnPL/Components/PnPL_init.c 

C_DEPS += \
./Addons/PnPL/Components/Controller_PnPL.d \
./Addons/PnPL/Components/Deviceinformation_PnPL.d \
./Addons/PnPL/Components/Firmware_Info_PnPL.d \
./Addons/PnPL/Components/Inference_Mcu_PnPL.d \
./Addons/PnPL/Components/PnPL_init.d 

OBJS += \
./Addons/PnPL/Components/Controller_PnPL.o \
./Addons/PnPL/Components/Deviceinformation_PnPL.o \
./Addons/PnPL/Components/Firmware_Info_PnPL.o \
./Addons/PnPL/Components/Inference_Mcu_PnPL.o \
./Addons/PnPL/Components/PnPL_init.o 


# Each subdirectory must supply rules for building sources it contributes
Addons/PnPL/Components/%.o Addons/PnPL/Components/%.su Addons/PnPL/Components/%.cyclo: ../Addons/PnPL/Components/%.c Addons/PnPL/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -I../Addons/PnPL/Components -I../Addons/PnPL/AppModel -I../Middlewares/ST/PnPLCompManager/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Addons-2f-PnPL-2f-Components

clean-Addons-2f-PnPL-2f-Components:
	-$(RM) ./Addons/PnPL/Components/Controller_PnPL.cyclo ./Addons/PnPL/Components/Controller_PnPL.d ./Addons/PnPL/Components/Controller_PnPL.o ./Addons/PnPL/Components/Controller_PnPL.su ./Addons/PnPL/Components/Deviceinformation_PnPL.cyclo ./Addons/PnPL/Components/Deviceinformation_PnPL.d ./Addons/PnPL/Components/Deviceinformation_PnPL.o ./Addons/PnPL/Components/Deviceinformation_PnPL.su ./Addons/PnPL/Components/Firmware_Info_PnPL.cyclo ./Addons/PnPL/Components/Firmware_Info_PnPL.d ./Addons/PnPL/Components/Firmware_Info_PnPL.o ./Addons/PnPL/Components/Firmware_Info_PnPL.su ./Addons/PnPL/Components/Inference_Mcu_PnPL.cyclo ./Addons/PnPL/Components/Inference_Mcu_PnPL.d ./Addons/PnPL/Components/Inference_Mcu_PnPL.o ./Addons/PnPL/Components/Inference_Mcu_PnPL.su ./Addons/PnPL/Components/PnPL_init.cyclo ./Addons/PnPL/Components/PnPL_init.d ./Addons/PnPL/Components/PnPL_init.o ./Addons/PnPL/Components/PnPL_init.su

.PHONY: clean-Addons-2f-PnPL-2f-Components

