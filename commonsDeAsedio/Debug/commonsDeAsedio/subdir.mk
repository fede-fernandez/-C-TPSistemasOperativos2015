################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../commonsDeAsedio/cliente-servidor.c \
../commonsDeAsedio/configuracion.c \
../commonsDeAsedio/error.c \
../commonsDeAsedio/estructuras.c \
../commonsDeAsedio/mmap.c \
../commonsDeAsedio/select.c \
../commonsDeAsedio/semaforos.c \
../commonsDeAsedio/thread.c 

OBJS += \
./commonsDeAsedio/cliente-servidor.o \
./commonsDeAsedio/configuracion.o \
./commonsDeAsedio/error.o \
./commonsDeAsedio/estructuras.o \
./commonsDeAsedio/mmap.o \
./commonsDeAsedio/select.o \
./commonsDeAsedio/semaforos.o \
./commonsDeAsedio/thread.o 

C_DEPS += \
./commonsDeAsedio/cliente-servidor.d \
./commonsDeAsedio/configuracion.d \
./commonsDeAsedio/error.d \
./commonsDeAsedio/estructuras.d \
./commonsDeAsedio/mmap.d \
./commonsDeAsedio/select.d \
./commonsDeAsedio/semaforos.d \
./commonsDeAsedio/thread.d 


# Each subdirectory must supply rules for building sources it contributes
commonsDeAsedio/%.o: ../commonsDeAsedio/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


