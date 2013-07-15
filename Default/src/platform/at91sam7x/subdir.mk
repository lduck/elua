################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/at91sam7x/aic.c \
../src/platform/at91sam7x/board_lowlevel.c \
../src/platform/at91sam7x/board_memories.c \
../src/platform/at91sam7x/pio.c \
../src/platform/at91sam7x/pit.c \
../src/platform/at91sam7x/platform.c \
../src/platform/at91sam7x/platform_int.c \
../src/platform/at91sam7x/pmc.c \
../src/platform/at91sam7x/pwmc.c \
../src/platform/at91sam7x/tc.c \
../src/platform/at91sam7x/usart.c 

C_DEPS += \
./src/platform/at91sam7x/aic.d \
./src/platform/at91sam7x/board_lowlevel.d \
./src/platform/at91sam7x/board_memories.d \
./src/platform/at91sam7x/pio.d \
./src/platform/at91sam7x/pit.d \
./src/platform/at91sam7x/platform.d \
./src/platform/at91sam7x/platform_int.d \
./src/platform/at91sam7x/pmc.d \
./src/platform/at91sam7x/pwmc.d \
./src/platform/at91sam7x/tc.d \
./src/platform/at91sam7x/usart.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/at91sam7x/%.o: ../src/platform/at91sam7x/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


