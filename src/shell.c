#include<skiOS/shell.h>

// Input buffer
static char inputBuffer[MAX_INPUT_SIZE];
static uint32_t inputIndex = 0;

// Array of shell command handlers and counter
static shellCmd_t shellCommands[MAX_COMMANDS];
static uint32_t shellCmdCount = 0;

// Initial shell prompt
static char shellPrompt[MAX_PROMPT_LENGTH] = "skiOS> ";

// Shell Command Handlers
static void cmdHelp(int, char *[]) {
    for(uint32_t i = 0; i < shellCmdCount; i++) printf("%s - %s\n", shellCommands[i].name, shellCommands[i].desc);
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
    int fgRed = stringToInt(argv[1]);
    int fgGreen = stringToInt(argv[2]);
    int fgBlue = stringToInt(argv[3]);

    // Clamp RGB values to range (0 - 255)
    fgRed = clamp(fgRed, 0, 255);
    fgBlue = clamp(fgBlue, 0, 255);
    fgGreen = clamp(fgGreen, 0, 255);

    // Set foreground color
    setFgColor(rgbToHex(fgRed, fgGreen, fgBlue));
}
static void cmdSetBgColor(int argc, char *argv[]) {
    // Ensure that the user has entered all the RGB values
    if(argc != 4) {
        printf("Usage: bgcolor <red> <green> <blue>\n");
        return;
    }

    // Parse RGB values
    int bgRed = stringToInt(argv[1]);
    int bgGreen = stringToInt(argv[2]);
    int bgBlue = stringToInt(argv[3]);

    // Clamp RGB values to range (0 - 255)
    bgRed = clamp(bgRed, 0, 255);
    bgBlue = clamp(bgBlue, 0, 255);
    bgGreen = clamp(bgGreen, 0, 255);

    // Set the background color and clear the screen for the command to take effect properly
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
        char prompt[MAX_PROMPT_LENGTH] = "";
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
static void displayPrompt(void) {printf("%s", shellPrompt);}

// Parse Command And Find And Execute Command Handler
static void parseAndExecuteCmd(char *input) {
    int argc = 0;                           // Argument count
    char *argv[MAX_INPUT_SIZE];             // Arguments

    // Split input into arguments
    char *token = input;
    while(token && argc < MAX_INPUT_SIZE) {
        argv[argc++] = token;
        token = strchr(token, ' ');
        if(token) {
            *token = '\0';
            token++;
            while (*token == ' ') token++;  // Skip extra spaces
        }
    }

    // No command entered
    if(argc == 0) return;

    // Find and execute the registered shell command handler
    for(uint32_t i = 0; i < shellCmdCount; i++) {
        if(strcmp(argv[0], shellCommands[i].name) == 0) {
            shellCommands[i].handler(argc, argv);
            return;
        }
    }

    // Invalid command
    printf("Unknown command: %s\n", argv[0]);
}

// Clear the input buffer
static void clearInputBuffer(void) {
    memset(inputBuffer, '\0', MAX_INPUT_SIZE);
    inputIndex = 0;
}

// Callback function to handle keyboard input
static void shellKeyCallback(char key, bool pressed) {
    switch(key) {
        // Enter
        case '\n':
            if(pressed) {
                // Null-terminate and process the command
                printf("\n");
                inputBuffer[inputIndex] = '\0';
                parseAndExecuteCmd(inputBuffer);

                // Reset the input buffer and display the shell prompt again
                clearInputBuffer();
                displayPrompt();
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
    // Initialize the keyboard driver
    initKb();
    kbRegisterKeyCallback(&shellKeyCallback);

    // Register some shell commands
    shellRegisterCmd("help", &cmdHelp, "Prints all available commands and what they do");
    shellRegisterCmd("clear", &cmdClear, "Clear the screen");
    shellRegisterCmd("echo", &cmdEcho, "Echoes whatever input it gets");
    shellRegisterCmd("fgcolor", &cmdSetFgColor, "Set the text/foreground color (RGB)");
    shellRegisterCmd("bgcolor", &cmdSetBgColor, "Set the background color (RGB)");
    shellRegisterCmd("prompt", &cmdSetPrompt, "Set a custom shell prompt");

    // Display the initial shell prompt
    displayPrompt();
}

// Set shell prompt
void shellSetPrompt(const char *prompt) {
    // Check if the new prompt fits withn the buffer including the space
    if(strlen(prompt) + 1 >= sizeof(shellPrompt)) {
        printf("Error: Prompt is too long.\n");
        return;
    }

    // Copy the new prompt into the buffer - Also add a space at the end
    snprintf(shellPrompt, sizeof(shellPrompt), "%s ", prompt);
}

// Register a shell command
void shellRegisterCmd(const char *name, cmdHandler handler, const char *desc) {
    if(shellCmdCount != MAX_COMMANDS) {
        shellCommands[shellCmdCount].name = name;
        shellCommands[shellCmdCount].handler = handler;
        shellCommands[shellCmdCount].desc = desc;
        shellCmdCount++;
    }
}
