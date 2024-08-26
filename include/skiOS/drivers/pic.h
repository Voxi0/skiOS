#pragma once

// Freestanding headers
#include<stdint.h>

// skiOS
#include<skiOS/cpu/io.h>

// PIC functions
void picRemap(int offset1, int offset2);
void picMask(uint8_t irqNum);
void picUnmask(uint8_t irqNum);
uint16_t picGetIRR(void);
uint16_t picGetISR(void);
void picSendEOI(uint8_t irqNum);
void picDisable(void);
