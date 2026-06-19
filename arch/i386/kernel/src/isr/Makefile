# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/isr_common_handler.o \
	$(BUILD)/isr_common_stub.o \
	$(BUILD)/isr.o

$(BUILD)/%.o: arch/i386/kernel/src/isr/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: arch/i386/kernel/src/isr/%.asm
	nasm $(ASMFLAGS) $< -o $@
