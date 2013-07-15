################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/buf.c \
../src/common.c \
../src/common_tmr.c \
../src/common_uart.c \
../src/dlmalloc.c \
../src/elua_adc.c \
../src/elua_int.c \
../src/elua_mmc.c \
../src/elua_uip.c \
../src/eluarpc.c \
../src/linenoise.c \
../src/linenoise_posix.c \
../src/luarpc_desktop_serial.c \
../src/luarpc_elua_uart.c \
../src/main.c \
../src/mmcfs.c \
../src/romfs.c \
../src/salloc.c \
../src/semifs.c \
../src/shell.c \
../src/term.c \
../src/xmodem.c 

C_DEPS += \
./src/buf.d \
./src/common.d \
./src/common_tmr.d \
./src/common_uart.d \
./src/dlmalloc.d \
./src/elua_adc.d \
./src/elua_int.d \
./src/elua_mmc.d \
./src/elua_uip.d \
./src/eluarpc.d \
./src/linenoise.d \
./src/linenoise_posix.d \
./src/luarpc_desktop_serial.d \
./src/luarpc_elua_uart.d \
./src/main.d \
./src/mmcfs.d \
./src/romfs.d \
./src/salloc.d \
./src/semifs.d \
./src/shell.d \
./src/term.d \
./src/xmodem.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


