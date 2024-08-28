#include<skiOS/memory/paging.h>

// Initialize paging
void initPaging(struct limine_paging_mode_response *pagingModeResponse) {
    // Ensure that Limine has set up paging as we requested
    if(pagingModeResponse == NULL) {
        // Disable system interrupts and halt system
        __asm__("cli");
        for(;;) __asm__("hlt");
    }
}
