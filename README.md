# skiOS
skiOS is a simple hobby 64-Bit operating system written from scratch using C for x86 machines.

### How to build
To build skiOS, you need an x86-64 GCC cross compiler, NASM (Assmbler), Make (Build tool) and QEMU (Emulator). To install these on Arch Linux using the "yay" AUR package manager, run the following command - <br>
```yay -S x86_64-elf-gcc make nasm qemu-full``` <br><br>

Running ```make``` in the root of the project directory will build and emulate skiOS in QEMU.

### TODO
<ul>
    <li>Memory Management</li>
    <li>ACPI</li>
    <li>Networking</li>
    <li>Filesystem Driver</li>
</ul>

### Libraries
<ul>
    <li><a href="https://gitlab.com/bztsrc/scalable-font2">Scalable Screen Font 2 (SSFN) Renderer</a> - For rendering text.</li>
    <li><a href="https://github.com/mpaland/printf">Tiny Printf</a> - Tiny, fast, non-dependent and fully loaded printf implementation for embedded systems.</li>
</ul>
