################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm

# All of the sources participating in the build are defined here
-include sources.mk
-include src/uip/subdir.mk
-include src/serial/subdir.mk
-include src/remotefs/subdir.mk
-include src/platform/str9/subdir.mk
-include src/platform/str7/subdir.mk
-include src/platform/stm32/FWLib/library/src/subdir.mk
-include src/platform/stm32/subdir.mk
-include src/platform/sim/subdir.mk
-include src/platform/lpc288x/subdir.mk
-include src/platform/lpc24xx/subdir.mk
-include src/platform/lpc17xx/drivers/src/subdir.mk
-include src/platform/lpc17xx/subdir.mk
-include src/platform/lm3s/usblib/host/subdir.mk
-include src/platform/lm3s/usblib/device/subdir.mk
-include src/platform/lm3s/usblib/subdir.mk
-include src/platform/lm3s/driverlib/subdir.mk
-include src/platform/lm3s/subdir.mk
-include src/platform/i386/subdir.mk
-include src/platform/avr32/subdir.mk
-include src/platform/at91sam7x/subdir.mk
-include src/platform/subdir.mk
-include src/newlib/subdir.mk
-include src/modules/subdir.mk
-include src/lua/subdir.mk
-include src/fatfs/subdir.mk
-include src/subdir.mk
-include rfs_server_src/subdir.mk
-include mux_src/subdir.mk
-include .sconf_temp/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: elua

# Tool invocations
	@echo 'No tool found that can build the extension specified with the build artifact name $@'
# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(CC_DEPS)$(CPP_DEPS)$(CXX_DEPS)$(C_UPPER_DEPS)$(S_UPPER_DEPS) elua
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
