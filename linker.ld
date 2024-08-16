/* Output Format and Architecture */
OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

/* OS Entry Point */
ENTRY(kmain)

/* Program Headers so The Bootloader Gives us The Right MMU Permissions */
/* Also Allows us to Exert More Control Over The Linking Process */
PHDRS
{
    headers PT_PHDR PHDRS;
    text    PT_LOAD FILEHDR PHDRS;
    rodata  PT_LOAD;
    data    PT_LOAD;
    dynamic PT_DYNAMIC;
}

SECTIONS
{
    /* Place The Kernel in The Topmost 2GB of The Address Space For Optimizations and Because The Limine Spec Mandates it */
    /* Any Address in This Region Will do, But Often 0xffffffff80000000 is Chosen Because It's The Beginning of That Region */
    /* Leave Space For The ELF Headers by Adding "SIZEOF_HEADERS" to The Base Load Address */
    . = 0xffffffff80000000 + SIZEOF_HEADERS;

    .text : {
        *(.text .text.*)
    } :text

    /* Move to The next Memory Page For ".rodata" */
    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    /* Move to the next memory page for .data */
    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .data : {
        *(.data .data.*)

        /* Place the sections that contain the Limine requests as part of the .data */
        /* output section. */
        KEEP(*(.requests_start_marker))
        KEEP(*(.requests))
        KEEP(*(.requests_end_marker))
    } :data

    /* Dynamic section for relocations, both in its own PHDR and inside data PHDR. */
    .dynamic : {
        *(.dynamic)
    } :data :dynamic

    /* NOTE: .bss needs to be the last thing mapped to :data, otherwise lots of */
    /* unnecessary zeros will be written to the binary. */
    /* If you need, for example, .init_array and .fini_array, those should be placed */
    /* above this. */
    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :data

    /* Discard .note.* and .eh_frame* since they may cause issues on some hosts. */
    /* Also discard the program interpreter section since we do not need one. This is */
    /* more or less equivalent to the --no-dynamic-linker linker flag, except that it */
    /* works with ld.gold. */
    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
        *(.interp)
    }
}
