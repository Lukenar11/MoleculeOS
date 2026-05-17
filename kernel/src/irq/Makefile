# 
#    Copyright (c) 2026 Lukenar11 (Luke Matthes)
#    MIT Licensed
#    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/irq_common_handler.o \
	$(BUILD)/irq_common_stub.o \
	$(BUILD)/irq.o

$(BUILD)/irq_common_handler.o: kernel/src/irq/irq_common_handler.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/irq_common_stub.o: kernel/src/irq/irq_common_stub.asm
	nasm $(ASMFLAGS) $< -o $@

$(BUILD)/irq.o: kernel/src/irq/irq.asm
	nasm $(ASMFLAGS) $< -o $@
