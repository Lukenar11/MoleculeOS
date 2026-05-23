# 
#    Copyright (c) 2026 Lukenar11 (Luke Matthes)
#    MIT Licensed
#    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/port_io.o \
	$(BUILD)/string.o \

$(BUILD)/%.o: runtime/c/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: runtime/c/src/%.asm
	nasm $(ASMFLAGS) $< -o $@
