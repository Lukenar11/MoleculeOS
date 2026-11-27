# 
# copyright: Copyright (c) 2026 Lukenar11 (Luke Matthes)
# license:   https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# author:    Lukenar11 (Luke Matthes)
# 


.DEFAULT_GOAL := all

ARCHITECTURE	   := i386
ARCHITECTURE_MACRO := _ARCHITECTURE_I386_

LINKER_SCRIPT    := arch/i386/tools/linker-config.ld
GRUB_CONFIG_FILE := arch/i386/iso/boot/grub/grub.cfg

GENERAL_COMPILER_FLAGS :=
GENERAL_COMPILER_FLAGS += -target $(ARCHITECTURE)-pc-none-elf
GENERAL_COMPILER_FLAGS += -I.
GENERAL_COMPILER_FLAGS += -march=$(ARCHITECTURE)
GENERAL_COMPILER_FLAGS += -m32
GENERAL_COMPILER_FLAGS += -Wlogical-op-parentheses
GENERAL_COMPILER_FLAGS += -ffreestanding
GENERAL_COMPILER_FLAGS += -fno-builtin
GENERAL_COMPILER_FLAGS += -fno-stack-protector
GENERAL_COMPILER_FLAGS += -fno-rtti
GENERAL_COMPILER_FLAGS += -fno-exceptions
GENERAL_COMPILER_FLAGS += -fno-pie
GENERAL_COMPILER_FLAGS += -no-pie
GENERAL_COMPILER_FLAGS += -fvisibility=hidden
GENERAL_COMPILER_FLAGS += -fdata-sections
GENERAL_COMPILER_FLAGS += -ffunction-sections
GENERAL_COMPILER_FLAGS += -Wall
GENERAL_COMPILER_FLAGS += -Wextra
GENERAL_COMPILER_FLAGS += -Wignored-qualifiers
GENERAL_COMPILER_FLAGS += -fno-threadsafe-statics
GENERAL_COMPILER_FLAGS += -flto
GENERAL_COMPILER_FLAGS += -Os
GENERAL_COMPILER_FLAGS += -fno-omit-frame-pointer

CPP_COMPILER := clang++

CPP_COMPILER_FLAGS :=
CPP_COMPILER_FLAGS += $(GENERAL_COMPILER_FLAGS)
CPP_COMPILER_FLAGS += -std=c++20
CPP_COMPILER_FLAGS += -D$(ARCHITECTURE_MACRO)

C_COMPILER := clang

C_COMPILER_FLAGS :=
C_COMPILER_FLAGS += $(GENERAL_COMPILER_FLAGS)
C_COMPILER_FLAGS += -std=c17
C_COMPILER_FLAGS += -D$(ARCHITECTURE_MACRO)

ASM_COMPILER       := nasm
ASM_COMPILER_FLAGS := -f elf32

LINKER := ld.lld

LINKER_FLAGS :=
LINKER_FLAGS += -T $(LINKER_SCRIPT)
LINKER_FLAGS += -m elf_i386
LINKER_FLAGS += -nostdlib
LINKER_FLAGS += -static

BUILD_DIRECTORY := $(CURDIR)/build
BUILD_TARGET    := $(BUILD_DIRECTORY)/MoleculeOS

ISO_BUILD_FOLDER := $(BUILD_DIRECTORY)/iso
ISO_IMAGE	     := $(BUILD_DIRECTORY)/MoleculeOS.iso

COMPILED_SYSTEM := $(ISO_BUILD_FOLDER)/boot/MoleculeOS
ISO_CONFIG_PATH := $(ISO_BUILD_FOLDER)/boot/grub/grub.cfg

HDD_IMAGE			  := $(BUILD_DIRECTORY)/MoleculeOS.img
HDD_IMAGE_SECTOR_SIZE := 2048

# All source tree modules
MODULE_MAKEFILES :=
MODULE_MAKEFILES += arch/$(ARCHITECTURE)/iso
MODULE_MAKEFILES += arch/$(ARCHITECTURE)/kernel/src/idt
MODULE_MAKEFILES += arch/$(ARCHITECTURE)/kernel/src/sys
MODULE_MAKEFILES += arch/$(ARCHITECTURE)/kernel/src
MODULE_MAKEFILES += arch/$(ARCHITECTURE)/stdlib/src
MODULE_MAKEFILES += drivers/ata
MODULE_MAKEFILES += drivers/ps2
MODULE_MAKEFILES += drivers/vga
MODULE_MAKEFILES += kernel/filesys
MODULE_MAKEFILES += kernel/heap
MODULE_MAKEFILES += kernel
MODULE_MAKEFILES += shell/src
MODULE_MAKEFILES += stdlib
MODULE_MAKEFILES += terminal/src

ALL_OBJECTS 	 :=
ALL_MODULE_PATHS :=


.PHONY: FORCE
.PHONY: all
.PHONY: clean
.PHONY: iso
.PHONY: run


FORCE:
include $(addsuffix /Makefile,$(MODULE_MAKEFILES))

# Builds the complete source tree if `make all` is entered. 
all: $(BUILD_TARGET)


# Cleans the build directory for this module.

ifneq ($(filter clean,$(MAKECMDGOALS)),)

CLEAN_MODULE_PATHS := $(filter $(ALL_MODULE_PATHS), \
					  $(filter-out clean,$(MAKECMDGOALS)))

ifneq ($(strip $(CLEAN_MODULE_PATHS)),)

clean: $(addprefix clean-,$(CLEAN_MODULE_PATHS))

else

clean:
	rm -rf "$(BUILD_DIRECTORY)"

endif
else

clean:
	rm -rf "$(BUILD_DIRECTORY)"

endif


$(BUILD_TARGET): $(ALL_OBJECTS) $(LINKER_SCRIPT)
	@mkdir -p "$(dir $@)"
	$(LINKER) $(LINKER_FLAGS) $(ALL_OBJECTS) -o $@

# Builds a HDD-image (`.img` file) if we use `make run` and
# a HDD image doesn't exists.
$(HDD_IMAGE):
	@mkdir -p "$(dir $@)"
	dd if=/dev/zero of="$@" bs=512 count=$(HDD_IMAGE_SECTOR_SIZE)

# Builds a ISO-image with `GRUB` if `make iso` is entered.
iso: $(BUILD_TARGET) $(GRUB_CONFIG_FILE)
	rm -rf "$(ISO_BUILD_FOLDER)"
	@mkdir -p "$(dir $(COMPILED_SYSTEM))"
	@mkdir -p "$(dir $(ISO_CONFIG_PATH))"
	cp "$(BUILD_TARGET)" "$(COMPILED_SYSTEM)"
	cp "$(GRUB_CONFIG_FILE)" "$(ISO_CONFIG_PATH)"
	grub-mkrescue -o "$(ISO_IMAGE)" "$(ISO_BUILD_FOLDER)"
	rm -rf "$(ISO_BUILD_FOLDER)"

# Runs `QEMU` if `make run` is entered. 
run: iso $(HDD_IMAGE)
	qemu-system-i386 -drive file="$(HDD_IMAGE)",format=raw \
					 -cdrom "$(ISO_IMAGE)" \
					 -boot order=d
