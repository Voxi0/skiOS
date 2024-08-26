#pragma once

// Freestanding headers
#include<stdint.h>

// IO functions
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
void ioWait(void);
