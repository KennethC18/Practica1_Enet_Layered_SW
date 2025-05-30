################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../utilities/fsl_assert.c \
../utilities/fsl_debug_console.c 

C_DEPS += \
./utilities/fsl_assert.d \
./utilities/fsl_debug_console.d 

OBJS += \
./utilities/fsl_assert.o \
./utilities/fsl_debug_console.o 


# Each subdirectory must supply rules for building sources it contributes
utilities/%.o: ../utilities/%.c utilities/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_MK66FN2M0VMD18 -DCPU_MK66FN2M0VMD18_cm4 -DPRINTF_ADVANCED_ENABLE=1 -DFRDM_K66F -DFREEDOM -DMCUXPRESSO_SDK -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"/home/kenneth/Redes/Practica1_Enet_Layered_SW/source" -I"/home/kenneth/Redes/Practica1_Enet_Layered_SW/mdio" -I"/home/kenneth/Redes/Practica1_Enet_Layered_SW/phy" -I"/home/kenneth/Redes/Practica1_Enet_Layered_SW/utilities" -I"/home/kenneth/Redes/Practica1_Enet_Layered_SW/drivers" -I"/home/kenneth/Redes/Practica1_Enet_Layered_SW/device" -I"/home/kenneth/Redes/Practica1_Enet_Layered_SW/component/uart" -I"/home/kenneth/Redes/Practica1_Enet_Layered_SW/component/lists" -I"/home/kenneth/Redes/Practica1_Enet_Layered_SW/CMSIS" -I"/home/kenneth/Redes/Practica1_Enet_Layered_SW/board" -I"/home/kenneth/Redes/Practica1_Enet_Layered_SW/frdmk66f/driver_examples/enet/txrx_transfer" -O0 -fno-common -g3 -gdwarf-4 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-utilities

clean-utilities:
	-$(RM) ./utilities/fsl_assert.d ./utilities/fsl_assert.o ./utilities/fsl_debug_console.d ./utilities/fsl_debug_console.o

.PHONY: clean-utilities

