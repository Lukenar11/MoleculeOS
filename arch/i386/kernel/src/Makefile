# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/init_kernel.o \

$(BUILD)/%.o: arch/i386/kernel/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
