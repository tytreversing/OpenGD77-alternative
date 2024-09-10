################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../application/include/user_interface/languages/languages_builder.c 

OBJS += \
./application/include/user_interface/languages/languages_builder.o 

C_DEPS += \
./application/include/user_interface/languages/languages_builder.d 


# Each subdirectory must supply rules for building sources it contributes
application/include/user_interface/languages/%.o application/include/user_interface/languages/%.su: ../application/include/user_interface/languages/%.c application/include/user_interface/languages/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F405xx -DPLATFORM_MDUV380 -DNDEBUG -DDEBUG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../application/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../USB_DEVICE/Target -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../USB_DEVICE/App -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../ -Os -ffunction-sections -fdata-sections -Wall -Wno-format -Wno-format-truncation -Wno-stringop-overflow -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-application-2f-include-2f-user_interface-2f-languages

clean-application-2f-include-2f-user_interface-2f-languages:
	-$(RM) ./application/include/user_interface/languages/languages_builder.d ./application/include/user_interface/languages/languages_builder.o ./application/include/user_interface/languages/languages_builder.su

.PHONY: clean-application-2f-include-2f-user_interface-2f-languages

