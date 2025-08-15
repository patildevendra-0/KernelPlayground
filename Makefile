
# Makefile
# If you have a cross-compiler, set CROSS=i686-elf-  (note the trailing dash)
CROSS = i686-linux-gnu-

AS      := nasm
CC      := $(CROSS)gcc
LD      := $(CROSS)gcc      # use gcc driver to link so libgcc is found
OBJCOPY := $(CROSS)objcopy

BUILD   := build
DIST    := dist
ISO_DIR := iso

CFLAGS  := -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-pic -fno-pie -fno-stack-protector
LDFLAGS := -ffreestanding -nostdlib -Wl,-T,src/linker.ld -Wl,--build-id=none -Wl,-no-pie

# If not using a cross-compiler, force 32-bit codegen & linking
ifeq ($(CROSS),)
  CFLAGS  += -m32
endif

SRCS := src/boot.s src/kernel.c src/keyboard.asm
OBJS := $(BUILD)/boot.o $(BUILD)/kernel.o $(BUILD)/keyboard.o

.PHONY: all clean run iso gdb dirs

all: dirs $(DIST)/kernel.elf

$(BUILD)/boot.o: src/boot.s
	$(AS) -f elf32 $< -o $@

$(BUILD)/keyboard.o: src/keyboard.asm
	$(AS) -f elf32 $< -o $@

$(BUILD)/kernel.o: src/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

$(DIST)/kernel.elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -lgcc -o $@

iso: all
	@mkdir -p $(ISO_DIR)/boot/grub
	cp $(DIST)/kernel.elf $(ISO_DIR)/boot/kernel.elf
	grub-mkrescue -o $(DIST)/myos.iso $(ISO_DIR)

run: iso
	qemu-system-i386 -cdrom $(DIST)/myos.iso

# Launch QEMU paused and waiting for GDB on port 1234
run-gdb: iso
	qemu-system-i386 -cdrom $(DIST)/myos.iso -s -S

# In another terminal: make gdb (or run the commands manually)
gdb:
	@echo "(gdb) target remote localhost:1234"; \
	echo "(gdb) symbol-file $(DIST)/kernel.elf"; \
	echo "(gdb) continue";

clean:
	rm -rf $(BUILD) $(DIST) $(ISO_DIR)/boot/kernel.elf

dirs:
	@mkdir -p $(BUILD) $(DIST)