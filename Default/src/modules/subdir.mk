################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/modules/adc.c \
../src/modules/bit.c \
../src/modules/bitarray.c \
../src/modules/can.c \
../src/modules/cpu.c \
../src/modules/elua.c \
../src/modules/i2c.c \
../src/modules/lpack.c \
../src/modules/luarpc.c \
../src/modules/net.c \
../src/modules/pd.c \
../src/modules/pio.c \
../src/modules/pwm.c \
../src/modules/spi.c \
../src/modules/term.c \
../src/modules/tmr.c \
../src/modules/uart.c 

C_DEPS += \
./src/modules/adc.d \
./src/modules/bit.d \
./src/modules/bitarray.d \
./src/modules/can.d \
./src/modules/cpu.d \
./src/modules/elua.d \
./src/modules/i2c.d \
./src/modules/lpack.d \
./src/modules/luarpc.d \
./src/modules/net.d \
./src/modules/pd.d \
./src/modules/pio.d \
./src/modules/pwm.d \
./src/modules/spi.d \
./src/modules/term.d \
./src/modules/tmr.d \
./src/modules/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/%.o: ../src/modules/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


