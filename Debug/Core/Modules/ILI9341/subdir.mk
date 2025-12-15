################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Modules/ILI9341/fonts.c \
../Core/Modules/ILI9341/ili9341.c 

OBJS += \
./Core/Modules/ILI9341/fonts.o \
./Core/Modules/ILI9341/ili9341.o 

C_DEPS += \
./Core/Modules/ILI9341/fonts.d \
./Core/Modules/ILI9341/ili9341.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Modules/ILI9341/%.o: ../Core/Modules/ILI9341/%.c Core/Modules/ILI9341/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/HK251/VXL/LAB/tkll/CAMERA_MCU/Core/Modules" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

