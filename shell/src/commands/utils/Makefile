# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/helpers.o

$(BUILD)/%.o: shell/src/commands/utils/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
