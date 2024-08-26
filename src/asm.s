[BITS 64]
[extern interruptHandler]

; Macros to shorten code
%macro isrErrStub 1
	isr%+%1:
		push rdi
		push rsi
		mov rdi, %1
		mov rsi, [rsp+16]
		call interruptHandler
		pop rsi
		pop rdi
		iretq
%endmacro
%macro isrNoErrStub 1
	isr%+%1:
		push rdi
		xor rsi, rsi
		mov rdi, %1
		call interruptHandler
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

; Code section
section .text
	; Create all the ISRs
	createExceptions
	createIRQs

	; Helper functions
	; Load the GDT
	global loadGDT
    loadGDT:
        ; Load the GDT
        lgdt [rdi]

        ; Reload code segment register
        push 0x08               ; Kernel code segment selector (Index 1 in the GDT)
        lea rax, [rel .next]    ; Load memory address of .next into the RAX register
        push rax                ; Push this value to the stack
        retfq                   ; Far return to update CS
    .next:
        ; Reload data segment registers
        mov ax, 0x10          ; Kernel data segment selector (Index 2 in the GDT)
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        ret

	; Load the IDT and ISR stub table to prevent excessive code reuse
	global loadIDT, isrStubTable
	loadIDT:
		lidt [rdi]
		sti
		ret
	isrStubTable:
		; 32 CPU exceptions ISRs
		%assign i 0
		%rep 32
			dq isr%+i
			%assign i i+1
		%endrep
		
		; 16 IRQs (Hardware interrupts)
		%rep 16
			dq isr%+i
			%assign i i+1
		%endrep
