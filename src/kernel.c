// Freestanding headers
#include<stddef.h>
#include<stdint.h>
#include<stdbool.h>

// Limine
#include<limine/limine.h>

// skiOS
#include<skiOS/cpu/gdt.h>
#include<skiOS/cpu/idt.h>

#include<skiOS/drivers/video.h>
#include<skiOS/shell.h>

// Set limine base revision version number to the latest as specified in the specs
__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2)

// Mark the start and end markers for limine requests
__attribute__((used, section(".requests_start_marker")))
static volatile LIMINE_REQUESTS_START_MARKER
__attribute__((used, section(".requests_end_marker")))
static volatile LIMINE_REQUESTS_END_MARKER

// Disable system interrupts and halt system
static void halt(void) {
    asm("cli");
    for(;;) asm("hlt");
}

// Kernel main function
void kmain(void) {
    // Ensure that the set base revision version is supported
    if(LIMINE_BASE_REVISION_SUPPORTED != true) halt();

    // Initialize basic things
    initGDT();
    initIDT();

    // Initialize the video driver
    initVideo(8, 16);
    setBgColor(rgbToHex(30, 30, 46));
    setFgColor(rgbToHex(205, 214, 244));
    resetScreen();

    // Initialize the shell
    initShell();

    // Infinite loop
    while(1);
}
