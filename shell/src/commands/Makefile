# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/commands.o \
	$(BUILD)/create.o \
	$(BUILD)/echo.o \
	$(BUILD)/help.o \
	$(BUILD)/info.o \
	$(BUILD)/list.o \
	$(BUILD)/remove.o

$(BUILD)/%.o: shell/src/commands/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
