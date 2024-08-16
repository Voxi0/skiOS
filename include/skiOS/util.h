#ifndef UTIL_H
#define UTIL_H

// Freestanding Headers
#include<stddef.h>
#include<stdint.h>

// GCC and Clang Reserves The Right to Generate Calls to The Following 4 Functions Even if They Aren't Directly Called
// Must be Implemented as The C Specs Mandates
// DO NOT REMOVE OR RENAME THESE FUNCTIONS OR STUFF WILL EVENTUALLY BREAK!
// The CAN be Moved to A Different '.c' File
void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dst, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

// Extra
int min(int a, int b);
int max(int a, int b);
int clamp(int value, int minValue, int maxValue);

int stringToInt(const char *str);
unsigned long strlen(const char *str);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, int n);
char *strchr(const char *str, int c);
char *strcat(char *dest, const char *src);

#endif
