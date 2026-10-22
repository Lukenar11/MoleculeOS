# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += $(BUILD)/console.o \
		$(BUILD)/memory.o \
		$(BUILD)/circular_buffer.o \
		$(BUILD)/string.o \
		$(BUILD)/compiler_c_api.o \

$(BUILD)/%.o: runtime/stdlib/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: runtime/stdlib/src/utils/%.c
	$(CXX) $(CXXFLAGS) -c $< -o $@
