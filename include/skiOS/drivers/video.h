#pragma once

// Freestanding headers
#include<stddef.h>
#include<stdint.h>

// Limine
#include<limine/limine.h>

// Tiny printf
#include<printf.h>

// skiOS
#include<skiOS/shell.h>

// Definitions
#define TAB_SPACES 4

// Video functions
// General
void initVideo(struct limine_framebuffer *framebuffer, const uint8_t fontWidth, const uint8_t fontHeight);
void resetScreen(void);
uint32_t rgbToHex(int r, int g, int b);

// Getters
uint8_t getFontWidth(void);
uint8_t getFontHeight(void);
size_t getCursorX(void);
size_t getCursorY(void);

// Setters
void setBgColor(uint32_t color);
void setFgColor(uint32_t color);

// Draw shapes
void drawRect(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t color);
