# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += $(BUILD)/idt.o \
		$(BUILD)/idt_helpers.o \
		$(BUILD)/irq_handler.o \
		$(BUILD)/irq_helpers.o \
		$(BUILD)/irq_stub.o \
		$(BUILD)/irq.o \
		$(BUILD)/isr_handler.o \
		$(BUILD)/isr_helpers.o \
		$(BUILD)/isr_stub.o \
		$(BUILD)/isr.o \
		$(BUILD)/load_idt.o \

$(BUILD)/%.o: arch/i386/kernel/src/idt/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: arch/i386/kernel/src/idt/utils/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: arch/i386/kernel/src/idt/%.asm
	nasm $(ASM_FLAGS) $< -o $@

$(BUILD)/%.o: arch/i386/kernel/src/idt/utils/%.asm
	nasm $(ASM_FLAGS) $< -o $@
