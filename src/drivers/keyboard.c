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

// Registered key callback handler
static KeyCallback keyCallback = NULL;

// Keyboard IRQ handler
static void kbHandler(uint64_t irqNum) {
    uint8_t scancodeRaw = inb(KB_PORT_DATA);
    uint8_t scancode = scancodeRaw & ~KB_SCANCODE_RELEASE_MASK;
    bool pressed = !(scancodeRaw & KB_SCANCODE_RELEASE_MASK);     // Key pressed (1) or released (0)

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
        
        // Enter key
        case KB_SCANCODE_ENTER:
            key = '\n';
            break;

        // Normal scancode
        default:
            if(pressed && scancode < sizeof(asciiMapLower)) {
                // Translate scancode to ASCII
                key = shiftOn || capsLock ? asciiMapUpper[scancode] : asciiMapLower[scancode];
            }
            break;
    }

    // Call the registered key callback function if available                                                                              
    if(keyCallback != NULL && key != 0) keyCallback(key, pressed);

    // Send End-Of-Interrupt (EOI) signal to the PICs
    picSendEndOfInterrupt(irqNum);
}

// Initialize/Deinitialize the keyboard driver
void initKb(void) {
    // Unmask (Enable) keyboard IRQ and register a handler for it
    picUnmask(1);
    irqRegisterHandler(1, &kbHandler);

    // Clear keyboard data register
    while(inb(KB_PORT_STATUS) & 0x1) inb(KB_PORT_DATA);

    // Enable keyboard scanning - To get scancodes
    outb(KB_PORT_DATA, KB_CMD_ENABLE_SCANNING);
}
void disableKb(void) {
    // Mask (Disable) keyboard IRQ and deregister it's handler
    picMask(1);
    irqDeregisterHandler(1);
}

// Send a command byte to the keyboard
void kbSendCmd(uint8_t cmdByte) {
    // Wait until the keyboard is ready
    while(inb(KB_PORT_COMMAND) & 0x2) outb(KB_PORT_DATA, cmdByte);
}

// Register/Unregister the key callback handler
void kbRegisterKeyCallback(KeyCallback callback) {keyCallback = callback;}
void kbUnregisterKeyCallback(void) {keyCallback = NULL;}
