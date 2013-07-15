################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/avr32/adc.c \
../src/platform/avr32/ethernet.c \
../src/platform/avr32/flashc.c \
../src/platform/avr32/gpio.c \
../src/platform/avr32/i2c.c \
../src/platform/avr32/intc.c \
../src/platform/avr32/lcd.c \
../src/platform/avr32/platform.c \
../src/platform/avr32/platform_int.c \
../src/platform/avr32/pm.c \
../src/platform/avr32/pm_conf_clocks.c \
../src/platform/avr32/pwm.c \
../src/platform/avr32/sdramc.c \
../src/platform/avr32/spi.c \
../src/platform/avr32/tc.c \
../src/platform/avr32/usart.c \
../src/platform/avr32/usb-cdc.c 

C_DEPS += \
./src/platform/avr32/adc.d \
./src/platform/avr32/ethernet.d \
./src/platform/avr32/flashc.d \
./src/platform/avr32/gpio.d \
./src/platform/avr32/i2c.d \
./src/platform/avr32/intc.d \
./src/platform/avr32/lcd.d \
./src/platform/avr32/platform.d \
./src/platform/avr32/platform_int.d \
./src/platform/avr32/pm.d \
./src/platform/avr32/pm_conf_clocks.d \
./src/platform/avr32/pwm.d \
./src/platform/avr32/sdramc.d \
./src/platform/avr32/spi.d \
./src/platform/avr32/tc.d \
./src/platform/avr32/usart.d \
./src/platform/avr32/usb-cdc.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/avr32/%.o: ../src/platform/avr32/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


