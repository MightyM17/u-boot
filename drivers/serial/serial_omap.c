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
	struct ns16550 *com_port = (struct ns16550 *)CONFIG_DEBUG_UART_BASE;
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
	serial_dout(&com_port->mdr1, 0x0);
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

static void hax_lcd_putc_xy0(struct console_t *pcons, ushort x, ushort y, char c)
{
	int fg_color = 0xFFFFFFFF;
	int bg_color = 0x00000000;
	int i, row;
	int *dst = (int *)(long int)(0xbef00000) +
				  y * (1024 * 4) +
				  x;

	for (row = 0; row < VIDEO_FONT_HEIGHT; row++) {
		uchar bits = video_fontdata[c * VIDEO_FONT_HEIGHT + row];
		for (i = 0; i < VIDEO_FONT_WIDTH; ++i) {
			*dst++ = (bits & 0x80) ? fg_color : bg_color;
			bits <<= 1;
		}
		dst += ((1024 * 4) - VIDEO_FONT_WIDTH);
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
	while (*s)
		_debug_uart_putc(*s++);
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
