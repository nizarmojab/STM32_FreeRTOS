################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Third-Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/port.c 

OBJS += \
./Third-Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/port.o 

C_DEPS += \
./Third-Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/port.d 


# Each subdirectory must supply rules for building sources it contributes
Third-Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/%.o: ../Third-Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F446RETx -DNUCLEO_F446RE -DDEBUG -DSTM32F446xx -DUSE_STDPERIPH_DRIVER -I"C:/Users/PC/Downloads/MasteringRTOS-master/MasteringRTOS-master/RTOS_workspace/Legacy/STM32_FreeRTOS_Led_and_Button_IT/StdPeriph_Driver/inc" -I"C:/Users/PC/Downloads/MasteringRTOS-master/MasteringRTOS-master/RTOS_workspace/Legacy/STM32_FreeRTOS_Led_and_Button_IT/Config" -I"C:/Users/PC/Downloads/MasteringRTOS-master/MasteringRTOS-master/RTOS_workspace/Legacy/STM32_FreeRTOS_Led_and_Button_IT/Third-Party/SEGGER/Config" -I"C:/Users/PC/Downloads/MasteringRTOS-master/MasteringRTOS-master/RTOS_workspace/Legacy/STM32_FreeRTOS_Led_and_Button_IT/Third-Party/SEGGER/OS" -I"C:/Users/PC/Downloads/MasteringRTOS-master/MasteringRTOS-master/RTOS_workspace/Legacy/STM32_FreeRTOS_Led_and_Button_IT/Third-Party/SEGGER/SEGGER" -I"C:/Users/PC/Downloads/MasteringRTOS-master/MasteringRTOS-master/RTOS_workspace/Legacy/STM32_FreeRTOS_Led_and_Button_IT/Third-Party/FreeRTOS/org/Source/include" -I"C:/Users/PC/Downloads/MasteringRTOS-master/MasteringRTOS-master/RTOS_workspace/Legacy/STM32_FreeRTOS_Led_and_Button_IT/Third-Party/FreeRTOS/org/Source/portable/GCC/ARM_CM4F" -I"C:/Users/PC/Downloads/MasteringRTOS-master/MasteringRTOS-master/RTOS_workspace/Legacy/STM32_FreeRTOS_Led_and_Button_IT/inc" -I"C:/Users/PC/Downloads/MasteringRTOS-master/MasteringRTOS-master/RTOS_workspace/Legacy/STM32_FreeRTOS_Led_and_Button_IT/CMSIS/device" -I"C:/Users/PC/Downloads/MasteringRTOS-master/MasteringRTOS-master/RTOS_workspace/Legacy/STM32_FreeRTOS_Led_and_Button_IT/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


