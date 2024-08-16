#include<skiOS/drivers/video.h>

// SSFN
#define SSFN_NOIMPLEMENTATION
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#include<scalable-font2/ssfn.h>

// Framebuffer
__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request fbRequest = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
};

// Font
extern char _binary_fonts_unifont_sfn_start;

// Framebuffer
static volatile struct limine_framebuffer *fb = NULL;

// Terminal and Font Dimensions and Cursor Last Position
static int termRows, termColumns;
static int fontWidth, fontHeight, lastCursorX, lastCursorY;

// More Shell Command Handlers
// Get Framebuffer Information
static void cmdFbInfo(int, char *[]) {
    volatile struct limine_framebuffer *fb = fbRequest.response->framebuffers[0];
    printf("Framebuffer Info:\n");
    printf("\tWidth: %d\n", fb->width);
    printf("\tHeight: %d\n", fb->height);
    printf("\tPitch: %d\n", fb->pitch);
    printf("\tBPP: %d\n", fb->bpp);
    printf("\tGlyph Width: %d\n", fontWidth);
    printf("\tGlyph Height: %d\n", fontHeight);
}

// Terminal Scrolling
static void scrollTerm(void) {
	// Framebuffer Address and The Total Visible Screen Area Minus The Height of One Row
    uint8_t *fbPtr = (uint8_t*)fb->address;
    size_t screenSizeInBytes = fb->pitch * (fb->height - fontHeight);

    // Move The Framebuffer Content up by One Row
    memmove(fbPtr, fbPtr + fb->pitch * fontHeight, screenSizeInBytes);

    // Clear The Last Row
    // Pointer to The Start of The Last Row and Size of 1 Row in Bytes
    uint32_t *lastRow = (uint32_t*)(fbPtr + screenSizeInBytes);
    size_t lastRowSizeInBytes = fb->width * sizeof(uint32_t);

    // Stack-Allocated Buffer to Fill The Last Row With The Background Color
    uint32_t tempRow[fb->width];
    for(size_t i = 0; i < fb->width; i++) tempRow[i] = ssfn_dst.bg;
    memcpy(lastRow, tempRow, lastRowSizeInBytes);

    // Reset Cursor Position to The Beginning of The Last Row
    ssfn_dst.x = 0;
    ssfn_dst.y = fb->height - fontHeight;
}

// Initialize The Video Driver
void initVideo(size_t glyphWidth, size_t glyphHeight) {
    // Ensure That we Have A Framebuffer
    if(fbRequest.response == NULL || fbRequest.response->framebuffer_count < 1) {
        // Disable System Interrupts and Halt System
        asm("cli");
        for(;;) asm("hlt");
    }

	// Fetch The First Available Framebuffer
	fb = fbRequest.response->framebuffers[0];

	// Set Glyph and Terminal Dimensions
	fontWidth = glyphWidth;
	fontHeight = glyphHeight;
	termRows = fb->width / glyphWidth;
	termColumns = fb->height / glyphHeight;

    // Initialize SSFN
    ssfn_src = (ssfn_font_t*)&_binary_fonts_unifont_sfn_start;
    ssfn_dst.ptr = (uint8_t*)fb->address;
    ssfn_dst.w = fb->width;
    ssfn_dst.h = fb->height;
    ssfn_dst.p = fb->pitch;
    ssfn_dst.x = ssfn_dst.y = 0;

    // Register More Shell Commands
    shellRegisterCmd("fbInfo", &cmdFbInfo, "Prints framebuffer information");
}

// Color Functions
uint32_t rgbToHex(int r, int g, int b) {
    return ((r & 0xFF) << 16) + ((g & 0xFF) << 8) + (b & 0XFF);
}
uint32_t rgbaToHex(int r, int g, int b, int a) {
    return ((r & 0xFF) << 24) + ((g & 0xFF) << 16) + ((b & 0xFF) << 8) + (a & 0xFF);
}
void setBgColor(uint32_t bgColor) {ssfn_dst.bg = bgColor;}
void setFgColor(uint32_t fgColor) {ssfn_dst.fg = fgColor;}

// Draw A Pixel
void drawPixel(uint64_t xPos, uint64_t yPos, uint32_t color) {
	// Ensure The Pixel Isn't Out of Bounds
	if(xPos >= fb->width || yPos >= fb->height) return;

	// Framebuffer Address Pointer and Location to Draw The Pixel at
	uint8_t *fbPtr = (uint8_t*)fb->address;
    uint32_t *address = (uint32_t*)(fbPtr + (yPos * fb->pitch) + (xPos * sizeof(uint32_t)));
    *address = color;
}

// Reset Screen
void resetScreen(void) {
	// Reset Cursor Position
	ssfn_dst.x = ssfn_dst.y = 0;
    drawRect(0, 0, fb->width, fb->height, ssfn_dst.bg);
}

// Draw Shapes
void drawRect(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t color) {
	// Ensure That The Rectangle is Within The Bounds of The Framebuffer
    if(x + width > fb->width) width = fb->width - x;
    if(y + height > fb->height) height = fb->height - y;

    // Framebuffer Address and Pointer to The Start of The Row
    uint8_t *fbPtr = (uint8_t*)fb->address;
    uint32_t *rowPtr = (uint32_t*)(fbPtr + y * fb->pitch) + x;
    uint64_t pitchInPixels = fb->pitch / sizeof(uint32_t);

    // Draw The Rectangle
    for(uint64_t row = 0; row < height; row++) {
        // Fill The Entire Row With The Specified Color
		uint32_t *pixelPtr = rowPtr;
        for(uint64_t col = 0; col < width; col++) *pixelPtr++ = color;

        // Move The Pointer to The Start of The Next Row
        rowPtr += pitchInPixels;
    }
}
void drawCircle(int centerX, int centerY, int radius, uint32_t color) {
	int radiusSquared = radius * radius;
	for(int y = centerY - radius; y <= (centerY + radius); y++) {
        for(int x = centerX - radius; x <= (centerX + radius); x++) {
            if((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY) <= radiusSquared) {
                drawPixel(x, y, color);
            }
        }
    }
}

// Text Functions
void _putchar(char character) {
    // Clear Previous Cursor and Mark it as Dirty
    drawRect(lastCursorX, lastCursorY, 1, fontHeight, ssfn_dst.bg);

    // Handle Special Characters
    switch(character) {
        case '\n':
            ssfn_dst.x = 0;
            ssfn_dst.y += fontHeight;
            if((ssfn_dst.y + fontHeight) > (int)fb->height) scrollTerm();
            break;

        case '\t':
            for(uint8_t i = 0; i < 4; i++) _putchar(' ');
            break;

        case '\b':
            if(ssfn_dst.x >= fontWidth) ssfn_dst.x -= fontWidth;
            else if(ssfn_dst.y >= fontHeight) {
                ssfn_dst.y -= fontHeight;
                ssfn_dst.x = fb->width - fontWidth;
            }

            // Delete The Previous Character and Mark That Area as Dirty
			drawRect(ssfn_dst.x, ssfn_dst.y, fontWidth, fontHeight, ssfn_dst.bg);
            break;

        default:
            if((ssfn_dst.x + fontWidth) > (int)fb->width) {
                ssfn_dst.x = 0;
                ssfn_dst.y += fontHeight;
            }
            if((ssfn_dst.y + fontHeight) > (int)fb->height) scrollTerm();

            // Render The Character and Mark it as Dirty
            ssfn_putc(character);
            break;
    }

    // Redraw Cursor at The New Position and Save The Position
	drawRect(ssfn_dst.x, ssfn_dst.y, 1, fontHeight, ssfn_dst.fg);
    lastCursorX = ssfn_dst.x;
    lastCursorY = ssfn_dst.y;
}

// Cursor
void setCursorPos(uint64_t xOffset, uint64_t yOffset) {
    // Calculate The New X and Y Position
    uint64_t newX = ssfn_dst.x + xOffset;
    uint64_t newY = ssfn_dst.y + yOffset;

    // Ensure Cursor is Withn The Framebuffer
    if(newX > fb->width - fontWidth) newX = fb->width - fontWidth;
    if(newY > fb->height - fontHeight) newY = fb->height - fontHeight;

    // Clear Previous Cursor
    drawRect(ssfn_dst.x, ssfn_dst.y, 1, fontHeight, ssfn_dst.bg);

    // Update Cursor Position
    ssfn_dst.x = newX;
    ssfn_dst.y = newY;

    // Redraw Cursor at The New Position
    drawRect(ssfn_dst.x, ssfn_dst.y, 1, fontHeight, ssfn_dst.fg);
    lastCursorX = ssfn_dst.x;
    lastCursorY = ssfn_dst.y;
}

// Getters
int getFontWidth(void) {return fontWidth;}
int getFontHeight(void) {return fontHeight;}
int getCursorPosX(void) {return ssfn_dst.x;}
int getCursorPosY(void) {return ssfn_dst.y;}
