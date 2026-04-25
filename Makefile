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

include Boot/Makefile
include Drivers/Makefile
include Runtime/Makefile
include Kernel/Makefile

OBJS := $(OBJS)

all: dirs $(KERNEL)

$(BUILD):
	mkdir -p $(BUILD)

$(OBJS): | $(BUILD)

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

run: all
	qemu-system-i386 -kernel $(KERNEL)

clean: 
	rm -rf $(BUILD)

.PHONY: all run clean dirs
