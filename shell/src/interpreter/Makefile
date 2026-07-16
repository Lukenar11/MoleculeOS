# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += $(BUILD)/shell/src/interpreter/interpreter.o

$(BUILD)/shell/src/interpreter/%.o: shell/src/interpreter/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
