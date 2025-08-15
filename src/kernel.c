#include <stdint.h>
#include <stddef.h>

/* ==== VGA TEXT MODE ==== */
static volatile uint16_t* const VGA = (uint16_t*)0xB8000;
static const size_t VGA_COLS = 80;
static const size_t VGA_ROWS = 25;

size_t cursor_row = 2;
size_t cursor_col = 0;

static inline uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

void vga_clear(uint8_t color) {
    for(size_t y=0;y<VGA_ROWS;y++)
        for(size_t x=0;x<VGA_COLS;x++)
            VGA[y*VGA_COLS+x] = vga_entry(' ', color);
}

void vga_put_char(char c, uint8_t color) {
    if(c=='\n'){ cursor_row++; cursor_col=0; return; }
    VGA[cursor_row*VGA_COLS+cursor_col] = vga_entry(c,color);
    cursor_col++;
    if(cursor_col>=VGA_COLS){ cursor_col=0; cursor_row++; }
}

/* ==== LOW LEVEL I/O ==== */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1,%0":"=a"(ret):"Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0,%1"::"a"(val),"Nd"(port));
}

/* ==== SCANCODE TABLE ==== */
const char scancode_table[128] = {
    0,27,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z','x',
    'c','v','b','n','m',',','.','/',0,'*',0,' ',0
};

/* ==== KEYBOARD CALLBACK ==== */
void keyboard_callback() {
    uint8_t sc = inb(0x60);
    if(!(sc & 0x80)) { // key press only
        char c = scancode_table[sc];
        if(c) vga_put_char(c,0x0F);
    }
    outb(0x20,0x20); // EOI PIC
}

/* ==== IDT STRUCTURE ==== */
struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_high;
}__attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    struct idt_entry* base;
}__attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void keyboard_handler(); // ISR stub in assembly

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void idt_install() {
    idtp.limit = (sizeof(struct idt_entry)*256)-1;
    idtp.base = (struct idt_entry*)&idt;
    __asm__ volatile("lidtl (%0)"::"r"(&idtp));
}

/* ==== PIC REMAP ==== */
void pic_remap() {
    uint8_t a1 = inb(0x21);
    uint8_t a2 = inb(0xA1);

    outb(0x20,0x11);
    outb(0xA0,0x11);
    outb(0x21,0x20);
    outb(0xA1,0x28);
    outb(0x21,0x04);
    outb(0xA1,0x02);
    outb(0x21,0x01);
    outb(0xA1,0x01);
    outb(0x21,a1);
    outb(0xA1,a2);
}

/* ==== KERNEL MAIN ==== */
void kernel_main(void){
    vga_clear(0x0F);
    const char* msg = "Interrupt-driven keyboard demo:";
    for(size_t i=0; msg[i]; i++) vga_put_char(msg[i],0x0F);
    cursor_row++; cursor_col=0;

    pic_remap();
    idt_install();
    idt_set_gate(0x21,(uint32_t)keyboard_handler,0x08,0x8E); // IRQ1 keyboard

    __asm__ volatile("sti"); // Enable interrupts

    while(1){
        __asm__ volatile("hlt"); // idle CPU, wait for interrupt
    }
}
