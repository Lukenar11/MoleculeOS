# 
#    Copyright (c) 2026 Lukenar11 (Luke Matthes)
#    MIT Licensed
#    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/keyboard_input.o 

$(BUILD)/keyboard_input.o: drivers/ps2/keyboard_input/src/keyboard_input.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
