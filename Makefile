# Compiler and linker options
CC := x86_64-elf-gcc
LD := x86_64-elf-ld
CFLAGS := -I include/ \
		-m64 \
		-Wall -Werror -Wextra \
		-ffreestanding -fcommon -fno-stack-protector -fno-builtin -fPIE \
		-lgcc
LDFLAGS := -nostdlib

# Variables
OS_NAME := skiOS
SRC_DIR := src
OBJ_DIR := build
ISO_DIR := $(OBJ_DIR)/isodir
BOOT_DIR := $(ISO_DIR)/boot
EFI_BOOT_DIR := $(ISO_DIR)/EFI/BOOT
OBJ_FILES := $(OBJ_DIR)/asm.o $(OBJ_DIR)/util.o $(OBJ_DIR)/io.o $(OBJ_DIR)/gdt.o $(OBJ_DIR)/idt.o $(OBJ_DIR)/kernel.o $(OBJ_DIR)/pmm.o $(OBJ_DIR)/vmm.o $(OBJ_DIR)/pic.o $(OBJ_DIR)/video.o $(OBJ_DIR)/keyboard.o $(OBJ_DIR)/shell.o $(OBJ_DIR)/printf.o $(OBJ_DIR)/unifont.o

# Phony Targets and Default Target
.PHONY: all run clean
all: build/$(OS_NAME).iso

# Run/Emulate OS
run: build/$(OS_NAME).iso
	@qemu-system-x86_64 --enable-kvm -m 4048 -drive file=$<,format=raw -serial mon:stdio

# OS ISO File
build/$(OS_NAME).iso: limine.conf $(BOOT_DIR)/$(OS_NAME).bin
	@mkdir -p $(BOOT_DIR)/limine $(EFI_BOOT_DIR)
	@cp -v limine.conf include/limine/limine-bios.sys include/limine/limine-bios-cd.bin include/limine/limine-uefi-cd.bin $(BOOT_DIR)/limine
	@cp -v include/limine/BOOTIA32.EFI include/limine/BOOTX64.EFI $(EFI_BOOT_DIR)/
	@xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine/limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label \
		$(ISO_DIR) -o $@
	@./include/limine/limine bios-install $@

# OS Binary
$(BOOT_DIR)/$(OS_NAME).bin: linker.ld $(OBJ_FILES)
	@mkdir -p $(BOOT_DIR)
	@$(LD) -o $@ $(LDFLAGS) -T linker.ld $(OBJ_FILES)
	@rm -f $(OBJ_FILES)

# Compilation Rules
# "src/"
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# "src/cpu/"
$(OBJ_DIR)/%.o: $(SRC_DIR)/cpu/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# "src/memory/"
$(OBJ_DIR)/%.o: $(SRC_DIR)/memory/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# "src/drivers/"
$(OBJ_DIR)/%.o: $(SRC_DIR)/drivers/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# Assembly Files in
# "src/"
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	@nasm -f elf64 $< -o $@

# Libraries in
# "include/printf/"
$(OBJ_DIR)/printf.o: include/printf/printf.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# Fonts in Fonts Directory
$(OBJ_DIR)/unifont.o: fonts/unifont.sfn
	@$(LD) -r -b binary -o $@ $<

# Clean Everything
clean:
	@rm -rf build
	@mkdir -p $(BOOT_DIR)/limine $(EFI_BOOT_DIR)
