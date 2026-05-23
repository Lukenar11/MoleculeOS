# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/kernel_system_cpu_flags.o \
	$(BUILD)/panic.o \
	$(BUILD)/reboot.o \
	$(BUILD)/shutdown.o \
	$(BUILD)/kernel_system_enable_interrupts.o \
	$(BUILD)/kernel_system_sleep.o \
	$(BUILD)/kernel_system_hang.o \

$(BUILD)/%.o: kernel/src/system/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: kernel/src/system/%.asm
	nasm $(ASMFLAGS) $< -o $@
