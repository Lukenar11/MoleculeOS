# 
#    Copyright (c) 2026 Lukenar11 (Luke Matthes)
#    MIT Licensed
#    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/idt.o \
	$(BUILD)/load_idt.o

$(BUILD)/idt.o: kernel/src/idt/idt.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/load_idt.o: kernel/src/idt/load_idt.asm
	nasm $(ASMFLAGS) $< -o $@
