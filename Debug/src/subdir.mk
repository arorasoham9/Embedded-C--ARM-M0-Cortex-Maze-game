################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/background.c \
../src/lcd.c \
../src/line.c \
../src/main.c \
../src/maze.c \
../src/midi.c \
../src/scale.c \
../src/speaker.c \
../src/step.c \
../src/support.c \
../src/syscalls.c \
../src/system_stm32f0xx.c \
../src/wavetable.c 

OBJS += \
./src/background.o \
./src/lcd.o \
./src/line.o \
./src/main.o \
./src/maze.o \
./src/midi.o \
./src/scale.o \
./src/speaker.o \
./src/step.o \
./src/support.o \
./src/syscalls.o \
./src/system_stm32f0xx.o \
./src/wavetable.o 

C_DEPS += \
./src/background.d \
./src/lcd.d \
./src/line.d \
./src/main.d \
./src/maze.d \
./src/midi.d \
./src/scale.d \
./src/speaker.d \
./src/step.d \
./src/support.d \
./src/syscalls.d \
./src/system_stm32f0xx.d \
./src/wavetable.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F091RCTx -DDEBUG -DSTM32F091 -DUSE_STDPERIPH_DRIVER -I"C:/Users/arora/workspace/miniProject/StdPeriph_Driver/inc" -I"C:/Users/arora/workspace/miniProject/inc" -I"C:/Users/arora/workspace/miniProject/CMSIS/device" -I"C:/Users/arora/workspace/miniProject/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


