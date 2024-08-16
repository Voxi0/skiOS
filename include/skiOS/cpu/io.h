#ifndef IO_H
#define IO_H

// Freestanding Headers
#include<stdint.h>

// IO Functions
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void ioWait(void);

#endif
