#ifndef IDT_H
#define IDT_H

// Freestanding Headers
#include<stddef.h>
#include<stdint.h>

// skiOS
#include<skiOS/drivers/pic.h>
#include<skiOS/drivers/video.h>

// Enums and Definitions
#define IRQ_OFFSET 32
#define IDT_MAX_DESCRIPTORS 256
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

// Structures
typedef struct {
    uint16_t isrLow;        // Lower 16-Bits of The ISR's Address
    uint16_t kernelCS;      // Kernel Code Segment Selector That The CPU Will Load Into CS Before Calling The ISR
    uint8_t ist;            // The IST in The TSS That The CPU Will Load Into RSP - Set to Zero For Now
    uint8_t attributes;     // Type and Attributes
    uint16_t isrMid;        // Higher 16-Bits of The Lower 32-Bits of The ISR's Address
    uint32_t isrHigh;       // Higher 32-Bits of The ISR's Address
    uint32_t always0;       // Reserved - Always Set to Zero
} __attribute__((packed)) idtEntry_t;
typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idtr_t;

// IRQ Handler Type
typedef void (*irqHandler)(uint64_t irqNum);

// List of All The 32 CPU Exceptions - Order Matters
extern const char *cpuExceptionMsg[32];

// Array of IRQ Handlers
extern void (*irqHandlers[16])(uint64_t);

// IDT Functions
void initIDT(void);
void idtSetDesc(uint8_t index, void *isr, uint8_t flags);
void irqRegisterHandler(int irq, irqHandler handler);
void irqDeregisterHandler(int irq);

#endif
