################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/stm32/FWLib/library/src/misc.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_adc.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_bkp.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_can.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_crc.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_dac.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_dbgmcu.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_dma.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_exti.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_flash.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_fsmc.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_gpio.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_i2c.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_iwdg.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_pwr.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_rcc.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_rtc.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_sdio.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_spi.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_tim.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_usart.c \
../src/platform/stm32/FWLib/library/src/stm32f10x_wwdg.c 

C_DEPS += \
./src/platform/stm32/FWLib/library/src/misc.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_adc.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_bkp.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_can.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_crc.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_dac.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_dbgmcu.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_dma.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_exti.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_flash.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_fsmc.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_gpio.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_i2c.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_iwdg.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_pwr.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_rcc.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_rtc.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_sdio.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_spi.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_tim.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_usart.d \
./src/platform/stm32/FWLib/library/src/stm32f10x_wwdg.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/stm32/FWLib/library/src/%.o: ../src/platform/stm32/FWLib/library/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


