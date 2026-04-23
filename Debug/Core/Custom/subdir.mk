################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Custom/ChargingStage.cpp \
../Core/Custom/CoolingSystem.cpp \
../Core/Custom/DigitalFilter.cpp \
../Core/Custom/FaultDetector.cpp \
../Core/Custom/IO_Handler.cpp \
../Core/Custom/RotatingBlock.cpp 

OBJS += \
./Core/Custom/ChargingStage.o \
./Core/Custom/CoolingSystem.o \
./Core/Custom/DigitalFilter.o \
./Core/Custom/FaultDetector.o \
./Core/Custom/IO_Handler.o \
./Core/Custom/RotatingBlock.o 

CPP_DEPS += \
./Core/Custom/ChargingStage.d \
./Core/Custom/CoolingSystem.d \
./Core/Custom/DigitalFilter.d \
./Core/Custom/FaultDetector.d \
./Core/Custom/IO_Handler.d \
./Core/Custom/RotatingBlock.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Custom/%.o Core/Custom/%.su Core/Custom/%.cyclo: ../Core/Custom/%.cpp Core/Custom/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -I../Addons/PnPL/Components -I../Addons/PnPL/AppModel -I../Middlewares/ST/PnPLCompManager/Inc -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Custom

clean-Core-2f-Custom:
	-$(RM) ./Core/Custom/ChargingStage.cyclo ./Core/Custom/ChargingStage.d ./Core/Custom/ChargingStage.o ./Core/Custom/ChargingStage.su ./Core/Custom/CoolingSystem.cyclo ./Core/Custom/CoolingSystem.d ./Core/Custom/CoolingSystem.o ./Core/Custom/CoolingSystem.su ./Core/Custom/DigitalFilter.cyclo ./Core/Custom/DigitalFilter.d ./Core/Custom/DigitalFilter.o ./Core/Custom/DigitalFilter.su ./Core/Custom/FaultDetector.cyclo ./Core/Custom/FaultDetector.d ./Core/Custom/FaultDetector.o ./Core/Custom/FaultDetector.su ./Core/Custom/IO_Handler.cyclo ./Core/Custom/IO_Handler.d ./Core/Custom/IO_Handler.o ./Core/Custom/IO_Handler.su ./Core/Custom/RotatingBlock.cyclo ./Core/Custom/RotatingBlock.d ./Core/Custom/RotatingBlock.o ./Core/Custom/RotatingBlock.su

.PHONY: clean-Core-2f-Custom

