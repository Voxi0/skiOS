#include<skiOS/drivers/keyboard.h>
#include<skiOS/keymap.h>

// Enums
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
    // Scancode definitions
    KB_SCANCODE_RELEASE_MASK = 0x80,

    // Special keys scancodes
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

// Keyboard state
static bool shiftOn = false, capsLock = false;

// Key callback
static KeyCallback keyCallback = NULL;

// Keyboard IRQ handler
static void kbHandler(uint8_t irqNum) {
    // Get keyboard data from data port
    uint8_t scancodeRaw = inb(KB_PORT_DATA);
    uint8_t scancode = scancodeRaw & ~KB_SCANCODE_RELEASE_MASK;     // What key is pressed
    bool pressed = !(scancodeRaw & KB_SCANCODE_RELEASE_MASK);       // Key pressed (1) or released (0)

    // Process scancode
    char key = 0;
    switch(scancode) {
        // Shift keys
        case KB_SCANCODE_LEFT_SHIFT:
        case KB_SCANCODE_RIGHT_SHIFT:
            shiftOn = pressed;
            break;
        
        // Caps lock
        case KB_SCANCODE_CAPS_LOCK:
            if(pressed) capsLock = !capsLock;
            break;
        
        // Backspace
        case KB_SCANCODE_BACKSPACE:
            key = '\b';
            break;
        
        // Enter
        case KB_SCANCODE_ENTER:
            key = '\n';
            break;
        
        // Normal - Translate scancode to ASCII
        default:
            if(pressed && scancode < sizeof(asciiMapLower)) {
                // Translate scancode to ASCII
                key = shiftOn || capsLock ? asciiMapUpper[scancode] : asciiMapLower[scancode];
            }
            break;
    }

    // Call the key callback function if it's registered
    if(keyCallback != NULL && key != 0) keyCallback(key, pressed);

    // Send End of Interrupt (EOI) signal to the PICs
    picSendEOI(irqNum);
}

// Enable/Disable the keyboard
void initKb(void) {
    // Register a keyboard handler and unmask (Enable) keyboard interrupts
    irqRegisterHandler(1, &kbHandler);
    picUnmask(1);

    // Clear keyboard data register
    while(inb(KB_PORT_STATUS) & 0x1) inb(KB_PORT_DATA);

    // Enable keyboard scanning - To get scancodes
    outb(KB_PORT_DATA, KB_CMD_ENABLE_SCANNING);
}
void disableKb(void) {
    // Deregister the keyboard handler and mask (Disable) keyboard interrupts
    irqDeregisterHandler(1);
    picMask(1);
}

// Register/Deregister key callback handler
void kbRegisterKeyCallback(KeyCallback handler) {keyCallback = handler;}
void kbDeregisterKeyCallback(void) {keyCallback = NULL;}
