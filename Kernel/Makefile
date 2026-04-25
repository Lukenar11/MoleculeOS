OBJS += \
$(BUILD)/kernel_main.o \
$(BUILD)/IDT.o \
$(BUILD)/isr_common_handler.o \
$(BUILD)/isr.o \
$(BUILD)/LoadIDT.o \
$(BUILD)/Heap.o

$(BUILD)/kernel_main.o: Kernel/src/kernel_main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/IDT.o: Kernel/src/IDT/IDT.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/LoadIDT.o: Kernel/src/IDT/LoadIDT.asm
	nasm $(ASMFLAGS) $< -o $@

$(BUILD)/%.o: Kernel/src/IDT/ISR/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: Kernel/src/IDT/ISR/%.asm
	nasm $(ASMFLAGS) $< -o $@

$(BUILD)/Heap.o: Kernel/src/Heap/Heap.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
