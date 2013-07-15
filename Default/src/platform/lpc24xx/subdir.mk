################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/lpc24xx/irq.c \
../src/platform/lpc24xx/platform.c \
../src/platform/lpc24xx/platform_int.c \
../src/platform/lpc24xx/target.c 

C_DEPS += \
./src/platform/lpc24xx/irq.d \
./src/platform/lpc24xx/platform.d \
./src/platform/lpc24xx/platform_int.d \
./src/platform/lpc24xx/target.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/lpc24xx/%.o: ../src/platform/lpc24xx/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


