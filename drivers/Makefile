# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 


COMMON_FLAGS += -Idrivers/ata \
				-Idrivers/ps2 \
				-Idrivers/vga \
				-Idrivers/utils \

OBJS += $(BUILD)/pio.o \
		$(BUILD)/keyboard_input.o \
		$(BUILD)/text_mode.o \
		

$(BUILD)/%.o: drivers/ata/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: drivers/ps2/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: drivers/vga/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
