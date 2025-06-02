################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/Drivers/Security.c \
../source/Drivers/aes.c \
../source/Drivers/crc.c 

C_DEPS += \
./source/Drivers/Security.d \
./source/Drivers/aes.d \
./source/Drivers/crc.d 

OBJS += \
./source/Drivers/Security.o \
./source/Drivers/aes.o \
./source/Drivers/crc.o 


# Each subdirectory must supply rules for building sources it contributes
source/Drivers/%.o: ../source/Drivers/%.c source/Drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_MK66FN2M0VMD18 -DCPU_MK66FN2M0VMD18_cm4 -DPRINTF_ADVANCED_ENABLE=1 -DFRDM_K66F -DFREEDOM -DMCUXPRESSO_SDK -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -DSDK_OS_BAREMETAL -I"C:\Redes\Practica1_Enet_Layered_SW\mdio" -I"C:\Redes\Practica1_Enet_Layered_SW\phy" -I"C:\Redes\Practica1_Enet_Layered_SW\utilities" -I"C:\Redes\Practica1_Enet_Layered_SW\drivers" -I"C:\Redes\Practica1_Enet_Layered_SW\device" -I"C:\Redes\Practica1_Enet_Layered_SW\component\uart" -I"C:\Redes\Practica1_Enet_Layered_SW\component\lists" -I"C:\Redes\Practica1_Enet_Layered_SW\CMSIS" -I"C:\Redes\Practica1_Enet_Layered_SW\source" -I"C:\Redes\Practica1_Enet_Layered_SW\board" -O0 -fno-common -g3 -gdwarf-4 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-Drivers

clean-source-2f-Drivers:
	-$(RM) ./source/Drivers/Security.d ./source/Drivers/Security.o ./source/Drivers/aes.d ./source/Drivers/aes.o ./source/Drivers/crc.d ./source/Drivers/crc.o

.PHONY: clean-source-2f-Drivers

