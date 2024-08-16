#ifndef GDT_H
#define GDT_H

// Freestanding Headers
#include<stdint.h>

// Definitions and Enums
#define GDT_ENTRIES 5
typedef enum {
    GDT_ACCESS_CODE_READABLE = 0x02,
    GDT_ACCESS_DATA_WRITABLE = 0x02,

    GDT_ACCESS_CODE_CONFORMING = 0x04,
    GDT_ACCESS_DATA_DIRECTION_NORMAL = 0x00,
    GDT_ACCESS_DATA_DIRECTION_DOWNWARDS = 0x04,

    GDT_ACCESS_DATA_SEGMENT = 0x10,
    GDT_ACCESS_CODE_SEGMENT = 0x18,

    GDT_ACCESS_DESCRIPTOR_TSS = 0x00,

    GDT_ACCESS_RING0 = 0x00,
    GDT_ACCESS_RING1 = 0x20,
    GDT_ACCESS_RING2 = 0x40,
    GDT_ACCESS_RING3 = 0x60,

    GDT_ACCESS_PRESENT = 0x80,
} GDT_ACCESS;
typedef enum {
    GDT_FLAG_16BIT = 0x00,
    GDT_FLAG_32BIT = 0x40,
    GDT_FLAG_64BIT = 0x20,

    GDT_FLAG_GRANULARITY_1B = 0x00,
    GDT_FLAG_GRANULARITY_4K = 0x80,
} GDT_FLAGS;

// Structures - Order of Elements in These Structs Matter. DO NOT CHANGE.
typedef struct {
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMid;
    uint8_t access;
    uint8_t granularity;
    uint8_t baseHigh;
} __attribute__((packed)) gdtEntry_t;
typedef struct {
    uint16_t limit;                     // Sizeof (GDT - 1)
    uint64_t ptr;                       // Address of GDT
} __attribute__((packed)) gdtr_t;

// GDT Functions
void initGDT(void);
void setGDTDesc(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

#endif
