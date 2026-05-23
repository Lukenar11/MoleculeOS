# 
#    Copyright (c) 2026 Lukenar11 (Luke Matthes)
#    MIT Licensed
#    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/textmode.o

$(BUILD)/%.o: drivers/vga/textmode/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
