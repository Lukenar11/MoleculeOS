# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/commands.o

$(BUILD)/%.o: shell/src/commands/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
