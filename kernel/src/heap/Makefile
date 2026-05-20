# 
#    Copyright (c) 2026 Lukenar11 (Luke Matthes)
#    MIT Licensed
#    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/bump.o

$(BUILD)/bump.o: kernel/src/heap/bump.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
