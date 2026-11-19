# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += $(BUILD)/console.o \
		$(BUILD)/memory.o \
		$(BUILD)/string.o \
		$(BUILD)/compiler_c_runtime.o \

$(BUILD)/%.o: runtime/stdlib/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: runtime/stdlib/compcrtl/%.c
	$(CXX) $(CXXFLAGS) -c $< -o $@
