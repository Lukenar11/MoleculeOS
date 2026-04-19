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

SRCS_C = \
Runtime/C/string.c

SRCS_CPP = \
Kernel/src/kernel_main.cpp \
Kernel/src/IDT/IDT.cpp \
Kernel/src/IDT/ISR/isr_common_handler.cpp \
Drivers/VGA/src/VGA.cpp \
Terminal/src/Terminal.cpp \
Runtime/CPP/ConsoleIO.cpp

SRCS_ASM = \
Boot/Boot.asm \
Boot/MultibootHeader.asm \
Kernel/src/IDT/ISR/ISR.asm \
Kernel/src/IDT/LoadIDT.asm

C_OBJ   = $(SRCS_C:%.c=$(BUILD)/%.o)
CPP_OBJ = $(SRCS_CPP:%.cpp=$(BUILD)/%.o)
ASM_OBJ = $(SRCS_ASM:%.asm=$(BUILD)/%.o)

OBJS = $(C_OBJ) $(CPP_OBJ) $(ASM_OBJ)

all: dirs $(KERNEL)

dirs:
	@mkdir -p $(BUILD)

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# C
$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

# C++
$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ASM
$(BUILD)/%.o: %.asm
	@mkdir -p $(dir $@)
	nasm $(ASMFLAGS) $< -o $@

run: all
	qemu-system-i386 -kernel $(KERNEL)

clean:
	rm -rf $(BUILD)

.PHONY: all run clean dirs
