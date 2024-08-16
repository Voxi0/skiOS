#include<skiOS/drivers/keyboard.h>
#include<skiOS/keymap.h>

// Keyboard State
static bool shiftOn = false, capsLock = false;

// Registered Key Callback
static KeyCallback keyCallback = NULL;

// Keyboard IRQ Handler
static void kbHandler(uint64_t irqNum) {
    uint8_t scancodeRaw = inb(KB_PORT_DATA);
    uint8_t scancode = scancodeRaw & ~KB_SCANCODE_RELEASE_MASK;
    bool pressed = !(scancodeRaw & KB_SCANCODE_RELEASE_MASK);     // Key Pressed (1) or Released (0)

    // Process Scancode
    char key = 0;
    switch(scancode) {
        // Shift Keys
        case KB_SCANCODE_LEFT_SHIFT:
        case KB_SCANCODE_RIGHT_SHIFT:
            shiftOn = pressed;
            break;

        // Caps Lock
        case KB_SCANCODE_CAPS_LOCK:
            if(pressed) capsLock = !capsLock;
            break;

        // Backspace
        case KB_SCANCODE_BACKSPACE:
            key = '\b';
            break;
        
        // Enter Key
        case KB_SCANCODE_ENTER:
            key = '\n';
            break;

        default:
            if(pressed && scancode < sizeof(asciiMapLower)) {
                // Translate Scancode to ASCII
                key = shiftOn || capsLock ? asciiMapUpper[scancode] : asciiMapLower[scancode];
            }
            break;
    }

    // Call The Registered Key Callback if There's One
    if(keyCallback != NULL && key != 0) keyCallback(key, pressed);

    // Send End of Interrupt Signal to The PICs
    picSendEndOfInterrupt(irqNum);
}

// Initialize/Deinitialize Keyboard Driver
void kbInit(void) {
    // Unmask/Enable Keyboard IRQs and Install Handler
    picUnmask(1);
    irqRegisterHandler(1, &kbHandler);

    // Clear Keyboard Data Register
    while(inb(KB_PORT_STATUS) & 0x1) inb(KB_PORT_DATA);

    // Enable Keyboard Scanning - To Get Scancodes
    outb(KB_PORT_DATA, KB_CMD_ENABLE_SCANNING);
}
void disableKb(void) {
    // Mask/Disable Keyboard IRQs and Uninstall Handler
    picMask(1);
    irqDeregisterHandler(1);
}

// Send A Command Byte to The Keyboard
void kbSendCmd(uint8_t cmdByte) {
    // Wait Until The Keyboard is Ready
    while(inb(KB_PORT_COMMAND) & 0x2) outb(KB_PORT_DATA, cmdByte);
}

// Register/Unregister Key Callback Function
void kbRegisterKeyCallback(KeyCallback callback) {keyCallback = callback;}
void kbUnregisterKeyCallback(void) {keyCallback = NULL;}
