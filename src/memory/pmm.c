#include<skiOS/memory/pmm.h>

// Initialize the Physical Memory Manager (PMM)
void initPMM(struct limine_memmap_response *memoryMap) {
    // Ensure that we have a memory map
    assert(memoryMap != NULL || memoryMap->entry_count != 0);
}
