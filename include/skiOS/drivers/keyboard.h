#ifndef KEYBOARD_H
#define KEYBOARD_H

// Freestanding Headers
#include<stddef.h>
#include<stdint.h>
#include<stdbool.h>

// skiOS
#include<skiOS/drivers/pic.h>
#include<skiOS/cpu/idt.h>
#include<skiOS/cpu/io.h>

// Definitions and Enums
typedef enum {
    KB_PORT_DATA = 0x60,
    KB_PORT_STATUS = 0x64,
    KB_PORT_COMMAND = 0x64
} KB_PORT;
typedef enum {
    KB_CMD_ENABLE_SCANNING = 0xF4,
    KB_CMD_DISABLE_SCANNING = 0xF5
} KB_CMD;
typedef enum {
    // Scancode Definitions
    KB_SCANCODE_RELEASE_MASK = 0x80,

    // Special Keys Scancodes
    KB_SCANCODE_LEFT_SHIFT = 0x2A,
    KB_SCANCODE_RIGHT_SHIFT = 0x36,
    KB_SCANCODE_CAPS_LOCK = 0x3A,
    KB_SCANCODE_BACKSPACE = 0x0E,
    KB_SCANCODE_ENTER = 0x1C,
    KB_SCANCODE_LEFT_ARROW = 0x4B,
    KB_SCANCODE_RIGHT_ARROW = 0x4D,
    KB_SCANCODE_UP_ARROW = 0x48,
    KB_SCANCODE_DOWN_ARROW = 0x50
} KB_SCANCODE;

// Types
typedef void (*KeyCallback)(char key, bool pressed);

// Keyboard Functions
void kbInit(void);
void kbSendCmd(uint8_t cmdByte);
void disableKb(void);

// Register/Unregister Key Callback Function
void kbRegisterKeyCallback(KeyCallback callback);
void kbUnregisterKeyCallback(void);

#endif
