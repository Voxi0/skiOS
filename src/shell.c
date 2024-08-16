#include<skiOS/shell.h>

// Input Buffer
static char inputBuffer[MAX_INPUT_SIZE];
static int inputIndex = 0;

// Array of Shell Command Handlers and Counter
static shellCmd_t shellCommands[MAX_COMMANDS];
static int cmdCount = 0;

// Shell Prompt
static char shellPrompt[MAX_PROMPT_LENGTH] = "skiOS> ";

// Shell Command Handlers
static void cmdHelp(int, char *[]) {
    for(int i = 0; i < cmdCount; i++) printf("%s - %s\n", shellCommands[i].name, shellCommands[i].description);
}
static void cmdClear(int, char *[]) {resetScreen();}
static void cmdEcho(int argc, char *argv[]) {
    for(int i = 1; i < argc; i++) printf("%s ", argv[i]);
    printf("\n");
}
static void cmdSetFgColor(int argc, char *argv[]) {
    // Ensure That The User Has Entered The RGB Values
    if(argc != 4) {
        printf("Usage: fgcolor <red> <green> <blue>\n");
        return;
    }

    // Parse RGB Values
    int fgRed = stringToInt(argv[1]);
    int fgGreen = stringToInt(argv[2]);
    int fgBlue = stringToInt(argv[3]);

    // Clamp RGB Values to Range (0 - 255)
    fgRed = clamp(fgRed, 0, 255);
    fgBlue = clamp(fgBlue, 0, 255);
    fgGreen = clamp(fgGreen, 0, 255);

    // Set Foreground Color
    setFgColor(rgbToHex(fgRed, fgGreen, fgBlue));
}
static void cmdSetBgColor(int argc, char *argv[]) {
    // Ensure That The User Has Entered The RGB Values
    if(argc != 4) {
        printf("Usage: bgcolor <red> <green> <blue>\n");
        return;
    }

    // Parse RGB Values
    int bgRed = stringToInt(argv[1]);
    int bgGreen = stringToInt(argv[2]);
    int bgBlue = stringToInt(argv[3]);

    // Clamp RGB Values to Range (0 - 255)
    bgRed = clamp(bgRed, 0, 255);
    bgBlue = clamp(bgBlue, 0, 255);
    bgGreen = clamp(bgGreen, 0, 255);

    // Set Background Color and Clear The Screen For The Command to Take Effect Properly
    setBgColor(rgbToHex(bgRed, bgGreen, bgBlue));
    resetScreen();
}
static void cmdSetPrompt(int argc, char *argv[]) {
    // Make Sure A Valid Prompt Was Provided And Stop The User From Entering an Empty Prompt
    if(argc < 2 || strlen(argv[1]) == 0) {
        printf("Usage: prompt <shell_prompt>\n");
        return;
    }

    // Set Shell Prompt
    if(argc > 2) {
        // Concatenate All Command Arguments Into Prompt
        char prompt[MAX_PROMPT_LENGTH] = "";
        for(int i = 1; i < argc; i++) {
            // Ensure That The New Prompt Doesn't Exceed The Max
            if(strlen(prompt) + strlen(argv[i]) + 1 >= sizeof(prompt)) {
                printf("Error: Prompt is Too Long.\n");
                return;
            }

            // Concatenate All The Command Arguments Into Prompt
            if(i > 1) strcat(prompt, " ");
            strcat(prompt, argv[i]);
        }

        // Set Shell Prompt
        setShellPrompt(prompt);
    } else setShellPrompt(argv[1]);
}

// Set/Display Shell Prompt
void setShellPrompt(char *prompt) {
    // Check if The New Prompt Fits Within The Buffer Including The Space
    if(strlen(prompt) + 1 >= sizeof(shellPrompt)) {
        printf("Error: Prompt is too long.\n");
        return;
    }

    // Copy The New Prompt Into The Buffer - Also Add A Space at The End
    snprintf(shellPrompt, sizeof(shellPrompt), "%s ", prompt);
}
static void displayPrompt(void) {printf("%s", shellPrompt);}

// Clear Input Buffer
static void clearInputBuffer(void) {
    memset(inputBuffer, '\0', MAX_INPUT_SIZE);
    inputIndex = 0;
}

// Parse Command And Find And Execute Command Handler
static void parseAndExecuteCmd(char *input) {
    int argc = 0;
    char *argv[10];

    // Split Input Into Arguments
    char *token = input;
    while(token && argc < 10) {
        argv[argc++] = token;
        token = strchr(token, ' ');
        if(token) {
            *token = '\0';
            token++;
            while (*token == ' ') token++;              // Skip Extra Spaces
        }
    }

    // No Command Entered
    if(argc == 0) return;

    // Find and Execute Command Handler
    for(int i = 0; i < cmdCount; i++) {
        if(strcmp(argv[0], shellCommands[i].name) == 0) {
            shellCommands[i].handler(argc, argv);
            return;
        }
    }

    // Invalid Command
    printf("Unknown Command: %s\n", argv[0]);
}

// Callback Function to Handle Key Presses
static void shellKeyCallback(char key, bool pressed) {
    switch(key) {
        // Enter Key
        case '\n':
            if(pressed) {
                // Null-Terminate and Process The Command
                printf("\n");
                inputBuffer[inputIndex] = '\0';
                parseAndExecuteCmd(inputBuffer);

                // Reset Input Buffer Index and Display Prompt Again
                clearInputBuffer();
                displayPrompt();
            }
            break;
        
        // Backspace Key
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

// Initialize The Shell
void shellInit(void) {
    // Initialize The Keyboard Driver and Register The Shell's Key Callback
    kbInit();
    kbRegisterKeyCallback(&shellKeyCallback);

    // Register All Shell Commands
    shellRegisterCmd("help", &cmdHelp, "Prints all available commands and what they do");
    shellRegisterCmd("clear", &cmdClear, "Clear the screen");
    shellRegisterCmd("echo", &cmdEcho, "Echoes whatever input it gets");
    shellRegisterCmd("fgcolor", &cmdSetFgColor, "Set the text/foreground color (RGB)");
    shellRegisterCmd("bgcolor", &cmdSetBgColor, "Set the background color (RGB)");
    shellRegisterCmd("prompt", &cmdSetPrompt, "Set a custom shell prompt");

    // Display Initial Shell Prompt
    displayPrompt();
}

// Register/Unregister Shell Commands
void shellRegisterCmd(const char *name, cmdHandler handler, const char *desc) {
    if(cmdCount != MAX_COMMANDS) {
        shellCommands[cmdCount].name = name;
        shellCommands[cmdCount].handler = handler;
        shellCommands[cmdCount].description = desc;
        cmdCount++;
    }
}
