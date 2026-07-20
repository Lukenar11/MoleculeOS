# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += $(BUILD)/text_output.o \
		$(BUILD)/memory_manip.o \
		$(BUILD)/circular_buffer.o \
		$(BUILD)/string_manip.o \
		$(BUILD)/compiler_c_api.o \

$(BUILD)/%.o: runtime/cpp/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: runtime/cpp/src/utils/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
