################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/enet_txrx_transfer.c \
../source/semihost_hardfault.c 

C_DEPS += \
./source/enet_txrx_transfer.d \
./source/semihost_hardfault.d 

OBJS += \
./source/enet_txrx_transfer.o \
./source/semihost_hardfault.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK66FN2M0VMD18 -DCPU_MK66FN2M0VMD18_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Redes\Practica1_Enet_Layered_SW\mdio" -I"C:\Redes\Practica1_Enet_Layered_SW\phy" -I"C:\Redes\Practica1_Enet_Layered_SW\utilities" -I"C:\Redes\Practica1_Enet_Layered_SW\drivers" -I"C:\Redes\Practica1_Enet_Layered_SW\device" -I"C:\Redes\Practica1_Enet_Layered_SW\component\uart" -I"C:\Redes\Practica1_Enet_Layered_SW\component\lists" -I"C:\Redes\Practica1_Enet_Layered_SW\CMSIS" -I"C:\Redes\Practica1_Enet_Layered_SW\board" -I"C:\Redes\Practica1_Enet_Layered_SW\source" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/enet_txrx_transfer.d ./source/enet_txrx_transfer.o ./source/semihost_hardfault.d ./source/semihost_hardfault.o

.PHONY: clean-source

