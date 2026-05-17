# 
#    Copyright (c) 2026 Lukenar11 (Luke Matthes)
#    MIT Licensed
#    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/halt.o \
	$(BUILD)/panic.o \
	$(BUILD)/reboot.o \
	$(BUILD)/shutdown.o \

$(BUILD)/panic.o: kernel/src/system/panic.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/reboot.o: kernel/src/system/reboot.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/shutdown.o: kernel/src/system/shutdown.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/halt.o: kernel/src/system/halt.asm
	nasm $(ASMFLAGS) $< -o $@
