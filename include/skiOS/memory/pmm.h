#ifndef PMM_H
#define PMM_H

// Freestanding headers
#include<stddef.h>
#include<stdint.h>
#include<stdbool.h>

// Limine
#include<limine/limine.h>

// skiOS
#include<skiOS/drivers/video.h>

// Definitions
#define PAGE_SIZE 4096
#define BITMAP_SIZE (4 << 20)   // 4MB - Upto 128GB of RAM assuming that pages are 4KB in size

// PMM functions
void initPMM(void);
void *allocPages(size_t numPagesToAlloc);
void freePages(void *addr, size_t numPagesToFree);
void markPageAsUsed(size_t page);
void markPageAsFree(size_t page);

#endif
