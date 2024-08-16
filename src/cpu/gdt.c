#include<skiOS/cpu/gdt.h>

// Assembly Functions
extern void loadGDT(gdtr_t *gdtr);

// GDT
static gdtEntry_t gdt[GDT_ENTRIES];
static gdtr_t gdtr;

// Initialize The GDT
void initGDT(void) {
    // GDT Descriptor
    gdtr.ptr = (uint64_t)&gdt[0];
    gdtr.limit = sizeof(gdtEntry_t) * GDT_ENTRIES - 1;

    // Null Segment and 64-Bit Kernel and User Code/Data Segment
    setGDTDesc(0, 0, 0, 0, 0); // Null segment
    setGDTDesc(1, 0, 0xFFFFF,
               GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE,
               GDT_FLAG_64BIT | GDT_FLAG_GRANULARITY_4K); // Kernel code segment
    setGDTDesc(2, 0, 0xFFFFF,
               GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITABLE,
               GDT_FLAG_64BIT | GDT_FLAG_GRANULARITY_4K); // Kernel data segment
    setGDTDesc(3, 0, 0xFFFFF,
               GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE,
               GDT_FLAG_64BIT | GDT_FLAG_GRANULARITY_4K); // User code segment
    setGDTDesc(4, 0, 0xFFFFF,
               GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITABLE,
               GDT_FLAG_64BIT | GDT_FLAG_GRANULARITY_4K); // User data segment

    // Load The GDT
    loadGDT(&gdtr);
}

// Set A GDT Descriptor
void setGDTDesc(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt[num].baseLow = base & 0xFFFF;
    gdt[num].baseMid = (base >> 16) & 0xFF;
    gdt[num].baseHigh = (base >> 24) & 0xFF;
    gdt[num].limitLow = limit & 0xFFFF;
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= (granularity & 0xF0);
    gdt[num].access = access;
}
