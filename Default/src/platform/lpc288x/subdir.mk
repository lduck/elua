################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/lpc288x/platform.c \
../src/platform/lpc288x/target.c \
../src/platform/lpc288x/uart.c 

C_DEPS += \
./src/platform/lpc288x/platform.d \
./src/platform/lpc288x/target.d \
./src/platform/lpc288x/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/lpc288x/%.o: ../src/platform/lpc288x/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


