################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/i386/common.c \
../src/platform/i386/descriptor_tables.c \
../src/platform/i386/isr.c \
../src/platform/i386/kb.c \
../src/platform/i386/main.c \
../src/platform/i386/monitor.c \
../src/platform/i386/platform.c \
../src/platform/i386/timer.c 

C_DEPS += \
./src/platform/i386/common.d \
./src/platform/i386/descriptor_tables.d \
./src/platform/i386/isr.d \
./src/platform/i386/kb.d \
./src/platform/i386/main.d \
./src/platform/i386/monitor.d \
./src/platform/i386/platform.d \
./src/platform/i386/timer.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/i386/%.o: ../src/platform/i386/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


