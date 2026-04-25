CC  = clang
CXX = clang++
ASM = nasm
LD  = ld.lld

BUILD  = build/bin
KERNEL = $(BUILD)/MoleculeOS.elf
LINKER = linker.ld

TARGET = -target i386-pc-none-elf

COMMON_FLAGS = $(TARGET) \
	-I. -IRuntime -IKernel/include -IDrivers/VGA/include \
	-ffreestanding -fno-builtin \
	-fno-stack-protector \
	-fno-rtti -fno-exceptions \
	-fno-pie -no-pie \
	-Os -Wall -Wextra

CFLAGS   = $(COMMON_FLAGS) -c
CXXFLAGS = $(COMMON_FLAGS) -std=c++20
ASMFLAGS = -f elf32

LDFLAGS  = -T $(LINKER) -m elf_i386 -nostdlib -static

OBJS = \
$(BUILD)/string.o \
$(BUILD)/kernel_main.o \
$(BUILD)/IDT.o \
$(BUILD)/isr_common_handler.o \
$(BUILD)/VGADriver.o \
$(BUILD)/Terminal.o \
$(BUILD)/ConsoleIO.o \
$(BUILD)/Boot.o \
$(BUILD)/StackTop.o \
$(BUILD)/MultibootHeader.o \
$(BUILD)/isr.o \
$(BUILD)/LoadIDT.o \
$(BUILD)/Heap.o

all: dirs $(KERNEL)

dirs:
	@mkdir -p $(BUILD)

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD)/%.o: Runtime/C/%.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD)/%.o: Kernel/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: Kernel/src/IDT/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: Kernel/src/IDT/ISR/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: Kernel/src/Heap/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: Runtime/CPP/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: Drivers/VGA/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: Terminal/src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: Boot/%.asm
	nasm $(ASMFLAGS) $< -o $@


$(BUILD)/%.o: Kernel/src/IDT/ISR/%.asm
	nasm $(ASMFLAGS) $< -o $@

$(BUILD)/%.o: Kernel/src/IDT/%.asm
	nasm $(ASMFLAGS) $< -o $@

run: all
	qemu-system-i386 -kernel $(KERNEL)

clean:
	rm -rf $(BUILD)

.PHONY: all run clean dirs
