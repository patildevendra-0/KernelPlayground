global keyboard_handler
extern keyboard_callback

keyboard_handler:
    pusha
    call keyboard_callback
    popa
    mov al,0x20
    out 0x20, al  ; Send EOI to PIC
    iret           ; Return from interrupt
