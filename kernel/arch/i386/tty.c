#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;
static uint16_t cursor_x = 0;
static uint16_t cursor_y = 0;

static inline void outb(unsigned short port, unsigned char value)
{
    __asm__ __volatile__("outb %0, %1"
        : /* no output */
        : "a"(value), "Nd"(port));
}
void set_cursor(uint16_t x, uint16_t y)
{
    unsigned short cursor_location = y * VGA_WIDTH + x;
    outb(0x3D4, 0x0E);
    outb(0x3D5, (cursor_location >> 8) & 0xFF);
    outb(0x3D4, 0x0F); // Select low byte of cursor position
    outb(0x3D5, cursor_location & 0xFF);
}

void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(14, VGA_COLOR_BLACK);
    terminal_buffer = VGA_MEMORY;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void scrollUp()
{
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = ' ' | terminal_color;
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = ' ' | terminal_color;
    }
}

void terminal_newline()
{
    if (terminal_row < terminal_column - 1) {
        terminal_row++;
        terminal_column = 0;
    } else {
        scrollUp();
        terminal_column = 0;
    }
}
void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    if (c == '\n') {
        terminal_newline();
        terminal_column--;
    } else {
        terminal_buffer[index] = vga_entry(c, color);
        set_cursor(cursor_x++, cursor_y);
    }
}

void terminal_putchar(char c)
{
    unsigned char uc = c;
    terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
    }
}

void terminal_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

void terminal_writestring(const char* data)
{
    terminal_write(data, strlen(data));
}
