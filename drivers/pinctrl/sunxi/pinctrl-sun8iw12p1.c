/*
* Allwinner sun8iw12p1 SoCs pinctrl driver.
*
* Copyright(c) 2016-2020 Allwinnertech Co., Ltd.
* Author: matteo <duanmintao@allwinnertech.com>
*
* This file is licensed under the terms of the GNU General Public
* License version 2.  This program is licensed "as is" without any
* warranty of any kind, whether express or implied.
*/

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/pinctrl/pinctrl.h>

#include "pinctrl-sunxi.h"

static const struct sunxi_desc_pin sun8iw12p1_pins[] = {
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),		/* RXD3 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 0)),	/* PA_EINT0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),		/* RXD2 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 1)),	/* PA_EINT1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),		/* RXD1 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 2)),	/* PA_EINT2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),		/* RXD0 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 3)),	/* PA_EINT3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),		/* RXCK*/
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 4)),	/* PA_EINT4 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),		/* CRS_DV */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 5)),	/* PA_EINT5 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),		/* RXER */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 6)),	/* PA_EINT6 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),		/* TXD3 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 7)),	/* PA_EINT7 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),		/* TXD2 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 8)),	/* PA_EINT8 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),		/* TXD1 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 9)), /* PA_EINT9 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),		/* TXD0 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 10)), /* PA_EINT10 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 11),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),		/* TXCK */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 11)), /* PA_EINT11 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 12),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "gmac0"),		/* TXCTL */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 12)), /* PA_EINT12 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 13),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "mdc0"),		/* MDC */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 13)), /* PA_EINT13 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 14),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "mdio0"),		/* MDIO */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 14)), /* PA_EINT14 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(A, 15),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "ephy0"),		/* EPHY_25 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 15)), /* PA_EINT15 */

	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 0),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "uart2"),		/* TX */
		  SUNXI_FUNCTION(0x3, "jtag0"),		/* MS0 */
		  SUNXI_FUNCTION(0x4, "sync0"),		/* IN0 */
		  SUNXI_FUNCTION(0x7, "io_disabled"),
		  SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 0)),	/* PB_EINT0 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 1),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "uart2"),		/* RX */
		  SUNXI_FUNCTION(0x3, "jtag0"),		/* CK0 */
		  SUNXI_FUNCTION(0x4, "sync0"),		/* IN1 */
		  SUNXI_FUNCTION(0x5, "vdevice"),	/* virtual */
		  SUNXI_FUNCTION(0x7, "io_disabled"),
		  SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 1)),	/* PB_EINT1 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 2),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "uart2"),		/* RTS */
		  SUNXI_FUNCTION(0x3, "jtag0"),		/* DO0 */
		  SUNXI_FUNCTION(0x4, "sync0"),		/* IN2 */
		  SUNXI_FUNCTION(0x5, "vdevice"),	/* virtual */
		  SUNXI_FUNCTION(0x7, "io_disabled"),
		  SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 2)),	/* PB_EINT2 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 3),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "uart2"),		/* CTS */
		  SUNXI_FUNCTION(0x3, "jtag0"),		/* DI0 */
		  SUNXI_FUNCTION(0x4, "sync0"),		/* OUT */
		  SUNXI_FUNCTION(0x7, "io_disabled"),
		  SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 3)),	/* PB_EINT3 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 4),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "aif2"),		/* SYNC */
		  SUNXI_FUNCTION(0x3, "i2s0"),		/* LRCK */
		  SUNXI_FUNCTION(0x7, "io_disabled"),
		  SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 4)),	/* PB_EINT4 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 5),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "aif2"),		/* BCLK */
		  SUNXI_FUNCTION(0x3, "i2s0"),		/* BCLK */
		  SUNXI_FUNCTION(0x7, "io_disabled"),
		  SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 5)),	/* PB_EINT5 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 6),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "aif2"),		/* DOUT */
		  SUNXI_FUNCTION(0x3, "i2s0"),		/* DOUT */
		  SUNXI_FUNCTION(0x7, "io_disabled"),
		  SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 6)),	/* PB_EINT6 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 7),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "aif2"),		/* DIN */
		  SUNXI_FUNCTION(0x3, "i2s0"),		/* DIN */
		  SUNXI_FUNCTION(0x7, "io_disabled"),
		  SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 7)),	/* PB_EINT7 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 8),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x3, "i2s0"),		/* MCLK */
		  SUNXI_FUNCTION(0x4, "i2s2"),		/* MCLK */
		  SUNXI_FUNCTION(0x7, "io_disabled"),
		  SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 8)),	/* PB_EINT8 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 9),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "uart0"),		/* TX */
		  SUNXI_FUNCTION(0x7, "io_disabled"),
		  SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 9)),	/* PB_EINT9 */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 10),
		  SUNXI_FUNCTION(0x0, "gpio_in"),
		  SUNXI_FUNCTION(0x1, "gpio_out"),
		  SUNXI_FUNCTION(0x2, "uart0"),		/* RX */
		  SUNXI_FUNCTION(0x7, "io_disabled"),
		  SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 10)),	/* PB_EINT10 */

	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* WE */
		SUNXI_FUNCTION(0x4, "spi0"),		/* CLK */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* ALE */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* DS */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* CLE */
		SUNXI_FUNCTION(0x4, "spi0"),		/* MOSI */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* CE0 */
		SUNXI_FUNCTION(0x4, "spi0"),		/* MISO */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* RE */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* CLK */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* RB0 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* CMD */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ0 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D0 */
		SUNXI_FUNCTION(0x4, "spi0"),		/* HOLD */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ1 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D1 */
		SUNXI_FUNCTION(0x4, "spi0"),		/* WP */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ2 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D2 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ3 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D3 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ4 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D4 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 11),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ5 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D5 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 12),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ6 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D6 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 13),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ7 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D7 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 14),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQS */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* RST */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 15),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* CE1 */
		SUNXI_FUNCTION(0x4, "spi0"),		/* CS0 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 16),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* RB1 */
		SUNXI_FUNCTION(0x4, "spi0"),		/* CS1 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D2 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VP0 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D3 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VN0 */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D0 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D4 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VP1 */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D1 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D5 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VN1 */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D2 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D6 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VP2 */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D3 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D7 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VN2 */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D4 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D10 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VPC */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D5 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D11 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VNC */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D6 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D12 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VP3 */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D7 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D13 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VN3 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D14 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VP0 */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D8 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 11),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D15 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VN0 */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D9 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 12),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D18 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VP1 */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D10 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 13),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D19 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VN1 */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D11 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 14),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D20 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VP2 */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D12 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 15),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D21 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VN2 */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D13 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 16),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D22 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VPC */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D14 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 17),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D23 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VNC */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* D15 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 18),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* CLK */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VP3 */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* CLK */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 19),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* DE */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VN3 */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* DE*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 20),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* HSYNC */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* HSYNC */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 21),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* VSYNC */
		SUNXI_FUNCTION(0x4, "bt1120"),		/* VSYNC */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 22),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "pwm8"),		/* PWM8 */
		SUNXI_FUNCTION(0x7, "io_disabled")),

	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* PCLK */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi_mclk0"),		/* MASTERCLK0 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* HSYNC */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* VSYNC */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D0 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D1 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D2 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D3 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D4 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D5 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D6 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 11),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D7 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 12),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D8 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 13),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D9 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 14),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D10 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 15),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D11 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 16),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi_cci0"),		/* SCK */
		SUNXI_FUNCTION(0x3, "twi2"),		/* SCK */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 17),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi_cci0"),		/* SDA */
		SUNXI_FUNCTION(0x3, "twi2"),		/* SDA */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* D1 */
		SUNXI_FUNCTION(0x3, "jtag0"),		/* MS1 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 0)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* D0 */
		SUNXI_FUNCTION(0x3, "jtag0"),		/* DI1 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 1)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* CLK */
		SUNXI_FUNCTION(0x3, "uart0"),		/* TX */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 2)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* CMD */
		SUNXI_FUNCTION(0x3, "jtag0"),		/* DO1 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 3)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* D3 */
		SUNXI_FUNCTION(0x3, "uart0"),		/* RX */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 4)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* D2 */
		SUNXI_FUNCTION(0x3, "jtag0"),		/* CK1 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 5)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 6)),

#if defined(CONFIG_FPGA_V4_PLATFORM)
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 16),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc2"),		/* d0 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 17),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc2"),		/* d1 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 18),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc2"),		/* d2 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 19),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc2"),		/* d3 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 20),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc2"),		/* d4 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 21),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc2"),		/* d5 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 22),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc2"),		/* d6 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 23),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc2"),		/* d7 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 24),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc2"),		/* cmd */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 25),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc2"),		/* clk */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 27),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc2"),		/* ds */
		SUNXI_FUNCTION(0x7, "io_disabled")),
#endif

	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* CLK */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 0)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* CMD */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 1)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* D0 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 2)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* D1 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 3)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* D2 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 4)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* D3 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 5)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart1"),		/* TX */
		SUNXI_FUNCTION(0x3, "pwm0"),		/* PWM0 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 6)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart1"),		/* RX */
		SUNXI_FUNCTION(0x3, "pwm1"),		/* PWM1 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 7)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart1"),		/* RTS */
		SUNXI_FUNCTION(0x3, "pwm2"),		/* PWM2 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 8)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart1"),		/* CTS */
		SUNXI_FUNCTION(0x3, "pwm3"),		/* PWM3 */
		SUNXI_FUNCTION(0x5, "dmic0"),		/* DATA3 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 9)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "aif3"),		/* BCLK */
		SUNXI_FUNCTION(0x3, "i2s2"),		/* BCLK */
		SUNXI_FUNCTION(0x4, "uart3"),		/* TX */
		SUNXI_FUNCTION(0x5, "dmic0"),		/* CLK */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 10)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 11),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "aif3"),		/* SYNC */
		SUNXI_FUNCTION(0x3, "i2s2"),		/* LRCK */
		SUNXI_FUNCTION(0x4, "uart3"),		/* RX */
		SUNXI_FUNCTION(0x5, "dmic0"),		/* DATA0 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 11)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 12),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "aif3"),		/* DOUT */
		SUNXI_FUNCTION(0x3, "i2s2"),		/* DOUT */
		SUNXI_FUNCTION(0x4, "uart3"),		/* RTS */
		SUNXI_FUNCTION(0x5, "dmic0"),		/* DATA1 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 12)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 13),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "aif3"),		/* DIN */
		SUNXI_FUNCTION(0x3, "i2s2"),		/* DIN */
		SUNXI_FUNCTION(0x4, "uart3"),		/* CTS */
		SUNXI_FUNCTION(0x5, "dmic0"),		/* DATA2 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 13)),

	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi0"),		/* SCK */
		SUNXI_FUNCTION(0x5, "uart4"),		/* TX */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 0)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi0"),		/* SDA */
		SUNXI_FUNCTION(0x5, "uart4"),		/* RX */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 1)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi1"),		/* SCK */
		SUNXI_FUNCTION(0x3, "pwm4"),		/* PWM4 */
		SUNXI_FUNCTION(0x4, "csi_f0"),		/* F_TRIGGER */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 2)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi1"),		/* SDA */
		SUNXI_FUNCTION(0x3, "pwm5"),		/* PWM5 */
		SUNXI_FUNCTION(0x4, "csi_f0"),		/* F_SHUTTER */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 3)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x4, "dmic0"),		/* DATA0 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 4)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart3"),		/* TX */
		SUNXI_FUNCTION(0x4, "dmic0"),		/* DATA1 */
		SUNXI_FUNCTION(0x5, "bist0"),
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 5)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart3"),		/* RX */
		SUNXI_FUNCTION(0x3, "spi1"),		/* MISO */
		SUNXI_FUNCTION(0x4, "dmic0"),		/* DATA2 */
		SUNXI_FUNCTION(0x4, "bist1"),
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 6)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi2"),		/* SCK */
		SUNXI_FUNCTION(0x3, "spi1"),		/* CS0 */
		SUNXI_FUNCTION(0x4, "dmic0"),		/* DATA3 */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 7)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi2"),		/* SDA */
		SUNXI_FUNCTION(0x3, "spi1"),		/* CS1 */
		SUNXI_FUNCTION(0x4, "dmic0"),		/* CLK */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 8)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi3"),		/* SCK */
		SUNXI_FUNCTION(0x3, "pwm6"),		/* PWM6 */
		SUNXI_FUNCTION(0x4, "csi"),		/* R_TRIGGER */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 9)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi3"),		/* SDA */
		SUNXI_FUNCTION(0x3, "pwm7"),		/* PWM7 */
		SUNXI_FUNCTION(0x4, "csi"),		/* R_SHUTTER */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 10)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 11),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "hscl0"),		/* HSCL */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 11)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 12),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "hsda0"),		/* HSDA */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 12)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 13),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "hcec0"),		/* HCEC */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 13)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 14),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "spi1"),		/* MOSI */
		SUNXI_FUNCTION(0x5, "uart4"),		/* CTS */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 14)),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 15),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "spi1"),		/* CLK */
		SUNXI_FUNCTION(0x5, "uart4"),		/* RTS */
		SUNXI_FUNCTION(0x7, "io_disabled"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 15)),
	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* FIELD*/
		SUNXI_FUNCTION(0x3, "csi_mck2"),		/* MASTERCLK2*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart3"),		/* RTS*/
		SUNXI_FUNCTION(0x3, "csi_cci2"),		/* SCK*/
		SUNXI_FUNCTION(0x4, "spi2"),		/* SCK*/
		SUNXI_FUNCTION(0x5, "twi2"),		/* SCK*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart3"),		/* CTS*/
		SUNXI_FUNCTION(0x3, "csi_cci2"),		/* SDA*/
		SUNXI_FUNCTION(0x4, "spi2"),		/* MOSI*/
		SUNXI_FUNCTION(0x5, "twi2"),		/* SDA*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart3"),		/* TX*/
		SUNXI_FUNCTION(0x4, "spi2"),		/* MISO*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart3"),		/* RX*/
		SUNXI_FUNCTION(0x4, "spi2"),		/* CS0*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "csi_sm0"),		/* HS*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "csi_sm0"),		/* VS*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D12*/
		SUNXI_FUNCTION(0x3, "csi_mclk3"),		/* MASTERCLK3*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D13*/
		SUNXI_FUNCTION(0x3, "csi_cci3"),		/* SCK*/
		SUNXI_FUNCTION(0x4, "spi3"),		/* SCK*/
		SUNXI_FUNCTION(0x5, "twi3"),		/* SDA*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D14*/
		SUNXI_FUNCTION(0x3, "csi_cci3"),		/* SDA*/
		SUNXI_FUNCTION(0x4, "spi3"),		/* SDA*/
		SUNXI_FUNCTION(0x5, "twi3"),		/* SDA*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi2"),		/* D15*/
		SUNXI_FUNCTION(0x4, "spi3"),		/* MISO*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 11),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x4, "spi3"),		/* CS0*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 12),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* FIELD*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 13),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D12*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 14),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D13*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 15),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D14*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 16),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D15*/
		SUNXI_FUNCTION(0x7, "io_disabled")),

	/* HOLE */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* PCLK*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi_mck1"),		/* MCLK*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* HSYNC*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* VSYNC*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D0*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D1*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D2*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D3*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D4*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D5*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D6*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 11),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D7*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 12),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D8*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 13),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D9*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 14),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D10*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 15),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi3"),		/* D11*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 16),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi_cci1"),	/* SCK*/
		SUNXI_FUNCTION(0x3, "twi3"),	/* SCK*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 17),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi_cci1"),	/* SDA*/
		SUNXI_FUNCTION(0x3, "twi3"),	/* SDA*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
};

static const unsigned sun8iw12p1_irq_bank_base[] = {
	SUNXI_PIO_BANK_BASE(PA_BASE, 0),
	SUNXI_PIO_BANK_BASE(PB_BASE, 1),
	SUNXI_PIO_BANK_BASE(PF_BASE, 2),
	SUNXI_PIO_BANK_BASE(PG_BASE, 3),
	SUNXI_PIO_BANK_BASE(PH_BASE, 4),
};

static const struct sunxi_pinctrl_desc sun8iw12p1_pinctrl_data = {
	.pins = sun8iw12p1_pins,
	.npins = ARRAY_SIZE(sun8iw12p1_pins),
	.pin_base = 0,
	.irq_banks = 5,
	.irq_bank_base = sun8iw12p1_irq_bank_base,
};

static int sun8iw12p1_pinctrl_probe(struct platform_device *pdev)
{
	return sunxi_pinctrl_init(pdev, &sun8iw12p1_pinctrl_data);
}

static struct of_device_id sun8iw12p1_pinctrl_match[] = {
	{ .compatible = "allwinner,sun8iw12p1-pinctrl", },
	{}
};
MODULE_DEVICE_TABLE(of, sun8iw12p1_pinctrl_match);

static struct platform_driver sun8iw12p1_pinctrl_driver = {
	.probe	= sun8iw12p1_pinctrl_probe,
	.driver	= {
		.name		= "sun8iw12p1-pinctrl",
		.owner		= THIS_MODULE,
		.of_match_table	= sun8iw12p1_pinctrl_match,
	},
};

static int __init sun8iw12p1_pio_init(void)
{
	int ret;
	ret = platform_driver_register(&sun8iw12p1_pinctrl_driver);
	if (IS_ERR_VALUE(ret)) {
		pr_err("register sun8iw12p1 pio controller failed\n");
		return -EINVAL;
	}
	return 0;
}
postcore_initcall(sun8iw12p1_pio_init);

MODULE_AUTHOR("WimHuang<huangwei@allwinnertech.com>");
MODULE_DESCRIPTION("Allwinner sun8iw12p1 pio pinctrl driver");
MODULE_LICENSE("GPL");
