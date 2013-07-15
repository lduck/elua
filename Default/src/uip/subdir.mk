################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/uip/dhcpc.c \
../src/uip/psock.c \
../src/uip/resolv.c \
../src/uip/timer.c \
../src/uip/uip-fw.c \
../src/uip/uip-neighbor.c \
../src/uip/uip-split.c \
../src/uip/uip.c \
../src/uip/uip_arp.c \
../src/uip/uiplib.c 

C_DEPS += \
./src/uip/dhcpc.d \
./src/uip/psock.d \
./src/uip/resolv.d \
./src/uip/timer.d \
./src/uip/uip-fw.d \
./src/uip/uip-neighbor.d \
./src/uip/uip-split.d \
./src/uip/uip.d \
./src/uip/uip_arp.d \
./src/uip/uiplib.d 


# Each subdirectory must supply rules for building sources it contributes
src/uip/%.o: ../src/uip/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


