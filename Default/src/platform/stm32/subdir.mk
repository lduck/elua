################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/stm32/core_cm3.c \
../src/platform/stm32/enc.c \
../src/platform/stm32/lcd.c \
../src/platform/stm32/lstm32_mod.c \
../src/platform/stm32/lua_lcd.c \
../src/platform/stm32/platform.c \
../src/platform/stm32/platform_int.c \
../src/platform/stm32/stm32f10x_it.c \
../src/platform/stm32/system_stm32f10x.c 

C_DEPS += \
./src/platform/stm32/core_cm3.d \
./src/platform/stm32/enc.d \
./src/platform/stm32/lcd.d \
./src/platform/stm32/lstm32_mod.d \
./src/platform/stm32/lua_lcd.d \
./src/platform/stm32/platform.d \
./src/platform/stm32/platform_int.d \
./src/platform/stm32/stm32f10x_it.d \
./src/platform/stm32/system_stm32f10x.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/stm32/%.o: ../src/platform/stm32/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


