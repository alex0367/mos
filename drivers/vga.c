/* vim: tabstop=4 shiftwidth=4 noexpandtab
 * This file is part of ToaruOS and is released under the terms
 * of the NCSA / University of Illinois License - see LICENSE.md
 * Copyright (C) 2014 Kevin Lange
 *
 * Bochs VBE / QEMU vga=std Graphics Driver
 */

#include <drivers/vga.h>
#include <boot/multiboot.h>
#include <drivers/pci.h>
#include <mm/mm.h>


_STARTDATA unsigned resolution_x;
_STARTDATA unsigned resolution_y;
_STARTDATA unsigned _vga_width;
_STARTDATA unsigned _vga_height;

/* Binary Literals */
#define b(x) ((unsigned char)b_(0 ## x ## uL))
#define b_(x) ((x & 1) | (x >> 2 & 2) | (x >> 4 & 4) | (x >> 6 & 8) | (x >> 8 & 16) | (x >> 10 & 32) | (x >> 12 & 64) | (x >> 14 & 128))

static unsigned char number_font[][12] = {
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111110),
		b(11000011),
		b(10000001), /* 4 */
		b(10100101),
		b(10000001),
		b(10111101),
		b(10011001), /* 8 */
		b(11000011),
		b(01111110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111110),
		b(11111111),
		b(11111111), /* 4 */
		b(11011011),
		b(11111111),
		b(11000011),
		b(11100111), /* 8 */
		b(11111111),
		b(01111110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(01000100),
		b(11101110), /* 4 */
		b(11111110),
		b(11111110),
		b(11111110),
		b(01111100), /* 8 */
		b(00111000),
		b(00010000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00010000),
		b(00111000),
		b(01111100), /* 4 */
		b(11111110),
		b(11111110),
		b(01111100),
		b(00111000), /* 8 */
		b(00010000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00011000),
		b(00111100),
		b(00111100), /* 4 */
		b(11111111),
		b(11100111),
		b(11100111),
		b(00011000), /* 8 */
		b(00011000),
		b(01111110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00011000),
		b(00111100),
		b(01111110), /* 4 */
		b(11111111),
		b(11111111),
		b(01111110),
		b(00011000), /* 8 */
		b(00011000),
		b(01111110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(00111100),
		b(01111110),
		b(01111110),
		b(00111100), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(11111111),
		b(11111111),
		b(11111111),
		b(11111111), /* 4 */
		b(11000011),
		b(10000001),
		b(10000001),
		b(11000011), /* 8 */
		b(11111111),
		b(11111111),
		b(11111111),
		b(11111111) /* 01 */
	},
	{	b(00000000),
		b(00000000),
		b(00111100),
		b(01111110), /* 4 */
		b(01100110),
		b(01000010),
		b(01000010),
		b(01100110), /* 8 */
		b(01111110),
		b(00111100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(11111111),
		b(11111111),
		b(11000011),
		b(10000001), /* 4 */
		b(10011001),
		b(10111101),
		b(10111101),
		b(10011001), /* 8 */
		b(10000001),
		b(11000011),
		b(11111111),
		b(11111111) /* 01 */
	},
	{	b(00000000),
		b(00111110),
		b(00001110),
		b(00111010), /* 4 */
		b(01110010),
		b(11111000),
		b(11001100),
		b(11001100), /* 8 */
		b(11001100),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00111100),
		b(01100110),
		b(01100110), /* 4 */
		b(01100110),
		b(00111100),
		b(00011000),
		b(01111110), /* 8 */
		b(00011000),
		b(00011000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00011111),
		b(00011001),
		b(00011001), /* 4 */
		b(00011111),
		b(00011000),
		b(00011000),
		b(01111000), /* 8 */
		b(11111000),
		b(01110000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111111),
		b(01100011),
		b(01111111), /* 4 */
		b(01100011),
		b(01100011),
		b(01100011),
		b(01100111), /* 8 */
		b(11100111),
		b(11100110),
		b(11000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00011000),
		b(11011011), /* 4 */
		b(01111110),
		b(11100111),
		b(11100111),
		b(01111110), /* 8 */
		b(11011011),
		b(00011000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(10000000),
		b(11000000),
		b(11100000), /* 4 */
		b(11111000),
		b(11111110),
		b(11111000),
		b(11100000), /* 8 */
		b(11000000),
		b(10000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000010),
		b(00000110),
		b(00001110), /* 4 */
		b(00111110),
		b(11111110),
		b(00111110),
		b(00001110), /* 8 */
		b(00000110),
		b(00000010),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00011000),
		b(00111100),
		b(01111110), /* 4 */
		b(00011000),
		b(00011000),
		b(00011000),
		b(01111110), /* 8 */
		b(00111100),
		b(00011000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01100110),
		b(01100110),
		b(01100110), /* 4 */
		b(01100110),
		b(01100110),
		b(00000000),
		b(00000000), /* 8 */
		b(01100110),
		b(01100110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111111),
		b(11011011),
		b(11011011), /* 4 */
		b(11011011),
		b(01111011),
		b(00011011),
		b(00011011), /* 8 */
		b(00011011),
		b(00011011),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111110),
		b(01100011),
		b(00110000), /* 4 */
		b(00111100),
		b(01100110),
		b(01100110),
		b(00111100), /* 8 */
		b(00001100),
		b(11000110),
		b(01111110),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(11111110), /* 8 */
		b(11111110),
		b(11111110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00011000),
		b(00111100),
		b(01111110), /* 4 */
		b(00011000),
		b(00011000),
		b(00011000),
		b(01111110), /* 8 */
		b(00111100),
		b(00011000),
		b(01111110),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00011000),
		b(00111100),
		b(01111110), /* 4 */
		b(00011000),
		b(00011000),
		b(00011000),
		b(00011000), /* 8 */
		b(00011000),
		b(00011000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00011000),
		b(00011000),
		b(00011000), /* 4 */
		b(00011000),
		b(00011000),
		b(00011000),
		b(01111110), /* 8 */
		b(00111100),
		b(00011000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00011000), /* 4 */
		b(00001100),
		b(11111110),
		b(00001100),
		b(00011000), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00110000), /* 4 */
		b(01100000),
		b(11111110),
		b(01100000),
		b(00110000), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(11000000),
		b(11000000),
		b(11111110),
		b(00000000), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00100100), /* 4 */
		b(01100110),
		b(11111111),
		b(01100110),
		b(00100100), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00010000),
		b(00010000), /* 4 */
		b(00111000),
		b(00111000),
		b(01111100),
		b(01111100), /* 8 */
		b(11111110),
		b(11111110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(11111110),
		b(11111110), /* 4 */
		b(01111100),
		b(01111100),
		b(00111000),
		b(00111000), /* 8 */
		b(00010000),
		b(00010000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00110000),
		b(01111000),
		b(01111000), /* 4 */
		b(00110000),
		b(00110000),
		b(00000000),
		b(00110000), /* 8 */
		b(00110000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01100110),
		b(01100110),
		b(00100100), /* 4 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01101100),
		b(01101100),
		b(11111110), /* 4 */
		b(01101100),
		b(01101100),
		b(01101100),
		b(11111110), /* 8 */
		b(01101100),
		b(01101100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00110000),
		b(00110000),
		b(01111100),
		b(11000000), /* 4 */
		b(11000000),
		b(01111000),
		b(00001100),
		b(00001100), /* 8 */
		b(11111000),
		b(00110000),
		b(00110000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(11000100),
		b(11001100), /* 4 */
		b(00011000),
		b(00110000),
		b(01100000),
		b(11001100), /* 8 */
		b(10001100),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01110000),
		b(11011000),
		b(11011000), /* 4 */
		b(01110000),
		b(11111010),
		b(11011110),
		b(11001100), /* 8 */
		b(11011100),
		b(01110110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00110000),
		b(00110000),
		b(00110000), /* 4 */
		b(01100000),
		b(00000000),
		b(00000000),
		b(00000000), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00001100),
		b(00011000),
		b(00110000), /* 4 */
		b(01100000),
		b(01100000),
		b(01100000),
		b(00110000), /* 8 */
		b(00011000),
		b(00001100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01100000),
		b(00110000),
		b(00011000), /* 4 */
		b(00001100),
		b(00001100),
		b(00001100),
		b(00011000), /* 8 */
		b(00110000),
		b(01100000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(01100110), /* 4 */
		b(00111100),
		b(11111111),
		b(00111100),
		b(01100110), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /** 4 */
		b(00011000),
		b(00011000),
		b(01111110),
		b(00011000), /* 8 */
		b(00011000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 8 */
		b(00111000),
		b(00111000),
		b(01100000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(00000000),
		b(00000000),
		b(11111110),
		b(00000000), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 8 */
		b(00111000),
		b(00111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000010),
		b(00000110), /* 4 */
		b(00001100),
		b(00011000),
		b(00110000),
		b(01100000), /* 8 */
		b(11000000),
		b(10000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111100),
		b(11000110),
		b(11001110), /* 4 */
		b(11011110),
		b(11010110),
		b(11110110),
		b(11100110), /* 8 */
		b(11000110),
		b(01111100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00010000),
		b(00110000),
		b(11110000), /* 4 */
		b(00110000),
		b(00110000),
		b(00110000),
		b(00110000), /* 8 */
		b(00110000),
		b(11111100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111000),
		b(11001100),
		b(11001100), /* 4 */
		b(00001100),
		b(00011000),
		b(00110000),
		b(01100000), /* 8 */
		b(11001100),
		b(11111100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111000),
		b(11001100),
		b(00001100), /* 4 */
		b(00001100),
		b(00111000),
		b(00001100),
		b(00001100), /* 8 */
		b(11001100),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00001100),
		b(00011100),
		b(00111100), /* 4 */
		b(01101100),
		b(11001100),
		b(11111110),
		b(00001100), /* 8 */
		b(00001100),
		b(00011110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11111100),
		b(11000000),
		b(11000000), /* 4 */
		b(11000000),
		b(11111000),
		b(00001100),
		b(00001100), /* 8 */
		b(11001100),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00111000),
		b(01100000),
		b(11000000), /* 4 */
		b(11000000),
		b(11111000),
		b(11001100),
		b(11001100), /* 8 */
		b(11001100),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11111110),
		b(11000110),
		b(11000110), /* 4 */
		b(00000110),
		b(00001100),
		b(00011000),
		b(00110000), /* 8 */
		b(00110000),
		b(00110000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111000),
		b(11001100),
		b(11001100), /* 4 */
		b(11001100),
		b(01111000),
		b(11001100),
		b(11001100), /* 8 */
		b(11001100),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111000),
		b(11001100),
		b(11001100), /* 4 */
		b(11001100),
		b(01111100),
		b(00011000),
		b(00011000), /* 8 */
		b(00110000),
		b(01110000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00111000), /* 4 */
		b(00111000),
		b(00000000),
		b(00000000),
		b(00111000), /* 8 */
		b(00111000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00111000), /* 4 */
		b(00111000),
		b(00000000),
		b(00000000),
		b(00111000), /* 8 */
		b(00111000),
		b(00011000),
		b(00110000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00001100),
		b(00011000),
		b(00110000), /* 4 */
		b(01100000),
		b(11000000),
		b(01100000),
		b(00110000), /* 8 */
		b(00011000),
		b(00001100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(01111110),
		b(00000000),
		b(01111110),
		b(00000000), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01100000),
		b(00110000),
		b(00011000), /* 4 */
		b(00001100),
		b(00000110),
		b(00001100),
		b(00011000), /* 8 */
		b(00110000),
		b(01100000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111000),
		b(11001100),
		b(00001100), /* 4 */
		b(00011000),
		b(00110000),
		b(00110000),
		b(00000000), /* 8 */
		b(00110000),
		b(00110000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111100),
		b(11000110),
		b(11000110), /* 4 */
		b(11011110),
		b(11010110),
		b(11011110),
		b(11000000), /* 8 */
		b(11000000),
		b(01111100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00110000),
		b(01111000),
		b(11001100), /* 4 */
		b(11001100),
		b(11001100),
		b(11111100),
		b(11001100), /* 8 */
		b(11001100),
		b(11001100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11111100),
		b(01100110),
		b(01100110), /* 4 */
		b(01100110),
		b(01111100),
		b(01100110),
		b(01100110), /* 8 */
		b(01100110),
		b(11111100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00111100),
		b(01100110),
		b(11000110), /* 4 */
		b(11000000),
		b(11000000),
		b(11000000),
		b(11000110), /* 8 */
		b(01100110),
		b(00111100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11111000),
		b(01101100),
		b(01100110), /* 4 */
		b(01100110),
		b(01100110),
		b(01100110),
		b(01100110), /* 8 */
		b(01101100),
		b(11111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11111110),
		b(01100010),
		b(01100000), /* 4 */
		b(01100100),
		b(01111100),
		b(01100100),
		b(01100000), /* 8 */
		b(01100010),
		b(11111110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11111110),
		b(01100110),
		b(01100010), /* 4 */
		b(01100100),
		b(01111100),
		b(01100100),
		b(01100000), /* 8 */
		b(01100000),
		b(11110000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00111100),
		b(01100110),
		b(11000110), /* 4 */
		b(11000000),
		b(11000000),
		b(11001110),
		b(11000110), /* 8 */
		b(01100110),
		b(00111110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11001100),
		b(11001100),
		b(11001100), /* 4 */
		b(11001100),
		b(11111100),
		b(11001100),
		b(11001100), /* 8 */
		b(11001100),
		b(11001100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111000),
		b(00110000),
		b(00110000), /* 4 */
		b(00110000),
		b(00110000),
		b(00110000),
		b(00110000), /* 8 */
		b(00110000),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00011110),
		b(00001100),
		b(00001100), /* 4 */
		b(00001100),
		b(00001100),
		b(11001100),
		b(11001100), /* 8 */
		b(11001100),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11100110),
		b(01100110),
		b(01101100), /* 4 */
		b(01101100),
		b(01111000),
		b(01101100),
		b(01101100), /* 8 */
		b(01100110),
		b(11100110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11110000),
		b(01100000),
		b(01100000), /* 4 */
		b(01100000),
		b(01100000),
		b(01100010),
		b(01100110), /* 8 */
		b(01100110),
		b(11111110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11000110),
		b(11101110),
		b(11111110), /* 4 */
		b(11111110),
		b(11010110),
		b(11000110),
		b(11000110), /* 8 */
		b(11000110),
		b(11000110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11000110),
		b(11000110),
		b(11100110), /* 4 */
		b(11110110),
		b(11111110),
		b(11011110),
		b(11001110), /* 8 */
		b(11000110),
		b(11000110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00111000),
		b(01101100),
		b(11000110), /* 4 */
		b(11000110),
		b(11000110),
		b(11000110),
		b(11000110), /* 8 */
		b(01101100),
		b(00111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11111100),
		b(01100110),
		b(01100110), /* 4 */
		b(01100110),
		b(01111100),
		b(01100000),
		b(01100000), /* 8 */
		b(01100000),
		b(11110000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00111000),
		b(01101100),
		b(11000110), /* 4 */
		b(11000110),
		b(11000110),
		b(11001110),
		b(11011110), /* 8 */
		b(01111100),
		b(00001100),
		b(00011110),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11111100),
		b(01100110),
		b(01100110), /* 4 */
		b(01100110),
		b(01111100),
		b(01101100),
		b(01100110), /* 8 */
		b(01100110),
		b(11100110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111000),
		b(11001100),
		b(11001100), /* 4 */
		b(11000000),
		b(01110000),
		b(00011000),
		b(11001100), /* 8 */
		b(11001100),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11111100),
		b(10110100),
		b(00110000), /* 4 */
		b(00110000),
		b(00110000),
		b(00110000),
		b(00110000), /* 8 */
		b(00110000),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11001100),
		b(11001100),
		b(11001100), /* 4 */
		b(11001100),
		b(11001100),
		b(11001100),
		b(11001100), /* 8 */
		b(11001100),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11001100),
		b(11001100),
		b(11001100), /* 4 */
		b(11001100),
		b(11001100),
		b(11001100),
		b(11001100), /* 8 */
		b(01111000),
		b(00110000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11000110),
		b(11000110),
		b(11000110), /* 4 */
		b(11000110),
		b(11010110),
		b(11010110),
		b(01101100), /* 8 */
		b(01101100),
		b(01101100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11001100),
		b(11001100),
		b(11001100), /* 4 */
		b(01111000),
		b(00110000),
		b(01111000),
		b(11001100), /* 8 */
		b(11001100),
		b(11001100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11001100),
		b(11001100),
		b(11001100), /* 4 */
		b(11001100),
		b(01111000),
		b(00110000),
		b(00110000), /* 8 */
		b(00110000),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11111110),
		b(11001110),
		b(10011000), /* 4 */
		b(00011000),
		b(00110000),
		b(01100000),
		b(01100010), /* 8 */
		b(11000110),
		b(11111110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00111100),
		b(00110000),
		b(00110000), /* 4 */
		b(00110000),
		b(00110000),
		b(00110000),
		b(00110000), /* 8 */
		b(00110000),
		b(00111100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(10000000),
		b(11000000),
		b(01100000), /* 4 */
		b(00110000),
		b(00011000),
		b(00001100),
		b(00000110), /* 8 */
		b(00000010),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00111100),
		b(00001100),
		b(00001100), /* 4 */
		b(00001100),
		b(00001100),
		b(00001100),
		b(00001100), /* 8 */
		b(00001100),
		b(00111100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00010000),
		b(00111000),
		b(01101100),
		b(11000110), /* 4 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 8 */
		b(00000000),
		b(00000000),
		b(11111111),
		b(00000000) /* 12 */
	},
	{	b(00110000),
		b(00110000),
		b(00011000),
		b(00000000), /* 4 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(01111000),
		b(00001100),
		b(01111100),
		b(11001100), /* 8 */
		b(11001100),
		b(01110110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11100000),
		b(01100000),
		b(01100000), /* 4 */
		b(01111100),
		b(01100110),
		b(01100110),
		b(01100110), /* 8 */
		b(01100110),
		b(11011100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(01111000),
		b(11001100),
		b(11000000),
		b(11000000), /* 8 */
		b(11001100),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00011100),
		b(00001100),
		b(00001100), /* 4 */
		b(01111100),
		b(11001100),
		b(11001100),
		b(11001100), /* 8 */
		b(11001100),
		b(01110110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(01111000),
		b(11001100),
		b(11111100),
		b(11000000), /* 8 */
		b(11001100),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00111000),
		b(01101100),
		b(01100000), /* 4 */
		b(01100000),
		b(11111000),
		b(01100000),
		b(01100000), /* 8 */
		b(01100000),
		b(11110000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(01110110),
		b(11001100),
		b(11001100),
		b(11001100), /* 8 */
		b(01111100),
		b(00001100),
		b(11001100),
		b(01111000) /* 12 */
	},
	{	b(00000000),
		b(11100000),
		b(01100000),
		b(01100000), /* 4 */
		b(01101100),
		b(01110110),
		b(01100110),
		b(01100110), /* 8 */
		b(01100110),
		b(11100110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00011000),
		b(00011000),
		b(00000000), /* 4 */
		b(01111000),
		b(00011000),
		b(00011000),
		b(00011000), /* 8 */
		b(00011000),
		b(01111110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00001100),
		b(00001100),
		b(00000000), /* 4 */
		b(00011100),
		b(00001100),
		b(00001100),
		b(00001100), /* 8 */
		b(00001100),
		b(11001100),
		b(11001100),
		b(01111000) /* 12 */
	},
	{	b(00000000),
		b(11100000),
		b(01100000),
		b(01100000), /* 4 */
		b(01100110),
		b(01101100),
		b(01111000),
		b(01101100), /* 8 */
		b(01100110),
		b(11100110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01111000),
		b(00011000),
		b(00011000), /* 4 */
		b(00011000),
		b(00011000),
		b(00011000),
		b(00011000), /* 8 */
		b(00011000),
		b(01111110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(11111100),
		b(11010110),
		b(11010110),
		b(11010110), /* 8 */
		b(11010110),
		b(11000110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(11111000),
		b(11001100),
		b(11001100),
		b(11001100), /* 8 */
		b(11001100),
		b(11001100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(01111000),
		b(11001100),
		b(11001100),
		b(11001100), /* 8 */
		b(11001100),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(11011100),
		b(01100110),
		b(01100110),
		b(01100110), /* 8 */
		b(01100110),
		b(01111100),
		b(01100000),
		b(11110000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(01110110),
		b(11001100),
		b(11001100),
		b(11001100), /* 8 */
		b(11001100),
		b(01111100),
		b(00001100),
		b(00011110) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(11101100),
		b(01101110),
		b(01110110),
		b(01100000), /* 8 */
		b(01100000),
		b(11110000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(01111000),
		b(11001100),
		b(01100000),
		b(00011000), /* 8 */
		b(11001100),
		b(01111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00100000),
		b(01100000), /* 4 */
		b(11111100),
		b(01100000),
		b(01100000),
		b(01100000), /* 8 */
		b(01101100),
		b(00111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(11001100),
		b(11001100),
		b(11001100),
		b(11001100), /* 8 */
		b(11001100),
		b(01110110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(11001100),
		b(11001100),
		b(11001100),
		b(11001100), /* 8 */
		b(01111000),
		b(00110000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(11000110),
		b(11000110),
		b(11010110),
		b(11010110), /* 8 */
		b(01101100),
		b(01101100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(11000110),
		b(01101100),
		b(00111000),
		b(00111000), /* 8 */
		b(01101100),
		b(11000110),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(01100110),
		b(01100110),
		b(01100110),
		b(01100110), /* 8 */
		b(00111100),
		b(00001100),
		b(00011000),
		b(11110000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(11111100),
		b(10001100),
		b(00011000),
		b(01100000), /* 8 */
		b(11000100),
		b(11111100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00011100),
		b(00110000),
		b(00110000), /* 4 */
		b(01100000),
		b(11000000),
		b(01100000),
		b(00110000), /* 8 */
		b(00110000),
		b(00011100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00011000),
		b(00011000),
		b(00011000), /* 4 */
		b(00011000),
		b(00000000),
		b(00011000),
		b(00011000), /* 8 */
		b(00011000),
		b(00011000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11100000),
		b(00110000),
		b(00110000), /* 4 */
		b(00011000),
		b(00001100),
		b(00011000),
		b(00110000), /* 8 */
		b(00110000),
		b(11100000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01110011),
		b(11011010),
		b(11001110), /* 4 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00000000),
		b(00000000),
		b(00010000), /* 4 */
		b(00111000),
		b(01101100),
		b(11000110),
		b(11000110), /* 8 */
		b(11111110),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01000100),
		b(01101100),
		b(00111000), /* 4 */
		b(00110000),
		b(01100000),
		b(11000000),
		b(11000000), /* 8 */
		b(01100000),
		b(00111000),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(00110000),
		b(00110000),
		b(11111110), /* 4 */
		b(00110000),
		b(00110000),
		b(01111010),
		b(10110110), /* 8 */
		b(01111100),
		b(00110010),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(11111110),
		b(00001100),
		b(00011000), /* 4 */
		b(00110000),
		b(00011000),
		b(00001100),
		b(01110110), /* 8 */
		b(11000110),
		b(01111100),
		b(00000000),
		b(00000000) /* 12 */
	},
	{	b(00000000),
		b(01100110),
		b(01100110),
		b(01100110), /* 4 */
		b(01100110),
		b(00000000),
		b(00000000),
		b(00111100), /* 8 */
		b(01100110),
		b(11000011),
		b(00000000),
		b(00000000) /* 12 */
	},

};


static unsigned char cursor_font[][12] = {
    {	b(00000000),
		b(01111110),
		b(01111110),
		b(01111110), /* 4 */
		b(01111110),
		b(01111110),
		b(01111110),
		b(01111110), /* 8 */
		b(01111110),
		b(01111110),
		b(01111110),
		b(00000000) /* 12 */
	},
    {	b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 4 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000), /* 8 */
		b(00000000),
		b(00000000),
		b(00000000),
		b(00000000) /* 12 */
	},
};

_STARTDATA unsigned int * fb_buffer = 0xE0000;

_START static void BgaWriteRegister(unsigned short IndexValue, unsigned short DataValue)
{
    write_word(VBE_DISPI_IOPORT_INDEX, IndexValue);
    write_word(VBE_DISPI_IOPORT_DATA, DataValue);
}
 
_START static unsigned short BgaReadRegister(unsigned short IndexValue)
{
    write_word(VBE_DISPI_IOPORT_INDEX, IndexValue);
    return read_word(VBE_DISPI_IOPORT_DATA);
}
 
_START static int BgaIsAvailable(void)
{
    unsigned short i = BgaReadRegister(VBE_DISPI_INDEX_ID);
    int old_version =  (BgaReadRegister(VBE_DISPI_INDEX_ID) != VBE_DISPI_ID5);
    int version;

    if (old_version) {
        BgaWriteRegister(VBE_DISPI_INDEX_ID, VBE_DISPI_ID5);
        version = BgaReadRegister(VBE_DISPI_INDEX_ID);

        return (version == VBE_DISPI_ID5);
    }

    return 1;
}
 
_START static void BgaSetVideoMode(unsigned int Width, unsigned int Height, unsigned int BitDepth, int UseLinearFrameBuffer, int ClearVideoMemory)
{
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    BgaWriteRegister(VBE_DISPI_INDEX_XRES, Width);
    BgaWriteRegister(VBE_DISPI_INDEX_YRES, Height);
    BgaWriteRegister(VBE_DISPI_INDEX_BPP, BitDepth);
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED |
        (UseLinearFrameBuffer ? VBE_DISPI_LFB_ENABLED : 0) |
        (ClearVideoMemory ? 0 : VBE_DISPI_NOCLEARMEM));
}
 
_START static void BgaSetBank(unsigned short BankNumber)
{
    BgaWriteRegister(VBE_DISPI_INDEX_BANK, BankNumber);
}


_START static void bochs_scan_pci(uint32_t device, uint16_t v, uint16_t d, void * extra) {
	if (v == 0x1234 && d == 0x1111) {
		unsigned t = pci_read_field(device, PCI_BAR0, 4);
		if (t > 0) {
			*((uint8_t **)extra) = (uint8_t *)(t & 0xFFFFFFF0);
		}
	}
}

#define char_height 12
#define char_width  8

unsigned char ** _number_font;
unsigned _fb_buffer;
unsigned _fb_buffer_phy;
unsigned _resolution_x;
unsigned _resolution_y;
unsigned _window_char_width;
unsigned _window_char_height;
unsigned _fb_font_width;
unsigned _fb_font_height;
char _fb_text[VGA_RESOLUTION_X*VGA_RESOLUTION_Y] = {0};

static void fb_set_point(int x, int y, unsigned value) {
	unsigned * disp = (unsigned *)_fb_buffer;
	unsigned * cell = &disp[y * _resolution_x + x];
	*cell = value;
}

void fb_write_char(int x, int y, int val, unsigned color) {
    unsigned char i, j, *c;

    x = x * char_width;
    y = y * char_height;

	if (val > 128) {
        if (val == 129) {
            c = cursor_font[0];
        } else if (val == 130) {
            c = cursor_font[1];
        }else{
            val = 4; 
            c = number_font[val];
        }
	}else{
        c = number_font[val];
    }
	for (i = 0; i < char_height; ++i) {
		for (j = 0; j < char_width; ++j) {
			if (c[i] & (1 << (8-j))) {
				fb_set_point(x+j,y+i,color);
			}else{
                fb_set_point(x+j,y+i, VGA_COLOR_BLACK);
            }
		}
	}
}

void fb_write_color(int x, int y, unsigned color) {
    unsigned char i, j;
    x = x * char_width;
    y = y * char_height;

	for (i = 0; i < char_height; ++i) {
		for (j = 0; j < char_width; ++j) {
            fb_set_point(x+j,y+i,color);
		}
	}
}

_START void fb_init(multiboot_info_t* mboot_ptr)
{
    int newest_version = BgaIsAvailable();
    
    unsigned bpp = (newest_version) ? VBE_DISPI_BPP_32 : VBE_DISPI_BPP_8;
	BgaSetVideoMode(VGA_RESOLUTION_X, VGA_RESOLUTION_Y, VGA_COLOR_DEPTH, 1, 1);

	pci_scan(bochs_scan_pci, -1, &fb_buffer);

    if (fb_buffer) {
        resolution_x = VGA_RESOLUTION_X;
        resolution_y = VGA_RESOLUTION_Y;
        _vga_width = resolution_x / char_width;
        _vga_height = resolution_y / char_height;
    }else{
        resolution_x = 0;
        resolution_y = 0;
        _vga_width = 0;
        _vga_height = 0;
    }

    /*
    write_char(0, 0, 'h', 0xFFFFFFFF);
    write_char(1*char_width, 0, 'e', 0xFFFFFFFF);
    write_char(2*char_width, 0, 'l', 0xFFFFFFFF);
    write_char(3*char_width, 0, 'l', 0xFFFFFFFF);
    write_char(4*char_width, 0, 'o', 0xFFFFFFFF);
    */

}

void fb_enable()
{
    unsigned mm_size = 0;
    unsigned dma = fb_buffer;
    _number_font = number_font;
    _fb_buffer_phy = fb_buffer;
    _resolution_x = resolution_x;
    _resolution_y = resolution_y;
    _window_char_width = _vga_width;
    _window_char_height = _vga_height;
    _fb_font_width = char_width;
    _fb_font_height = char_height;

    mm_size = resolution_x * resolution_y * 4;
    if (_fb_buffer_phy) {
        for (dma = fb_buffer; dma < (_fb_buffer_phy + mm_size); dma += PAGE_SIZE) {
            mm_add_resource_map(dma); 
        }
        _fb_buffer = _fb_buffer_phy;
    }
}



