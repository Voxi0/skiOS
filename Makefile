# Compiler and linker options
CC := x86_64-elf-gcc
LD := x86_64-elf-ld
CFLAGS := -I include/ -Wall -Werror -Wextra \
		-m64 -march=x86-64 -mno-80387 -mno-mmx -mno-red-zone \
		-ffreestanding -fcommon -fno-stack-protector -fno-stack-check -fPIE \
		-lgcc
LDFLAGS := -T linker.ld -m elf_x86_64 -nostdlib -static

# Variables
OS_NAME := skiOS

# Directories
SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := include
FONTS_DIR := fonts
ISO_DIR := $(BUILD_DIR)/isodir
BOOT_DIR := $(ISO_DIR)/boot
EFI_BOOT_DIR := $(ISO_DIR)/EFI/BOOT

# Source and object files
SRCS := $(shell find $(SRC_DIR) $(FONTS_DIR) -name '*.c' -or -name '*.s' -or -name '*.sfn') $(INCLUDE_DIR)/printf.c
OBJ_FILES := $(SRCS:%=$(BUILD_DIR)/%.o)

# Automatically generate dependencies
DEPFILES := $(OBJ_FILES:.o=.d)

# Phony targets and default target
.PHONY: all run clean
all: run

# Run/Emulate OS
run: $(BUILD_DIR)/$(OS_NAME).iso
	qemu-system-x86_64 --enable-kvm --drive format=raw,file=$< -m 4G

# OS ISO file
$(BUILD_DIR)/$(OS_NAME).iso: limine.conf $(BOOT_DIR)/$(OS_NAME).bin
	mkdir -p $(dir $@)
	mkdir -p $(BOOT_DIR)/limine $(EFI_BOOT_DIR)
	cp -v limine.conf $(addprefix $(INCLUDE_DIR)/limine/, limine-bios.sys limine-bios-cd.bin limine-uefi-cd.bin) $(BOOT_DIR)/limine/
	cp -v $(addprefix $(INCLUDE_DIR)/limine/, BOOTIA32.EFI BOOTX64.EFI) $(EFI_BOOT_DIR)/
	xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot boot/limine/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        $(ISO_DIR) -o $@
	./$(INCLUDE_DIR)/limine/limine bios-install $@

# OS binary file
$(BOOT_DIR)/$(OS_NAME).bin: linker.ld $(OBJ_FILES)
	mkdir -p $(dir $@)
	$(LD) -o $@ $(LDFLAGS) $(OBJ_FILES)

# Source files
# C
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# NASM assembly
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

# Include the generated dependency files
-include $(DEPFILES)
