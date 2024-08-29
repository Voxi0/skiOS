#include<skiOS/cpu/idt.h>

// Structures
typedef struct {
	uint16_t isrLow;		// Lower 16-Bits of the ISR's address
	uint16_t kernelCS;		// GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t ist;			// The IST in the TSS that the CPU will load into RSP - Set to 0 for now
	uint8_t attributes;		// Type and attributes
	uint16_t isrMid;		// Higher 16-Bits of the lower 32-Bits of the ISR's address
	uint32_t isrHigh;		// Higher 32-Bits of the ISR's address
	uint32_t reserved;		// Always set to 0
} __attribute__((packed)) idtEntry_t;
typedef struct {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed)) idtr_t;

// Definitions and enums
#define IDT_PRESENT 0x80
typedef enum {
    IDT_GATE_INTERRUPT_16BIT = 0x6,
    IDT_GATE_INTERRUPT_32BIT = 0xE,
    IDT_GATE_TRAP_16BIT = 0x7,
    IDT_GATE_TRAP_32BIT = 0xF,
    IDT_GATE_TASK = 0x5,
} IDT_GATE;
typedef enum {
    IDT_DPL_RING0 = 0x00,
    IDT_DPL_RING1 = 0x20,
    IDT_DPL_RING2 = 0x40,
    IDT_DPL_RING3 = 0x60,
} IDT_DPL;

// Assembly code
extern void loadIDT(idtr_t*);
extern void *isrStubTable[];

// IDT - Aligned for performance
__attribute__((aligned(0x10)))
static idtEntry_t idt[256];
static idtr_t idtr;

// IRQ (Hardware interrupts) handlers
void (*irqHandlers[16])(uint8_t irqNum) = {NULL};

// CPU exception messages
const char *cpuExceptionMsg[32] = {
	"Division Error",
	"Debug",
	"Non-Maskable Interrupt",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-Segment Fault",
	"General Protection Fault",
	"Page Fault",
	"Reserved",
	"x87 Floating-Point Exception",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point Exception",
	"Virtualization Exception",
	"Control Protection Exception",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Hyprvisor Injection Exception",
	"VMM Communication Exception",
	"Security Exception",
	"Reserved",
};

// Set a IDT descriptor
static void idtSetDesc(uint8_t vector, void* isr, uint8_t flags) {
    idtEntry_t *descriptor = &idt[vector];
    descriptor->isrLow = (uint64_t)isr & 0xFFFF;
    descriptor->kernelCS = 0x08;
    descriptor->ist = 0;
    descriptor->attributes = flags;
    descriptor->isrMid = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isrHigh = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved = 0;
}

// Initialize the IDT
void initIDT(void) {
	// Initialize the IDTR
	idtr.base = (uint64_t)&idt[0];
	idtr.limit = sizeof(idt) - 1;

	// Remap the PICs to avoid conflicts with the 32 CPU exception ISRs
	picRemap(IRQ_OFFSET, IRQ_OFFSET + 16);

	// Set the 32 CPU exception IDT descriptors
	for(uint8_t i = 0; i < 32; i++)
		idtSetDesc(i, isrStubTable[i], IDT_PRESENT | IDT_DPL_RING0 | IDT_GATE_INTERRUPT_32BIT);
	
	// Set the 16 IRQ (Hardware interrupts) IDT descriptors
	for(uint8_t i = 0; i < 16; i++)
		idtSetDesc(IRQ_OFFSET + i, isrStubTable[IRQ_OFFSET + i], IDT_PRESENT | IDT_DPL_RING0 | IDT_GATE_INTERRUPT_32BIT);
	
	// Load the IDT
	loadIDT(&idtr);
}

// Register/Deregister an IRQ (Hardware interrupt) handler
void irqRegisterHandler(uint8_t irqNum, irqHandler handler) {
	assert(irqNum < 16);
	irqHandlers[irqNum] = handler;
}
void irqDeregisterHandler(uint8_t irqNum) {
	assert(irqNum < 16);
	irqHandlers[irqNum] = NULL;
}
