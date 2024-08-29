#include<skiOS/cpu/io.h>

// IO functions
uint8_t inb(uint16_t port) {
	uint8_t input;
	__asm__ volatile ("inb %w1, %b0"
		: "=a"(input)
        : "Nd"(port)
        : "memory");
	return input;
}
void outb(uint16_t port, uint8_t value) {
	__asm__ volatile ( "outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}
void ioWait(void) {outb(0x80, 0);}
