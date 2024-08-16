#ifndef PIC_H
#define PIC_H

// Freestanding Headers
#include <stdint.h>

// skiOS
#include<skiOS/cpu/io.h>

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

// PIC Functions
void picRemap(uint8_t offset1, uint8_t offset2);
void picMask(uint8_t irq);
void picUnmask(uint8_t irq);
uint16_t picGetIRR(void);
uint16_t picGetISR(void);
void picSendEndOfInterrupt(uint8_t irq);
void picDisable(void);

#endif
