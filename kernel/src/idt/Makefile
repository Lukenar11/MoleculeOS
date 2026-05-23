# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/idt.o \
	$(BUILD)/load_idt.o

$(BUILD)/%.o: kernel/src/idt/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: kernel/src/idt/%.asm
	nasm $(ASMFLAGS) $< -o $@
