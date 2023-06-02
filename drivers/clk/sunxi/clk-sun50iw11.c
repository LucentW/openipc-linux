/*
 * Copyright (C) 2013 Allwinnertech, huanghuafeng <huafenghuang@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/delay.h>
#include <linux/clk/sunxi.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include "clk-sunxi.h"
#include "clk-factors.h"
#include "clk-periph.h"
#include "clk-sun50iw11.h"
#include "clk-sun50iw11_tbl.c"

#define FACTOR_SIZEOF(name) (sizeof(factor_pll##name##_tbl)/ \
			     sizeof(struct sunxi_clk_factor_freq))

#define FACTOR_SEARCH(name) (sunxi_clk_com_ftr_sr( \
		&sunxi_clk_factor_pll_##name, factor, \
		factor_pll##name##_tbl, index, \
		FACTOR_SIZEOF(name)))

#ifndef CONFIG_EVB_PLATFORM
	#define LOCKBIT(x) 31
#else
	#define LOCKBIT(x) x
#endif

DEFINE_SPINLOCK(clk_lock);
void __iomem *sunxi_clk_base;
void __iomem *sunxi_clk_cpus_base;
void __iomem *sunxi_clk_rtc_base;
int sunxi_clk_maxreg = SUNXI_CLK_MAX_REG;
int cpus_clk_maxreg = CPUS_CLK_MAX_REG;
#define REG_ADDR(x) (sunxi_clk_base + x)

/*                                ns  nw  ks  kw  ms  mw  ps  pw  d1s d1w d2s d2w {frac   out mode}   en-s    sdmss   sdmsw   sdmpat          sdmval*/
SUNXI_CLK_FACTORS(pll_cpu,        8,  8,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,    0,    0,  0,      31,     0,      0,      0,              0);
SUNXI_CLK_FACTORS(pll_periph0,    8,  8,  0,  0,  1,  1,  16, 3,  0,  0,  0,  0,    0,    0,  0,      31,     24,     1,      PLL_PERI0PAT0,  0xd1303333);
SUNXI_CLK_FACTORS(pll_audio0,     8,  8,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,    0,    0,  0,      31,     24,     1,      PLL_AUDIO0PAT0, 0xc00121ff);
SUNXI_CLK_FACTORS(pll_audio1,     8,  8,  0,  0,  0,  0,  16, 6,  0,  1,  1,  1,    0,    0,  0,      31,     24,     1,      PLL_AUDIO1PAT0, 0xc00121ff);

/* pll_de renamed as pll_com */

static int get_factors_pll_cpu(u32 rate, u32 parent_rate,
		struct clk_factors_value *factor)
{

	int index;
	u64 tmp_rate;

	if (!factor)
		return -1;
	tmp_rate = rate > pllcpu_max ? pllcpu_max : rate;
	do_div(tmp_rate, 1000000);
	index = tmp_rate;

	if (FACTOR_SEARCH(cpu))
		return -1;

	return 0;
}

static int get_factors_pll_periph0(u32 rate, u32 parent_rate,
		struct clk_factors_value *factor)
{
	int index;
	u64 tmp_rate;

	if (!factor)
		return -1;

	tmp_rate = rate > pllperiph0_max ? pllperiph0_max : rate;
	do_div(tmp_rate, 1000000);
	index = tmp_rate;

	if (FACTOR_SEARCH(periph0))
		return -1;

	return 0;
}

/*    pll_cpux: 24*N  */
static unsigned long calc_rate_pll_cpu(u32 parent_rate,
		struct clk_factors_value *factor)
{
	u64 tmp_rate = (parent_rate ? parent_rate : 24000000);
	tmp_rate = tmp_rate * (factor->factorn + 1);
	return (unsigned long)tmp_rate;
}

/*    pll_periph0: 24*N/M/p0/2    */
static unsigned long calc_rate_pll_periph(u32 parent_rate,
		struct clk_factors_value *factor)
{
	u64 tmp_rate = (parent_rate ? parent_rate : 24000000);
	tmp_rate = tmp_rate * (factor->factorn + 1);
	do_div(tmp_rate, 2 * (factor->factorm + 1) * (factor->factorp + 1));
	return (unsigned long)tmp_rate;
}

/*
 *    pll_audio: 24*N/D1/D2/P
 *
 *    NOTE: pll_audiox4 = 24*N/D1/2
 *          pll_audiox2 = 24*N/D1/4
 *
 *    pll_audiox4=2*pll_audiox2=4*pll_audio only when D2*P=8
 */
static unsigned long calc_rate_audio1(u32 parent_rate,
		struct clk_factors_value *factor)
{
	u64 tmp_rate = (parent_rate ? parent_rate : 24000000);
	if ((factor->factorn == 21) &&
			(factor->factorp == 11) &&
			(factor->factord1 == 0) &&
			(factor->factord2 == 1))
		return 22579200;
	else if ((factor->factorn == 23) &&
			(factor->factorp == 11) &&
			(factor->factord1 == 0) &&
			(factor->factord2 == 1))
		return 24576000;
	else if ((factor->factorn == 21) &&
			(factor->factorp == 2) &&
			(factor->factord1 == 0) &&
			(factor->factord2 == 1))
		return 90316800;
	else if ((factor->factorn == 23) &&
			(factor->factorp == 2) &&
			(factor->factord1 == 0) &&
			(factor->factord2 == 1))
		return 98304000;
	else {
		tmp_rate = tmp_rate * (factor->factorn + 1);
		do_div(tmp_rate, ((factor->factorp + 1) *
				(factor->factord1 + 1) *
				(factor->factord2 + 1)));
		return (unsigned long)tmp_rate;
	}
}

static int get_factors_pll_audio0(u32 rate, u32 parent_rate,
		struct clk_factors_value *factor)
{
	if (rate == 22579200) {
		factor->factorn = 21;
		factor->factorp = 11;
		factor->factord1 = 0;
		factor->factord2 = 1;
		sunxi_clk_factor_pll_audio0.sdmval = 0xC001288D;
	} else if (rate == 24576000) {
		factor->factorn = 23;
		factor->factorp = 11;
		factor->factord1 = 0;
		factor->factord2 = 1;
		sunxi_clk_factor_pll_audio0.sdmval = 0xC00126E9;
	} else if (rate == 90316800) {
		factor->factorn = 21;
		factor->factorp = 2;
		factor->factord1 = 0;
		factor->factord2 = 1;
		sunxi_clk_factor_pll_audio0.sdmval = 0xC001288D;
	} else if (rate == 98304000) {
		factor->factorn = 23;
		factor->factorp = 2;
		factor->factord1 = 0;
		factor->factord2 = 1;
		sunxi_clk_factor_pll_audio0.sdmval = 0xC00126E9;
	} else
		return -1;

	return 0;
}

static int get_factors_pll_audio1(u32 rate, u32 parent_rate,
		struct clk_factors_value *factor)
{
	if (rate == 22579200) {
		factor->factorn = 21;
		factor->factorp = 11;
		factor->factord1 = 0;
		factor->factord2 = 1;
		sunxi_clk_factor_pll_audio1.sdmval = 0xC001288D;
	} else if (rate == 24576000) {
		factor->factorn = 23;
		factor->factorp = 11;
		factor->factord1 = 0;
		factor->factord2 = 1;
		sunxi_clk_factor_pll_audio1.sdmval = 0xC00126E9;
	} else if (rate == 90316800) {
		factor->factorn = 21;
		factor->factorp = 2;
		factor->factord1 = 0;
		factor->factord2 = 1;
		sunxi_clk_factor_pll_audio1.sdmval = 0xC001288D;
	} else if (rate == 98304000) {
		factor->factorn = 23;
		factor->factorp = 2;
		factor->factord1 = 0;
		factor->factord2 = 1;
		sunxi_clk_factor_pll_audio1.sdmval = 0xC00126E9;
	} else
		return -1;

	return 0;
}

static const char *hosc_parents[] = {"hosc"};
struct factor_init_data sunxi_factos[] = {
	/* name         parent        parent_num, flags	                reg          lock_reg     lock_bit     lock_ctrl_reg   lock_en_bit  lock_mode           config                        get_factors                calc_rate             priv_ops*/
	{"pll_cpu",     hosc_parents, 1,          CLK_GET_RATE_NOCACHE, PLL_CPU,     PLL_CPU,     LOCKBIT(28), PLL_CPU,        29,          PLL_LOCK_NEW_MODE, &sunxi_clk_factor_pll_cpu,     &get_factors_pll_cpu,     &calc_rate_pll_cpu,    (struct clk_ops *)NULL},
	{"pll_periph0", hosc_parents, 1,          0,                    PLL_PERIPH0, PLL_PERIPH0, LOCKBIT(28), PLL_PERIPH0,    29,          PLL_LOCK_NEW_MODE, &sunxi_clk_factor_pll_periph0, &get_factors_pll_periph0, &calc_rate_pll_periph, (struct clk_ops *)NULL},
	{"pll_audio0",  hosc_parents, 1,          0,                    PLL_AUDIO0,  PLL_AUDIO0,  LOCKBIT(28), PLL_AUDIO0,     29,          PLL_LOCK_NEW_MODE, &sunxi_clk_factor_pll_audio0,  &get_factors_pll_audio1,  &calc_rate_audio1,     (struct clk_ops *)NULL},
	{"pll_audio1",  hosc_parents, 1,          0,                    PLL_AUDIO1,  PLL_AUDIO1,  LOCKBIT(28), PLL_AUDIO1,     29,          PLL_LOCK_NEW_MODE, &sunxi_clk_factor_pll_audio1,  &get_factors_pll_audio0,  &calc_rate_audio1,     (struct clk_ops *)NULL},
};

static const char *cpu_parents[] = {"hosc", "losc", "iosc", "pll_cpu", "pll_periph0", "pll_periph0x2", "pll_periph0800m"};
static const char *axi_parents[] = {"cpu"};
static const char *cpuapb_parents[] = {"cpu"};
static const char *psi_parents[] = {"hosc", "losc", "iosc", "pll_periph0"};
static const char *ahb1_parents[] = {"psi"};
static const char *ahb2_parents[] = {"psi"};
static const char *apb1_parents[] = {"hosc", "losc", "psi", "pll_periph0"};
static const char *apb2_parents[] = {"hosc", "losc", "psi", "pll_periph0"};
static const char *mbus_parents[] = {"hosc", "pll_periph0x2"};
static const char *ce_parents[] = {"hosc", "pll_periph0x2"};
static const char *aipu_parents[] = {"pll_periph0x2", "pll_periph0800m", "pll_audio0_div2", "pll_audio0_div5", "pll_cpu"};
static const char *ahb1mod_parents[] = {"ahb1"};
static const char *apb1mod_parents[] = {"apb1"};
static const char *apb2mod_parents[] = {"apb2"};
static const char *sdram_parents[] = {"pll_periph0x2", "pll_periph0800m", "pll_audio0_div2"};
static const char *nand_parents[] = {"hosc", "pll_periph0", "", "pll_periph0x2"};
static const char *smhc_parents[] = {"hosc", "pll_periph0", "pll_periph0x2"};
static const char *spi_parents[] = {"hosc", "pll_periph0", "pll_periph0x2"};
static const char *gmac_25m_parents[] = {"pll_periph0"};
static const char *ir_parents[] = {"losc", "hosc", "pll_periph0", "pll_periph1"};
static const char *audio_parents[] = {"pll_audio1", "pll_audio1x4", "pll_audio0_div2", "pll_audio0_div5"};
static const char *usbohci12m_parents[] = {"osc48md4", "hoscd2", "losc"};
static const char *ledc_parents[] = {"hosc", "pll_periph0"};
static const char *cpurahbs_parents[] = {"hosc", "losc", "iosc", "pll_periph0", "pll_audio0_div2"};
static const char *cpurapbs_parents[] = {"hosc", "losc", "iosc", "pll_periph0", "pll_audio0_div2"};
static const char *pll_lock_dg_parents[] = {"pll_cpu", "pll_audio0", "pll_audio1", "pll_periph0"};
static const char *pll_dg_parents[] = {"pll_audio0", "pll_cpu", "pll_audio1", "pll_periph0", "hosc"};
static const char *mad_parents[] = {"pll_audio0_div5", "pll_audio_div2", "pll_audio1", "pll_audio1x4"};
static const char *apbs0mod_parents[] = {"cpurapbs0"};
static const char *apbs1mod_parents[] = {"cpurapbs1"};
static const char *ahbsmod_parents[] = {"cpurahbs"};
static const char *cpurcir_parents[] = {"losc", "hosc"};
static const char *r_pwm_parents[] = {"dcxo_out", "losc", "iosc"};
static const char *psram_parents[] = {"hosc", "pll_audio0_div5", "pll_audio0_div2", "pll_periph0x2"};
static const char *losc_parents[] = {"losc", "periph32k", "hosc32k"};
static const char *dcxo_parents[] = {"hosc"};
static const char *hosc32k_parents[] = {"hoscdiv32k"};


struct sunxi_clk_comgate com_gates[] = {
{"nand",    0, 0x7, BUS_GATE_SHARE|RST_GATE_SHARE|MBUS_GATE_SHARE, 0},
{"codec",   0, 0x3, BUS_GATE_SHARE|RST_GATE_SHARE, 0},
{"aipu",    0, 0x7, RST_GATE_SHARE|MBUS_GATE_SHARE, 0},
};

/*
SUNXI_CLK_PERIPH(name,           mux_reg,         mux_sft, mux_wid,      div_reg,            div_msft,  div_mwid,   div_nsft,   div_nwid,   gate_flag,  en_reg,          rst_reg,         bus_gate_reg,     drm_gate_reg,  en_sft,     rst_sft,    bus_gate_sft,   dram_gate_sft,lock,       com_gate,         com_gate_off)
*/
SUNXI_CLK_PERIPH(cpu,            CPU_CFG,         24,      3,            0,                  0,         0,          0,          0,          0,          0,               0,               0,                0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(axi,            0,               0,       0,            CPU_CFG,            0,         2,          0,          0,          0,          0,               0,               0,                0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpuapb,         0,               0,       0,            CPU_CFG,            8,         2,          0,          0,          0,          0,               0,               0,                0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(psi,            PSI_CFG,         24,      2,            PSI_CFG,            0,         2,          8,          2,          0,          0,               PSI_GATE,        PSI_GATE,         0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(ahb1,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               0,               0,                0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(ahb2,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               0,               0,                0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(apb1,           APB1_CFG,        24,      2,            APB1_CFG,           0,         2,          8,          2,          0,          0,               0,               0,                0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(apb2,           APB2_CFG,        24,      2,            APB2_CFG,           0,         2,          8,          2,          0,          0,               0,               0,                0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(mbus,           MBUS_CFG,        24,      2,            MBUS_CFG,           0,         3,          0,          0,          0,          MBUS_CFG,        MBUS_CFG,        0,                0,             31,         30,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(ce,             CE_CFG,          24,      1,            CE_CFG,             0,         4,          8,          2,          0,          CE_CFG,          CE_GATE,         CE_GATE,          MBUS_GATE,     31,         16,         0,              2,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(aipu,           AIPU_CFG,        24,      3,            AIPU_CFG,           0,         4,          8,          2,          0,          AIPU_CFG,        AIPU_GATE,       AIPU_GATE,        MBUS_GATE,     31,         16,         1,              16,            &clk_lock, &com_gates[2],    0);
SUNXI_CLK_PERIPH(aipu_slv,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               AIPU_GATE,       AIPU_GATE,        MBUS_GATE,     0,          16,         0,              16,            &clk_lock, &com_gates[2],    1);
SUNXI_CLK_PERIPH(dma,            0,                0,      0,            0,                  0,         0,          0,          0,          0,          0,               DMA_GATE,        DMA_GATE,         MBUS_GATE,     0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(msgbox0,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               MSGBOX_GATE,     MSGBOX_GATE,      0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(msgbox1,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               MSGBOX_GATE,     MSGBOX_GATE,      0,             0,          17,         1,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(hwspinlock,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               SPINLOCK_GATE,   SPINLOCK_GATE,    0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(hstimer,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               HSTIMER_GATE,    HSTIMER_GATE,     0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(avs,            0,               0,       0,            0,                  0,         0,          0,          0,          0,          AVS_CFG,         0,               0,                0,             31,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(dbgsys,         0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               DBGSYS_GATE,     DBGSYS_GATE,      0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(pwm,            0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               PWM_GATE,        PWM_GATE,         0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdram,          DRAM_CFG,        24,      2,            DRAM_CFG,           0,         2,          0,          0,          0,          0,               DRAM_GATE,       DRAM_GATE,        0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(nand0,          NAND0_CFG,       24,      3,            NAND0_CFG,          0,         4,          8,          2,          0,          NAND0_CFG,       NAND_GATE,       NAND_GATE,        MBUS_GATE,     31,         16,         0,              5,             &clk_lock, &com_gates[0],    0);
SUNXI_CLK_PERIPH(nand1,          NAND1_CFG,       24,      3,            NAND1_CFG,          0,         4,          8,          2,          0,          NAND1_CFG,       NAND_GATE,       NAND_GATE,        MBUS_GATE,     31,         16,         0,              5,             &clk_lock, &com_gates[0],    1);
SUNXI_CLK_PERIPH(sdmmc0_mod,     SMHC0_CFG,       24,      2,            SMHC0_CFG,          0,         4,          8,          2,          0,          SMHC0_CFG,       0,               0,                0,             31,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc0_rst,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               SMHC_GATE,       0,                0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc0_bus,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               0,               SMHC_GATE,        0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc1_mod,     SMHC1_CFG,       24,      2,            SMHC1_CFG,          0,         4,          8,          2,          0,          SMHC1_CFG,       0,               0,                0,             31,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc1_rst,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               SMHC_GATE,       0,                0,             0,          17,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(sdmmc1_bus,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               0,               SMHC_GATE,        0,             0,          0,          1,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(uart0,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               UART_GATE,       UART_GATE,        0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(uart1,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               UART_GATE,       UART_GATE,        0,             0,          17,         1,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(uart2,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               UART_GATE,       UART_GATE,        0,             0,          18,         2,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(uart3,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               UART_GATE,       UART_GATE,        0,             0,          19,         3,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(twi0,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               TWI_GATE,        TWI_GATE,         0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(twi1,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               TWI_GATE,        TWI_GATE,         0,             0,          17,         1,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(scr0,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               SCR_GATE,        SCR_GATE,         0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(spi0,           SPI0_CFG,        24,      3,            SPI0_CFG,           0,         4,          8,          2,          0,          SPI0_CFG,        SPI_GATE,        SPI_GATE,         0,             31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(spi1,           SPI1_CFG,        24,      3,            SPI1_CFG,           0,         4,          8,          2,          0,          SPI1_CFG,        SPI_GATE,        SPI_GATE,         0,             31,         17,         1,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(gmac0_25m,      0,               0,       0,            0,                  0,         0,          0,          0,          0,          GMAC0_25M_CFG,   0,               GMAC0_25M_CFG,    0,             31,         0,          30,             0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(gmac0,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               GMAC_GATE,       GMAC_GATE,        0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(irrx,           IRRX_CFG,        24,      3,            IRRX_CFG,           0,         4,          8,          2,          0,          IRRX_CFG,        IRRX_GATE,       IRRX_GATE,        0,             31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(irtx,           IRTX_CFG,        24,      3,            IRTX_CFG,           0,         4,          8,          2,          0,          IRTX_CFG,        IRTX_GATE,       IRTX_GATE,        0,             31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(i2s0,           I2S0_CFG,        24,      2,            I2S0_CFG,           0,         0,          8,          2,          0,          I2S0_CFG,        I2S_GATE,        I2S_GATE,         0,             31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(i2s1,           I2S1_CFG,        24,      2,            I2S1_CFG,           0,         0,          8,          2,          0,          I2S1_CFG,        I2S_GATE,        I2S_GATE,         0,             31,         17,         1,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(i2s2,           I2S2_CFG,        24,      2,            I2S2_CFG,           0,         0,          8,          2,          0,          I2S2_CFG,        I2S_GATE,        I2S_GATE,         0,             31,         18,         2,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(spdif,          SPDIF_CFG,       24,      2,            SPDIF_CFG,          0,         0,          8,          2,          0,          SPDIF_CFG,       SPDIF_GATE,      SPDIF_GATE,       0,             31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbphy0,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          USB0_CFG,        USB0_CFG,        0,                0,             29,         30,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbphy1,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          USB1_CFG,        USB1_CFG,        0,                0,             29,         30,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbohci0,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          USB0_CFG,        USB_GATE,        USB_GATE,         0,             31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbohci1,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          USB1_CFG,        USB_GATE,        USB_GATE,         0,             31,         17,         1,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbohci0_12m,   USB0_CFG,        24,      2,            0,                  0,         0,          0,          0,          0,          0,               0,               0,                0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbohci1_12m,   USB1_CFG,        24,      2,            0,                  0,         0,          0,          0,          0,          0,               0,               0,                0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbehci0,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               USB_GATE,        USB_GATE,         0,             0,          20,         4,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbehci1,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               USB_GATE,        USB_GATE,         0,             0,          21,         5,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbotg0,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               USB_GATE,        USB_GATE,         0,             0,          24,         8,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(usbotg1,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               USB_GATE,        USB_GATE,         0,             0,          25,         9,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(ledc,           LEDC_CFG,        24,      2,            LEDC_CFG,           0,         4,          8,          2,          0,          LEDC_CFG,        LEDC_GATE,       LEDC_GATE,        0,             31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(pio,            0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               0,               0,                0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpurahbs,       CPUS_AHBS_CFG,   24,      3,            CPUS_AHBS_CFG,      0,         5,          8,          2,          0,          0,               0,               0,                0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpurapbs0,      CPUS_APBS0_CFG,  24,      3,            CPUS_APBS0_CFG,     0,         5,          8,          2,          0,          0,               0,               0,                0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpurapbs1,      CPUS_APBS1_CFG,  24,      3,            CPUS_APBS1_CFG,     0,         5,          8,          2,          0,          0,               0,               0,                0,             0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(pll_lock_dg,    CPUS_LOCKDG_CFG, 20,      5,            0,                  0,         0,          0,          0,          0,          CPUS_LOCKDG_CFG, 0,               0,                0,             31,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(pll_dg,         CPUS_DG_CFG,     24,      4,            0,                  0,         0,          0,          0,          0,          CPUS_DG_CFG,     0,               0,                0,             31,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(mad,            CPUS_MAD_CFG,    24,      2,            CPUS_MAD_CFG,       0,         5,          16,         2,          0,          CPUS_MAD_CFG,    CPUS_MAD_GATE,   CPUS_MAD_GATE,    0,             31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(mad_cfg,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_MAD_GATE,   CPUS_MAD_GATE,    0,             0,          17,         1,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(lpsd,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_MAD_GATE,   0,                0,             0,          18,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(mad_sram,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_MAD_GATE,   CPUS_MAD_GATE,    0,             0,          19,         3,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(gpadc,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_GPADC_GATE, CPUS_GPADC_GATE,  0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(ths,            0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_THS_GATE,   CPUS_THS_GATE,    0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(r_dma,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_DMA_GATE,   CPUS_DMA_GATE,    MBUS_GATE,     0,          16,         0,              3,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(timer,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_TIMER_GATE, CPUS_TIMER_GATE,  0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(watchdog,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_TWDOG_GATE, CPUS_TWDOG_GATE,  0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(r_pwm,          CPUS_PWM_CFG,    24,      2,            0,                  0,         0,          0,          0,          0,          CPUS_PWM_CFG,    CPUS_PWM_GATE,   CPUS_PWM_GATE,    0,             31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(codec_adc,      CODEC_ADC_CFG,   24,      2,            CODEC_ADC_CFG,      0,         5,          8,          2,          0,          CODEC_ADC_CFG,   CODEC_GATE,      CODEC_GATE,       0,             31,         16,         0,              0,             &clk_lock, &com_gates[1],    0);
SUNXI_CLK_PERIPH(codec_dac,      CODEC_DAC_CFG,   24,      2,            CODEC_ADC_CFG,      0,         5,          8,          2,          0,          CODEC_DAC_CFG,   CODEC_GATE,      CODEC_GATE,       0,             31,         16,         0,              0,             &clk_lock, &com_gates[1],    0);
SUNXI_CLK_PERIPH(dmic,           CPUS_DMIC_CFG,   24,      2,            CODEC_ADC_CFG,      0,         5,          8,          2,          0,          CPUS_DMIC_CFG,   CPUS_DMIC_GATE,  CPUS_DMIC_GATE,   0,             31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(lradc,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_LRADC_GATE, CPUS_LRADC_GATE,  0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(r_i2s0,         CPUS_I2S0_CFG,   24,      2,            CPUS_I2S0_CFG,      0,         5,          8,          2,          0,          CPUS_I2S0_CFG,   CPUS_I2S_GATE,   CPUS_I2S_GATE,    0,             31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(r_i2s0_asrc,      I2S0_ASRC_CFG,   24,      2,            I2S0_ASRC_CFG,      0,         5,          8,          2,          0,          I2S0_ASRC_CFG,   0,               0,                0,             31,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(r_i2s1,         CPUS_I2S1_CFG,   24,      2,            CPUS_I2S1_CFG,      0,         5,          8,          2,          0,          CPUS_I2S1_CFG,   CPUS_I2S_GATE,   CPUS_I2S_GATE,    0,             31,         17,         1,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(r_uart,         0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_UART_GATE,  CPUS_UART_GATE,   0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(r_twi0,         0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_TWI_GATE,   CPUS_TWI_GATE,    0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(r_ppu,          0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_PPU_GATE,   CPUS_PPU_GATE,    0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(dsp0,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_DSP_GATE,   CPUS_DSP_GATE,    0,             0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(dsp0_cfg,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_DSP_GATE,   CPUS_DSP_GATE,    0,             0,          17,         1,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(dsp1,           0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_DSP_GATE,   CPUS_DSP_GATE,    0,             0,          19,         2,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(dsp1_cfg,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_DSP_GATE,   CPUS_DSP_GATE,    0,             0,          20,         3,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(dsp0_dbg,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_DSP_GATE,   0,                0,             0,          18,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpurcir,        CPUS_IRRX_CFG,  24,       2,            CPUS_IRRX_CFG,      0,         5,          8,          2,          0,          CPUS_IRRX_CFG,   CPUS_IRRX_GATE,  CPUS_IRRX_GATE,   0,            31,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(r_msgbox,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_MSGBOX_GATE, CPUS_MSGBOX_GATE, 0,            0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(r_hwspinlock,   0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_SPINLOCK_GATE, CPUS_SPINLOCK_GATE, 0,        0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(r_dsp_sram,     0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_DSP_SRAM_GATE, CPUS_DSP_SRAM_GATE, 0,        0,          18,         2,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(r_dsp_cache0,   0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_DSP_SRAM_GATE, CPUS_DSP_SRAM_GATE, 0,        0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(r_dsp_cache1,   0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_DSP_SRAM_GATE, CPUS_DSP_SRAM_GATE, 0,        0,          17,         1,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(rtc,            0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_RTC_GATE,    CPUS_RTC_GATE,    0,            0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(psram,          CPUS_PSARM_CFG, 24,      2,            CPUS_PSARM_CFG,     0,         5,          8,          2,          0,          CPUS_PSARM_CFG,  CPUS_PSARM_GATE,  CPUS_PSARM_GATE,  0,            31,         16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpucfg,         0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               CPUS_CPUCFG_GATE, CPUS_CPUCFG_GATE, 0,            0,          16,         0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(losc_out,       LOSC_OUT_GATE,   1,       2,            0,                  0,         0,          0,          0,          0,          0,               0,                LOSC_OUT_GATE,    0,            0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(cpurpio,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          0,               0,                0,                0,            0,          0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(dcxo_out,       0,               0,       0,            0,                  0,         0,          0,          0,          0,          DCXO_OUT_CFG,    0,                0,                0,            31,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(rtc_spi,        0,               0,       0,            RTC_SPI_CFG,        0,         5,          0,          0,          0,          RTC_SPI_CFG,     0,                0,                0,            31,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(hosc32k,        0,               0,       0,            0,                  0,         0,          0,          0,          0,          LOSC_OUT_GATE,   0,                0,                0,            16,         0,          0,              0,             &clk_lock, NULL,             0);
SUNXI_CLK_PERIPH(ir0,            IR0_CFG,        24,       2,            IR0_CFG,            0,         4,          16,         2,           0,          IR0_CFG,         BUS_RST3,         BUS_GATE2,       0,            31,         6,          6,              0,             &clk_lock, NULL,             0);

struct periph_init_data sunxi_periphs_init[] = {
	{"cpu",            CLK_GET_RATE_NOCACHE, cpu_parents,            ARRAY_SIZE(cpu_parents),            &sunxi_clk_periph_cpu              },
	{"axi",            0,                    axi_parents,            ARRAY_SIZE(axi_parents),            &sunxi_clk_periph_axi              },
	{"cpuapb",         0,                    cpuapb_parents,         ARRAY_SIZE(cpuapb_parents),         &sunxi_clk_periph_cpuapb           },
	{"psi",            0,                    psi_parents,            ARRAY_SIZE(psi_parents),            &sunxi_clk_periph_psi              },
	{"ahb1",           0,                    ahb1_parents,           ARRAY_SIZE(ahb1_parents),           &sunxi_clk_periph_ahb1             },
	{"ahb2",           0,                    ahb2_parents,           ARRAY_SIZE(ahb2_parents),           &sunxi_clk_periph_ahb2             },
	{"apb1",           0,                    apb1_parents,           ARRAY_SIZE(apb1_parents),           &sunxi_clk_periph_apb1             },
	{"apb2",           0,                    apb2_parents,           ARRAY_SIZE(apb2_parents),           &sunxi_clk_periph_apb2             },
	{"mbus",           0,                    mbus_parents,           ARRAY_SIZE(mbus_parents),           &sunxi_clk_periph_mbus             },
	{"ce",             0,                    ce_parents,             ARRAY_SIZE(ce_parents),             &sunxi_clk_periph_ce               },
	{"aipu",           0,                    aipu_parents,           ARRAY_SIZE(aipu_parents),           &sunxi_clk_periph_aipu             },
	{"aipu_slv",       0,                    aipu_parents,           ARRAY_SIZE(aipu_parents),           &sunxi_clk_periph_aipu_slv         },
	{"dma",            0,                    ahb1mod_parents,        ARRAY_SIZE(ahb1mod_parents),        &sunxi_clk_periph_dma              },
	{"msgbox0",        0,                    ahb1mod_parents,        ARRAY_SIZE(ahb1mod_parents),        &sunxi_clk_periph_msgbox0          },
	{"msgbox1",        0,                    ahb1mod_parents,        ARRAY_SIZE(ahb1mod_parents),        &sunxi_clk_periph_msgbox1          },
	{"hwspinlock",     0,                    ahb1mod_parents,        ARRAY_SIZE(ahb1mod_parents),        &sunxi_clk_periph_hwspinlock       },
	{"hstimer",        0,                    ahb1mod_parents,        ARRAY_SIZE(ahb1mod_parents),        &sunxi_clk_periph_hstimer          },
	{"avs",            0,                    hosc_parents,           ARRAY_SIZE(hosc_parents),           &sunxi_clk_periph_avs              },
	{"dbgsys",         0,                    hosc_parents,           ARRAY_SIZE(hosc_parents),           &sunxi_clk_periph_dbgsys           },
	{"pwm",            0,                    apb1mod_parents,        ARRAY_SIZE(apb1mod_parents),        &sunxi_clk_periph_pwm              },
	{"sdram",          0,                    sdram_parents,          ARRAY_SIZE(sdram_parents),          &sunxi_clk_periph_sdram            },
	{"nand0",          0,                    nand_parents,           ARRAY_SIZE(nand_parents),           &sunxi_clk_periph_nand0            },
	{"nand1",          0,                    nand_parents,           ARRAY_SIZE(nand_parents),           &sunxi_clk_periph_nand1            },
	{"sdmmc0_mod",     0,                    smhc_parents,           ARRAY_SIZE(smhc_parents),           &sunxi_clk_periph_sdmmc0_mod       },
	{"sdmmc0_rst",     0,                    smhc_parents,           ARRAY_SIZE(smhc_parents),           &sunxi_clk_periph_sdmmc0_rst       },
	{"sdmmc0_bus",     0,                    smhc_parents,           ARRAY_SIZE(smhc_parents),           &sunxi_clk_periph_sdmmc0_bus       },
	{"sdmmc1_mod",     0,                    smhc_parents,           ARRAY_SIZE(smhc_parents),           &sunxi_clk_periph_sdmmc1_mod       },
	{"sdmmc1_rst",     0,                    smhc_parents,           ARRAY_SIZE(smhc_parents),           &sunxi_clk_periph_sdmmc1_rst       },
	{"sdmmc1_bus",     0,                    smhc_parents,           ARRAY_SIZE(smhc_parents),           &sunxi_clk_periph_sdmmc1_bus       },
	{"uart0",          0,                    apb2mod_parents,        ARRAY_SIZE(apb2mod_parents),        &sunxi_clk_periph_uart0            },
	{"uart1",          0,                    apb2mod_parents,        ARRAY_SIZE(apb2mod_parents),        &sunxi_clk_periph_uart1            },
	{"uart2",          0,                    apb2mod_parents,        ARRAY_SIZE(apb2mod_parents),        &sunxi_clk_periph_uart2            },
	{"uart3",          0,                    apb2mod_parents,        ARRAY_SIZE(apb2mod_parents),        &sunxi_clk_periph_uart3            },
	{"twi0",           0,                    apb2mod_parents,        ARRAY_SIZE(apb2mod_parents),        &sunxi_clk_periph_twi0             },
	{"twi1",           0,                    apb2mod_parents,        ARRAY_SIZE(apb2mod_parents),        &sunxi_clk_periph_twi1             },
	{"scr0",           0,                    apb2mod_parents,        ARRAY_SIZE(apb2mod_parents),        &sunxi_clk_periph_scr0             },
	{"spi0",           0,                    spi_parents,            ARRAY_SIZE(spi_parents),            &sunxi_clk_periph_spi0             },
	{"spi1",           0,                    spi_parents,            ARRAY_SIZE(spi_parents),            &sunxi_clk_periph_spi1             },
	{"gmac0_25m",      0,                    gmac_25m_parents,       ARRAY_SIZE(gmac_25m_parents),       &sunxi_clk_periph_gmac0_25m        },
	{"gmac0",          0,                    ahb1mod_parents,        ARRAY_SIZE(ahb1mod_parents),        &sunxi_clk_periph_gmac0            },
	{"irrx",           0,                    ir_parents,             ARRAY_SIZE(ir_parents),             &sunxi_clk_periph_irrx             },
	{"irtx",           0,                    ir_parents,             ARRAY_SIZE(ir_parents),             &sunxi_clk_periph_irtx             },
	{"ir0",            0,                    ir_parents,             ARRAY_SIZE(ir_parents),             &sunxi_clk_periph_ir0              },
	{"i2s0",           0,                    audio_parents,          ARRAY_SIZE(audio_parents),          &sunxi_clk_periph_i2s0             },
	{"i2s1",           0,                    audio_parents,          ARRAY_SIZE(audio_parents),          &sunxi_clk_periph_i2s1             },
	{"i2s2",           0,                    audio_parents,          ARRAY_SIZE(audio_parents),          &sunxi_clk_periph_i2s2             },
	{"spdif",          0,                    audio_parents,          ARRAY_SIZE(audio_parents),          &sunxi_clk_periph_spdif            },
	{"usbphy0",        0,                    hosc_parents,           ARRAY_SIZE(hosc_parents),           &sunxi_clk_periph_usbphy0          },
	{"usbphy1",        0,                    hosc_parents,           ARRAY_SIZE(hosc_parents),           &sunxi_clk_periph_usbphy1          },
	{"usbohci0_12m",   0,                    usbohci12m_parents,     ARRAY_SIZE(usbohci12m_parents),     &sunxi_clk_periph_usbohci0_12m     },
	{"usbohci1_12m",   0,                    usbohci12m_parents,     ARRAY_SIZE(usbohci12m_parents),     &sunxi_clk_periph_usbohci1_12m    },
	{"usbohci0",       0,                    ahb1mod_parents,        ARRAY_SIZE(ahb1mod_parents),        &sunxi_clk_periph_usbohci0         },
	{"usbohci1",       0,                    ahb1mod_parents,        ARRAY_SIZE(ahb1mod_parents),        &sunxi_clk_periph_usbohci1         },
	{"usbehci0",       0,                    ahb1mod_parents,        ARRAY_SIZE(ahb1mod_parents),        &sunxi_clk_periph_usbehci0         },
	{"usbehci1",       0,                    ahb1mod_parents,        ARRAY_SIZE(ahb1mod_parents),        &sunxi_clk_periph_usbehci1         },
	{"usbotg0",        0,                    ahb1mod_parents,        ARRAY_SIZE(ahb1mod_parents),        &sunxi_clk_periph_usbotg0          },
	{"usbotg1",        0,                    ahb1mod_parents,        ARRAY_SIZE(ahb1mod_parents),        &sunxi_clk_periph_usbotg1          },
	{"ledc",           0,                    ledc_parents,           ARRAY_SIZE(ledc_parents),           &sunxi_clk_periph_ledc             },
	{"pio",            0,                    apb1mod_parents,        ARRAY_SIZE(apb1mod_parents),        &sunxi_clk_periph_pio              },
};

struct periph_init_data sunxi_periphs_cpus_init[] = {
	{"cpurahbs",        CLK_READONLY,                       cpurahbs_parents,       ARRAY_SIZE(cpurahbs_parents),       &sunxi_clk_periph_cpurahbs      },
	{"cpurapbs0",       CLK_READONLY,                       cpurapbs_parents,       ARRAY_SIZE(cpurapbs_parents),       &sunxi_clk_periph_cpurapbs0     },
	{"cpurapbs1",       CLK_READONLY,                       cpurapbs_parents,       ARRAY_SIZE(cpurapbs_parents),       &sunxi_clk_periph_cpurapbs1     },
	{"pll_lock_dg",     0,                                  pll_lock_dg_parents,    ARRAY_SIZE(pll_lock_dg_parents),    &sunxi_clk_periph_pll_lock_dg   },
	{"pll_dg",          0,                                  pll_dg_parents,         ARRAY_SIZE(pll_dg_parents),         &sunxi_clk_periph_pll_dg        },
	{"mad",             0,                                  mad_parents,            ARRAY_SIZE(mad_parents),            &sunxi_clk_periph_mad           },
	{"mad_cfg",         0,                                  mad_parents,            ARRAY_SIZE(mad_parents),            &sunxi_clk_periph_mad_cfg       },
	{"lpsd",            0,                                  mad_parents,            ARRAY_SIZE(mad_parents),            &sunxi_clk_periph_lpsd          },
	{"mad_sram",        0,                                  mad_parents,            ARRAY_SIZE(mad_parents),            &sunxi_clk_periph_mad_sram      },
	{"gpadc",           0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_gpadc         },
	{"ths",             0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_ths           },
	{"r_dma",           0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_r_dma         },
	{"timer",           0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_timer         },
	{"watchdog",        0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_watchdog      },
	{"r_pwm",           0,                                  r_pwm_parents,          ARRAY_SIZE(r_pwm_parents),          &sunxi_clk_periph_r_pwm         },
	{"codec_adc",       0,                                  mad_parents,            ARRAY_SIZE(mad_parents),            &sunxi_clk_periph_codec_adc     },
	{"codec_dac",       0,                                  mad_parents,            ARRAY_SIZE(mad_parents),            &sunxi_clk_periph_codec_dac     },
	{"dmic",            0,                                  mad_parents,            ARRAY_SIZE(mad_parents),            &sunxi_clk_periph_dmic          },
	{"lradc",           0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_lradc         },
	{"r_i2s0",          0,                                  mad_parents,            ARRAY_SIZE(mad_parents),            &sunxi_clk_periph_r_i2s0        },
	{"r_i2s0_asrc",     0,                                  mad_parents,            ARRAY_SIZE(mad_parents),            &sunxi_clk_periph_r_i2s0_asrc   },
	{"r_i2s1",          0,                                  mad_parents,            ARRAY_SIZE(mad_parents),            &sunxi_clk_periph_r_i2s1        },
	{"r_uart",          0,                                  apbs1mod_parents,       ARRAY_SIZE(apbs1mod_parents),       &sunxi_clk_periph_r_uart        },
	{"r_twi0",          0,                                  apbs1mod_parents,       ARRAY_SIZE(apbs1mod_parents),       &sunxi_clk_periph_r_twi0        },
	{"r_ppu",           0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_r_ppu         },
	{"dsp0",            0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_dsp0          },
	{"dsp0_cfg",        0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_dsp0_cfg      },
	{"dsp1",            0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_dsp1          },
	{"dsp1_cfg",        0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_dsp1_cfg      },
	{"dsp0_dbg",        0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_dsp0_dbg      },
	{"cpurcir",         0,                                  cpurcir_parents,        ARRAY_SIZE(cpurcir_parents),        &sunxi_clk_periph_cpurcir       },
	{"r_msgbox",        0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_r_msgbox      },
	{"r_hwspinlock",    0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_r_hwspinlock  },
	{"r_dsp_sram",      0,                                  ahbsmod_parents,        ARRAY_SIZE(ahbsmod_parents),        &sunxi_clk_periph_r_dsp_sram    },
	{"r_dsp_cache0",    0,                                  ahbsmod_parents,        ARRAY_SIZE(ahbsmod_parents),        &sunxi_clk_periph_r_dsp_cache0  },
	{"r_dsp_cache1",    0,                                  ahbsmod_parents,        ARRAY_SIZE(ahbsmod_parents),        &sunxi_clk_periph_r_dsp_cache1  },
	{"rtc",             0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_rtc           },
	{"psram",           0,                                  psram_parents,          ARRAY_SIZE(psram_parents),          &sunxi_clk_periph_psram         },
	{"cpucfg",          0,                                  apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_cpucfg        },
	{"cpurpio",         CLK_READONLY,                       apbs0mod_parents,       ARRAY_SIZE(apbs0mod_parents),       &sunxi_clk_periph_cpurpio       },
};

struct periph_init_data sunxi_periphs_rtc_init[] = {
	{"losc_out",        0,                                  losc_parents,           ARRAY_SIZE(losc_parents),           &sunxi_clk_periph_losc_out      },
	{"dcxo_out",        0,                                  dcxo_parents,           ARRAY_SIZE(dcxo_parents),           &sunxi_clk_periph_dcxo_out      },
	{"rtc_spi",         0,                                  ahbsmod_parents,        ARRAY_SIZE(ahbsmod_parents),        &sunxi_clk_periph_rtc_spi       },
	{"hosc32k",         0,                                  hosc32k_parents,        ARRAY_SIZE(hosc32k_parents),        &sunxi_clk_periph_hosc32k       },
};

/* dcxo private operations
 * dcxo control register(0x07000160),
 * if bit_31 == 0, dcxo 24M output for peripheral is enabled
 * if bit_31 == 1, dcxo 24M output for peripheral is disabled
 * we could enable dcxo 24M output for some peripheral devices such as
 * wifi modoule
 */
int dcxo_out_priv_enable(struct clk_hw *hw)
{
	unsigned long reg;

	if (sunxi_clk_periph_dcxo_out.gate.enable) {
		reg = readl(sunxi_clk_periph_dcxo_out.gate.enable);
		reg = SET_BITS(31, 1, reg, 0);
		writel(reg, sunxi_clk_periph_dcxo_out.gate.enable);
	}

	return 0;
}

void dcxo_out_priv_disable(struct clk_hw *hw)
{
	unsigned long reg;

	if (sunxi_clk_periph_dcxo_out.gate.enable) {
		reg = readl(sunxi_clk_periph_dcxo_out.gate.enable);
		reg = SET_BITS(31, 1, reg, 1);
		writel(reg, sunxi_clk_periph_dcxo_out.gate.enable);
	}
}

int dcxo_out_priv_is_enabled(struct clk_hw *hw)
{
	unsigned long reg;

	if (sunxi_clk_periph_dcxo_out.gate.enable) {
		reg = readl(sunxi_clk_periph_dcxo_out.gate.enable);
		if (GET_BITS(sunxi_clk_periph_dcxo_out.gate.enb_shift, 1, reg))
			return 0;
		else
			return 1;
	}

	return 0;
}

void set_dcxo_out_priv_ops(struct clk_ops *priv_ops)
{
	priv_ops->enable = dcxo_out_priv_enable;
	priv_ops->disable = dcxo_out_priv_disable;
	priv_ops->is_enabled = dcxo_out_priv_is_enabled;
}

struct clk_ops dcxo_out_priv_ops;
void sunxi_set_clk_priv_ops(char *clk_name, struct clk_ops  *clk_priv_ops,
	void (*set_priv_ops)(struct clk_ops *priv_ops))
{
	int i = 0;
	sunxi_clk_get_periph_ops(clk_priv_ops);
	set_priv_ops(clk_priv_ops);
	for (i = 0; i < (ARRAY_SIZE(sunxi_periphs_cpus_init)); i++) {
		if (!strcmp(sunxi_periphs_cpus_init[i].name, clk_name))
			sunxi_periphs_cpus_init[i].periph->priv_clkops =
								clk_priv_ops;
	}
}

/*
 * sunxi_clk_get_factor_by_name() - Get factor clk init config
 */
struct factor_init_data *sunxi_clk_get_factor_by_name(const char *name)
{
	struct factor_init_data *factor;
	int i;

	/* get pll clk init config */
	for (i = 0; i < ARRAY_SIZE(sunxi_factos); i++) {
		factor = &sunxi_factos[i];
		if (strcmp(name, factor->name))
			continue;
		return factor;
	}

	return NULL;
}

/*
 * sunxi_clk_get_periph_by_name() - Get periph clk init config
 */
struct periph_init_data *sunxi_clk_get_periph_by_name(const char *name)
{
	struct periph_init_data *perpih;
	int i;

	for (i = 0; i < ARRAY_SIZE(sunxi_periphs_init); i++) {
		perpih = &sunxi_periphs_init[i];
		if (strcmp(name, perpih->name))
			continue;
		return perpih;
	}

	return NULL;
}

/*
 * sunxi_clk_get_periph_cpus_by_name() - Get periph clk init config
 */
struct periph_init_data *sunxi_clk_get_periph_cpus_by_name(const char *name)
{
	struct periph_init_data *perpih;
	int i;

	for (i = 0; i < ARRAY_SIZE(sunxi_periphs_cpus_init); i++) {
		perpih = &sunxi_periphs_cpus_init[i];
		if (strcmp(name, perpih->name))
			continue;
		return perpih;
	}

	return NULL;
}

/*
 * sunxi_clk_get_periph_rtc_by_name() - Get periph clk init config
 */
struct periph_init_data *sunxi_clk_get_periph_rtc_by_name(const char *name)
{
	struct periph_init_data *perpih;
	int i;

	for (i = 0; i < ARRAY_SIZE(sunxi_periphs_rtc_init); i++) {
		perpih = &sunxi_periphs_rtc_init[i];
		if (strcmp(name, perpih->name))
			continue;
		return perpih;
	}

	return NULL;
}
struct periph_init_data *sunxi_cpus_clk_get_periph_by_name(const char *name)
{
	struct periph_init_data *perpih;
	int i;

	for (i = 0; i < ARRAY_SIZE(sunxi_periphs_cpus_init); i++) {
		perpih = &sunxi_periphs_cpus_init[i];
		if (strcmp(name, perpih->name))
			continue;
		return perpih;
	}

	return NULL;
}

void __init sunxi_clocks_init(struct device_node *node)
{
	sunxi_clk_base = of_iomap(node, 0);
	sunxi_clk_cpus_base = of_iomap(node, 1);
	sunxi_clk_rtc_base = of_iomap(node, 2);
/*
	sunxi_clk_periph_losc_out.gate.bus = of_iomap(node, 2) + LOSC_OUT_GATE;
	sunxi_clk_periph_dcxo_out.gate.enable = of_iomap(node, 2) +
								DCXO_OUT_CFG;
*/
	sunxi_clk_periph_r_dma.gate.dram = sunxi_clk_base + MBUS_GATE;
	sunxi_clk_periph_r_dma.gate.reset = sunxi_clk_base + CPUS_DMA_GATE;
	sunxi_clk_periph_r_dma.gate.bus = sunxi_clk_base + CPUS_DMA_GATE;
	sunxi_set_clk_priv_ops("dcxo_out", &dcxo_out_priv_ops,
						set_dcxo_out_priv_ops);
	/*do some initialize arguments here*/
	sunxi_clk_factor_initlimits();
}
void __init sunxi_cpu_clocks_init(struct device_node *node) {}
