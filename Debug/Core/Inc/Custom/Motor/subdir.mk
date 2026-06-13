################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Inc/Custom/Motor/MotorDriver.cpp 

OBJS += \
./Core/Inc/Custom/Motor/MotorDriver.o 

CPP_DEPS += \
./Core/Inc/Custom/Motor/MotorDriver.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Inc/Custom/Motor/%.o Core/Inc/Custom/Motor/%.su Core/Inc/Custom/Motor/%.cyclo: ../Core/Inc/Custom/Motor/%.cpp Core/Inc/Custom/Motor/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -I../Core/Custom -IC:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Source/ControllerFunctions -IC:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Inc-2f-Custom-2f-Motor

clean-Core-2f-Inc-2f-Custom-2f-Motor:
	-$(RM) ./Core/Inc/Custom/Motor/MotorDriver.cyclo ./Core/Inc/Custom/Motor/MotorDriver.d ./Core/Inc/Custom/Motor/MotorDriver.o ./Core/Inc/Custom/Motor/MotorDriver.su

.PHONY: clean-Core-2f-Inc-2f-Custom-2f-Motor

