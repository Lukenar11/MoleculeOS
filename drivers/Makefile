# 
#    Copyright (c) 2026 Lukenar11 (Luke Matthes)
#    MIT Licensed
#    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

COMMON_FLAGS += \
	-Idrivers/ps2/keyboard_input/include \
	-Idrivers/vga/textmode/include

include drivers/ps2/keyboard_input/src/Makefile
include drivers/vga/textmode/src/Makefile

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
