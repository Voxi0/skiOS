#ifndef IDT_H
#define IDT_H

// Freestanding headers
#include<stdint.h>

// skiOS
#include<skiOS/drivers/pic.h>
#include<skiOS/drivers/video.h>

// Definitions and enums
#define MAX_IDT_ENTRIES 256
#define IRQ_OFFSET 32

// IRQ handler type
typedef void (*irqHandler)(uint64_t irqNum);

// List of all the 32 CPU exceptions - Order matters
extern const char *cpuExceptionMsg[32];

// Array of IRQ handlers
extern void (*irqHandlers[16])(uint64_t irqNum);

// IDT functions
void initIDT(void);
void irqRegisterHandler(uint8_t irqNum, irqHandler handler);
void irqDeregisterHandler(uint8_t irqNum);

#endif
