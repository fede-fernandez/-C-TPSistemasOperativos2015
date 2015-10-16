################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Planificador

dependents:
	-cd /home/alexis/git/tp-2015-2c-los-javimancos/commonsDeAsedio/Debug && $(MAKE) clean all

# Tool invocations
Planificador: $(OBJS) $(USER_OBJS) /home/alexis/git/tp-2015-2c-los-javimancos/commonsDeAsedio/Debug/libcommonsDeAsedio.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/alexis/git/tp-2015-2c-los-javimancos/commonsDeAsedio/Debug" -o "Planificador" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Planificador
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/alexis/git/tp-2015-2c-los-javimancos/commonsDeAsedio/Debug/libcommonsDeAsedio.so:

-include ../makefile.targets
