# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/cpu_flags.o \
	$(BUILD)/reboot.o \
	$(BUILD)/shutdown.o \
	$(BUILD)/enable_interrupts.o \
	$(BUILD)/sleep.o \
	$(BUILD)/hang.o \

$(BUILD)/%.o: arch/i386/kernel/src/system/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: arch/i386/kernel/src/system/%.asm
	nasm $(ASMFLAGS) $< -o $@
