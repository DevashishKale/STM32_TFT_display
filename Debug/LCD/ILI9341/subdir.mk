################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LCD/ILI9341/ILI9341_Driver.c 

OBJS += \
./LCD/ILI9341/ILI9341_Driver.o 

C_DEPS += \
./LCD/ILI9341/ILI9341_Driver.d 


# Each subdirectory must supply rules for building sources it contributes
LCD/ILI9341/%.o LCD/ILI9341/%.su LCD/ILI9341/%.cyclo: ../LCD/ILI9341/%.c LCD/ILI9341/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I"D:/STM32Workspace/tft_display/LCD/Display" -I"D:/STM32Workspace/tft_display/LCD/Icons" -I"D:/STM32Workspace/tft_display/LCD/ILI9341" -I"D:/STM32Workspace/tft_display/LCD/Images" -I"D:/STM32Workspace/tft_display/LCD" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-LCD-2f-ILI9341

clean-LCD-2f-ILI9341:
	-$(RM) ./LCD/ILI9341/ILI9341_Driver.cyclo ./LCD/ILI9341/ILI9341_Driver.d ./LCD/ILI9341/ILI9341_Driver.o ./LCD/ILI9341/ILI9341_Driver.su

.PHONY: clean-LCD-2f-ILI9341

