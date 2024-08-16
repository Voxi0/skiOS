#include<skiOS/memory/paging.h>

// Higher-Half Direct Map Request
__attribute__((used, section(".requests")))
static volatile struct limine_hhdm_request hhdmRequest = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0,
};

// Initialized and Enable Paging - Currently Just Use The One Provided by Limine
void initPaging(void) {
    // Ensure That we Have A Valid Higher-Half Direct Map
    if(hhdmRequest.response == NULL) {
        printf("Paging Error! No Higher-Half Direct Map Available\n");
        asm("cli");
        for(;;) asm("hlt");
    }

    // Optional: Verify That Paging is Enabled
    uint64_t cr0After;
    asm volatile("mov %%cr0, %0" : "=r"(cr0After));
    if(!(cr0After & (1 << 31))) {
        printf("Paging Error! Paging Not Enabled. CR0: %lx\n", cr0After);
        asm("cli");
        for(;;) asm("hlt");
    }

    // Get The Base Address of The Higher-Half Direct Map
    // uint64_t hhdmBase = (uint64_t)hhdmRequest.response->offset;
}
