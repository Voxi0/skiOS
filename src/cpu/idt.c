#include<skiOS/cpu/idt.h>

// Structures
typedef struct {
    uint16_t isrLow;        // The lower 16-Bits of the ISR's address
    uint16_t kernelCS;      // Kernel code segment selector - Depends on how the GDT is set up
    uint8_t ist;            // The IST in the TSS that the CPU will load into RSP - Set to 0 for now
    uint8_t attributes;     // Type and attributes
    uint16_t isrMid;        // The higher 16-Bits of the lower 32-Bits of the ISR's address
    uint32_t isrHigh;       // The higher 32-Bits of the ISR's address
    uint32_t reserved;      // Always set to 0
} __attribute__((packed)) idtEntry_t;
typedef struct {
    uint16_t limit;         // Size of the IDT
    uint64_t base;          // Pointer to the first entry in the IDT
} __attribute__((packed)) idtr_t;

// Enums
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

// Assembly
extern void loadIDT(idtr_t*);
extern void *isrStubTable[];

// IDT
__attribute__((aligned(0x10))) 
static idtEntry_t idt[MAX_IDT_ENTRIES];
static idtr_t idtr;

// List of all the 32 CPU exceptions - Order matters
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
    "Invalid Task-State-Segment",
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
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved",
};

// IRQ (Hardware interrupts) handlers
void (*irqHandlers[16])(uint64_t irqNum) = {NULL};

// Define an IDT entry
static void setIDTDesc(uint8_t num, void *isr, uint8_t flags) {
    // Pointer to an entry in the IDT
    idtEntry_t *entry = &idt[num];

    // Define the entry
    entry->isrLow = (uint64_t)isr & 0xFFFF;
    entry->kernelCS = 0x08;
    entry->ist = 0;
    entry->attributes = flags;
    entry->isrMid = ((uint64_t)isr >> 16) & 0xFFFF;
    entry->isrHigh = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    entry->reserved = 0;
}

// Initialize the IDT
void initIDT(void) {
    // Initialize the IDT pointer
    idtr.base = (uint64_t)&idt[0];
    idtr.limit = sizeof(idt) - 1;

    // Remap the PICs so that the IRQs (Hardware interrupts) don't mess with the CPU exceptions ISRs
    picRemap(IRQ_OFFSET, IRQ_OFFSET + 8);

    // Create the 32 IDT entries for CPU exceptions
    for(uint8_t i = 0; i < 32; i++)
        setIDTDesc(i, isrStubTable[i], IDT_PRESENT | IDT_DPL_RING0 | IDT_GATE_INTERRUPT_32BIT);

    // Create the 16 IDT entries for IRQs (Hardware interrupts)
    for(uint8_t i = 0; i < 16; i++)
        setIDTDesc(IRQ_OFFSET + i, isrStubTable[IRQ_OFFSET + i], IDT_PRESENT | IDT_DPL_RING0 | IDT_GATE_INTERRUPT_32BIT);
    
    // Load the new IDT and enable system interrupts
    loadIDT(&idtr);
}

// Register/Deregister an IRQ (Hardware interrupt) handler
void irqRegisterHandler(uint8_t irqNum, irqHandler handler) {
    if(irqNum < 16) irqHandlers[irqNum] = handler;
    else printf("Invalid IRQ number: %d\n", irqNum);
}
void irqDeregisterHandler(uint8_t irqNum) {
    if(irqNum < 16) irqHandlers[irqNum] = NULL;
    else printf("Invalid IRQ number: %d\n", irqNum);
}

// Interrupt handler
void interruptHandler(uint64_t intNum, uint64_t errCode) {
    // Handle CPU exceptions
    if(intNum < 32) {
        // Display an error message and if applicable, an error code too
        if(errCode != 0) printf("CPU exception occurred: %s (Error code: %llu)\n", cpuExceptionMsg[intNum], errCode);
        else printf("CPU exception occurred: %s\n", cpuExceptionMsg[intNum]);

        // Disable system interrupts and indefinitely halt the system
        asm("cli");
        for(;;) asm("hlt");
    }

    // Handle IRQs (Hardware interrupts)
    else if(intNum < 48) {
        uint8_t irqNum = intNum - IRQ_OFFSET;
        if(irqNum < 16 && irqHandlers[irqNum] != NULL) irqHandlers[irqNum](irqNum);
        else printf("No handler for IRQ: %d\n", irqNum);
    }

    // Invalid ISR number
    else printf("Invalid ISR number: %llu\n", intNum);
}
