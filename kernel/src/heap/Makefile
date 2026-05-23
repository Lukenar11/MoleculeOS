# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/linear_area.o

$(BUILD)/%.o: kernel/src/heap/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
