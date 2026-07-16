# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

COMMON_FLAGS += \
	-Ishell/include/utils \
	-Ishell/include/commands \
	-Ishell/include/commands/utils \
	-Ishell/include/commands/interpreter \
	-Ishell/include/commands/interpreter/utils \

include shell/src/commands/Makefile
include shell/src/interpreter/Makefile

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
