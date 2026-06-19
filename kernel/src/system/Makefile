# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/panic.o \

$(BUILD)/%.o: kernel/src/system/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: kernel/src/system/%.asm
	nasm $(ASMFLAGS) $< -o $@
