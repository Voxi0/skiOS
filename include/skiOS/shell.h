#ifndef SHELL_H
#define SHELL_H

// Freestanding headers
#include<stdint.h>

// skiOS
#include<skiOS/drivers/keyboard.h>
#include<skiOS/drivers/video.h>
#include<skiOS/util.h>

// Definitions
#define MAX_PROMPT_LENGTH 50
#define MAX_INPUT_SIZE 128
#define MAX_COMMANDS 50

// Shell command handler
typedef void (*cmdHandler)(int argc, char *argv[]);
typedef struct {
    const char *name;
    cmdHandler handler;
    const char *desc;
} shellCmd_t;

// Shell functions
void initShell(void);
void shellSetPrompt(const char *prompt);
void shellRegisterCmd(const char *name, cmdHandler handler, const char *desc);

#endif
