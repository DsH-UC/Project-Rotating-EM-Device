################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/BlinkTask.cpp \
../Core/Src/ChargingStage.cpp \
../Core/Src/ChargingStageTest.cpp \
../Core/Src/CoolingSystem.cpp \
../Core/Src/CoolingSystemTest.cpp \
../Core/Src/FullSystemTest.cpp \
../Core/Src/IO_Handler.cpp \
../Core/Src/IRBreakSensor.cpp \
../Core/Src/RotaryEncoder.cpp \
../Core/Src/RotatingBlock.cpp \
../Core/Src/RotatingBlockTest.cpp \
../Core/Src/Thermistor2.cpp \
../Core/Src/UI_CommandReceiver.cpp \
../Core/Src/UI_TelemetryTransmitter.cpp \
../Core/Src/main.cpp 

C_SRCS += \
../Core/Src/adc.c \
../Core/Src/arm_biquad_cascade_df1_init_f32.c \
../Core/Src/arm_pid_init_f32.c \
../Core/Src/dma.c \
../Core/Src/freertos.c \
../Core/Src/gpio.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_hal_timebase_tim.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/arm_biquad_cascade_df1_init_f32.d \
./Core/Src/arm_pid_init_f32.d \
./Core/Src/dma.d \
./Core/Src/freertos.d \
./Core/Src/gpio.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_hal_timebase_tim.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d 

OBJS += \
./Core/Src/BlinkTask.o \
./Core/Src/ChargingStage.o \
./Core/Src/ChargingStageTest.o \
./Core/Src/CoolingSystem.o \
./Core/Src/CoolingSystemTest.o \
./Core/Src/FullSystemTest.o \
./Core/Src/IO_Handler.o \
./Core/Src/IRBreakSensor.o \
./Core/Src/RotaryEncoder.o \
./Core/Src/RotatingBlock.o \
./Core/Src/RotatingBlockTest.o \
./Core/Src/Thermistor2.o \
./Core/Src/UI_CommandReceiver.o \
./Core/Src/UI_TelemetryTransmitter.o \
./Core/Src/adc.o \
./Core/Src/arm_biquad_cascade_df1_init_f32.o \
./Core/Src/arm_pid_init_f32.o \
./Core/Src/dma.o \
./Core/Src/freertos.o \
./Core/Src/gpio.o \
./Core/Src/main.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_hal_timebase_tim.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o 

CPP_DEPS += \
./Core/Src/BlinkTask.d \
./Core/Src/ChargingStage.d \
./Core/Src/ChargingStageTest.d \
./Core/Src/CoolingSystem.d \
./Core/Src/CoolingSystemTest.d \
./Core/Src/FullSystemTest.d \
./Core/Src/IO_Handler.d \
./Core/Src/IRBreakSensor.d \
./Core/Src/RotaryEncoder.d \
./Core/Src/RotatingBlock.d \
./Core/Src/RotatingBlockTest.d \
./Core/Src/Thermistor2.d \
./Core/Src/UI_CommandReceiver.d \
./Core/Src/UI_TelemetryTransmitter.d \
./Core/Src/main.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.cpp Core/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -I../Core/Custom -IC:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Source/ControllerFunctions -IC:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DSTM32_THREAD_SAFE_STRATEGY=4 -DARM_MATH_CM4 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -IC:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Source/ControllerFunctions -IC:/Users/devhu/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.3/Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/BlinkTask.cyclo ./Core/Src/BlinkTask.d ./Core/Src/BlinkTask.o ./Core/Src/BlinkTask.su ./Core/Src/ChargingStage.cyclo ./Core/Src/ChargingStage.d ./Core/Src/ChargingStage.o ./Core/Src/ChargingStage.su ./Core/Src/ChargingStageTest.cyclo ./Core/Src/ChargingStageTest.d ./Core/Src/ChargingStageTest.o ./Core/Src/ChargingStageTest.su ./Core/Src/CoolingSystem.cyclo ./Core/Src/CoolingSystem.d ./Core/Src/CoolingSystem.o ./Core/Src/CoolingSystem.su ./Core/Src/CoolingSystemTest.cyclo ./Core/Src/CoolingSystemTest.d ./Core/Src/CoolingSystemTest.o ./Core/Src/CoolingSystemTest.su ./Core/Src/FullSystemTest.cyclo ./Core/Src/FullSystemTest.d ./Core/Src/FullSystemTest.o ./Core/Src/FullSystemTest.su ./Core/Src/IO_Handler.cyclo ./Core/Src/IO_Handler.d ./Core/Src/IO_Handler.o ./Core/Src/IO_Handler.su ./Core/Src/IRBreakSensor.cyclo ./Core/Src/IRBreakSensor.d ./Core/Src/IRBreakSensor.o ./Core/Src/IRBreakSensor.su ./Core/Src/RotaryEncoder.cyclo ./Core/Src/RotaryEncoder.d ./Core/Src/RotaryEncoder.o ./Core/Src/RotaryEncoder.su ./Core/Src/RotatingBlock.cyclo ./Core/Src/RotatingBlock.d ./Core/Src/RotatingBlock.o ./Core/Src/RotatingBlock.su ./Core/Src/RotatingBlockTest.cyclo ./Core/Src/RotatingBlockTest.d ./Core/Src/RotatingBlockTest.o ./Core/Src/RotatingBlockTest.su ./Core/Src/Thermistor2.cyclo ./Core/Src/Thermistor2.d ./Core/Src/Thermistor2.o ./Core/Src/Thermistor2.su ./Core/Src/UI_CommandReceiver.cyclo ./Core/Src/UI_CommandReceiver.d ./Core/Src/UI_CommandReceiver.o ./Core/Src/UI_CommandReceiver.su ./Core/Src/UI_TelemetryTransmitter.cyclo ./Core/Src/UI_TelemetryTransmitter.d ./Core/Src/UI_TelemetryTransmitter.o ./Core/Src/UI_TelemetryTransmitter.su ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/arm_biquad_cascade_df1_init_f32.cyclo ./Core/Src/arm_biquad_cascade_df1_init_f32.d ./Core/Src/arm_biquad_cascade_df1_init_f32.o ./Core/Src/arm_biquad_cascade_df1_init_f32.su ./Core/Src/arm_pid_init_f32.cyclo ./Core/Src/arm_pid_init_f32.d ./Core/Src/arm_pid_init_f32.o ./Core/Src/arm_pid_init_f32.su ./Core/Src/dma.cyclo ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/dma.su ./Core/Src/freertos.cyclo ./Core/Src/freertos.d ./Core/Src/freertos.o ./Core/Src/freertos.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_hal_timebase_tim.cyclo ./Core/Src/stm32f4xx_hal_timebase_tim.d ./Core/Src/stm32f4xx_hal_timebase_tim.o ./Core/Src/stm32f4xx_hal_timebase_tim.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src

