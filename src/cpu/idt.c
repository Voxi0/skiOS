#include<skiOS/cpu/idt.h>

// Assembly Functions
extern void loadIDT(idtr_t *idtr);
extern void *isrStubTable[IDT_MAX_DESCRIPTORS];

// IDT
__attribute__((aligned(0x10)))
static idtEntry_t idt[IDT_MAX_DESCRIPTORS]; // Array of IDT Entries - Aligned For Performance
static idtr_t idtr;

// List of All The 32 CPU Exceptions - Order Matters
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

// Array of IRQ Handlers
void (*irqHandlers[16])(uint64_t) = {0};

// Load The IDT
void initIDT(void) {
    // Initialize The IDT Pointer
    idtr.base = (uint64_t)&idt[0];
    idtr.limit = (uint16_t)(sizeof(idt) - 1);

    // Remap The PICs so The IRQs Don't Mess With The ISRs
    picRemap(IRQ_OFFSET, IRQ_OFFSET + 8);

    // Create 32 IDT Entries For CPU Exceptions
    for(uint8_t i = 0; i < 32; i++)
        idtSetDesc(i, isrStubTable[i], IDT_DPL_RING0 | IDT_GATE_INTERRUPT_32BIT | IDT_PRESENT);

    // Create 16 IDT Entries For Hardware Interrupts
    for(uint8_t i = 0; i < 16; i++)
        idtSetDesc(IRQ_OFFSET + i, isrStubTable[IRQ_OFFSET + i], IDT_DPL_RING0 | IDT_GATE_INTERRUPT_32BIT | IDT_PRESENT);

    // Load The New IDT and Set The Interrupt Flag (To Enable Interrupts)
    loadIDT(&idtr);
}

// Helper Function to Define an Entry in The IDT
void idtSetDesc(uint8_t index, void *isr, uint8_t flags) {
    idtEntry_t *entry = &idt[index];
    entry->isrLow = (uint64_t)isr & 0xFFFF;
    entry->kernelCS = 0x08;
    entry->ist = 0;
    entry->attributes = flags;
    entry->isrMid = ((uint64_t)isr >> 16) & 0xFFFF;
    entry->isrHigh = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    entry->always0 = 0;
}

// Install/Uninstall A Handler For an IRQ
void irqRegisterHandler(int irq, irqHandler handler) {
    if(irq >= 0 && irq < 16) irqHandlers[irq] = handler;
    else printf("Invalid IRQ Number: %d\n", irq);
}
void irqDeregisterHandler(int irq) {
    if(irq >= 0 && irq < 16) irqHandlers[irq] = NULL;
    else printf("Invalid IRQ Number: %d\n", irq);
}

// Interrupt Service Routine Handler - Handles CPU Exceptions and Hardware Interrupts
void isrHandler(uint64_t intNum, uint64_t errCode) {
    // Check if The Interrupt/ISR Number is in Range
    if(intNum < 32) {
        // Display an Error Message and if Applicable, An Error Code Too
        if(errCode != 0) printf("CPU Exception Occurred: %s (Error Code: %llu)\n", cpuExceptionMsg[intNum], errCode);
        else printf("CPU Exception Occurred: %s\n", cpuExceptionMsg[intNum]);

        // Disable Interrupts and Indefinitely Halt The System
        asm("cli");
        while(1) asm("hlt");
    } else if(intNum < 48) {
        int irqNum = intNum - IRQ_OFFSET;
        if (irqNum >= 0 && irqNum < 16 && irqHandlers[irqNum] != NULL) irqHandlers[irqNum](irqNum);
        else printf("No Handler For IRQ %d\n", irqNum);
    } else printf("Invalid ISR Number: %llu\n", intNum);
}
