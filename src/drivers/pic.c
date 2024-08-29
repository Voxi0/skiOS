#include<skiOS/drivers/pic.h>

// Enums
typedef enum {
	PIC_PORT_MASTER_CMD = 0x20,
	PIC_PORT_SLAVE_CMD = 0xA0,
	PIC_PORT_MASTER_DATA = (PIC_PORT_MASTER_CMD + 1),
	PIC_PORT_SLAVE_DATA = (PIC_PORT_SLAVE_CMD + 1),
} PIC_PORT;
typedef enum {
	PIC_ICW1_ICW4 = 0x01,			// Indicates that ICW4 will be present
	PIC_ICW1_SINGLE = 0x02,			// Single (Cascade) mode
	PIC_ICW1_INTERVAL4 = 0x04,		// Call address interval 4 (8)
	PIC_ICW1_LEVEL = 0x08,			// Level triggered (Edge) mode
	PIC_ICW1_INIT = 0x10,			// Initialization - REQUIRED
} PIC_ICW1;
typedef enum {
	ICW4_8086 = 0x01,				// 8086/88 (MCS-80/85) mode
	ICW4_AUTO = 0x02,				// Automatically send EOI signal (Normal behaviour)
	ICW4_BUF_SLAVE = 0x08,			// Buffered mode/slave
	ICW4_BUF_MASTER = 0x0C,			// Buffered mode/master
	ICW4_SFNM = 0x10,				// Special fully nested (Not)
} PIC_ICW4;
typedef enum {
	PIC_OCW2_EOI = 0x20,
	PIC_OCW3_READ_IRR = 0x0A,
	PIC_OCW3_READ_ISR = 0x0B,
} PIC_OCW;

void picRemap(int offset1, int offset2) {
	// Save the masks
	uint8_t a1 = inb(PIC_PORT_MASTER_DATA), a2 = inb(PIC_PORT_SLAVE_DATA);

	//  ICW1
	outb(PIC_PORT_MASTER_CMD, PIC_ICW1_INIT | PIC_ICW1_ICW4);  // Starts the initialization sequence (in cascade mode)
	ioWait();
	outb(PIC_PORT_SLAVE_CMD, PIC_ICW1_INIT | PIC_ICW1_ICW4);
	ioWait();

	// ICW2
	outb(PIC_PORT_MASTER_DATA, offset1);                 	// ICW2: Master PIC vector offset
	ioWait();
	outb(PIC_PORT_SLAVE_DATA, offset2);                 	// ICW2: Slave PIC vector offset
	ioWait();

	// ICW3
	outb(PIC_PORT_MASTER_DATA, 4);                       	// Tell the master PIC (PIC1) that there is a slave PIC (PIC2) at IRQ2 (0000 0100)
	ioWait();
	outb(PIC_PORT_SLAVE_DATA, 2);                       	// Tell the slave PIC (PIC2) it's cascade identity (0000 0010)
	ioWait();

	// ICW4
	outb(PIC_PORT_MASTER_DATA, ICW4_8086);               	// Tell the PICs to use use the 8086 mode (And not 8080 mode)
	ioWait();
	outb(PIC_PORT_SLAVE_DATA, ICW4_8086);
	ioWait();
	
	// Restore the saved masks
	outb(PIC_PORT_MASTER_DATA, a1);
	outb(PIC_PORT_SLAVE_DATA, a2);
}

// Mask (Disable) / Unmask (Enable) an IRQ (Hardware interrupt)
void picMask(uint8_t irqNum) {
	uint16_t port;
	uint8_t value;
	if(irqNum < 8) port = PIC_PORT_MASTER_DATA;
	else {
		port = PIC_PORT_SLAVE_DATA;
		irqNum -= 8;
	}
	value = inb(port) | (1 << irqNum);
	outb(port, value);
}
void picUnmask(uint8_t irqNum) {
	uint16_t port;
	uint8_t value;
	if(irqNum < 8) port = PIC_PORT_MASTER_DATA;
	else {
		port = PIC_PORT_SLAVE_DATA;
		irqNum -= 8;
	}
	value = inb(port) & ~(1 << irqNum);
	outb(port, value);
}

// Helper func to get the IRQ register
static uint16_t picGetIrqReg(int ocw3) {
    // OCW3 to PIC CMD to get the register values.
	// PIC2 is chained, and * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain
    outb(PIC_PORT_MASTER_CMD, ocw3);
    outb(PIC_PORT_SLAVE_CMD, ocw3);
    return (inb(PIC_PORT_SLAVE_CMD) << 8) | inb(PIC_PORT_MASTER_CMD);
}

// Returns the combined value of the cascaded PICs IRQ-Request/In-Service register
uint16_t picGetIRR(void) {return picGetIrqReg(PIC_OCW3_READ_IRR);}
uint16_t picGetISR(void) {return picGetIrqReg(PIC_OCW3_READ_ISR);}

// Send End Of Interrupt (EOI) signal to the PICs
void picSendEOI(uint8_t irqNum) {
	if(irqNum >= 8) outb(PIC_PORT_SLAVE_CMD, PIC_OCW2_EOI);
	outb(PIC_PORT_MASTER_CMD, PIC_OCW2_EOI);
}

// It's required to disable the PICs if you want to use the processor's local APIC and the IOAPIC
void picDisable(void) {
    outb(PIC_PORT_MASTER_DATA, 0xff);
    outb(PIC_PORT_SLAVE_DATA, 0xff);
}
