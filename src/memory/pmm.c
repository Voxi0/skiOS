#include<skiOS/memory/pmm.h>

// Memory Map
__attribute__((used, section(".requests")))
static volatile struct limine_memmap_request memoryMapRequest = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

static size_t numPages;
static uint8_t memoryBitmap[BITMAP_SIZE];   // Adjust Size as Required - Currently Set to 1MB

// Initialize The Physical Memory Manager (PMM)
void initPMM(void) {
    // Ensure That we Have A Memory Map
    if(memoryMapRequest.response == NULL  || memoryMapRequest.response->entry_count == 0) {
        // Display Error Message
        printf("PMM Error! No Memory Map Available\n");

        // Disable System Interrupts and Halt System if Not
        asm("cli");
        for(;;) asm("hlt");
    }

    // Get The Total Amount of Usable System Primary Memory and Mark Reserved Memory Regions in The Bitmap
    uint64_t totalMemory = 0;
    for(size_t i = 0; i < memoryMapRequest.response->entry_count; i++) {
        if(memoryMapRequest.response->entries[i]->type == LIMINE_MEMMAP_USABLE) {
            // Get The Start and End Address of Usable Memory and The Total Memory
            uint64_t start = memoryMapRequest.response->entries[i]->base;
            uint64_t end = start + memoryMapRequest.response->entries[i]->length;
            totalMemory += (uint64_t)memoryMapRequest.response->entries[i]->length;

            // Mark Usable Memory Regions as Free
            for(uint64_t addr = start; addr < end; addr += PAGE_SIZE) markPageFree(addr);
        }
    }

    // Calculate The Number of Pages
    size_t numPages = totalMemory / PAGE_SIZE;
    size_t bitmapSize = (numPages + 7) / 8;

    // Ensure That The Bitmap Size Doesn't Exceed The Allocated Space
    if(bitmapSize > BITMAP_SIZE) {
        printf("PMM Error! Bitmap Size Exceeds Allocated Space\n");
        asm("cli");
        for(;;) asm("hlt");
    }

    // Initialize The Memory Bitmap to All Free
    memset(memoryBitmap, 0xFF, BITMAP_SIZE);
}

// Allocate/Free Memory
void *allocateFrame(void) {
    size_t bitmapSize = (numPages + 7) / 8;
    for(size_t i = 0; i < bitmapSize; i++) {
        // Check if There's A Free Page
        if(memoryBitmap[i] != 0xFF) {
            for(int j = 0; j < 8; j++) {
                // Found A Free Page
                if(!(memoryBitmap[i] & (1 << j))) {
                    uint64_t address = (i * 8 + j) * PAGE_SIZE;
                    markPageUsed(address);
                    return (void*)address;
                }
            }
        }
    }

    // No Free Frames Available
    return NULL;
}
void freeFrame(void *frame) {
    uint64_t address = (uint64_t)frame;
    markPageFree(address);
}

// Mark A Memory Page as Used/Free
void markPageUsed(uint64_t address) {
    size_t index = address / PAGE_SIZE;
    memoryBitmap[index / 8] &= ~(1 << (index % 8));
}
void markPageFree(uint64_t address) {
    size_t index = address / PAGE_SIZE;
    memoryBitmap[index / 8] |= (1 << (index % 8));
}
