#include<skiOS/drivers/pic.h>

// Definitions
#define PIC1_COMMAND 0x20
#define PIC2_COMMAND 0xA0
#define PIC1_DATA (PIC1_COMMAND + 1)
#define PIC2_DATA (PIC2_COMMAND + 1)
#define PIC_EOI 0x20

// First Initialization Command Word (ICW1)
#define ICW1_ICW4 0x01			// Indicates that ICW4 will be present
#define ICW1_SINGLE 0x02		// Single (Cascade) mode
#define ICW1_INTERVAL4 0x04		// Call address interval 4 (8)
#define ICW1_LEVEL 0x08			// Level triggered (Edge) mode
#define ICW1_INIT 0x10			// Initialization - REQUIRED

// Fourth Initialization Command Word (ICW4)
#define ICW4_8086 0x01			// 8086/88 (MCS-80/85) mode
#define ICW4_AUTO 0x02			// Automatically send EOI signal (Normal behaviour)
#define ICW4_BUF_SLAVE 0x08		// Buffered mode/slave
#define ICW4_BUF_MASTER 0x0C	// Buffered mode/master
#define ICW4_SFNM 0x10			// Special fully nested (Not)

#define PIC_READ_IRR 0x0a		// OCW3 irq ready next CMD read
#define PIC_READ_ISR 0x0b		// OCW3 irq service next CMD read

void picRemap(int offset1, int offset2) {
	// Save the masks
	uint8_t a1 = inb(PIC1_DATA), a2 = inb(PIC2_DATA);

	//  ICW1
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // Starts the initialization sequence (in cascade mode)
	ioWait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	ioWait();

	// ICW2
	outb(PIC1_DATA, offset1);                 	// ICW2: Master PIC vector offset
	ioWait();
	outb(PIC2_DATA, offset2);                 	// ICW2: Slave PIC vector offset
	ioWait();

	// ICW3
	outb(PIC1_DATA, 4);                       	// Tell the master PIC (PIC1) that there is a slave PIC (PIC2) at IRQ2 (0000 0100)
	ioWait();
	outb(PIC2_DATA, 2);                       	// Tell the slave PIC (PIC2) it's cascade identity (0000 0010)
	ioWait();

	// ICW4
	outb(PIC1_DATA, ICW4_8086);               	// Tell the PICs to use use the 8086 mode (And not 8080 mode)
	ioWait();
	outb(PIC2_DATA, ICW4_8086);
	ioWait();
	
	// Restore the saved masks
	outb(PIC1_DATA, a1);
	outb(PIC2_DATA, a2);
}

// Mask (Disable) / Unmask (Enable) an IRQ (Hardware interrupt)
void picMask(uint8_t irqNum) {
	uint16_t port;
	uint8_t value;
	if(irqNum < 8) port = PIC1_DATA;
	else {
		port = PIC2_DATA;
		irqNum -= 8;
	}
	value = inb(port) | (1 << irqNum);
	outb(port, value);
}
void picUnmask(uint8_t irqNum) {
	uint16_t port;
	uint8_t value;
	if(irqNum < 8) port = PIC1_DATA;
	else {
		port = PIC2_DATA;
		irqNum -= 8;
	}
	value = inb(port) & ~(1 << irqNum);
	outb(port, value);
}

// Helper func to get the IRQ register
static uint16_t __picGetIrqReg(int ocw3) {
    // OCW3 to PIC CMD to get the register values.
	// PIC2 is chained, and * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

// Returns the combined value of the cascaded PICs IRQ-Request/In-Service register
uint16_t picGetIRR(void) {return __picGetIrqReg(PIC_READ_IRR);}
uint16_t picGetISR(void) {return __picGetIrqReg(PIC_READ_ISR);}

// Send End Of Interrupt (EOI) signal to the PICs
void picSendEOI(uint8_t irqNum) {
	if(irqNum >= 8) outb(PIC2_COMMAND, PIC_EOI);
	outb(PIC1_COMMAND, PIC_EOI);
}

// It's required to disable the PICs if you want to use the processor's local APIC and the IOAPIC
void picDisable(void) {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}
