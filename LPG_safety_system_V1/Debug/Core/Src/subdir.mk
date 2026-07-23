################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/GSM.c \
../Core/Src/LCD.c \
../Core/Src/actuator.c \
../Core/Src/dht22.c \
../Core/Src/freertos.c \
../Core/Src/hx711.c \
../Core/Src/main.c \
../Core/Src/mq2_sensor.c \
../Core/Src/mq4_sensor.c \
../Core/Src/mq7_sensor.c \
../Core/Src/mq_common.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_hal_timebase_tim.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/GSM.o \
./Core/Src/LCD.o \
./Core/Src/actuator.o \
./Core/Src/dht22.o \
./Core/Src/freertos.o \
./Core/Src/hx711.o \
./Core/Src/main.o \
./Core/Src/mq2_sensor.o \
./Core/Src/mq4_sensor.o \
./Core/Src/mq7_sensor.o \
./Core/Src/mq_common.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_hal_timebase_tim.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/GSM.d \
./Core/Src/LCD.d \
./Core/Src/actuator.d \
./Core/Src/dht22.d \
./Core/Src/freertos.d \
./Core/Src/hx711.d \
./Core/Src/main.d \
./Core/Src/mq2_sensor.d \
./Core/Src/mq4_sensor.d \
./Core/Src/mq7_sensor.d \
./Core/Src/mq_common.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_hal_timebase_tim.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/GSM.cyclo ./Core/Src/GSM.d ./Core/Src/GSM.o ./Core/Src/GSM.su ./Core/Src/LCD.cyclo ./Core/Src/LCD.d ./Core/Src/LCD.o ./Core/Src/LCD.su ./Core/Src/actuator.cyclo ./Core/Src/actuator.d ./Core/Src/actuator.o ./Core/Src/actuator.su ./Core/Src/dht22.cyclo ./Core/Src/dht22.d ./Core/Src/dht22.o ./Core/Src/dht22.su ./Core/Src/freertos.cyclo ./Core/Src/freertos.d ./Core/Src/freertos.o ./Core/Src/freertos.su ./Core/Src/hx711.cyclo ./Core/Src/hx711.d ./Core/Src/hx711.o ./Core/Src/hx711.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mq2_sensor.cyclo ./Core/Src/mq2_sensor.d ./Core/Src/mq2_sensor.o ./Core/Src/mq2_sensor.su ./Core/Src/mq4_sensor.cyclo ./Core/Src/mq4_sensor.d ./Core/Src/mq4_sensor.o ./Core/Src/mq4_sensor.su ./Core/Src/mq7_sensor.cyclo ./Core/Src/mq7_sensor.d ./Core/Src/mq7_sensor.o ./Core/Src/mq7_sensor.su ./Core/Src/mq_common.cyclo ./Core/Src/mq_common.d ./Core/Src/mq_common.o ./Core/Src/mq_common.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_hal_timebase_tim.cyclo ./Core/Src/stm32f4xx_hal_timebase_tim.d ./Core/Src/stm32f4xx_hal_timebase_tim.o ./Core/Src/stm32f4xx_hal_timebase_tim.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

