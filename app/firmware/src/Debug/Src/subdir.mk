################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/led_effects.c \
../Src/led_strip.c \
../Src/main.c \
../Src/protocol.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/led_effects.o \
./Src/led_strip.o \
./Src/main.o \
./Src/protocol.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/led_effects.d \
./Src/led_strip.d \
./Src/main.d \
./Src/protocol.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F1 -DSTM32F103C6Tx -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/led_effects.cyclo ./Src/led_effects.d ./Src/led_effects.o ./Src/led_effects.su ./Src/led_strip.cyclo ./Src/led_strip.d ./Src/led_strip.o ./Src/led_strip.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/protocol.cyclo ./Src/protocol.d ./Src/protocol.o ./Src/protocol.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src

