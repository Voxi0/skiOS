#include<skiOS/cpu/io.h>

// Read/Write a byte to/from a port
uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
void outb(uint16_t port, uint8_t data) {__asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));}

// Read/Write a word (16-Bits) to/from a port
uint16_t inw(uint16_t port) {
    uint16_t result;
    __asm__ volatile ("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
void outw(uint16_t port, uint16_t data) {__asm__ volatile ("outw %0, %1" : : "a"(data), "Nd"(port));}

// Read/Write a double word (32-Bits) to/from a port
uint32_t inl(uint16_t port) {
    uint32_t result;
    __asm__ volatile ("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
void outl(uint16_t port, uint32_t data) {__asm__ volatile ("outl %0, %1" : : "a"(data), "Nd"(port));}

// Read an unused IO port to create a delay
void ioWait(void) {asm volatile ("outb %%al, $0x80" : : "a"(0));}
