#include<skiOS/drivers/pic.h>

// Enums
typedef enum {
    PIC_PORT_MASTER_COMMAND = 0x20,
    PIC_PORT_MASTER_DATA = 0x21,
    PIC_PORT_SLAVE_COMMAND = 0xA0,
    PIC_PORT_SLAVE_DATA = 0xA1,
} PIC_PORT;
typedef enum {
    PIC_ICW1_ICW4 = 0x01,
    PIC_ICW1_SINGLE = 0x02,
    PIC_ICW1_INTERVAL4 = 0x04,
    PIC_ICW1_LEVEL = 0x08,
    PIC_ICW1_INITIALIZE = 0x10,
} PIC_ICW1;
typedef enum {
    PIC_ICW4_8086 = 0x1,
    PIC_ICW4_AUTO_EOI = 0x2,
    PIC_ICW4_BUFFER_MASTER = 0x4,
    PIC_ICW4_BUFFER_SLAVE = 0x0,
    PIC_ICW4_BUFFERRED = 0x8,
    PIC_ICW4_SFNM = 0x10,
} PIC_ICW4;
typedef enum {
    PIC_CMD_EOI = 0x20,
    PIC_CMD_READ_IRR = 0x0A,
    PIC_CMD_READ_ISR = 0x0B,
} PIC_CMD;

// Remap the PICs to avoid conflicts with the CPU exceptions ISRs
void picRemap(uint8_t offset1, uint8_t offset2) {
    // Save masks
    uint8_t a1 = inb(PIC_PORT_MASTER_DATA);
    uint8_t a2 = inb(PIC_PORT_SLAVE_DATA);

    // Start initialization sequence
    outb(PIC_PORT_MASTER_COMMAND, PIC_ICW1_INITIALIZE | PIC_ICW1_ICW4);
    outb(PIC_PORT_SLAVE_COMMAND, PIC_ICW1_INITIALIZE | PIC_ICW1_ICW4);

    // Set vector offsets
    outb(PIC_PORT_MASTER_DATA, offset1);
    outb(PIC_PORT_SLAVE_DATA, offset2);

    // Tell the PICs that there's a master/slave relationship
    outb(PIC_PORT_MASTER_DATA, 0x04);
    outb(PIC_PORT_SLAVE_DATA, 0x02);

    // Set 8086/88 mode
    outb(PIC_PORT_MASTER_DATA, PIC_ICW4_8086);
    outb(PIC_PORT_SLAVE_DATA, PIC_ICW4_8086);

    // Restore masks
    outb(PIC_PORT_MASTER_DATA, a1);
    outb(PIC_PORT_SLAVE_DATA, a2);
}

// Mask (Disable) / Unmask (Enable) a specific IRQ (Hardware interrupt)
void picMask(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    if(irq < 8) port = PIC_PORT_MASTER_DATA;
    else {
        port = PIC_PORT_SLAVE_DATA;
        irq -= 8;
    }
    value = inb(port);
    value |= (1 << irq);
    outb(port, value);
}
void picUnmask(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    if(irq < 8) port = PIC_PORT_MASTER_DATA;
    else {
        port = PIC_PORT_SLAVE_DATA;
        irq -= 8;
    }
    value = inb(port);
    value &= ~(1 << irq);
    outb(port, value);
}

// Get Interrupt Request Register (IRR) / In-Service Register (ISR)
uint16_t picGetIRR(void) {
    outb(PIC_PORT_MASTER_COMMAND, PIC_CMD_READ_IRR);
    outb(PIC_PORT_SLAVE_COMMAND, PIC_CMD_READ_IRR);
    return (inb(PIC_PORT_MASTER_DATA) | (inb(PIC_PORT_SLAVE_DATA) << 8));
}
uint16_t picGetISR(void) {
    outb(PIC_PORT_MASTER_COMMAND, PIC_CMD_READ_ISR);
    outb(PIC_PORT_SLAVE_COMMAND, PIC_CMD_READ_ISR);
    return (inb(PIC_PORT_MASTER_DATA) | (inb(PIC_PORT_SLAVE_DATA) << 8));
}

// Send End-Of-Interrupt (EOI) signal to the PICs - Always call at the end of an IRQ (Hardware interrupt) handler
void picSendEndOfInterrupt(uint8_t irq) {
    if(irq >= 8) outb(PIC_PORT_SLAVE_COMMAND, PIC_CMD_EOI);
    outb(PIC_PORT_MASTER_COMMAND, PIC_CMD_EOI);
}

// Disable the PICs - Use when using APICs instead of the PICs
void picDisable(void) {
    outb(PIC_PORT_MASTER_COMMAND, 0xFF);
    outb(PIC_PORT_SLAVE_COMMAND, 0xFF);
}
