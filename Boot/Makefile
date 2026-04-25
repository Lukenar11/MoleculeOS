OBJS += \
$(BUILD)/Boot.o \
$(BUILD)/StackTop.o \
$(BUILD)/MultibootHeader.o

$(BUILD)/%.o: Boot/%.asm
	nasm $(ASMFLAGS) $< -o $@
