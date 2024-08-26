#pragma once

// Freestanding headers
#include<stdint.h>
#include<stdbool.h>

// skiOS
#include<skiOS/cpu/idt.h>
#include<skiOS/drivers/pic.h>
#include<skiOS/cpu/io.h>
#include<skiOS/drivers/video.h>

// Key callback type
typedef void (*KeyCallback)(char key, bool pressed);

// Keyboard functions
void initKb(void);
void kbRegisterKeyCallback(KeyCallback handler);
void kbDeregisterKeyCallback(void);
void disableKb(void);
