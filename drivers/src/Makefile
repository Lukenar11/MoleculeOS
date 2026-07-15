# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += $(BUILD)/pio.o \
		$(BUILD)/keyboard_in.o \
		$(BUILD)/text_mode.o \
		$(BUILD)/ata_pio_helpers.o \
		$(BUILD)/ps2_keyboard_in_helpers.o \
		$(BUILD)/vga_text_mode_helpers.o \

$(BUILD)/%.o: drivers/src/ata/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: drivers/src/ps2/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: drivers/src/vga/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: drivers/src/utils/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
