################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/platform/lm3s/usblib/device/usbdaudio.c \
../src/platform/lm3s/usblib/device/usbdbulk.c \
../src/platform/lm3s/usblib/device/usbdcdc.c \
../src/platform/lm3s/usblib/device/usbdcdesc.c \
../src/platform/lm3s/usblib/device/usbdcomp.c \
../src/platform/lm3s/usblib/device/usbdconfig.c \
../src/platform/lm3s/usblib/device/usbddfu-rt.c \
../src/platform/lm3s/usblib/device/usbdenum.c \
../src/platform/lm3s/usblib/device/usbdhandler.c \
../src/platform/lm3s/usblib/device/usbdhid.c \
../src/platform/lm3s/usblib/device/usbdhidkeyb.c \
../src/platform/lm3s/usblib/device/usbdhidmouse.c \
../src/platform/lm3s/usblib/device/usbdmsc.c 

C_DEPS += \
./src/platform/lm3s/usblib/device/usbdaudio.d \
./src/platform/lm3s/usblib/device/usbdbulk.d \
./src/platform/lm3s/usblib/device/usbdcdc.d \
./src/platform/lm3s/usblib/device/usbdcdesc.d \
./src/platform/lm3s/usblib/device/usbdcomp.d \
./src/platform/lm3s/usblib/device/usbdconfig.d \
./src/platform/lm3s/usblib/device/usbddfu-rt.d \
./src/platform/lm3s/usblib/device/usbdenum.d \
./src/platform/lm3s/usblib/device/usbdhandler.d \
./src/platform/lm3s/usblib/device/usbdhid.d \
./src/platform/lm3s/usblib/device/usbdhidkeyb.d \
./src/platform/lm3s/usblib/device/usbdhidmouse.d \
./src/platform/lm3s/usblib/device/usbdmsc.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/lm3s/usblib/device/%.o: ../src/platform/lm3s/usblib/device/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


