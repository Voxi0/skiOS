#pragma once

// Paging consists of a page directory table that can contain references to 1024 page tables, each of which can point to
// 1024 sections of physical memory called page frames. Each page frame is 4096 bytes large (4KB) 

// Freestanding headers
#include<stddef.h>

// Limine
#include<limine/limine.h>

// Paging functions
void initPaging(struct limine_paging_mode_response *pagingModeResponse);
