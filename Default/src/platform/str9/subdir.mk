################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/str9/91x_adc.c \
../src/platform/str9/91x_fmi.c \
../src/platform/str9/91x_gpio.c \
../src/platform/str9/91x_i2c.c \
../src/platform/str9/91x_scu.c \
../src/platform/str9/91x_ssp.c \
../src/platform/str9/91x_tim.c \
../src/platform/str9/91x_uart.c \
../src/platform/str9/91x_vic.c \
../src/platform/str9/91x_wiu.c \
../src/platform/str9/interrupt.c \
../src/platform/str9/platform.c \
../src/platform/str9/platform_int.c \
../src/platform/str9/str9_pio.c 

C_DEPS += \
./src/platform/str9/91x_adc.d \
./src/platform/str9/91x_fmi.d \
./src/platform/str9/91x_gpio.d \
./src/platform/str9/91x_i2c.d \
./src/platform/str9/91x_scu.d \
./src/platform/str9/91x_ssp.d \
./src/platform/str9/91x_tim.d \
./src/platform/str9/91x_uart.d \
./src/platform/str9/91x_vic.d \
./src/platform/str9/91x_wiu.d \
./src/platform/str9/interrupt.d \
./src/platform/str9/platform.d \
./src/platform/str9/platform_int.d \
./src/platform/str9/str9_pio.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/str9/%.o: ../src/platform/str9/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


