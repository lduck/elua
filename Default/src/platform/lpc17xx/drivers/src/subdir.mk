################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/lpc17xx/drivers/src/lpc17xx_adc.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_can.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_clkpwr.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_dac.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_emac.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_gpdma.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_gpio.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_i2c.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_i2s.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_libcfg_default.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_mcpwm.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_nvic.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_pinsel.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_pwm.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_qei.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_rit.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_rtc.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_spi.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_ssp.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_timer.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_uart.c \
../src/platform/lpc17xx/drivers/src/lpc17xx_wdt.c 

C_DEPS += \
./src/platform/lpc17xx/drivers/src/lpc17xx_adc.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_can.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_clkpwr.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_dac.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_emac.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_gpdma.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_gpio.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_i2c.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_i2s.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_libcfg_default.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_mcpwm.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_nvic.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_pinsel.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_pwm.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_qei.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_rit.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_rtc.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_spi.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_ssp.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_timer.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_uart.d \
./src/platform/lpc17xx/drivers/src/lpc17xx_wdt.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/lpc17xx/drivers/src/%.o: ../src/platform/lpc17xx/drivers/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


