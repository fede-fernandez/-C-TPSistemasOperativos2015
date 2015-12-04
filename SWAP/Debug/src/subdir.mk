################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/SWAP.c \
../src/funcionesSWAP.c \
../src/huecosSWAP.c \
../src/instruccionesSWAP.c \
../src/particionSWAP.c 

OBJS += \
./src/SWAP.o \
./src/funcionesSWAP.o \
./src/huecosSWAP.o \
./src/instruccionesSWAP.o \
./src/particionSWAP.o 

C_DEPS += \
./src/SWAP.d \
./src/funcionesSWAP.d \
./src/huecosSWAP.d \
./src/instruccionesSWAP.d \
./src/particionSWAP.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2015-2c-los-javimancos/commonsDeAsedio" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


