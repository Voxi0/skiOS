#ifndef PIC_H
#define PIC_H

// Freestanding headers
#include<stdint.h>

// skiOS
#include<skiOS/cpu/io.h>

// PIC functions
void picRemap(uint8_t offset1, uint8_t offset2);
void picMask(uint8_t irq);
void picUnmask(uint8_t irq);
uint16_t picGetIRR(void);
uint16_t picGetISR(void);
void picSendEndOfInterrupt(uint8_t irq);
void picDisable(void);

#endif
