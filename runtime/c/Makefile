# 
#    Copyright (c) 2026 Lukenar11 (Luke Matthes)
#    MIT Licensed
#    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/port_io.o \
	$(BUILD)/string.o \

$(BUILD)/string.o: runtime/c/src/string.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/port_io.o: runtime/c/src/port_io.asm
	nasm $(ASMFLAGS) $< -o $@
