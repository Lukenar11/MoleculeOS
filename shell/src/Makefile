# 
# copyright: Copyright (c) 2026 Lukenar11 (Luke Matthes)
# license:   https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# author:    Lukenar11 (Luke Matthes)
# 


MODULE_NAME      := shell
MODULE_DIRECTORY := $(MODULE_NAME)/src

MODULE_INCLUDE_DIRECTORY += $(MODULE_NAME)/include

MODULE_SOURCE_FILES :=
MODULE_SOURCE_FILES += cmds/clear.cpp
MODULE_SOURCE_FILES += cmds/copy.cpp
MODULE_SOURCE_FILES += cmds/create.cpp
MODULE_SOURCE_FILES += cmds/echo.cpp
MODULE_SOURCE_FILES += cmds/exists.cpp
MODULE_SOURCE_FILES += cmds/help.cpp
MODULE_SOURCE_FILES += cmds/info.cpp
MODULE_SOURCE_FILES += cmds/list.cpp
MODULE_SOURCE_FILES += cmds/reboot.cpp
MODULE_SOURCE_FILES += cmds/remove.cpp
MODULE_SOURCE_FILES += cmds/rename.cpp
MODULE_SOURCE_FILES += cmds/shutdown.cpp
MODULE_SOURCE_FILES += utils/cmds_helpers.cpp
MODULE_SOURCE_FILES += utils/shell_helpers.cpp
MODULE_SOURCE_FILES += shell.cpp

MODULE_BUILD_PATH := $(BUILD_DIRECTORY)/$(MODULE_DIRECTORY)

MODULE_OBJECTS :=
MODULE_OBJECTS += $(patsubst %.cpp,$(MODULE_BUILD_PATH)/%.obj, \
				  $(filter %.cpp,$(MODULE_SOURCE_FILES)))

$(MODULE_OBJECTS): MODULE_INCLUDE_FLAGS := $(addprefix -I,$(MODULE_INCLUDE_DIRECTORY))

ALL_OBJECTS 	 += $(MODULE_OBJECTS)
ALL_MODULE_PATHS += $(MODULE_DIRECTORY)


# Compiles all C++ files in this module.
$(MODULE_BUILD_PATH)/%.obj: $(MODULE_DIRECTORY)/%.cpp
	@mkdir -p "$(dir $@)"
	$(CPP_COMPILER) $(CPP_COMPILER_FLAGS) $(MODULE_INCLUDE_FLAGS) -c $< -o $@


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
