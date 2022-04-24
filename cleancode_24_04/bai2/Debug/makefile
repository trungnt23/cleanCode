################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include SDK_1.0.3_NUCLEO-F401RE/shared/Utilities/subdir.mk
-include SDK_1.0.3_NUCLEO-F401RE/shared/Middle/ucglib/subdir.mk
-include SDK_1.0.3_NUCLEO-F401RE/shared/Middle/serial/subdir.mk
-include SDK_1.0.3_NUCLEO-F401RE/shared/Middle/sensor/subdir.mk
-include SDK_1.0.3_NUCLEO-F401RE/shared/Middle/rtos/subdir.mk
-include SDK_1.0.3_NUCLEO-F401RE/shared/Middle/led/subdir.mk
-include SDK_1.0.3_NUCLEO-F401RE/shared/Middle/buzzer/subdir.mk
-include SDK_1.0.3_NUCLEO-F401RE/shared/Middle/button/subdir.mk
-include SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/STM32F401RE_StdPeriph_Driver/src/subdir.mk
-include SDK_1.0.3_NUCLEO-F401RE/shared/Drivers/CMSIS/Source/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := bai2
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
bai2.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
bai2.list \

OBJCOPY_BIN += \
bai2.bin \


# All Target
all: main-build

# Main-build Target
main-build: bai2.elf secondary-outputs

# Tool invocations
bai2.elf: $(OBJS) $(USER_OBJS) E:\IoT\Application\Funix_303_iot\Lab2\bai2\STM32F401RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "bai2.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"E:\IoT\Application\Funix_303_iot\Lab2\bai2\STM32F401RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="bai2.map" -Wl,--gc-sections -static -L"E:\IoT\Application\Funix_303_iot\thuvien\show\SDK_1.0.3_NUCLEO-F401RE\lib_stm" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

bai2.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "bai2.list"
	@echo 'Finished building: $@'
	@echo ' '

bai2.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "bai2.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SIZE_OUTPUT)$(OBJDUMP_LIST)$(EXECUTABLES)$(OBJS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS)$(OBJCOPY_BIN) bai2.elf
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
