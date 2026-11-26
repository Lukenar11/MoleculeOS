# 
# copyright: Copyright (c) 2026 Lukenar11 (Luke Matthes)
# license:   https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# author:    Lukenar11 (Luke Matthes)
# 


MODULE_NAME      := filesys
MODULE_DIRECTORY := kernel/$(MODULE_NAME)

MODULE_SOURCE_FILES :=
MODULE_SOURCE_FILES += mofs.cpp

MODULE_BUILD_PATH := $(BUILD_DIRECTORY)/$(MODULE_DIRECTORY)

MODULE_OBJECTS :=
MODULE_OBJECTS += $(patsubst %.cpp,$(MODULE_BUILD_PATH)/%.obj, \
				  $(filter %.cpp,$(MODULE_SOURCE_FILES)))

ALL_OBJECTS 	 += $(MODULE_OBJECTS)
ALL_MODULE_PATHS += $(MODULE_DIRECTORY)


# Compiles all C++ files in this module.
$(MODULE_BUILD_PATH)/%.obj: $(MODULE_DIRECTORY)/%.cpp
	@mkdir -p "$(dir $@)"
	$(CPP_COMPILER) $(CPP_COMPILER_FLAGS) -c $< -o $@


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
