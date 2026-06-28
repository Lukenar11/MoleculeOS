# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

COMMON_FLAGS += \
	-Iarch/i386/kernel/include \

include arch/i386/kernel/src/idt/Makefile
include arch/i386/kernel/src/irq/Makefile
include arch/i386/kernel/src/isr/Makefile
include arch/i386/kernel/src/system/Makefile
include arch/i386/kernel/src/Makefile

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
