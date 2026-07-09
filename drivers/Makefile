# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

COMMON_FLAGS += \
	-Idrivers/ata/pio/include \
	-Idrivers/ps2/keyboardin/include \
	-Idrivers/vga/textmode/include

include drivers/ata/pio/src/Makefile
include drivers/ps2/keyboardin/src/Makefile
include drivers/vga/textmode/src/Makefile

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
