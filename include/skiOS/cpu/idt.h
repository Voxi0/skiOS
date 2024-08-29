#pragma once

// Freestanding headers
#include<stddef.h>
#include<stdint.h>

// skiOS
#include<skiOS/util.h>
#include<skiOS/drivers/pic.h>

// Definitions
#define IRQ_OFFSET 32

// IRQ handler type and array of all the IRQ handlers
typedef void (*irqHandler)(uint8_t irqNum);
extern void (*irqHandlers[16])(uint8_t irqNum);

// List of all the 32 CPU exception messages
extern const char *cpuExceptionMsg[32];

// IDT functions
void initIDT(void);
void irqRegisterHandler(uint8_t irqNum, irqHandler handler);
void irqDeregisterHandler(uint8_t irqNum);
