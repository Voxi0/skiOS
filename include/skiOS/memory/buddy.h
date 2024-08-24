#ifndef BUDDY_H
#define BUDDY_H

// Freestanding headers
#include<stddef.h>
#include<stdint.h>

// skiOS
#include<skiOS/memory/pmm.h>

// Types
typedef struct buddyBlock {
    size_t size;                // Size of the block
    struct buddy_block *next;   // Next block in the free list
} buddyBlock_t;
typedef struct buddyAllocator {
    size_t totalSize;           // Total size managed by the allocator
    buddyBlock_t **freeList;    // Array of pointers to free lists for each block size
    void *baseAddr;             // Base address of the memory managed by this allocator
} buddyAllocator_t;

// Buddy allocator functions
void buddyInit(buddyAllocator_t *allocator, size_t totalSize, void *memory);

#endif
