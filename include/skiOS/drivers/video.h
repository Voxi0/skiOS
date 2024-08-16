#ifndef VIDEO_H
#define VIDEO_H

// Freestanding Headers
#include<stddef.h>
#include<stdint.h>

// Limine
#include<limine/limine.h>

// Tiny Printf
#include<printf/printf.h>

// skiOS
#include<skiOS/util.h>
#include<skiOS/shell.h>

// Color Functions
uint32_t rgbToHex(int r, int g, int b);
uint32_t rgbaToHex(int r, int g, int b, int a);
void setBgColor(uint32_t bgColor);
void setFgColor(uint32_t fgColor);

// Graphical Functions
void initVideo(size_t glyphWidth, size_t glyphHeight);
void drawPixel(uint64_t xPos, uint64_t yPos, uint32_t color);
void resetScreen(void);

// Draw Shapes
void drawRect(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t color);
void drawCircle(int centerX, int centerY, int radius, uint32_t color);

// Cursor
void setCursorPos(uint64_t xOffset, uint64_t yOffset);

// Getters
int getFontWidth(void);
int getFontHeight(void);
int getCursorPosX(void);
int getCursorPosY(void);

#endif
