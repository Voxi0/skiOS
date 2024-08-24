#ifndef IO_H
#define IO_H

// Freestanding headers
#include<stdint.h>

// Read/Write a byte to/from a port
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t data);

// Read/Write a word (16-Bits) to/from a port
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t data);

// Read/Write a double word (32-Bits) to/from a port
uint32_t inl(uint16_t port);
void outl(uint16_t port, uint32_t data);

// Read an unused IO port to create a delay
void ioWait(void);

#endif
