################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Memoria.c \
../src/algoritmos.c \
../src/configuracion.c \
../src/funcionesMemoria.c \
../src/funcionesSeniales.c \
../src/impresionesEnPantalla.c \
../src/validaciones.c 

OBJS += \
./src/Memoria.o \
./src/algoritmos.o \
./src/configuracion.o \
./src/funcionesMemoria.o \
./src/funcionesSeniales.o \
./src/impresionesEnPantalla.o \
./src/validaciones.o 

C_DEPS += \
./src/Memoria.d \
./src/algoritmos.d \
./src/configuracion.d \
./src/funcionesMemoria.d \
./src/funcionesSeniales.d \
./src/impresionesEnPantalla.d \
./src/validaciones.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/brian/git/tp-2015-2c-los-javimancos/commonsDeAsedio" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


