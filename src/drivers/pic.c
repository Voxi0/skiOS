#include<skiOS/drivers/pic.h>

// Remap The PICs to Avoid Conflicts With The CPU Exceptions ISRs
void picRemap(uint8_t offset1, uint8_t offset2) {
    // Save Masks
    uint8_t a1 = inb(PIC_PORT_MASTER_DATA);
    uint8_t a2 = inb(PIC_PORT_SLAVE_DATA);

    // Start Initialization Sequence
    outb(PIC_PORT_MASTER_COMMAND, PIC_ICW1_INITIALIZE | PIC_ICW1_ICW4);
    outb(PIC_PORT_SLAVE_COMMAND, PIC_ICW1_INITIALIZE | PIC_ICW1_ICW4);

    // Set Vector Offsets
    outb(PIC_PORT_MASTER_DATA, offset1);
    outb(PIC_PORT_SLAVE_DATA, offset2);

    // Tell The PICs That There's A Master/Slave Relationship
    outb(PIC_PORT_MASTER_DATA, 0x04);
    outb(PIC_PORT_SLAVE_DATA, 0x02);

    // Set 8086/88 Mode
    outb(PIC_PORT_MASTER_DATA, PIC_ICW4_8086);
    outb(PIC_PORT_SLAVE_DATA, PIC_ICW4_8086);

    // Restore Masks
    outb(PIC_PORT_MASTER_DATA, a1);
    outb(PIC_PORT_SLAVE_DATA, a2);
}

// Mask (Disable) / Unmask (Enable) A Specific IRQ
void picMask(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    if (irq < 8) port = PIC_PORT_MASTER_DATA;
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
    if (irq < 8) port = PIC_PORT_MASTER_DATA;
    else {
        port = PIC_PORT_SLAVE_DATA;
        irq -= 8;
    }
    value = inb(port);
    value &= ~(1 << irq);
    outb(port, value);
}

// Get Interrupt Request Register / In-Service Register
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

// Send End of Interrupt Signal to The PICs - Call at The End of an IRQ Handler
void picSendEndOfInterrupt(uint8_t irq) {
    if(irq >= 8) outb(PIC_PORT_SLAVE_COMMAND, PIC_CMD_EOI);
    outb(PIC_PORT_MASTER_COMMAND, PIC_CMD_EOI);
}

// Disable The PICs - Use When Using APICs Instead
void picDisable(void) {
    outb(PIC_PORT_MASTER_COMMAND, 0xFF);
    outb(PIC_PORT_SLAVE_COMMAND, 0xFF);
}
