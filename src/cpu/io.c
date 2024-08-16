#include<skiOS/cpu/io.h>

// Input Byte From A Port
uint8_t inb(uint16_t port) {
    unsigned char data;
    asm volatile ("inb %1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

// Send A Byte/Word to A Port
void outb(uint16_t port, uint8_t value) {asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));}
void outw(uint16_t port, uint16_t value) {asm volatile ("outw %0, %1" : : "a"(value), "Nd"(port));}

// Read an Unused IO Port to Create A Delay
void ioWait(void) {asm volatile ("outb %%al, $0x80" : : "a"(0));}
