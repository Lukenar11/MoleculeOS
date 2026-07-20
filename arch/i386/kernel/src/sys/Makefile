# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += $(BUILD)/eflags.o \
		$(BUILD)/reboot.o \
		$(BUILD)/panic.o \
		$(BUILD)/shutdown.o \
		$(BUILD)/interrupts.o \
		$(BUILD)/halt.o \

$(BUILD)/%.o: arch/i386/kernel/src/sys/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: arch/i386/kernel/src/sys/%.asm
	nasm $(ASM_FLAGS) $< -o $@
