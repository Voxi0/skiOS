#pragma once

// Freestanding headers
#include<stddef.h>
#include<stdint.h>
#include<stdbool.h>

// Limine
#include<limine/limine.h>

// skiOS
#include<skiOS/util.h>

// PMM functions
void initPMM(struct limine_memmap_response *memoryMap);
