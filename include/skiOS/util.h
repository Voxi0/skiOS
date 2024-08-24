#ifndef UTIL_H
#define UTIL_H

// Freestanding headers
#include<stddef.h>
#include<stdint.h>

// GCC and Clang Reserves The Right to Generate Calls to The Following 4 Functions Even if They Aren't Directly Called
// DO NOT REMOVE OR RENAME THESE FUNCTIONS OR STUFF WILL EVENTUALLY BREAK!
void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dst, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

// Get the minimum/maximum value of two values
int min(int a, int b);
int max(int a, int b);

// Clamp a value to the minimum and maximum value
int clamp(int value, int minValue, int maxValue);

// String functions
int stringToInt(const char *str);
unsigned long strlen(const char *str);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, int n);
char *strchr(const char *str, int c);
char *strcat(char *dst, const char *src);

#endif
