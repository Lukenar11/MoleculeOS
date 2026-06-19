# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

$(shell mkdir -p build/bin)

CC = clang
CXX = clang++
ASM = nasm
LD = ld.lld

BUILD = build/bin
ISO = build/iso

KERNEL = $(BUILD)/MoleculeOS.elf

ISO_KERNEL = $(ISO)/boot/kernel.elf
ISO_OUT = build/MoleculeOS.iso

LINKER = tools/linker.ld
TARGET = -target i386-pc-none-elf

COMMON_FLAGS = $(TARGET) \
	-I. \
	-march=i386 -m32 \
	-ffreestanding -fno-builtin \
	-fno-stack-protector \
	-fno-rtti -fno-exceptions \
	-fno-pie -no-pie \
	-fvisibility=hidden \
	-fdata-sections -ffunction-sections \
	-Wall -Wextra -Wignored-qualifiers \
	-fno-threadsafe-statics \
	-Os -fno-omit-frame-pointer

CFLAGS = $(COMMON_FLAGS) -c -std=c17
CXXFLAGS = $(COMMON_FLAGS) -std=c++20
ASMFLAGS = -f elf32

LDFLAGS  = -T $(LINKER) -m elf_i386 -nostdlib -static

include arch/i386/iso/boot/Makefile
include arch/i386/runtime/src/Makefile
include drivers/Makefile
include runtime/Makefile
include kernel/Makefile
include terminal/Makefile
include shell/Makefile

all: dirs $(KERNEL)

$(BUILD):
	mkdir -p $(BUILD)

dirs:
	mkdir -p build/bin
	mkdir -p build/iso/boot/grub

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

iso: $(KERNEL)
	mkdir -p $(ISO)/boot/grub
	cp $(KERNEL) $(ISO_KERNEL)
	cp arch/i386/iso/boot/grub/grub.cfg $(ISO)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_OUT) $(ISO)

run: iso
	qemu-system-i386 -cdrom $(ISO_OUT)

clean:
	rm -rf build

.PHONY: all iso run clean dirs
