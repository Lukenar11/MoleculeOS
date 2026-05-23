# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/keyboard_input.o 

$(BUILD)/%.o: drivers/ps2/keyboardin/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
