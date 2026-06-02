# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/file_system.o

$(BUILD)/%.o: kernel/src/filesystem/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
