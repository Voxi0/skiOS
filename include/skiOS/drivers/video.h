#ifndef VIDEO_H
#define VIDEO_H

// Freestanding headers
#include<stdint.h>

// Limine
#include<limine/limine.h>

// Tiny Printf
#include<printf.h>

// skiOS
#include<skiOS/memory/buddy.h>
#include<skiOS/util.h>
#include<skiOS/shell.h>

// Definitions
#define TAB_SPACES 4

// Video functions
void initVideo(uint8_t glyphWidth, uint8_t glyphHeight);
void resetScreen(void);

// Convert RGB to hex color code
uint32_t rgbToHex(int r, int g, int b);

// Getters
int getFontWidth(void);
int getFontHeight(void);
int getCursorPosX(void);
int getCursorPosY(void);

// Setters
void setFgColor(uint32_t color);
void setBgColor(uint32_t color);

// Draw shapes
void drawRect(uint64_t x, uint64_t y, int width, int height, uint32_t color);

#endif
