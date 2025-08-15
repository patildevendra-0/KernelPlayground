#include <stdint.h>
#include <stddef.h>

/* ==== VGA TEXT MODE DEFINES ==== */
static volatile uint16_t* const VGA = (uint16_t*)0xB8000;
static const size_t VGA_COLS = 80;
static const size_t VGA_ROWS = 25;

static inline uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

static void vga_clear(uint8_t color) {
    for (size_t y = 0; y < VGA_ROWS; y++) {
        for (size_t x = 0; x < VGA_COLS; x++) {
            VGA[y * VGA_COLS + x] = vga_entry(' ', color);
        }
    }
}

/* ==== LOW LEVEL I/O ==== */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* ==== KEYBOARD SCAN CODE TABLE (US LAYOUT) ==== */
const char scancode_table[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', // 0-9
    '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', // 10-19
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,   // 20-29
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', // 30-39
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', // 40-49
    'm', ',', '.', '/', 0, '*', 0, ' ', 0,            // 50-59
};

/* ==== VGA PRINT CURSOR ==== */
size_t cursor_row = 2;
size_t cursor_col = 0;

static void vga_put_char(char c, uint8_t color) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
        return;
    }
    VGA[cursor_row * VGA_COLS + cursor_col] = vga_entry(c, color);
    cursor_col++;
    if (cursor_col >= VGA_COLS) {
        cursor_col = 0;
        cursor_row++;
    }
}

/* ==== GET CHAR FROM KEYBOARD ==== */
char get_char(void) {
    static uint8_t last_sc = 0;
    uint8_t sc = inb(0x60);

    if (sc == last_sc) {
        return 0; // same key → ignore
    }
    last_sc = sc;

    if (sc & 0x80) {
        return 0; // key release → ignore
    }
    return scancode_table[sc];
}


/* ==== KERNEL MAIN ==== */
void kernel_main(void) {
    vga_clear(0x0F);
    const char* welcome = "Type something (polling mode):";
    for (size_t i = 0; welcome[i]; i++) {
        vga_put_char(welcome[i], 0x0F);
    }
    cursor_row++;
    cursor_col = 0;

    while (1) {
        char c = get_char();
        if (c) {
            vga_put_char(c, 0x0F);
        }
    }
}
