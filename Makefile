# Variables
OS_NAME := skiOS
SRC_DIR := src
INCLUDE_DIR := include
FONTS_DIR := fonts
BUILD_DIR := build
ISO_DIR := $(BUILD_DIR)/isodir
BOOT_DIR := $(ISO_DIR)/boot
EFI_BOOT_DIR := $(ISO_DIR)/EFI/BOOT

# Source and object files
SRC_FILES := $(shell find $(SRC_DIR) $(FONTS_DIR) -name '*.c' -or -name '*.s' -or -name '*.sfn') $(INCLUDE_DIR)/printf.c
OBJ_FILES := $(SRC_FILES:%=$(BUILD_DIR)/%.o)

# Compiler and linker options
CC := x86_64-elf-gcc
LD := x86_64-elf-ld
CFLAGS := -I $(INCLUDE_DIR) -Wall -Wextra -Werror -std=c23 -pedantic \
		  -m64 -march=x86-64 -mno-80387 -mno-mmx -mno-red-zone \
		  -ffreestanding -fcommon -fno-stack-protector -fno-stack-check -fPIE \
		  -MMD -MP \
		  -lgcc
LDFLAGS := -T linker.ld -m elf_x86_64 -nostdlib -static

# Run/Emulate OS
run: $(BUILD_DIR)/$(OS_NAME).iso
	qemu-system-x86_64 --enable-kvm -m 4G -drive format=raw,file=$<

# OS ISO file
$(BUILD_DIR)/$(OS_NAME).iso: limine.conf $(BOOT_DIR)/$(OS_NAME).bin
	mkdir -p $(BOOT_DIR)/limine $(EFI_BOOT_DIR)
	cp -v limine.conf $(addprefix $(INCLUDE_DIR)/limine/, limine-bios.sys limine-bios-cd.bin limine-uefi-cd.bin) $(BOOT_DIR)/limine/
	cp -v $(addprefix $(INCLUDE_DIR)/limine/, BOOTIA32.EFI BOOTX64.EFI) $(EFI_BOOT_DIR)/
	xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot boot/limine/limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label \
        $(ISO_DIR) -o $@
	./$(INCLUDE_DIR)/limine/limine bios-install $@

# OS binary file
$(BOOT_DIR)/$(OS_NAME).bin: linker.ld $(OBJ_FILES)
	mkdir -p $(dir $@)
	$(LD) -o $@ $(LDFLAGS) $(OBJ_FILES)

# Compilation rules
# C
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# NASM x86_64 assembly
$(BUILD_DIR)/%.s.o: %.s
	mkdir -p $(dir $@)
	nasm -f elf64 $< -o $@

# SSFN fonts
$(BUILD_DIR)/%.sfn.o: %.sfn
	mkdir -p $(dir $@)
	$(LD) -r -b binary $< -o $@

# Clean everything
clean:
	rm -rf $(BUILD_DIR)

# Include the ".d" makefiles - The "-" at the front suppresses the errors of missing makefiles
# This is because initially, all the ".d" files will be missing and we don't want those errors to show up
-include $(DEPS)
