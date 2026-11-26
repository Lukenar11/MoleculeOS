# 
# copyright: Copyright (c) 2026 Lukenar11 (Luke Matthes)
# license:   https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# author:    Lukenar11 (Luke Matthes)
# 


include arch/i386/kernel/src/includes.mk


MODULE_NAME      := sys
MODULE_DIRECTORY := arch/i386/kernel/src/$(MODULE_NAME)

MODULE_INCLUDE_DIRECTORY :=
MODULE_INCLUDE_DIRECTORY += $(ARCH_BASE_KERNEL_INCLUDE)
MODULE_INCLUDE_DIRECTORY += arch/i386/kernel/include/$(MODULE_NAME)

MODULE_SOURCE_FILES :=
MODULE_SOURCE_FILES += eflags.asm
MODULE_SOURCE_FILES += halt.asm
MODULE_SOURCE_FILES += interrupts.asm
MODULE_SOURCE_FILES += panic.cpp
MODULE_SOURCE_FILES += reboot.cpp
MODULE_SOURCE_FILES += shutdown.cpp

MODULE_BUILD_PATH := $(BUILD_DIRECTORY)/$(MODULE_DIRECTORY)

MODULE_OBJECTS :=

MODULE_OBJECTS += $(patsubst %.cpp,$(MODULE_BUILD_PATH)/%.obj, \
				  $(filter %.cpp,$(MODULE_SOURCE_FILES)))

MODULE_OBJECTS += $(patsubst %.asm,$(MODULE_BUILD_PATH)/%.obj, \
				  $(filter %.asm,$(MODULE_SOURCE_FILES)))

$(MODULE_OBJECTS): MODULE_INCLUDE_FLAGS := $(addprefix -I,$(MODULE_INCLUDE_DIRECTORY))

ALL_OBJECTS 	 += $(MODULE_OBJECTS)
ALL_MODULE_PATHS += $(MODULE_DIRECTORY)


# Compiles all C++ files in this module.
$(MODULE_BUILD_PATH)/%.obj: $(MODULE_DIRECTORY)/%.cpp
	@mkdir -p "$(dir $@)"
	$(CPP_COMPILER) $(CPP_COMPILER_FLAGS) $(MODULE_INCLUDE_FLAGS) -c $< -o $@

# Compiles all Assembly files in this module.
$(MODULE_BUILD_PATH)/%.obj: $(MODULE_DIRECTORY)/%.asm
	@mkdir -p "$(dir $@)"
	$(ASM_COMPILER) $(ASM_COMPILER_FLAGS) $< -o $@


.PHONY: $(MODULE_DIRECTORY)
.PHONY: clean-$(MODULE_DIRECTORY)


ifeq ($(filter clean,$(MAKECMDGOALS)),)

$(MODULE_DIRECTORY): $(MODULE_OBJECTS) $(BUILD_TARGET)

ifneq ($(filter $(MODULE_DIRECTORY),$(MAKECMDGOALS)),)

$(MODULE_OBJECTS): FORCE

endif
else

$(MODULE_DIRECTORY):

endif

# Cleans the build directory for this module.

clean-$(MODULE_DIRECTORY): MODULE_CLEAN_PATH := $(MODULE_BUILD_PATH)

clean-$(MODULE_DIRECTORY):
	rm -rf "$(MODULE_CLEAN_PATH)"
