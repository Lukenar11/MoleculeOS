# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
    $(BUILD)/shell/src/commands/utils/helpers.o \
    $(BUILD)/shell/src/commands/create.o \
    $(BUILD)/shell/src/commands/echo.o \
    $(BUILD)/shell/src/commands/help.o \
    $(BUILD)/shell/src/commands/info.o \
    $(BUILD)/shell/src/commands/list.o \
    $(BUILD)/shell/src/commands/remove.o

$(BUILD)/shell/src/commands/%.o: shell/src/commands/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/shell/src/commands/utils/%.o: shell/src/commands/utils/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
