/*
 * dspo_device.h
 *
 * Copyright (c) 2007-2021 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef _DSPO_DEVICE_H
#define _DSPO_DEVICE_H

#include <linux/types.h>
#include <asm/unistd.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_iommu.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/dma-buf.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>
#include <linux/fs.h>
#include <linux/atomic.h>
#include <linux/dspo_drv.h>
#include "dspo_reg.h"
#include "dspo_param.h"
#include "dspo_timing.h"

struct dspo_device_t;

struct dspo_health_info_t {
	unsigned long sync_time[100];	/* for_debug */
	unsigned int sync_time_index;	/* for_debug */
	u64 irq_cnt;
	u32 err_cnt;
	u32 realtime_fps;
};


struct dspo_device_t {
	int (*enable)(struct dspo_device_t *p_dev, struct dspo_config_t *p_cfg);
	int (*disable)(struct dspo_device_t *p_dev);
	struct dspo_config_t * (*get_config)(struct dspo_device_t *p_dev);
	bool (*mode_support) (struct dspo_device_t *p_dev, enum dspo_output_mode mode);
	bool (*is_enable)(struct dspo_device_t *p_dev);
	int (*commit)(struct dspo_device_t *p_dev, struct dspo_dma_info_t *p_info);
	int (*get_health_info)(struct dspo_device_t *p_dev, struct dspo_health_info_t *p_info);

	struct device *dev;
	struct dspo_config_t cfg;
	bool enabled;
	void __iomem *io;
	u32 irq;
	struct clk *clk;
	struct clk *clk_parent;
	struct mutex dev_en_mutex;
	wait_queue_head_t queue;
	bool dma_finish_flag;
	struct list_head dmabuf_list;
	u32 dmabuf_cnt;
	struct dspo_health_info_t health;
};

struct dspo_device_t *create_dspo_device(struct device *p_dev);
int destroy_dspo_device(struct dspo_device_t *p_dev);

#endif /*End of file*/
