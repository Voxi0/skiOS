#pragma once

// Freestanding headers
#include<stdint.h>
#include<stdbool.h>

// skiOS
#include<skiOS/util.h>
#include<skiOS/drivers/keyboard.h>
#include<skiOS/drivers/video.h>

// Definitions
#define MAX_INPUT_SIZE 512
#define MAX_PROMPT_SIZE 30
#define MAX_COMMANDS 50

// Shell command handler type
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
