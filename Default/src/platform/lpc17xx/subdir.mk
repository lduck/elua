################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/lpc17xx/core_cm3.c \
../src/platform/lpc17xx/mbed_pio.c \
../src/platform/lpc17xx/platform.c \
../src/platform/lpc17xx/startup_LPC17xx.c \
../src/platform/lpc17xx/system_LPC17xx.c 

C_DEPS += \
./src/platform/lpc17xx/core_cm3.d \
./src/platform/lpc17xx/mbed_pio.d \
./src/platform/lpc17xx/platform.d \
./src/platform/lpc17xx/startup_LPC17xx.d \
./src/platform/lpc17xx/system_LPC17xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/lpc17xx/%.o: ../src/platform/lpc17xx/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


