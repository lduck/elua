################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/remotefs/client.c \
../src/remotefs/elua_os_io.c \
../src/remotefs/elua_rfs.c \
../src/remotefs/remotefs.c 

C_DEPS += \
./src/remotefs/client.d \
./src/remotefs/elua_os_io.d \
./src/remotefs/elua_rfs.d \
./src/remotefs/remotefs.d 


# Each subdirectory must supply rules for building sources it contributes
src/remotefs/%.o: ../src/remotefs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


