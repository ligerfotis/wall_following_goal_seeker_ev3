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
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
wall_following_goal_seeker_ev3.hex \

SECONDARY_SIZE += \
wall_following_goal_seeker_ev3.siz \


# All Target
all: wall_following_goal_seeker_ev3.elf secondary-outputs

# Tool invocations
wall_following_goal_seeker_ev3.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-linux-gnueabi-gcc -mcpu=arm9 -marm -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -Xlinker --gc-sections -s -L/usr/local/EV3-API-master -Wl,-Map,"wall_following_goal_seeker_ev3.map" -o "wall_following_goal_seeker_ev3.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

wall_following_goal_seeker_ev3.hex: wall_following_goal_seeker_ev3.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-linux-gnueabi-objcopy -O ihex "wall_following_goal_seeker_ev3.elf"  "wall_following_goal_seeker_ev3.hex"
	@echo 'Finished building: $@'
	@echo ' '

wall_following_goal_seeker_ev3.siz: wall_following_goal_seeker_ev3.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-linux-gnueabi-size --format=berkeley "wall_following_goal_seeker_ev3.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) wall_following_goal_seeker_ev3.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
