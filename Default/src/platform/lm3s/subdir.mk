################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/lm3s/disp.c \
../src/platform/lm3s/platform.c \
../src/platform/lm3s/platform_int.c \
../src/platform/lm3s/rit128x96x4.c \
../src/platform/lm3s/startup_gcc.c \
../src/platform/lm3s/usb_serial_structs.c 

C_DEPS += \
./src/platform/lm3s/disp.d \
./src/platform/lm3s/platform.d \
./src/platform/lm3s/platform_int.d \
./src/platform/lm3s/rit128x96x4.d \
./src/platform/lm3s/startup_gcc.d \
./src/platform/lm3s/usb_serial_structs.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/lm3s/%.o: ../src/platform/lm3s/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


