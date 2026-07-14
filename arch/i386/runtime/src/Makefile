# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/port_io.o \

$(BUILD)/%.o: arch/i386/runtime/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: arch/i386/runtime/src/%.asm
	nasm $(ASM_FLAGS) $< -o $@
