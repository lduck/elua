################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/str7/71x_apb.c \
../src/platform/str7/71x_gpio.c \
../src/platform/str7/71x_rccu.c \
../src/platform/str7/71x_tim.c \
../src/platform/str7/71x_uart.c \
../src/platform/str7/platform.c 

C_DEPS += \
./src/platform/str7/71x_apb.d \
./src/platform/str7/71x_gpio.d \
./src/platform/str7/71x_rccu.d \
./src/platform/str7/71x_tim.d \
./src/platform/str7/71x_uart.d \
./src/platform/str7/platform.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/str7/%.o: ../src/platform/str7/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


