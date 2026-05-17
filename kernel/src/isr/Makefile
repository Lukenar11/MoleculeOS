# 
#    Copyright (c) 2026 Lukenar11 (Luke Matthes)
#    MIT Licensed
#    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/isr_common_handler.o \
	$(BUILD)/isr_common_stub.o \
	$(BUILD)/isr.o

$(BUILD)/isr_common_handler.o: kernel/src/isr/isr_common_handler.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/isr_common_stub.o: kernel/src/isr/isr_common_stub.asm
	nasm $(ASMFLAGS) $< -o $@

$(BUILD)/isr.o: kernel/src/isr/isr.asm
	nasm $(ASMFLAGS) $< -o $@
