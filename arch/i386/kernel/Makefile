# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

COMMON_FLAGS += -Iarch/i386/kernel/include \
				-Iarch/i386/kernel/include/idt \
				-Iarch/i386/kernel/include/idt/utils \
				-Iarch/i386/kernel/include/sys \
				-Iarch/i386/kernel/include/utils \

include arch/i386/kernel/src/idt/Makefile
include arch/i386/kernel/src/sys/Makefile
include arch/i386/kernel/src/Makefile

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
