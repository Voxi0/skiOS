#ifndef SHELL_H
#define SHELL_H

// Freestanding Headers
#include<stddef.h>
#include<stdint.h>
#include<stdbool.h>

// skiOS
#include<skiOS/drivers/video.h>
#include<skiOS/drivers/keyboard.h>
#include<skiOS/util.h>

// Definitions
#define MAX_INPUT_SIZE 128
#define MAX_COMMANDS 40
#define MAX_PROMPT_LENGTH 50

// Shell Command Handler
typedef void (*cmdHandler)(int argc, char *argv[]);
typedef struct {
    const char *name;
    cmdHandler handler;
    const char *description;
} shellCmd_t;

// Shell Functions
void shellInit(void);
void setShellPrompt(char *prompt);
void shellRegisterCmd(const char *name, cmdHandler handler, const char *desc);

#endif
