// Freestanding Headers
#include<stdint.h>
#include<stddef.h>
#include<stdbool.h>

// Limine
#include<limine/limine.h>

// skiOS
#include<skiOS/util.h>

#include<skiOS/cpu/gdt.h>
#include<skiOS/cpu/idt.h>

#include<skiOS/memory/pmm.h>

#include<skiOS/drivers/video.h>
#include<skiOS/shell.h>

// Set Limine Base Revision to The Latest Base Revision Described by The Limine Boot Protocol Specs
__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2)

// Start And End Markers For Limine Requests
__attribute__((used, section(".requests_start_marker")))
static volatile LIMINE_REQUESTS_START_MARKER
__attribute__((used, section(".requests_end_marker")))
static volatile LIMINE_REQUESTS_END_MARKER

// Disable System Interrupts and Halt System
static void halt(void) {
    asm("cli");
    for(;;) asm("hlt");
}

// Kernel Entry Point
void kmain(void) {
    // Ensure That The Bootloader Supports Our Base Revision and That we Have A Framebuffer
    if(!LIMINE_BASE_REVISION_SUPPORTED) halt();

    // Fetch The First Framebuffer and Initialize The Video Driver
    uint64_t fontWidth = 8, fontHeight = 16;
    initVideo(fontWidth, fontHeight);
    setBgColor(rgbToHex(30, 30, 46));
    setFgColor(rgbToHex(205, 214, 244));
    resetScreen();

    // Initialize The GDT and IDT
    initGDT();
    printf("Initialized The GDT\n");
    initIDT();
    printf("Initialized The IDT\n");

    // Initialize The PMM (Physical Memory Manager)
    initPMM();
    printf("Initialized The Physical Memory Manager (PMM)\n");

    // Initialize The Shell and Register More Commands
    shellInit();

    // Main Loop
    while(1);
}
