################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Modules/src/camera/driver/ov7670.c \
../Core/Modules/src/camera/driver/ov7670_reg.c \
../Core/Modules/src/camera/driver/ov7670_sccb.c 

OBJS += \
./Core/Modules/src/camera/driver/ov7670.o \
./Core/Modules/src/camera/driver/ov7670_reg.o \
./Core/Modules/src/camera/driver/ov7670_sccb.o 

C_DEPS += \
./Core/Modules/src/camera/driver/ov7670.d \
./Core/Modules/src/camera/driver/ov7670_reg.d \
./Core/Modules/src/camera/driver/ov7670_sccb.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Modules/src/camera/driver/%.o: ../Core/Modules/src/camera/driver/%.c Core/Modules/src/camera/driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"D:/HCMUT/HK251/tkll/object_classifier/PROJECT/CAMERA_MCU/Core/Modules/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

