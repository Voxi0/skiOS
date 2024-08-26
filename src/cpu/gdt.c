#include<skiOS/cpu/gdt.h>

// Structures
typedef struct {
	uint16_t limitLow;
	uint16_t baseLow;
	uint8_t baseMid;
	uint8_t access;
	uint8_t granularity;
	uint8_t baseHigh;
} __attribute__((packed)) gdtEntry_t;
typedef struct {
	uint16_t limit;			// Pointer to the first entry in the GDT
	uint64_t base;			// Size of the GDT - 1 entry
} __attribute__((packed)) gdtr_t;

// Enums
typedef enum {
    GDT_ACCESS_PRESENT = 0x80,                  // Segment present in memory

    // Descriptor Privilege Level (DPL)
    GDT_ACCESS_RING0 = 0x00,                    // Ring 0 (Kernel)
    GDT_ACCESS_RING1 = 0x20,
    GDT_ACCESS_RING2 = 0x40,
    GDT_ACCESS_RING3 = 0x60,                    // Ring 3 (User)

    // Descriptor type bit
    GDT_ACCESS_SYSTEM_SEGMENT = 0x00,           // System segment e.g. Task State Segment (S bit clear)
    GDT_ACCESS_CODE_DATA_SEGMENT = 0x10,        // Code/Data segment descriptor (S bit set)

    // Segment executable (Code) / non-executable (Data)
    GDT_ACCESS_SEGMENT_EXECUTABLE = 0x08,
    GDT_ACCESS_SEGMENT_NON_EXECUTABLE = 0x00,

    // Conforming (For code) / Direction (For data)
    GDT_ACCESS_CODE_CONFORMING = 0x04,          // Code segment is conforming (C bit)
    GDT_ACCESS_DATA_DIRECTION_DOWNWARDS = 0x04, // Data segment grows downwards in memory

    GDT_ACCESS_READ_WRITE = 0x02,               // Segment readable (Code) / writable (Data)

    GDT_ACCESS_SEGMENT_ACCESSED = 0x01,         // Accessed bit
} GDT_ACCESS;
typedef enum {
    GDT_FLAG_GRANULARITY_1B = 0x00,     // Segment limit in 1B units
    GDT_FLAG_GRANULARITY_4K = 0x80,     // Segment limit in 4KB units (Granularity bit set)

    GDT_FLAG_16BIT = 0x00,              // 16-Bit protected mode segment (D/B bit clear)
    GDT_FLAG_32BIT = 0x40,              // 32-Bit protected mode segment (D/B bit set)
    GDT_FLAG_64BIT = 0x20,              // 64-Bit code segment (L bit set in code segment)

    GDT_FLAG_AVL_AVAILABLE = 0x10,      // Segment available for system software use
    GDT_FLAG_AVL_UNAVAILABLE = 0x00,    // Segment unavailable for system software use
} GDT_FLAG;

// Assembly code
extern void loadGDT(gdtr_t*);

// GDT and GDTR
static gdtEntry_t gdt[5];
static gdtr_t gdtr;

// Set a GDT descriptor
static void setGDTDesc(uint8_t num, uint64_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdtEntry_t *entry = &gdt[num];
	entry->baseLow = base & 0xFFFF;
	entry->baseMid = (base >> 16) & 0xFF;
	entry->baseHigh = (base >> 24) & 0xFF;
	entry->limitLow = limit & 0xFFFF;
	entry->granularity = (limit >> 16) & 0x0F;
	entry->granularity |= (granularity & 0xF0);
	entry->access = access;
}

// Initialize the GDT
void initGDT(void) {
	// Initialize the GDTR
	gdtr.base = (uint64_t)&gdt[0];
	gdtr.limit = sizeof(gdt) - 1;

	// Set GDT descriptors
	// Null descriptor
	setGDTDesc(0, 0, 0, 0, 0);

	// 64-Bit kernel code/data segment descriptors
	setGDTDesc(1, 0, 0xFFFF,
			GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_DATA_SEGMENT | GDT_ACCESS_SEGMENT_EXECUTABLE | GDT_ACCESS_READ_WRITE,
			GDT_FLAG_64BIT | GDT_FLAG_GRANULARITY_4K | GDT_FLAG_AVL_UNAVAILABLE);
	setGDTDesc(2, 0, 0xFFFF,
			GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_DATA_SEGMENT | GDT_ACCESS_SEGMENT_NON_EXECUTABLE | GDT_ACCESS_READ_WRITE,
			GDT_FLAG_64BIT | GDT_FLAG_GRANULARITY_4K | GDT_FLAG_AVL_UNAVAILABLE);

	// 64-Bit user code/data segment descriptors
	setGDTDesc(3, 0, 0xFFFF,
			GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_CODE_DATA_SEGMENT | GDT_ACCESS_SEGMENT_EXECUTABLE | GDT_ACCESS_READ_WRITE,
			GDT_FLAG_64BIT | GDT_FLAG_GRANULARITY_4K | GDT_FLAG_AVL_AVAILABLE);
	setGDTDesc(4, 0, 0xFFFF,
			GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_CODE_DATA_SEGMENT | GDT_ACCESS_SEGMENT_NON_EXECUTABLE | GDT_ACCESS_READ_WRITE,
			GDT_FLAG_64BIT | GDT_FLAG_GRANULARITY_4K | GDT_FLAG_AVL_AVAILABLE);

	// Load the GDT
	loadGDT(&gdtr);
}
