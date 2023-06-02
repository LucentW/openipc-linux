/*
 * combo csi module
 *
 * Copyright (c) 2019 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zheng Zequn <zequnzheng@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "combo_csi_reg_i.h"
#include "combo_csi_reg.h"
#include "../../utility/vin_io.h"
#include "../../platform/platform_cfg.h"

volatile void *cmb_csi_base_addr[VIN_MAX_MIPI];

int cmb_csi_set_base_addr(unsigned int sel, unsigned long addr)
{
	if (sel > VIN_MAX_MIPI - 1)
		return -1;
	cmb_csi_base_addr[sel] = (volatile void *)addr;

	return 0;
}

void cmb_phy_top_enable(unsigned int sel)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_TOP_REG_OFF,
		CMB_PHY_RSTN_MASK, 1 << CMB_PHY_RSTN);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_TOP_REG_OFF,
		CMB_PHY_PWDNZ_MASK, 1 << CMB_PHY_PWDNZ);
}

void cmb_phy_top_disable(unsigned int sel)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_TOP_REG_OFF,
		CMB_PHY_PWDNZ_MASK, 0 << CMB_PHY_PWDNZ);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_TOP_REG_OFF,
		CMB_PHY_RSTN_MASK, 0 << CMB_PHY_RSTN);
}

void cmb_phy_power_enable(unsigned int sel)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_TOP_REG_OFF,
		CMB_PHY_VREF_EN_MASK, 1 << CMB_PHY_VREF_EN);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_TOP_REG_OFF,
		CMB_PHY_LVLDO_EN_MASK, 1 << CMB_PHY_LVLDO_EN);
}

void cmb_phy_power_disable(unsigned int sel)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_TOP_REG_OFF,
		CMB_PHY_VREF_EN_MASK, 0 << CMB_PHY_VREF_EN);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_TOP_REG_OFF,
		CMB_PHY_LVLDO_EN_MASK, 0 << CMB_PHY_LVLDO_EN);
}

void cmb_phy0_en(unsigned int sel, unsigned int en)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_CTL_REG_OFF,
		CMB_PHY0_EN_MASK, en << CMB_PHY0_EN);
}

void cmb_phy_lane_num_enable(unsigned int sel)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_CTL_REG_OFF,
		CMB_PHY_LANEDT_EN_MASK, 0xf << CMB_PHY_LANEDT_EN);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_CTL_REG_OFF,
		CMB_PHY_LANECK_EN_MASK, 0x1 << CMB_PHY_LANECK_EN);
}

void cmb_phy0_ibias_en(unsigned int sel, unsigned int en)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_CTL_REG_OFF,
		CMB_PHY0_IBIAS_EN_MASK, en << CMB_PHY0_IBIAS_EN);
}

void cmb_phy0_term_dly(unsigned int sel, unsigned int dly)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_TERM_CTL_REG_OFF,
		CMB_PHY0_TERM_EN_DLY_MASK, dly << CMB_PHY0_TERM_EN_DLY);
}

void cmb_phy0_hs_dly(unsigned int sel, unsigned int dly)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_HS_CTL_REG_OFF,
		CMB_PHY0_HS_DLY_MASK, dly << CMB_PHY0_HS_DLY);
}

void cmb_phy0_s2p_width(unsigned int sel, unsigned int width)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_S2P_CTL_REG_OFF,
		CMB_PHY0_S2P_WIDTH_MASK, width << CMB_PHY0_S2P_WIDTH);
}

void cmb_phy0_s2p_dly(unsigned int sel, unsigned int dly)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_S2P_CTL_REG_OFF,
		CMB_PHY0_S2P_DLY_MASK, dly << CMB_PHY0_S2P_DLY);
}

void cmb_phy_mipi_lpnum_enable(unsigned int sel)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_MIPIRX_CTL_REG_OFF,
		CMB_PHY_MIPI_MPDT_EN_MASK, 0xf << CMB_PHY_MIPI_MPDT_EN);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_MIPIRX_CTL_REG_OFF,
		CMB_PHY_MIPI_LPCK_EN_MASK, 0x1 << CMB_PHY_MIPI_LPCK_EN);
}

void cmb_phy0_mipilp_dbc_en(unsigned int sel, unsigned int en)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PHY_MIPIRX_CTL_REG_OFF,
		CMB_PHY0_MIPILP_DBC_EN_MASK, en << CMB_PHY0_MIPILP_DBC_EN);
}

void cmb_port_enable(unsigned int sel)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_CTL_REG_OFF,
		CMB_PORT_EN_MASK, 1 << CMB_PORT_EN);
}

void cmb_port_disable(unsigned int sel)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_CTL_REG_OFF,
		CMB_PORT_EN_MASK, 0 << CMB_PORT_EN);
}

void cmb_port_lane_num(unsigned int sel, unsigned int num)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_CTL_REG_OFF,
		CMB_PORT_LANE_NUM_MASK, num << CMB_PORT_LANE_NUM);
}

void cmb_port_out_num(unsigned int sel, unsigned int num)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_CTL_REG_OFF,
		CMB_PORT_OUT_NUM_MASK, num << CMB_PORT_OUT_NUM);
}

void cmb_port_lane_map(unsigned int sel)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_LANE_MAP_REG0_OFF,
		CMB_PORT_LANE0_ID_MASK, 0x0 << CMB_PORT_LANE0_ID);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_LANE_MAP_REG0_OFF,
		CMB_PORT_LANE1_ID_MASK, 0x1 << CMB_PORT_LANE1_ID);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_LANE_MAP_REG0_OFF,
		CMB_PORT_LANE2_ID_MASK, 0x2 << CMB_PORT_LANE2_ID);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_LANE_MAP_REG0_OFF,
		CMB_PORT_LANE3_ID_MASK, 0x3 << CMB_PORT_LANE3_ID);
}

void cmb_port_mipi_enpack_enable(unsigned int sel)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_CFG_REG_OFF,
		CMB_MIPI_UNPACK_EN_MASK, 1 << CMB_MIPI_UNPACK_EN);
}

void cmb_port_mipi_enpack_disable(unsigned int sel)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_CFG_REG_OFF,
		CMB_MIPI_UNPACK_EN_MASK, 0 << CMB_MIPI_UNPACK_EN);
}

void cmb_port_mipi_yuv_seq(unsigned int sel, unsigned int seq)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_CFG_REG_OFF,
		CMB_MIPI_YUV_SEQ_MASK, seq << CMB_MIPI_YUV_SEQ);
}

void cmb_port_mipi_ch0_dt(unsigned int sel, unsigned int type)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_REG_OFF,
		CMB_MIPI_CH0_DT_MASK, type << CMB_MIPI_CH0_DT);
}

void cmb_port_mipi_ch_trig_en(unsigned int sel, unsigned int en)
{
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_FS_MASK, en << CMB_MIPI_FS);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_FE_MASK, en << CMB_MIPI_FE);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_LS_MASK, en << CMB_MIPI_LS);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_LE_MASK, en << CMB_MIPI_LE);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_GS0_MASK, en << CMB_MIPI_GS0);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_GS1_MASK, en << CMB_MIPI_GS1);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_GS2_MASK, en << CMB_MIPI_GS2);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_GS3_MASK, en << CMB_MIPI_GS3);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_GS4_MASK, en << CMB_MIPI_GS4);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_GS5_MASK, en << CMB_MIPI_GS5);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_GS6_MASK, en << CMB_MIPI_GS6);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_GS7_MASK, en << CMB_MIPI_GS7);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_GL_MASK, en << CMB_MIPI_GL);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_YUV_MASK, en << CMB_MIPI_YUV);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_RGB_MASK, en << CMB_MIPI_RGB);
	vin_reg_clr_set(cmb_csi_base_addr[sel] + CMB_PORT_MIPI_DI_TRIG_REG_OFF,
		CMB_MIPI_RAW_MASK, en << CMB_MIPI_RAW);
}

