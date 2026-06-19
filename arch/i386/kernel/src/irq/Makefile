# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/irq_common_handler.o \
	$(BUILD)/irq_common_stub.o \
	$(BUILD)/irq.o

$(BUILD)/%.o: arch/i386/kernel/src/irq/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: arch/i386/kernel/src/irq/%.asm
	nasm $(ASMFLAGS) $< -o $@
