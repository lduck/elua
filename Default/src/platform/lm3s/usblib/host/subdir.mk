################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/lm3s/usblib/host/usbhaudio.c \
../src/platform/lm3s/usblib/host/usbhhid.c \
../src/platform/lm3s/usblib/host/usbhhidkeyboard.c \
../src/platform/lm3s/usblib/host/usbhhidmouse.c \
../src/platform/lm3s/usblib/host/usbhmsc.c \
../src/platform/lm3s/usblib/host/usbhostenum.c \
../src/platform/lm3s/usblib/host/usbhscsi.c 

C_DEPS += \
./src/platform/lm3s/usblib/host/usbhaudio.d \
./src/platform/lm3s/usblib/host/usbhhid.d \
./src/platform/lm3s/usblib/host/usbhhidkeyboard.d \
./src/platform/lm3s/usblib/host/usbhhidmouse.d \
./src/platform/lm3s/usblib/host/usbhmsc.d \
./src/platform/lm3s/usblib/host/usbhostenum.d \
./src/platform/lm3s/usblib/host/usbhscsi.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/lm3s/usblib/host/%.o: ../src/platform/lm3s/usblib/host/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


