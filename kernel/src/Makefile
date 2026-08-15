# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += $(BUILD)/mofs2.o \
		$(BUILD)/block.o \
		$(BUILD)/storage_mgr.o \
		$(BUILD)/kernel_main.o \

$(BUILD)/%.o: kernel/src/filesys/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: kernel/src/allocator/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: kernel/src/storagemgr/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: kernel/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
