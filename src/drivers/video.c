#include<skiOS/drivers/video.h>

// SSFN
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#include<ssfn.h>

// Framebuffer
static struct limine_framebuffer *fb = NULL;

// Font
extern char _binary_fonts_unifont_sfn_start;
static uint8_t glyphWidth, glyphHeight;

// Cursor
static uint64_t lastCursorX = 0, lastCursorY = 0;

// Display the framebuffer information
static void cmdFbInfo(int, char *[]) {
	printf("Framebuffer Info:\n");
	printf("\tWidth: %d\n", fb->width);
    printf("\tHeight: %d\n", fb->height);
    printf("\tPitch: %d\n", fb->pitch);
    printf("\tBPP: %d\n", fb->bpp);
    printf("\tGlyph Width: %d\n", glyphWidth);
    printf("\tGlyph Height: %d\n", glyphHeight);
}

// Initialize the video driver
void initVideo(struct limine_framebuffer *framebuffer, const uint8_t fontWidth, const uint8_t fontHeight) {
	// Store and ensure that we have the framebuffer
	fb = framebuffer;
	assert(fb != NULL);

	// Store font dimensions
	glyphWidth = fontWidth;
	glyphHeight = fontHeight;

	// Initialize SSFN
	ssfn_src = (ssfn_font_t*)&_binary_fonts_unifont_sfn_start;
	ssfn_dst.ptr = (uint8_t*)fb->address;
	ssfn_dst.w = fb->width;
	ssfn_dst.h = fb->height;
	ssfn_dst.p = fb->pitch;
	ssfn_dst.x = ssfn_dst.y = 0;

	// Register shell commands
	shellRegisterCmd("fbInfo", cmdFbInfo, "Display information about the framebuffer");
}

// Reset the screen - Reset cursor position and clear screen with background color
void resetScreen(void) {
	ssfn_dst.x = ssfn_dst.y = 0;
	drawRect(0, 0, fb->width, fb->height, ssfn_dst.bg);
}

// Convert RGB to hex
uint32_t rgbToHex(int r, int g, int b) {
	return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

// Getters
uint8_t getFontWidth(void) {return glyphWidth;}
uint8_t getFontHeight(void) {return glyphHeight;}
size_t getCursorX(void) {return ssfn_dst.x;}
size_t getCursorY(void) {return ssfn_dst.y;}

// Setters
void setBgColor(uint32_t color) {ssfn_dst.bg = color;}
void setFgColor(uint32_t color) {ssfn_dst.fg = color;}

// Draw shapes
void drawRect(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t color) {
	// Ensure that the rectangle is inside the framebuffer
    if((x + width) > fb->width) width = fb->width - x;
    if((y + height) > fb->height) height = fb->height - y;

    // Pointer to the start of the row
    uint32_t *rowPtr = (uint32_t*)((uint8_t*)fb->address + (y * fb->pitch)) + x;
    uint32_t pitchInPixels = fb->pitch / sizeof(uint32_t);

    // Draw the rectangle
    for(uint64_t y = 0; y < height; y++) {
        // Fill the entire row with specified color
        uint32_t *pixelPtr = rowPtr;
        for(uint64_t x = 0; x < width; x++) *pixelPtr++ = color;

        // Move the pointer to the start of the next row
        rowPtr += pitchInPixels;
    }
}

// _putchar implementation which is required for Tiny Printf
void _putchar(char character) {
	// Delete the previous cursor before anything - Draw over the previous cursor using the current background color
	drawRect(lastCursorX, lastCursorY, 1, glyphHeight, ssfn_dst.bg);

	// Handle special characters
	switch (character) {
		// Newline
		case '\n':
			// Reset cursor to the beginning of a line
			ssfn_dst.x = 0;

			// Place cursor on the next line - Clear the screen and reset cursor position if it goes outside the screen vertically
			if((ssfn_dst.y + glyphHeight) > (int)fb->height) {
				resetScreen();
				ssfn_dst.y = 0;
			} else ssfn_dst.y += glyphHeight;
			break;

		// Tab
		case '\t':
			// Draw spaces for the tab character - _putchar will handle checking if the cursor goes out of bounds
			for(uint8_t i = 0; i < TAB_SPACES; i++) _putchar(' ');
			break;
		
		// Backspace
		case '\b':
			// Move the cursor back one character
            if(ssfn_dst.x >= glyphWidth) ssfn_dst.x -= glyphWidth;
            else if(ssfn_dst.y >= glyphHeight) {
                ssfn_dst.y -= glyphHeight;
                ssfn_dst.x = fb->width - glyphWidth;
            }

            // Delete the previous character - Draw a rectangle over it using the background color
            drawRect(ssfn_dst.x, ssfn_dst.y, glyphWidth, glyphHeight, ssfn_dst.bg);
			break;

		// Normal character
		default:
			// Check if the cursor will end up going out of bounds or not
			if((ssfn_dst.x + glyphWidth) > (int)fb->width) _putchar('\n');
			if((ssfn_dst.y + glyphHeight) > (int)fb->height) resetScreen();

			// Draw the character
			ssfn_putc(character);
			break;
	}

	// Draw the new cursor and save it's position
	drawRect(ssfn_dst.x, ssfn_dst.y, 1, glyphHeight, ssfn_dst.fg);
	lastCursorX = ssfn_dst.x;
	lastCursorY = ssfn_dst.y;
}
