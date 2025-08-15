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

static void vga_puts_at(const char* s, uint8_t color, size_t row) {
    size_t x = 0;
    while (*s && x < VGA_COLS) {
        VGA[row*VGA_COLS + x] = vga_entry(*s++, color);
        x++;
    }
}

void kernel_main(void) {
    // color: 0x0F = white on black
    vga_clear(0x0F);
    vga_puts_at("SHREE GANESH....", 0x0F, 12);
    vga_puts_at("15 Aug 2025 V1.1", 0x0F, 13);
}