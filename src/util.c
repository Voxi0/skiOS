#include<skiOS/util.h>

// GCC and Clang Reserves The Right to Generate Calls to The Following 4 Functions Even if They Aren't Directly Called
// Implement these as the C specs mandates
// DO NOT REMOVE OR RENAME THESE FUNCTIONS OR STUFF WILL EVENTUALLY BREAK!
void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *ptrDst = (uint8_t*)dst;
    const uint8_t *ptrSrc = (const uint8_t*)src;
    for(size_t i = 0; i < n; i++) ptrDst[i] = ptrSrc[i];
    return dst;
}
void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t*)s;
    for(size_t i = 0; i < n; i++) p[i] = (uint8_t)c;
    return s;
}
void *memmove(void *dst, const void *src, size_t n) {
    uint8_t *ptrDst = (uint8_t*)dst;
    const uint8_t *ptrSrc = (const uint8_t*)src;
    if(src > dst) {
        for(size_t i = 0; i < n; i++) ptrDst[i] = ptrSrc[i];
    } else if(src < dst) {
        for(size_t i = n; i > 0; i--) ptrDst[i-1] = ptrSrc[i-1];
    }
    return dst;
}
int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t*)s1;
    const uint8_t *p2 = (const uint8_t*)s2;
    for(size_t i = 0; i < n; i++) {
        if(p1[i] != p2[i]) return p1[i] < p2[i] ? -1 : 1;
    }
    return 0;
}

// Disable system interrupts and halt system
void halt(void) {
	__asm__("cli");
	for(;;) __asm__("hlt");
}

// Assert function - Disables system interrupts if the condition passed to it isn't met
void assert(bool expression) {if(expression != true) halt();}

// Get the minimum/maximum value of two values
int min(int a, int b) {return a < b ? a : b;}
int max(int a, int b) {return a > b ? a : b;}

// Clamp a value to the minimum and maximum value
int clamp(int value, int minValue, int maxValue) {return min(max(value, minValue), maxValue);}

// String functions
int stringToInt(const char *str) {
    int result = 0;
    while(*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}
unsigned long strlen(const char *str) {
    int length = 0;
    while(str[length] != '\0') length++;
    return length;
}
int strcmp(const char *str1, const char *str2) {
    while(*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}
int strncmp(const char *str1, const char *str2, size_t n) {
    size_t i = 0;
    while(i < n) {
        if(str1[i] != str2[i]) return (unsigned char)str1[i] - (unsigned char)str2[i];
        if(str1[i] == '\0' || str2[i] == '\0') break;
        i++;
    }
    return 0;
}
char *strcpy(char *dst, const char *src) {
    char *d = dst;
    while((*d++ = *src++) != '\0');
    return dst;
}
char *strncpy(char *dst, const char *src, int n) {
    char *d = dst;
    while(n-- > 0 && (*d++ = *src++) != '\0');
    return dst;
}
char *strchr(const char *str, int c) {
    while(*str != '\0') {
        if(*str == (char)c) return (char *)str;
        str++;
    }
    return NULL;
}
char *strcat(char *dst, const char *src) {
    char *d = dst;
    while(*d) d++;
    while((*d++ = *src++));
    return dst;
}
int tolower(int c) {return c | 32;}
