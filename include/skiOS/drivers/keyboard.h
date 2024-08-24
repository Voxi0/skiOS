#ifndef KEYBOARD_H
#define KEYBORAD_H

// Freestanding headers
#include<stddef.h>
#include<stdint.h>
#include<stdbool.h>

// skiOS
#include<skiOS/drivers/pic.h>
#include<skiOS/cpu/idt.h>

// Types
typedef void (*KeyCallback)(char key, bool pressed);

// Keyboard functions
void initKb(void);
void kbSendCmd(uint8_t cmdByte);
void disableKb(void);

// Register/Unregister a key callback function
void kbRegisterKeyCallback(KeyCallback callback);
void kbUnregisterKeyCallback(void);

#endif
