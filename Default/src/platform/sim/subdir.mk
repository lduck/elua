################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/sim/host.c \
../src/platform/sim/hostif_linux.c \
../src/platform/sim/platform.c 

C_DEPS += \
./src/platform/sim/host.d \
./src/platform/sim/hostif_linux.d \
./src/platform/sim/platform.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/sim/%.o: ../src/platform/sim/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


