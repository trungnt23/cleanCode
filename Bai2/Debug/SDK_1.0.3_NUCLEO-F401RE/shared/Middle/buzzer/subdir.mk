################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer/buzzer.c \
E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer/melody.c 

OBJS += \
./SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer/buzzer.o \
./SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer/melody.o 

C_DEPS += \
./SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer/buzzer.d \
./SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer/melody.d 


# Each subdirectory must supply rules for building sources it contributes
SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer/buzzer.o: E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer/buzzer.c SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F401RE -DSTM32 -DSTM32F401RETx -DSTM32F4 -c -I"E:/IoT/Application/Funix_303_iot/Lab7/Bai2" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/CMSIS/Include" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/inc" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/button" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/led" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/rtos" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/sensor" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/serial" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/ucglib" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Utilities" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer/melody.o: E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer/melody.c SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F401RE -DSTM32 -DSTM32F401RETx -DSTM32F4 -c -I"E:/IoT/Application/Funix_303_iot/Lab7/Bai2" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/CMSIS/Include" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/inc" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/button" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/led" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/rtos" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/sensor" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/serial" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Middle/ucglib" -I"E:/IoT/Application/Funix_303_iot/thuvien/show/SDK_1.0.3_NUCLEO-F401RE/shared/Utilities" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

