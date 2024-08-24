#include<skiOS/drivers/video.h>

// SSFN
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#include<ssfn.h>

// Framebuffer
__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request fbRequest = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
};
static volatile struct limine_framebuffer *fb = NULL;

// Font
extern char _binary_fonts_unifont_sfn_start;
static uint8_t fontWidth, fontHeight;

// Terminal and cursor
static int termWidth, termHeight, lastCursorX, lastCursorY;

// Shell command handlers
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

// Initialize the video driver
void initVideo(uint8_t glyphWidth, uint8_t glyphHeight) {
    // Ensure that we have a framebuffer
    if(fbRequest.response == NULL || fbRequest.response->framebuffer_count < 1) {
        // Disable system interrupts and halt system
        asm("cli");
        for(;;) asm("hlt");
    }

    // Get the first framebuffer
    fb = fbRequest.response->framebuffers[0];

    // Store terminal and font dimensions
    termWidth = fb->width / glyphWidth;
    termHeight = fb->height / glyphHeight;
    fontWidth = glyphWidth;
    fontHeight = glyphHeight;

    // Initialize SSFN
    ssfn_src = (ssfn_font_t*)&_binary_fonts_unifont_sfn_start;
    ssfn_dst.ptr = (uint8_t*)fb->address;
    ssfn_dst.w = fb->width;
    ssfn_dst.h = fb->height;
    ssfn_dst.p = fb->pitch;
    ssfn_dst.x = ssfn_dst.y = 0;

    // Register shell commands
    shellRegisterCmd("fbInfo", &cmdFbInfo, "Get framebuffer information");
}
void resetScreen(void) {
    // Reset cursor position and clear screen with currently set background color
    ssfn_dst.x = ssfn_dst.y = 0;
    drawRect(0, 0, fb->width, fb->height, ssfn_dst.bg);
}

// Convert RGB to hex color code
uint32_t rgbToHex(int r, int g, int b) {
    return ((r & 0xFF) << 16) + ((g & 0xFF) << 8) + (b & 0xFF);
}

// Getters
int getFontWidth(void) {return fontWidth;}
int getFontHeight(void) {return fontHeight;}
int getCursorPosX(void) {return ssfn_dst.x;}
int getCursorPosY(void) {return ssfn_dst.y;}

// Setters
void setFgColor(uint32_t color) {ssfn_dst.fg = color;}
void setBgColor(uint32_t color) {ssfn_dst.bg = color;}

// Draw shapes
void drawRect(uint64_t x, uint64_t y, int width, int height, uint32_t color) {
    // Ensure that the rectangle is inside the framebuffer
    if((x + width) > fb->width) width = fb->width - x;
    if((y + height) > fb->height) height = fb->height - y;

    // Pointer to the start of the row
    uint32_t *rowPtr = (uint32_t*)(fb->address + y * fb->pitch) + x;
    uint32_t pitchInPixels = fb->pitch / sizeof(uint32_t);

    // Draw the rectangle
    for(int y = 0; y < height; y++) {
        // Fill the entire row with specified color
        uint32_t *pixelPtr = rowPtr;
        for(int x = 0; x < width; x++) *pixelPtr++ = color;

        // Move the pointer to the start of the next row
        rowPtr += pitchInPixels;
    }
}

// Tiny printf requires this function to handle characters passed to printf
void _putchar(char character) {
    // Delete the previous character before anything else - Draw a rectangle over it using the background color
    drawRect(lastCursorX, lastCursorY, 1, fontHeight, ssfn_dst.bg);

    // Handle special characters
    switch(character) {
        // Newline
        case '\n':
            // Check if the cursor will end up going out of bounds or not
            ssfn_dst.x = 0;
            ssfn_dst.y += fontHeight;
            if((ssfn_dst.y + fontHeight) > (int)fb->height) resetScreen();
            break;
        
        // Tab
        case '\t':
            // Draw tab spaces - Putchar will handle checking if the cursor is out of bounds or not
            for(uint8_t i = 0; i < TAB_SPACES; i++) _putchar(' ');
            break;

        // Backspace
        case '\b':
            // Move the cursor back one character
            if(ssfn_dst.x >= fontWidth) ssfn_dst.x -= fontWidth;
            else if(ssfn_dst.y >= fontHeight) {
                ssfn_dst.y -= fontHeight;
                ssfn_dst.x = fb->width - fontWidth;
            }

            // Delete the previous character - Draw a rectangle over it using the background color
            drawRect(ssfn_dst.x, ssfn_dst.y, fontWidth, fontHeight, ssfn_dst.bg);
            break;

        // Default character - Just draw it
        default:
            // Check if the cursor will end up going out of bounds or not
            if((ssfn_dst.x + fontWidth) > (int)fb->width) _putchar('\n');
            if((ssfn_dst.y + fontHeight) > (int)fb->height) resetScreen();

            // Draw the character
            ssfn_putc(character);
            break;
    }

    // Redraw the cursor at the new position and save the position
    drawRect(ssfn_dst.x, ssfn_dst.y, 1, fontHeight, ssfn_dst.fg);
    lastCursorX = ssfn_dst.x;
    lastCursorY = ssfn_dst.y;
}
