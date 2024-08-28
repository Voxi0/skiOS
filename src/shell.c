#include<skiOS/shell.h>

// Default shell prompt
static char shellPrompt[MAX_PROMPT_SIZE] = {"skiOS> "};

// Input buffer
static char inputBuffer[MAX_INPUT_SIZE] = {'\0'};
static uint16_t inputIndex = 0;

// Shell commands
static shellCmd_t shellCommands[MAX_COMMANDS];
static uint8_t shellCmdCount = 0;

// Shell comand handlers
static void cmdHelp(int, char *[]) {
    for(int i = 0; i < shellCmdCount; i++) printf("%s - %s\n", shellCommands[i].name, shellCommands[i].desc);
}
static void cmdClear(int, char *[]) {resetScreen();}
static void cmdEcho(int argc, char *argv[]) {
    for(int i = 1; i < argc; i++) printf("%s ", argv[i]);
    printf("\n");
}
static void cmdSetFgColor(int argc, char *argv[]) {
    // Ensure that the user entered all the RGB values
    if(argc != 4) {
        printf("Usage: fgcolor <red> <green> <blue>\n");
        return;
    }

    // Parse RGB values
    uint8_t fgRed = stringToInt(argv[1]);
    uint8_t fgGreen = stringToInt(argv[2]);
    uint8_t fgBlue = stringToInt(argv[3]);

    // Clamp RGB values to range (0-255)
    fgRed = clamp(fgRed, 0, 255);
    fgGreen = clamp(fgGreen, 0, 255);
    fgBlue = clamp(fgBlue, 0, 255);

    // Set foreground color
    setFgColor(rgbToHex(fgRed, fgGreen, fgBlue));
}
static void cmdSetBgColor(int argc, char *argv[]) {
    // Ensure that the user entered all the RGB values
    if(argc != 4) {
        printf("Usage: bgcolor <red> <green> <blue>\n");
        return;
    }

    // Parse RGB values
    uint8_t bgRed = stringToInt(argv[1]);
    uint8_t bgGreen = stringToInt(argv[2]);
    uint8_t bgBlue = stringToInt(argv[3]);

    // Clamp RGB values to range (0-255)
    bgRed = clamp(bgRed, 0, 255);
    bgGreen = clamp(bgGreen, 0, 255);
    bgBlue = clamp(bgBlue, 0, 255);

    // Set the background color and clear the screen for the effect to take place properly
    setBgColor(rgbToHex(bgRed, bgGreen, bgBlue));
    resetScreen();
}
static void cmdSetPrompt(int argc, char *argv[]) {
    // Make sure that a valid prompt was provided and stop the user from entering an empty prompt
    if(argc < 2 || strlen(argv[1]) == 0) {
        printf("Usage: prompt <shell_prompt>\n");
        return;
    }

    // Set the shell prompt
    if(argc > 2) {
        char prompt[MAX_PROMPT_SIZE] = "";
        for(int i = 1; i < argc; i++) {
            // Ensure that the new prompt doesn't exceed the max prompt length
            if(strlen(prompt) + strlen(argv[i]) + 1 >= sizeof(prompt)) {
                printf("Error: Prompt is Too Long.\n");
                return;
            }

            // Concatenate all the command arguments into the prompt
            if(i > 1) strcat(prompt, " ");
            strcat(prompt, argv[i]);
        }

        // Set the shell prompt
        shellSetPrompt(prompt);
    } else shellSetPrompt(argv[1]);
}

// Display the shell prompt
static void displayShellPrompt(void) {printf("%s", shellPrompt);}

// Clear the input buffer
static void clearInputBuffer(void) {
    memset(inputBuffer, '\0', MAX_INPUT_SIZE);
    inputIndex = 0;
}

// Parse a command before finding and executing it's registered command handler
static void parseAndExecuteCmd(char *input) {
    // Argument count and string of arguments
    int argc = 0;
    char *argv[MAX_INPUT_SIZE];

    // Split command into arguments
    char *token = input;
    while(token && argc < MAX_INPUT_SIZE) {
        argv[argc++] = token;
        token = strchr(token, ' ');
        if(token) {
            *token = '\0';
            token++;
            while(*token == ' ') token++;       // Skip extra spaces
        }
    }

    // No command entered
    if(argc == 0) return;

    // Convert the command (argv[0]) to lowercase
    for(int i = 0; argv[0][i]; i++) argv[0][i] = tolower(argv[0][i]);

    // Find and execute the registered shell command handler
    for(uint8_t i = 0; i < shellCmdCount; i++) {
        if(strcmp(argv[0], shellCommands[i].name) == 0) {
            shellCommands[i].handler(argc, argv);
            return;
        }
    }

    // Invalid/Unknown command - Entered command not registered
    printf("Unknown command: %s\n", argv[0]);
}

// Shell keyboard input handler
static void shellKeyCallback(char key, bool pressed) {
    switch(key) {
        // Enter
        case '\n':
            if(pressed) {
                // Null-terminate and process the command
                printf("\n");
                inputBuffer[inputIndex] = '\0';
                parseAndExecuteCmd(inputBuffer);

                // Clear the input buffer and redisplay the shell prompt
                clearInputBuffer();
                displayShellPrompt();
            }
            break;
        
        // Backspace
        case '\b':
            if(pressed && inputIndex > 0) {
                inputIndex--;
                printf("\b \b");
            }
            break;
        
        // Normal
        default:
            if(pressed && inputIndex < MAX_INPUT_SIZE - 1) {
                inputBuffer[inputIndex++] = key;
                printf("%c", key);
            }
            break;
    }
}

// Initialize the shell
void initShell(void) {
    // Initialize the keyboard driver and register shell's key callback function to it
    initKb();
    kbRegisterKeyCallback(shellKeyCallback);

    // Register shell commands
    shellRegisterCmd("help", cmdHelp, "Displays all the available commands and their description");
    shellRegisterCmd("clear", cmdClear, "Clear the screen");
    shellRegisterCmd("echo", cmdEcho, "Echoes input text");
    shellRegisterCmd("fgcolor", cmdSetFgColor, "Set the foreground/text color in RGB");
    shellRegisterCmd("bgcolor", cmdSetBgColor, "Set the background color in RGB");
    shellRegisterCmd("prompt", cmdSetPrompt, "Set a custom shell prompt");

    // Display initial shell prompt
    displayShellPrompt();
}

// Set the shell prompt
void shellSetPrompt(const char *prompt) {
    // Check if the new prompt fits withn the buffer including the space
    if(strlen(prompt) + 1 >= MAX_PROMPT_SIZE) {
        printf("Shell error: Prompt is too long.\n");
        return;
    }

    // Copy the new prompt into the buffer - Also add a space at the end
    snprintf(shellPrompt, MAX_PROMPT_SIZE, "%s ", prompt);
}
void shellRegisterCmd(const char *name, cmdHandler handler, const char *desc) {
    if(shellCmdCount != MAX_COMMANDS) {
        shellCommands[shellCmdCount].name = name;
        shellCommands[shellCmdCount].handler = handler;
        shellCommands[shellCmdCount].desc = desc;
        shellCmdCount++;
    }
}
