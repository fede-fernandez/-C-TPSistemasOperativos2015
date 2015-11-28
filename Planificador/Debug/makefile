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
all: PlanificadorNew

dependents:
	-cd /home/alexis/git/TPSORespaldo/commonsDeAsedio/Debug && $(MAKE) clean all

# Tool invocations
PlanificadorNew: $(OBJS) $(USER_OBJS) /home/alexis/git/TPSORespaldo/commonsDeAsedio/Debug/libcommonsDeAsedio.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/alexis/git/TPSORespaldo/commonsDeAsedio/Debug" -o "PlanificadorNew" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) PlanificadorNew
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/alexis/git/TPSORespaldo/commonsDeAsedio/Debug/libcommonsDeAsedio.so:

-include ../makefile.targets
