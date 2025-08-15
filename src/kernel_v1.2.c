// src/kernel.c
#include <stdint.h>
#include <stddef.h>

static volatile uint16_t* const VGA = (uint16_t*)0xB8000;
static const size_t VGA_COLS = 80;
static const size_t VGA_ROWS = 25;

static inline uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

static void vga_clear(uint8_t color) {
    for (size_t y = 0; y < VGA_ROWS; y++) {
        for (size_t x = 0; x < VGA_COLS; x++) {
            VGA[y*VGA_COLS + x] = vga_entry(' ', color);
        }
    }
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}


static void set_cursor(size_t row, size_t col) {
    uint16_t pos = (row * VGA_COLS) + col;

    outb(0x3D4, 0x0F);                // Select low cursor byte
    outb(0x3D5, (uint8_t)(pos & 0xFF));

    outb(0x3D4, 0x0E);                // Select high cursor byte
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}


static void vga_puts_at(const char* s, uint8_t color, size_t row) {
    size_t x = 0;
    while (*s && x < VGA_COLS) {
        VGA[row*VGA_COLS + x] = vga_entry(*s++, color);
        x++;
    }
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0"
                      : "=a"(ret)   // output in AL register
                      : "Nd"(port) // input: DX = port number
    );
    return ret;
}


// Enable hardware cursor
static void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}


void kernel_main(void) {
    // color: 0x0F = white on black
    vga_clear(0x0F);
    enable_cursor(0, 15); // enable VGA cursor

    vga_puts_at("SHREE GANESH....", 0x0F, 12);
    vga_puts_at("15 Aug 2025 V1.1", 0x0F, 13);
    set_cursor(12, 17); // row 12, col 17

}