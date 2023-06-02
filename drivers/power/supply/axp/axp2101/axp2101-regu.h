/*
 * drivers/power/axp/axp2101/axp2101-regu.h
 * (C) Copyright 2010-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Pannan <pannan@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#ifndef AXP2101_REGULATOR_H
#define AXP2101_REGULATOR_H

enum {
	AXP2101_ID_LDO1,  /* RTCLDO */
	AXP2101_ID_LDO2,  /* RTCLDO1 */
	AXP2101_ID_LDO3,  /* ALDO1 */
	AXP2101_ID_LDO4,  /* ALDO2 */
	AXP2101_ID_LDO5,  /* ALDO3 */
	AXP2101_ID_LDO6,  /* ALDO4 */
	AXP2101_ID_LDO7,  /* BLDO1 */
	AXP2101_ID_LDO8,  /* BLDO2 */
	AXP2101_ID_LDO9,  /* DLDO1 */
	AXP2101_ID_LDO10, /* DLDO2 */
	AXP2101_ID_LDO11, /* CPULDOS */
	AXP2101_ID_DCDC1 = AXP_DCDC_ID_START,
	AXP2101_ID_DCDC2,
	AXP2101_ID_DCDC3,
	AXP2101_ID_DCDC4,
	AXP2101_ID_DCDC5,
};

enum AXP2101_POWER_LDO {
	AXP2101_DCDC1_BIT = 1U << 0,
	AXP2101_DCDC2_BIT = 1U << 1,
	AXP2101_DCDC3_BIT = 1U << 2,
	AXP2101_DCDC4_BIT = 1U << 3,
	AXP2101_ALDO1_BIT = 1U << 4,
	AXP2101_ALDO2_BIT = 1U << 5,
	AXP2101_ALDO3_BIT = 1U << 6,
	AXP2101_ALDO4_BIT = 1U << 7,
	AXP2101_BLDO1_BIT = 1U << 8,
	AXP2101_BLDO2_BIT = 1U << 9,
	AXP2101_DLDO1_BIT = 1U << 10,
	AXP2101_DLDO2_BIT = 1U << 11,
	AXP2101_CPULDOS_BIT = 1U << 12,
	AXP2101_RTC1_BIT = 1U << 13,
	AXP2101_RTC_BIT = 1U << 14,
	AXP2101_DCDC5_BIT = 1U << 15,
};

#define AXP2101_DCDC1         axp2101_DCDC1_CFG
#define AXP2101_DCDC2         axp2101_DCDC2_CFG
#define AXP2101_DCDC3         axp2101_DCDC3_CFG
#define AXP2101_DCDC4         axp2101_DCDC4_CFG
#define AXP2101_DCDC5         axp2101_DCDC5_CFG
#define AXP2101_LDO1          axp2101_COMM_STAT0
#define AXP2101_LDO2          axp2101_COMM_STAT0
#define AXP2101_LDO3          axp2101_ALDO1_CFG
#define AXP2101_LDO4          axp2101_ALDO2_CFG
#define AXP2101_LDO5          axp2101_ALDO3_CFG
#define AXP2101_LDO6          axp2101_ALDO4_CFG
#define AXP2101_LDO7          axp2101_BLDO1_CFG
#define AXP2101_LDO8          axp2101_BLDO2_CFG
#define AXP2101_LDO9          axp2101_CPUSLD_CFG
#define AXP2101_LDO10         axp2101_DLDO1_CFG
#define AXP2101_LDO11         axp2101_DLDO2_CFG

#define AXP2101_DCDC1EN       axp2101_DCDC_CFG0
#define AXP2101_DCDC2EN       axp2101_DCDC_CFG0
#define AXP2101_DCDC3EN       axp2101_DCDC_CFG0
#define AXP2101_DCDC4EN       axp2101_DCDC_CFG0
#define AXP2101_DCDC5EN       axp2101_DCDC_CFG0
#define AXP2101_DCDCMODE      axp2101_DCDC_CFG1
#define AXP2101_LDO1EN        axp2101_COMM_STAT0
#define AXP2101_LDO2EN        axp2101_COMM_STAT0
#define AXP2101_LDO3EN        axp2101_LDO_EN_CFG0
#define AXP2101_LDO4EN        axp2101_LDO_EN_CFG0
#define AXP2101_LDO5EN        axp2101_LDO_EN_CFG0
#define AXP2101_LDO6EN        axp2101_LDO_EN_CFG0
#define AXP2101_LDO7EN        axp2101_LDO_EN_CFG0
#define AXP2101_LDO8EN        axp2101_LDO_EN_CFG0
#define AXP2101_LDO9EN        axp2101_LDO_EN_CFG0
#define AXP2101_LDO10EN       axp2101_LDO_EN_CFG0
#define AXP2101_LDO11EN       axp2101_LDO_EN_CFG1

#define AXP2101_BUCKMODE      axp2101_DCDC_CFG1


#endif /* AXP2101_REGULATOR_H */
