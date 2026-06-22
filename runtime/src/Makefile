# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/text_output.o \
	$(BUILD)/memory_manipulation.o \
	$(BUILD)/string_manipulation.o \
	$(BUILD)/compiler_c_api.o \
	$(BUILD)/text_output.o \

$(BUILD)/%.o: runtime/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: runtime/src/utils/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
