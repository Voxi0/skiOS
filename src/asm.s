; NASM Directives
[BITS 64]
[extern isrHandler]

; Macros to Create ISRs
%macro isrErrStub 1
    isr%+%1:
        push rdi
        push rsi
        mov rdi, %1
        mov rsi, [rsp+16]
        call isrHandler
        pop rsi
        pop rdi
        iretq
%endmacro
%macro isrNoErrStub 1
    isr%+%1:
        push rdi
        xor rsi, rsi
        mov rdi, %1
        call isrHandler
        pop rdi
        iretq
%endmacro
%macro createExceptions 0
    %assign i 0
    %rep 32
        %if i = 8 | i = 10 | i = 11 | i = 12 | i = 13 | i = 14 | i = 17 | i = 30
            isrErrStub i
        %else
            isrNoErrStub i
        %endif
        %assign i i+1
    %endrep
%endmacro
%macro createIRQs 0
    %assign i 32
    %rep 16
        isrNoErrStub i
        %assign i i+1
    %endrep
%endmacro

; Code Section
section .text
    ; Create All The ISRs
    createExceptions
    createIRQs
    
    ; Utility Functions
    ; Function to Load The GDT
    global loadGDT
    loadGDT:
        ; Load the GDT
        lgdt [rdi]

        ; Reload Segment Registers
        mov ax, 0x10          ; Kernel data segment selector (index 2 in GDT)
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax

        ; Update Code Segment
        push 0x08             ; Kernel code segment selector (index 1 in GDT)
        lea rax, [next]
        push rax
        retfq                 ; Far return to update CS
    next:
        ret

    ; Function to Load The IDT and an ISR Stub Table Which is Used to Prevent Excessive Code Reuse
    global loadIDT, isrStubTable
    loadIDT:
        ; Load The IDT and Enable Interrupts
        lidt [rdi]
        sti
        ret
    isrStubTable:
        ; 32 ISRs
        %assign i 0
        %rep 32
            dq isr%+i
            %assign i i+1
        %endrep

        ; 16 IRQs
        %rep 16
            dq isr%+i
            %assign i i+1
        %endrep
