################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/lm3s/usblib/usbbuffer.c \
../src/platform/lm3s/usblib/usbdesc.c \
../src/platform/lm3s/usblib/usbkeyboardmap.c \
../src/platform/lm3s/usblib/usbmode.c \
../src/platform/lm3s/usblib/usbringbuf.c \
../src/platform/lm3s/usblib/usbtick.c 

C_DEPS += \
./src/platform/lm3s/usblib/usbbuffer.d \
./src/platform/lm3s/usblib/usbdesc.d \
./src/platform/lm3s/usblib/usbkeyboardmap.d \
./src/platform/lm3s/usblib/usbmode.d \
./src/platform/lm3s/usblib/usbringbuf.d \
./src/platform/lm3s/usblib/usbtick.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/lm3s/usblib/%.o: ../src/platform/lm3s/usblib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


