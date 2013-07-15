################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/lm3s/driverlib/adc.c \
../src/platform/lm3s/driverlib/can.c \
../src/platform/lm3s/driverlib/cpu.c \
../src/platform/lm3s/driverlib/ethernet.c \
../src/platform/lm3s/driverlib/flash.c \
../src/platform/lm3s/driverlib/gpio.c \
../src/platform/lm3s/driverlib/interrupt.c \
../src/platform/lm3s/driverlib/pwm.c \
../src/platform/lm3s/driverlib/ssi.c \
../src/platform/lm3s/driverlib/sysctl.c \
../src/platform/lm3s/driverlib/systick.c \
../src/platform/lm3s/driverlib/timer.c \
../src/platform/lm3s/driverlib/uart.c \
../src/platform/lm3s/driverlib/udma.c \
../src/platform/lm3s/driverlib/usb.c 

C_DEPS += \
./src/platform/lm3s/driverlib/adc.d \
./src/platform/lm3s/driverlib/can.d \
./src/platform/lm3s/driverlib/cpu.d \
./src/platform/lm3s/driverlib/ethernet.d \
./src/platform/lm3s/driverlib/flash.d \
./src/platform/lm3s/driverlib/gpio.d \
./src/platform/lm3s/driverlib/interrupt.d \
./src/platform/lm3s/driverlib/pwm.d \
./src/platform/lm3s/driverlib/ssi.d \
./src/platform/lm3s/driverlib/sysctl.d \
./src/platform/lm3s/driverlib/systick.d \
./src/platform/lm3s/driverlib/timer.d \
./src/platform/lm3s/driverlib/uart.d \
./src/platform/lm3s/driverlib/udma.d \
./src/platform/lm3s/driverlib/usb.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/lm3s/driverlib/%.o: ../src/platform/lm3s/driverlib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


