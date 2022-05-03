// SPDX-License-Identifier: GPL-2.0+
/*
 * Texas Instruments' OMAP serial driver
 *
 * Copyright (C) 2018 Texas Instruments Incorporated - http://www.ti.com/
 *	Lokesh Vutla <lokeshvutla@ti.com>
 */

#include <common.h>
#include <dm.h>
#include <dt-structs.h>
#include <log.h>
#include <ns16550.h>
#include <serial.h>
#include <clk.h>
#include <linux/err.h>
#include <video_font.h>
#include <lcd_console.h>

#ifndef CONFIG_SYS_NS16550_CLK
#define CONFIG_SYS_NS16550_CLK  0
#endif

#ifdef CONFIG_DEBUG_UART_OMAP

#ifndef CONFIG_SYS_NS16550_IER
#define CONFIG_SYS_NS16550_IER  0x00
#endif

#define UART_MCRVAL 0x00
#define UART_LCRVAL UART_LCR_8N1

static inline void serial_out_shift(void *addr, int shift, int value)
{
#ifdef CONFIG_SYS_NS16550_PORT_MAPPED
	outb(value, (ulong)addr);
#elif defined(CONFIG_SYS_NS16550_MEM32) && defined(CONFIG_SYS_LITTLE_ENDIAN)
	out_le32(addr, value);
#elif defined(CONFIG_SYS_NS16550_MEM32) && defined(CONFIG_SYS_BIG_ENDIAN)
	out_be32(addr, value);
#elif defined(CONFIG_SYS_NS16550_MEM32)
	writel(value, addr);
#elif defined(CONFIG_SYS_BIG_ENDIAN)
	writeb(value, addr + (1 << shift) - 1);
#else
	writeb(value, addr);
#endif
}

static inline int serial_in_shift(void *addr, int shift)
{
#ifdef CONFIG_SYS_NS16550_PORT_MAPPED
	return inb((ulong)addr);
#elif defined(CONFIG_SYS_NS16550_MEM32) && defined(CONFIG_SYS_LITTLE_ENDIAN)
	return in_le32(addr);
#elif defined(CONFIG_SYS_NS16550_MEM32) && defined(CONFIG_SYS_BIG_ENDIAN)
	return in_be32(addr);
#elif defined(CONFIG_SYS_NS16550_MEM32)
	return readl(addr);
#elif defined(CONFIG_SYS_BIG_ENDIAN)
	return readb(addr + (1 << shift) - 1);
#else
	return readb(addr);
#endif
}

#include <debug_uart.h>

static inline void _debug_uart_init(void)
{
	/*struct ns16550 *com_port = (struct ns16550 *)CONFIG_DEBUG_UART_BASE;
	int baud_divisor;

	baud_divisor = ns16550_calc_divisor(com_port, CONFIG_DEBUG_UART_CLOCK,
					    CONFIG_BAUDRATE);
	serial_dout(&com_port->ier, CONFIG_SYS_NS16550_IER);
	serial_dout(&com_port->mdr1, 0x7);
	serial_dout(&com_port->mcr, UART_MCRVAL);
	serial_dout(&com_port->fcr, UART_FCR_DEFVAL);

	serial_dout(&com_port->lcr, UART_LCR_BKSE | UART_LCRVAL);
	serial_dout(&com_port->dll, baud_divisor & 0xff);
	serial_dout(&com_port->dlm, (baud_divisor >> 8) & 0xff);
	serial_dout(&com_port->lcr, UART_LCRVAL);
	serial_dout(&com_port->mdr1, 0x0);*/
}

/*static inline void _debug_uart_putc(int ch)
{
	struct ns16550 *com_port = (struct ns16550 *)CONFIG_DEBUG_UART_BASE;

	while (!(serial_din(&com_port->lsr) & UART_LSR_THRE))
		;
	serial_dout(&com_port->thr, ch);
}*/

static struct console_t hax_cons = {.cols = 270, .rows = 150};

static inline void hax_console_newline(void)
{
	static hax_pgcnt = 0;

	hax_cons.curr_col = 0;
	hax_cons.curr_row += 1;
	if (hax_cons.curr_row > 150) {
		hax_cons.curr_row = 0;
		hax_pgcnt ++;
		hax_lcd_puts(": -- page ");
		for(int i = 0; i < hax_pgcnt; i++) {
			hax_lcd_puts(".");
		}
		hax_lcd_puts(" --\n");
	}
}

static void fill_this_bad_boy_up()
{
	int i = 0;

	/* 0 0x00 '^@' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 1 0x01 '^A' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x81; /* 10000001 */
	video_fontdata[i++] = 0xa5; /* 10100101 */
	video_fontdata[i++] = 0x81; /* 10000001 */
	video_fontdata[i++] = 0x81; /* 10000001 */
	video_fontdata[i++] = 0xbd; /* 10111101 */
	video_fontdata[i++] = 0x99; /* 10011001 */
	video_fontdata[i++] = 0x81; /* 10000001 */
	video_fontdata[i++] = 0x81; /* 10000001 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 2 0x02 '^B' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xdb; /* 11011011 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xc3; /* 11000011 */
	video_fontdata[i++] = 0xe7; /* 11100111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 3 0x03 '^C' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 4 0x04 '^D' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 5 0x05 '^E' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0xe7; /* 11100111 */
	video_fontdata[i++] = 0xe7; /* 11100111 */
	video_fontdata[i++] = 0xe7; /* 11100111 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 6 0x06 '^F' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 7 0x07 '^G' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 8 0x08 '^H' */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xe7; /* 11100111 */
	video_fontdata[i++] = 0xc3; /* 11000011 */
	video_fontdata[i++] = 0xc3; /* 11000011 */
	video_fontdata[i++] = 0xe7; /* 11100111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */

	/* 9 0x09 '^I' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x42; /* 01000010 */
	video_fontdata[i++] = 0x42; /* 01000010 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 10 0x0a '^J' */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xc3; /* 11000011 */
	video_fontdata[i++] = 0x99; /* 10011001 */
	video_fontdata[i++] = 0xbd; /* 10111101 */
	video_fontdata[i++] = 0xbd; /* 10111101 */
	video_fontdata[i++] = 0x99; /* 10011001 */
	video_fontdata[i++] = 0xc3; /* 11000011 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 1111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */

	/* 11 0x0b '^K' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x1e; /* 00011110 */
	video_fontdata[i++] = 0x0e; /* 00001110 */
	video_fontdata[i++] = 0x1a; /* 00011010 */
	video_fontdata[i++] = 0x32; /* 00110010 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 12 0x0c '^L' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 13 0x0d '^M' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x3f; /* 00111111 */
	video_fontdata[i++] = 0x33; /* 00110011 */
	video_fontdata[i++] = 0x3f; /* 00111111 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x70; /* 01110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xe0; /* 11100000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 14 0x0e '^N' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7f; /* 01111111 */
	video_fontdata[i++] = 0x63; /* 01100011 */
	video_fontdata[i++] = 0x7f; /* 01111111 */
	video_fontdata[i++] = 0x63; /* 01100011 */
	video_fontdata[i++] = 0x63; /* 01100011 */
	video_fontdata[i++] = 0x63; /* 01100011 */
	video_fontdata[i++] = 0x63; /* 01100011 */
	video_fontdata[i++] = 0x67; /* 01100111 */
	video_fontdata[i++] = 0xe7; /* 11100111 */
	video_fontdata[i++] = 0xe6; /* 11100110 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 15 0x0f '^O' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xdb; /* 11011011 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0xe7; /* 11100111 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0xdb; /* 11011011 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 16 0x10 '^P' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x80; /* 10000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xe0; /* 11100000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xe0; /* 11100000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0x80; /* 10000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 17 0x11 '^Q' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x02; /* 00000010 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x0e; /* 00001110 */
	video_fontdata[i++] = 0x1e; /* 00011110 */
	video_fontdata[i++] = 0x3e; /* 00111110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x3e; /* 00111110 */
	video_fontdata[i++] = 0x1e; /* 00011110 */
	video_fontdata[i++] = 0x0e; /* 00001110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x02; /* 00000010 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 18 0x12 '^R' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 19 0x13 '^S' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 20 0x14 '^T' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7f; /* 01111111 */
	video_fontdata[i++] = 0xdb; /* 11011011 */
	video_fontdata[i++] = 0xdb; /* 11011011 */
	video_fontdata[i++] = 0xdb; /* 11011011 */
	video_fontdata[i++] = 0x7b; /* 01111011 */
	video_fontdata[i++] = 0x1b; /* 00011011 */
	video_fontdata[i++] = 0x1b; /* 00011011 */
	video_fontdata[i++] = 0x1b; /* 00011011 */
	video_fontdata[i++] = 0x1b; /* 00011011 */
	video_fontdata[i++] = 0x1b; /* 00011011 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 21 0x15 '^U' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 22 0x16 '^V' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 23 0x17 '^W' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 24 0x18 '^X' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 25 0x19 '^Y' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 26 0x1a '^Z' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 27 0x1b '^[' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 28 0x1c '^\' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 29 0x1d '^]' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x28; /* 00101000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x28; /* 00101000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 30 0x1e '^^' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 31 0x1f '^_' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 32 0x20 ' ' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 33 0x21 '!' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 34 0x22 '"' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x24; /* 00100100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 35 0x23 '#' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 36 0x24 '$' */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc2; /* 11000010 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x86; /* 10000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 37 0x25 '%' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc2; /* 11000010 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x86; /* 10000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 38 0x26 '&' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 39 0x27 ''' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 40 0x28 '(' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 41 0x29 ')' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 42 0x2a '*' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 43 0x2b '+' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 44 0x2c ';' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 45 0x2d '-' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 46 0x2e '.' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 47 0x2f '/' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x02; /* 00000010 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0x80; /* 10000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 48 0x30 '0' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xd6; /* 11010110 */
	video_fontdata[i++] = 0xd6; /* 11010110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 49 0x31 '1' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 50 0x32 '2' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 51 0x33 '3' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 52 0x34 '4' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x1c; /* 00011100 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x1e; /* 00011110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 53 0x35 '5' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xfc; /* 11111100 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 54 0x36 '6' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xfc; /* 11111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 55 0x37 '7' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 56 0x38 '8' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 57 0x39 '9' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 58 0x3a ':' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 59 0x3b ';' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 60 0x3c '<' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 61 0x3d '=' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 62 0x3e '>' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 63 0x3f '?' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 64 0x40 '@' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xde; /* 11011110 */
	video_fontdata[i++] = 0xde; /* 11011110 */
	video_fontdata[i++] = 0xde; /* 11011110 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 65 0x41 'A' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 66 0x42 'B' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfc; /* 11111100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0xfc; /* 11111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 67 0x43 'C' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0xc2; /* 11000010 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc2; /* 11000010 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 68 0x44 'D' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 69 0x45 'E' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x62; /* 01100010 */
	video_fontdata[i++] = 0x68; /* 01101000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x68; /* 01101000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x62; /* 01100010 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 70 0x46 'F' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x62; /* 01100010 */
	video_fontdata[i++] = 0x68; /* 01101000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x68; /* 01101000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 71 0x47 'G' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0xc2; /* 11000010 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xde; /* 11011110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x3a; /* 00111010 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 72 0x48 'H' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 73 0x49 'I' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 74 0x4a 'J' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x1e; /* 00011110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 75 0x4b 'K' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xe6; /* 11100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0xe6; /* 11100110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 76 0x4c 'L' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x62; /* 01100010 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 77 0x4d 'M' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xee; /* 11101110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xd6; /* 11010110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 78 0x4e 'N' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xe6; /* 11100110 */
	video_fontdata[i++] = 0xf6; /* 11110110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xde; /* 11011110 */
	video_fontdata[i++] = 0xce; /* 11001110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 79 0x4f 'O' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 80 0x50 'P' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfc; /* 11111100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 81 0x51 'Q' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xd6; /* 11010110 */
	video_fontdata[i++] = 0xde; /* 11011110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0e; /* 00001110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 82 0x52 'R' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfc; /* 11111100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0xe6; /* 11100110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 83 0x53 'S' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 84 0x54 'T' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x5a; /* 01011010 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 85 0x55 'U' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 86 0x56 'V' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 87 0x57 'W' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xd6; /* 11010110 */
	video_fontdata[i++] = 0xd6; /* 11010110 */
	video_fontdata[i++] = 0xd6; /* 11010110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xee; /* 11101110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 88 0x58 'X' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 89 0x59 'Y' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 90 0x5a 'Z' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x86; /* 10000110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xc2; /* 11000010 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 91 0x5b '[' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 92 0x5c '\' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x80; /* 10000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xe0; /* 11100000 */
	video_fontdata[i++] = 0x70; /* 01110000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x1c; /* 00011100 */
	video_fontdata[i++] = 0x0e; /* 00001110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x02; /* 00000010 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 93 0x5d ']' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 94 0x5e '^' */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 95 0x5f '_' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 96 0x60 '`' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 97 0x61 'a' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 98 0x62 'b' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xe0; /* 11100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 99 0x63 'c' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 100 0x64 'd' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x1c; /* 00011100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 101 0x65 'e' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 102 0x66 'f' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x1c; /* 00011100 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x32; /* 00110010 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 103 0x67 'g' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 104 0x68 'h' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xe0; /* 11100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0xe6; /* 11100110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 105 0x69 'i' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 106 0x6a 'j' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x0e; /* 00001110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 107 0x6b 'k' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xe0; /* 11100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0xe6; /* 11100110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 108 0x6c 'l' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 109 0x6d 'm' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xec; /* 11101100 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xd6; /* 11010110 */
	video_fontdata[i++] = 0xd6; /* 11010110 */
	video_fontdata[i++] = 0xd6; /* 11010110 */
	video_fontdata[i++] = 0xd6; /* 11010110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 110 0x6e 'n' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 111 0x6f 'o' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 112 0x70 'p' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 113 0x71 'q' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x1e; /* 00011110 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 114 0x72 'r' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 115 0x73 's' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 116 0x74 't' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0xfc; /* 11111100 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x1c; /* 00011100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 117 0x75 'u' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 118 0x76 'v' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 119 0x77 'w' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xd6; /* 11010110 */
	video_fontdata[i++] = 0xd6; /* 11010110 */
	video_fontdata[i++] = 0xd6; /* 11010110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 120 0x78 'x' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 121 0x79 'y' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 122 0x7a 'z' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 123 0x7b '{' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x0e; /* 00001110 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x70; /* 01110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x0e; /* 00001110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 124 0x7c '|' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 125 0x7d '}' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x70; /* 01110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x0e; /* 00001110 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x70; /* 01110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 126 0x7e '~' */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 127 0x7f */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 128 0x80 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0xc2; /* 11000010 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc2; /* 11000010 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x70; /* 01110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 129 0x81 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 130 0x82 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 131 0x83 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 132 0x84 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 133 0x85 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 134 0x86 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 135 0x87 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x70; /* 01110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 136 0x88 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 137 0x89 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 138 0x8a */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 139 0x8b */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 140 0x8c */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 141 0x8d */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 142 0x8e */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 143 0x8f */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 144 0x90 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x62; /* 01100010 */
	video_fontdata[i++] = 0x68; /* 01101000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x68; /* 01101000 */
	video_fontdata[i++] = 0x62; /* 01100010 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 145 0x91 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xec; /* 11101100 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0x6e; /* 01101110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 146 0x92 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x3e; /* 00111110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xce; /* 11001110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 147 0x93 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x10; /* 00010000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 148 0x94 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 149 0x95 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 150 0x96 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 151 0x97 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 152 0x98 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 153 0x99 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 154 0x9a */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 155 0x9b */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 156 0x9c */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x64; /* 01100100 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xe6; /* 11100110 */
	video_fontdata[i++] = 0xfc; /* 11111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 157 0x9d */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 158 0x9e */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0xc4; /* 11000100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xde; /* 11011110 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 159 0x9f */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x0e; /* 00001110 */
	video_fontdata[i++] = 0x1b; /* 00011011 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0x70; /* 01110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 160 0xa0 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 161 0xa1 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 162 0xa2 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 163 0xa3 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 164 0xa4 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 165 0xa5 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xe6; /* 11100110 */
	video_fontdata[i++] = 0xf6; /* 11110110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xde; /* 11011110 */
	video_fontdata[i++] = 0xce; /* 11001110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 166 0xa6 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x3e; /* 00111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 167 0xa7 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 168 0xa8 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 169 0xa9 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 170 0xaa */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 171 0xab */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xe0; /* 11100000 */
	video_fontdata[i++] = 0x62; /* 01100010 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0x86; /* 10000110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3e; /* 00111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 172 0xac */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xe0; /* 11100000 */
	video_fontdata[i++] = 0x62; /* 01100010 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0xce; /* 11001110 */
	video_fontdata[i++] = 0x9a; /* 10011010 */
	video_fontdata[i++] = 0x3f; /* 00111111 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 173 0xad */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 174 0xae */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 175 0xaf */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 176 0xb0 */
	video_fontdata[i++] = 0x11; /* 00010001 */
	video_fontdata[i++] = 0x44; /* 01000100 */
	video_fontdata[i++] = 0x11; /* 00010001 */
	video_fontdata[i++] = 0x44; /* 01000100 */
	video_fontdata[i++] = 0x11; /* 00010001 */
	video_fontdata[i++] = 0x44; /* 01000100 */
	video_fontdata[i++] = 0x11; /* 00010001 */
	video_fontdata[i++] = 0x44; /* 01000100 */
	video_fontdata[i++] = 0x11; /* 00010001 */
	video_fontdata[i++] = 0x44; /* 01000100 */
	video_fontdata[i++] = 0x11; /* 00010001 */
	video_fontdata[i++] = 0x44; /* 01000100 */
	video_fontdata[i++] = 0x11; /* 00010001 */
	video_fontdata[i++] = 0x44; /* 01000100 */
	video_fontdata[i++] = 0x11; /* 00010001 */
	video_fontdata[i++] = 0x44; /* 01000100 */

	/* 177 0xb1 */
	video_fontdata[i++] = 0x55; /* 01010101 */
	video_fontdata[i++] = 0xaa; /* 10101010 */
	video_fontdata[i++] = 0x55; /* 01010101 */
	video_fontdata[i++] = 0xaa; /* 10101010 */
	video_fontdata[i++] = 0x55; /* 01010101 */
	video_fontdata[i++] = 0xaa; /* 10101010 */
	video_fontdata[i++] = 0x55; /* 01010101 */
	video_fontdata[i++] = 0xaa; /* 10101010 */
	video_fontdata[i++] = 0x55; /* 01010101 */
	video_fontdata[i++] = 0xaa; /* 10101010 */
	video_fontdata[i++] = 0x55; /* 01010101 */
	video_fontdata[i++] = 0xaa; /* 10101010 */
	video_fontdata[i++] = 0x55; /* 01010101 */
	video_fontdata[i++] = 0xaa; /* 10101010 */
	video_fontdata[i++] = 0x55; /* 01010101 */
	video_fontdata[i++] = 0xaa; /* 10101010 */

	/* 178 0xb2 */
	video_fontdata[i++] = 0xdd; /* 11011101 */
	video_fontdata[i++] = 0x77; /* 01110111 */
	video_fontdata[i++] = 0xdd; /* 11011101 */
	video_fontdata[i++] = 0x77; /* 01110111 */
	video_fontdata[i++] = 0xdd; /* 11011101 */
	video_fontdata[i++] = 0x77; /* 01110111 */
	video_fontdata[i++] = 0xdd; /* 11011101 */
	video_fontdata[i++] = 0x77; /* 01110111 */
	video_fontdata[i++] = 0xdd; /* 11011101 */
	video_fontdata[i++] = 0x77; /* 01110111 */
	video_fontdata[i++] = 0xdd; /* 11011101 */
	video_fontdata[i++] = 0x77; /* 01110111 */
	video_fontdata[i++] = 0xdd; /* 11011101 */
	video_fontdata[i++] = 0x77; /* 01110111 */
	video_fontdata[i++] = 0xdd; /* 11011101 */
	video_fontdata[i++] = 0x77; /* 01110111 */

	/* 179 0xb3 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */

	/* 180 0xb4 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */

	/* 181 0xb5 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */

	/* 182 0xb6 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0xf6; /* 11110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */

	/* 183 0xb7 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */

	/* 184 0xb8 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */

	/* 185 0xb9 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0xf6; /* 11110110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0xf6; /* 11110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */

	/* 186 0xba */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */

	/* 187 0xbb */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0xf6; /* 11110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */

	/* 188 0xbc */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0xf6; /* 11110110 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 189 0xbd */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 190 0xbe */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 191 0xbf */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */

	/* 192 0xc0 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x1f; /* 00011111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 193 0xc1 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 194 0xc2 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */

	/* 195 0xc3 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x1f; /* 00011111 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */

	/* 196 0xc4 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 197 0xc5 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */

	/* 198 0xc6 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x1f; /* 00011111 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x1f; /* 00011111 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */

	/* 199 0xc7 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x37; /* 00110111 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */

	/* 200 0xc8 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x37; /* 00110111 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x3f; /* 00111111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 201 0xc9 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x3f; /* 00111111 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x37; /* 00110111 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */

	/* 202 0xca */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0xf7; /* 11110111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 203 0xcb */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xf7; /* 11110111 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */

	/* 204 0xcc */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x37; /* 00110111 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x37; /* 00110111 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */

	/* 205 0xcd */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 206 0xce */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0xf7; /* 11110111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xf7; /* 11110111 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */

	/* 207 0xcf */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 208 0xd0 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 209 0xd1 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */

	/* 210 0xd2 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */

	/* 211 0xd3 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x3f; /* 00111111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 212 0xd4 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x1f; /* 00011111 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x1f; /* 00011111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 213 0xd5 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x1f; /* 00011111 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x1f; /* 00011111 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */

	/* 214 0xd6 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x3f; /* 00111111 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */

	/* 215 0xd7 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */

	/* 216 0xd8 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */

	/* 217 0xd9 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xf8; /* 11111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 218 0xda */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x1f; /* 00011111 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */

	/* 219 0xdb */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */

	/* 220 0xdc */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */

	/* 221 0xdd */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */
	video_fontdata[i++] = 0xf0; /* 11110000 */

	/* 222 0xde */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0f; /* 00001111 */

	/* 223 0xdf */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0xff; /* 11111111 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 224 0xe0 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 225 0xe1 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x78; /* 01111000 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xcc; /* 11001100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 226 0xe2 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 227 0xe3 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 228 0xe4 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 229 0xe5 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0x70; /* 01110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 230 0xe6 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 231 0xe7 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 232 0xe8 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 233 0xe9 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 234 0xea */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0xee; /* 11101110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 235 0xeb */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x1e; /* 00011110 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x3e; /* 00111110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 236 0xec */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0xdb; /* 11011011 */
	video_fontdata[i++] = 0xdb; /* 11011011 */
	video_fontdata[i++] = 0xdb; /* 11011011 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 237 0xed */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x03; /* 00000011 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0xdb; /* 11011011 */
	video_fontdata[i++] = 0xdb; /* 11011011 */
	video_fontdata[i++] = 0xf3; /* 11110011 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0xc0; /* 11000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 238 0xee */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x1c; /* 00011100 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x1c; /* 00011100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 239 0xef */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7c; /* 01111100 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0xc6; /* 11000110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 240 0xf0 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0xfe; /* 11111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 241 0xf1 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 242 0xf2 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x06; /* 00000110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 243 0xf3 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x60; /* 01100000 */
	video_fontdata[i++] = 0x30; /* 00110000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 244 0xf4 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x0e; /* 00001110 */
	video_fontdata[i++] = 0x1b; /* 00011011 */
	video_fontdata[i++] = 0x1b; /* 00011011 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */

	/* 245 0xf5 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0xd8; /* 11011000 */
	video_fontdata[i++] = 0x70; /* 01110000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 246 0xf6 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 247 0xf7 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x76; /* 01110110 */
	video_fontdata[i++] = 0xdc; /* 11011100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 248 0xf8 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x38; /* 00111000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 249 0xf9 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 250 0xfa */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 251 0xfb */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x0f; /* 00001111 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0xec; /* 11101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x1c; /* 00011100 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 252 0xfc */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x6c; /* 01101100 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x36; /* 00110110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 253 0xfd */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x3c; /* 00111100 */
	video_fontdata[i++] = 0x66; /* 01100110 */
	video_fontdata[i++] = 0x0c; /* 00001100 */
	video_fontdata[i++] = 0x18; /* 00011000 */
	video_fontdata[i++] = 0x32; /* 00110010 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 254 0xfe */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x7e; /* 01111110 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */

	/* 255 0xff */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
	video_fontdata[i++] = 0x00; /* 00000000 */
}

static void hax_lcd_putc_xy0(struct console_t *pcons, ushort x, ushort y, char c)
{
	int fg_color = 0xffdaad35;
	int bg_color = 0xFF0000FF;
	int i, row;
	int *dst = (int *)(long int)(0xbef00000) +
				  y * (1024) +
				  x;

	for (row = 0; row < VIDEO_FONT_HEIGHT; row++) {
		uchar bits = video_fontdata[c * VIDEO_FONT_HEIGHT + row];
		for (i = 0; i < VIDEO_FONT_WIDTH; ++i) {
			*dst++ = (bits & 0x80) ? fg_color : bg_color;
			bits <<= 1;
		}
		dst += ((1024) - VIDEO_FONT_WIDTH);
	}
}

static inline void hax_console_back(void)
{
	if (--hax_cons.curr_col < 0) {
		hax_cons.curr_col = hax_cons.cols - 1;
		if (--hax_cons.curr_row < 0)
			hax_cons.curr_row = 0;
	}

	hax_lcd_putc_xy0(&hax_cons,
			hax_cons.curr_col * VIDEO_FONT_WIDTH,
			hax_cons.curr_row * VIDEO_FONT_HEIGHT, ' ');
}

static void _debug_uart_putc(char c)
{
	switch (c) {
	case '\r':
		hax_cons.curr_col = 0;
		return;
	case '\n':
		hax_console_newline();

		return;
	case '\t':	/* Tab (8 chars alignment) */
		hax_cons.curr_col +=  8;
		hax_cons.curr_col &= ~7;

		if (hax_cons.curr_col >= hax_cons.cols)
			hax_console_newline();

		return;
	case '\b':
		hax_console_back();

		return;
	default:
		hax_lcd_putc_xy0(&hax_cons,
				hax_cons.curr_col * VIDEO_FONT_WIDTH,
				hax_cons.curr_row * VIDEO_FONT_HEIGHT, c);
		if (++hax_cons.curr_col >= hax_cons.cols)
			hax_console_newline();
	}
}

void hax_lcd_puts(const char *s)
{
	fill_this_bad_boy_up();
	hax_lcd_putc_xy0(&hax_cons,2,2,'U');
	hax_lcd_putc_xy0(&hax_cons,10,2,'B');
	hax_lcd_putc_xy0(&hax_cons,18,2,'O');
	hax_lcd_putc_xy0(&hax_cons,26,2,'O');
	hax_lcd_putc_xy0(&hax_cons,34,2,'T');
	hax_lcd_putc_xy0(&hax_cons,42,2,' ');
	/*while (*s)
		_debug_uart_putc(*s++);*/
}


DEBUG_UART_FUNCS

#endif

#if CONFIG_IS_ENABLED(DM_SERIAL)

#if CONFIG_IS_ENABLED(OF_REAL)
static int omap_serial_of_to_plat(struct udevice *dev)
{
	struct ns16550_plat *plat = dev_get_plat(dev);
	fdt_addr_t addr;
	struct clk clk;
	int err;

	/* try Processor Local Bus device first */
	addr = dev_read_addr(dev);
	if (addr == FDT_ADDR_T_NONE)
		return -EINVAL;

	plat->base = (unsigned long)map_physmem(addr, 0, MAP_NOCACHE);

	plat->reg_offset = dev_read_u32_default(dev, "reg-offset", 0);
	plat->reg_shift = 2;

	err = clk_get_by_index(dev, 0, &clk);
	if (!err) {
		err = clk_get_rate(&clk);
		if (!IS_ERR_VALUE(err))
			plat->clock = err;
	} else if (err != -ENOENT && err != -ENODEV && err != -ENOSYS) {
		debug("omap serial failed to get clock\n");
		return err;
	}

	if (!plat->clock)
		plat->clock = dev_read_u32_default(dev, "clock-frequency",
						   CONFIG_SYS_NS16550_CLK);
	if (!plat->clock) {
		debug("omap serial clock not defined\n");
		return -EINVAL;
	}

	plat->fcr = UART_FCR_DEFVAL;

	return 0;
}

static const struct udevice_id omap_serial_ids[] = {
	{ .compatible = "ti,omap2-uart", },
	{ .compatible = "ti,omap3-uart", },
	{ .compatible = "ti,omap4-uart", },
	{ .compatible = "ti,am3352-uart", },
	{ .compatible = "ti,am4372-uart", },
	{ .compatible = "ti,dra742-uart", },
	{ .compatible = "ti,am654-uart", },
	{}
};
#endif /* OF_REAL */

#if CONFIG_IS_ENABLED(SERIAL_PRESENT)
U_BOOT_DRIVER(omap_serial) = {
	.name	= "omap_serial",
	.id	= UCLASS_SERIAL,
#if CONFIG_IS_ENABLED(OF_REAL)
	.of_match = omap_serial_ids,
	.of_to_plat = omap_serial_of_to_plat,
	.plat_auto	= sizeof(struct ns16550_plat),
#endif
	.priv_auto	= sizeof(struct ns16550),
	.probe = ns16550_serial_probe,
	.ops	= &ns16550_serial_ops,
#if !CONFIG_IS_ENABLED(OF_CONTROL)
	.flags	= DM_FLAG_PRE_RELOC,
#endif
};
#endif
#endif /* DM_SERIAL */
