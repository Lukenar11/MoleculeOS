#
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
#

OBJS += $(BUILD)/shell.o \
		$(BUILD)/cmds.o \
		$(BUILD)/shell_helpers.o \
		$(BUILD)/cmds_helpers.o \
		$(BUILD)/utils.o \
		$(BUILD)/help.o \
		$(BUILD)/info.o \
		$(BUILD)/clear.o \
		$(BUILD)/reboot.o \
		$(BUILD)/shutdown.o \
		$(BUILD)/create.o \
		$(BUILD)/list.o \
		$(BUILD)/remove.o \
		$(BUILD)/echo.o \
		$(BUILD)/rename.o \

$(BUILD)/%.o: shell/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: shell/src/utils/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: shell/src/cmds/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
