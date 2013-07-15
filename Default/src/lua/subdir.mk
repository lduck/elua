################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lua/lapi.c \
../src/lua/lauxlib.c \
../src/lua/lbaselib.c \
../src/lua/lcode.c \
../src/lua/ldblib.c \
../src/lua/ldebug.c \
../src/lua/ldo.c \
../src/lua/ldump.c \
../src/lua/legc.c \
../src/lua/lfunc.c \
../src/lua/lgc.c \
../src/lua/linit.c \
../src/lua/liolib.c \
../src/lua/llex.c \
../src/lua/lmathlib.c \
../src/lua/lmem.c \
../src/lua/loadlib.c \
../src/lua/lobject.c \
../src/lua/lopcodes.c \
../src/lua/loslib.c \
../src/lua/lparser.c \
../src/lua/lrotable.c \
../src/lua/lstate.c \
../src/lua/lstring.c \
../src/lua/lstrlib.c \
../src/lua/ltable.c \
../src/lua/ltablib.c \
../src/lua/ltm.c \
../src/lua/lua.c \
../src/lua/luac.c \
../src/lua/lundump.c \
../src/lua/lvm.c \
../src/lua/lzio.c \
../src/lua/print.c 

C_DEPS += \
./src/lua/lapi.d \
./src/lua/lauxlib.d \
./src/lua/lbaselib.d \
./src/lua/lcode.d \
./src/lua/ldblib.d \
./src/lua/ldebug.d \
./src/lua/ldo.d \
./src/lua/ldump.d \
./src/lua/legc.d \
./src/lua/lfunc.d \
./src/lua/lgc.d \
./src/lua/linit.d \
./src/lua/liolib.d \
./src/lua/llex.d \
./src/lua/lmathlib.d \
./src/lua/lmem.d \
./src/lua/loadlib.d \
./src/lua/lobject.d \
./src/lua/lopcodes.d \
./src/lua/loslib.d \
./src/lua/lparser.d \
./src/lua/lrotable.d \
./src/lua/lstate.d \
./src/lua/lstring.d \
./src/lua/lstrlib.d \
./src/lua/ltable.d \
./src/lua/ltablib.d \
./src/lua/ltm.d \
./src/lua/lua.d \
./src/lua/luac.d \
./src/lua/lundump.d \
./src/lua/lvm.d \
./src/lua/lzio.d \
./src/lua/print.d 


# Each subdirectory must supply rules for building sources it contributes
src/lua/%.o: ../src/lua/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Mac OS X GCC C Compiler'
	arm-none-eabi-gcc -O2 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


