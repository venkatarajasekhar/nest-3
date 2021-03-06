#include "terminal.h"
#include "vga.h"
#include "lib/string.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "kernel.h"

void vga_initialize(void)
{
	vga_clearscreen();
	vga_setcolor(vga_makecolor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
	vga_buffer = (uint16_t*) VGA_ADDRESS;
}

void vga_setcursor(void)
{
	unsigned short position = (unsigned short)(vga_row*80+vga_column);
	
    outportb(BASE_VIDEO_IO_PORT, 0x0E);
    outportb(BASE_VIDEO_IO_PORT+1, position >> 8);
    
    outportb(BASE_VIDEO_IO_PORT, 0x0F);
    outportb(BASE_VIDEO_IO_PORT+1, position);
}

void vga_clearscreen(void)
{
	vga_gotoxy(0,0);
}

void vga_gotoxy(uint8_t x, uint8_t y)
{
	vga_row = y;
	vga_column = x;
}

void vga_putchar(char c)
{
	if (c == '\n')
	{
		vga_linefeed();
		return;
	}
	if (c == '\b')
	{
		vga_backspace();
		return;
	}
	vga_putentryat(c, vga_column, vga_row);
	if ( ++vga_column == VGA_WIDTH )
	{
		vga_column = 0;
		vga_newline();
	}
	vga_setcursor();
}

void vga_putentryat(char c, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	vga_buffer[index] = vga_makeentry(c, vga_color);
}

// Newline functions

void vga_linefeed(void)
{
	vga_column = 0;
	vga_newline();
	vga_setcursor();
}

void vga_newline(void)
{
	if (++vga_row == VGA_HEIGHT)
	{
		memcpy(vga_buffer, &vga_buffer[VGA_WIDTH], VGA_WIDTH * (VGA_HEIGHT-1));
		vga_row--;
	};
}

// VGA colors management functions

void vga_setcolor(uint8_t color)
{
	vga_color = color;
}

uint8_t vga_makecolor(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

uint16_t vga_makeentry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

void vga_backspace(void)
{
	if (vga_column == 0 && vga_row == 0)
	{
		return;
	}
	if (vga_column == 0)
	{
		vga_column = VGA_WIDTH - 1;
		vga_row -= 1;
	} else {
		vga_column -= 1;
	}
	vga_putentryat(' ', vga_column, vga_row);
	vga_setcursor();
}
