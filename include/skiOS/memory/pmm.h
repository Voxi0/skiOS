#ifndef PMM_H
#define PMM_H

// Freestanding Headers
#include<stdint.h>

// Limine
#include<limine/limine.h>

// skiOS
#include<skiOS/util.h>
#include<skiOS/drivers/video.h>

// Definitions
#define PAGE_SIZE 4096          // 4Kb
#define BITMAP_SIZE (4 << 20)   // 4Mb - Upto 128GB of RAM Assuming That Pages Are 4Kb in Size

// PMM Functions
void initPMM(void);
void markPageUsed(uint64_t address);
void markPageFree(uint64_t address);
void *allocateFrame(void);
void freeFrame(void *frame);

#endif
