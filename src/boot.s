; src/boot.s
; Build: nasm -f elf32

SECTION .multiboot
align 4
MBALIGN   equ 1<<0
MEMINFO   equ 1<<1
FLAGS     equ MBALIGN | MEMINFO
MAGIC     equ 0x1BADB002
CHECKSUM  equ -(MAGIC + FLAGS)
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

SECTION .text
    global start
    extern kernel_main

start:
    cli
    ; set up a temporary 16 KiB stack
    mov esp, stack_top
    call kernel_main

.hang:
    hlt
    jmp .hang

SECTION .bss
align 16
stack_bottom:
    resb 16384 ; 16 KiB
stack_top:
section .note.GNU-stack noalloc noexec nowrite align=4
