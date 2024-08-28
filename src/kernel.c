// Freestanding headers
#include<stddef.h>
#include<stdint.h>
#include<stdbool.h>

// Limine
#include<limine/limine.h>

// skiOS
// Some useful C functions
#include<skiOS/util.h>

// CPU
#include<skiOS/cpu/gdt.h>
#include<skiOS/cpu/idt.h>

// Memory management
#include<skiOS/memory/paging.h>

// Drivers
#include<skiOS/drivers/video.h>

// Shell
#include<skiOS/shell.h>

// Set the base revision to the latest version as specified in the Limine boot protocol specs
__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2)

// Limine requests
__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request fbRequest = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.response = NULL,
};

__attribute__((used, section(".requests")))
static volatile struct limine_paging_mode_request pagingModeRequest = {
    .id = LIMINE_PAGING_MODE_REQUEST,
    .mode = LIMINE_PAGING_MODE_X86_64_4LVL,
    .min_mode = LIMINE_PAGING_MODE_MIN,
    .max_mode = LIMINE_PAGING_MODE_MAX,
    .response = NULL,
};

// Define the start and end markers for Limine requests
__attribute__((used, section(".requests_start_marker")))
static volatile LIMINE_REQUESTS_START_MARKER
__attribute__((used, section(".requests_end_marker")))
static volatile LIMINE_REQUESTS_END_MARKER

// Disable system interrupts and halt system
static void halt(void) {
	__asm__("cli");
	for(;;) __asm__("hlt");
}

// Main kernel function
void kmain(void) {
	// Ensure that the base revision is supported
	if(LIMINE_BASE_REVISION_SUPPORTED == false) halt();

	// Initialize the Global Descriptor Table (GDT) and Interrupt Descriptor Table (IDT)
	initGDT();
	initIDT();

	// Initialize memory management
	initPaging(pagingModeRequest.response);

	// Initialize the video driver - Pass the first available framebuffer
	initVideo(fbRequest.response->framebuffers[0], 8, 16);
	setBgColor(rgbToHex(30, 30, 46));
	setFgColor(rgbToHex(205, 214, 244));
	resetScreen();

	// Initialize the shell
	initShell();

	// Loop
	while(1);
}

// Interrupt handler
void interruptHandler(uint64_t intNum, uint64_t errCode) {
	// Handle CPU exceptions
	if(intNum < 32) {
		// Display error message and error code (If applicable) before halting the system indefinitely
		if(errCode != 0) printf("CPU exception occurred: %s (Error code: %llu)\n", cpuExceptionMsg[intNum], errCode);
		else printf("CPU exception occurred: %s\n", cpuExceptionMsg[intNum]);
		halt();
	}

	// Handle IRQs (Hardware interrupts)
	else if(intNum < 32 + IRQ_OFFSET) {
		uint8_t irqNum = intNum - IRQ_OFFSET;
		if(irqHandlers[irqNum] != NULL) irqHandlers[irqNum](irqNum);
		else printf("No handler for IRQ: %d\n", irqNum);
	}

	// Invalid interrupt number
	else printf("Invalid interrupt number %llu\n", intNum);
}
