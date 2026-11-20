# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

COMMON_FLAGS += -Istdlib \
				-Istdlib/compcrtl \
				-Iapis \
				-Itypes \

OBJS += $(BUILD)/console.o \
		$(BUILD)/memory.o \
		$(BUILD)/string.o \
		$(BUILD)/compiler_c_runtime_library.o \

$(BUILD)/%.o: stdlib/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: stdlib/compcrtl/%.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
