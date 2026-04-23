################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/startup_ARMCM0.c \
C:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/system_ARMCM0.c 

C_DEPS += \
./DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/startup_ARMCM0.d \
./DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/system_ARMCM0.d 

OBJS += \
./DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/startup_ARMCM0.o \
./DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/system_ARMCM0.o 


# Each subdirectory must supply rules for building sources it contributes
DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/startup_ARMCM0.o: C:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/startup_ARMCM0.c DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -I../Addons/PnPL/Components -I../Addons/PnPL/AppModel -I../Middlewares/ST/PnPLCompManager/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/system_ARMCM0.o: C:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/system_ARMCM0.c DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -I../Addons/PnPL/Components -I../Addons/PnPL/AppModel -I../Middlewares/ST/PnPLCompManager/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DSP-2f-Examples-2f-ARM-2f-arm_convolution_example-2f-RTE-2f-Device-2f-ARMCM0

clean-DSP-2f-Examples-2f-ARM-2f-arm_convolution_example-2f-RTE-2f-Device-2f-ARMCM0:
	-$(RM) ./DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/startup_ARMCM0.cyclo ./DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/startup_ARMCM0.d ./DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/startup_ARMCM0.o ./DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/startup_ARMCM0.su ./DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/system_ARMCM0.cyclo ./DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/system_ARMCM0.d ./DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/system_ARMCM0.o ./DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM0/system_ARMCM0.su

.PHONY: clean-DSP-2f-Examples-2f-ARM-2f-arm_convolution_example-2f-RTE-2f-Device-2f-ARMCM0

