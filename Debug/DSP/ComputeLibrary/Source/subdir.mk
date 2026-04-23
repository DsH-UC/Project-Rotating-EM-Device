################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/ComputeLibrary/Source/arm_cl_tables.c 

C_DEPS += \
./DSP/ComputeLibrary/Source/arm_cl_tables.d 

OBJS += \
./DSP/ComputeLibrary/Source/arm_cl_tables.o 


# Each subdirectory must supply rules for building sources it contributes
DSP/ComputeLibrary/Source/arm_cl_tables.o: C:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/ComputeLibrary/Source/arm_cl_tables.c DSP/ComputeLibrary/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -I../Addons/PnPL/Components -I../Addons/PnPL/AppModel -I../Middlewares/ST/PnPLCompManager/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DSP-2f-ComputeLibrary-2f-Source

clean-DSP-2f-ComputeLibrary-2f-Source:
	-$(RM) ./DSP/ComputeLibrary/Source/arm_cl_tables.cyclo ./DSP/ComputeLibrary/Source/arm_cl_tables.d ./DSP/ComputeLibrary/Source/arm_cl_tables.o ./DSP/ComputeLibrary/Source/arm_cl_tables.su

.PHONY: clean-DSP-2f-ComputeLibrary-2f-Source

