################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Inc/Custom/Reference\ Classes/thermistor.cpp 

C_SRCS += \
../Core/Inc/Custom/Reference\ Classes/IRBreak.c \
../Core/Inc/Custom/Reference\ Classes/encoder.c \
../Core/Inc/Custom/Reference\ Classes/tim.c 

C_DEPS += \
./Core/Inc/Custom/Reference\ Classes/IRBreak.d \
./Core/Inc/Custom/Reference\ Classes/encoder.d \
./Core/Inc/Custom/Reference\ Classes/tim.d 

OBJS += \
./Core/Inc/Custom/Reference\ Classes/IRBreak.o \
./Core/Inc/Custom/Reference\ Classes/encoder.o \
./Core/Inc/Custom/Reference\ Classes/thermistor.o \
./Core/Inc/Custom/Reference\ Classes/tim.o 

CPP_DEPS += \
./Core/Inc/Custom/Reference\ Classes/thermistor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Inc/Custom/Reference\ Classes/IRBreak.o: ../Core/Inc/Custom/Reference\ Classes/IRBreak.c Core/Inc/Custom/Reference\ Classes/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DSTM32_THREAD_SAFE_STRATEGY=4 -DARM_MATH_CM4 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -IC:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Source/ControllerFunctions -IC:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Core/Inc/Custom/Reference Classes/IRBreak.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Inc/Custom/Reference\ Classes/encoder.o: ../Core/Inc/Custom/Reference\ Classes/encoder.c Core/Inc/Custom/Reference\ Classes/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DSTM32_THREAD_SAFE_STRATEGY=4 -DARM_MATH_CM4 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -IC:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Source/ControllerFunctions -IC:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Core/Inc/Custom/Reference Classes/encoder.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Inc/Custom/Reference\ Classes/thermistor.o: ../Core/Inc/Custom/Reference\ Classes/thermistor.cpp Core/Inc/Custom/Reference\ Classes/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -I../Core/Custom -IC:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Source/ControllerFunctions -IC:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Core/Inc/Custom/Reference Classes/thermistor.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Inc/Custom/Reference\ Classes/tim.o: ../Core/Inc/Custom/Reference\ Classes/tim.c Core/Inc/Custom/Reference\ Classes/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DSTM32_THREAD_SAFE_STRATEGY=4 -DARM_MATH_CM4 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -IC:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Source/ControllerFunctions -IC:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Core/Inc/Custom/Reference Classes/tim.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Inc-2f-Custom-2f-Reference-20-Classes

clean-Core-2f-Inc-2f-Custom-2f-Reference-20-Classes:
	-$(RM) ./Core/Inc/Custom/Reference\ Classes/IRBreak.cyclo ./Core/Inc/Custom/Reference\ Classes/IRBreak.d ./Core/Inc/Custom/Reference\ Classes/IRBreak.o ./Core/Inc/Custom/Reference\ Classes/IRBreak.su ./Core/Inc/Custom/Reference\ Classes/encoder.cyclo ./Core/Inc/Custom/Reference\ Classes/encoder.d ./Core/Inc/Custom/Reference\ Classes/encoder.o ./Core/Inc/Custom/Reference\ Classes/encoder.su ./Core/Inc/Custom/Reference\ Classes/thermistor.cyclo ./Core/Inc/Custom/Reference\ Classes/thermistor.d ./Core/Inc/Custom/Reference\ Classes/thermistor.o ./Core/Inc/Custom/Reference\ Classes/thermistor.su ./Core/Inc/Custom/Reference\ Classes/tim.cyclo ./Core/Inc/Custom/Reference\ Classes/tim.d ./Core/Inc/Custom/Reference\ Classes/tim.o ./Core/Inc/Custom/Reference\ Classes/tim.su

.PHONY: clean-Core-2f-Inc-2f-Custom-2f-Reference-20-Classes

