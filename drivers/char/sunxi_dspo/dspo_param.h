/*
 * dspo_param.h
 *
 * Copyright (c) 2007-2021 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
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
#ifndef _DSPO_PARAM_H
#define _DSPO_PARAM_H

#include "linux/kernel.h"
#include "linux/mm.h"
#include <asm/uaccess.h>
#include <asm/memory.h>
#include <asm/unistd.h>
#include "linux/semaphore.h"
#include <linux/vmalloc.h>
#include <linux/types.h>
#include <linux/fb.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/delay.h>
#include "asm-generic/int-ll64.h"
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/string.h>

//interrupt define
#define DSPO_V_INT            0x1
#define DSPO_L_INT             0x2
#define DSPO_DMA_DONE_INT      0x4
#define DSPO_DMA_DESC_INT     0x8
#define DSPO_FRAME_DONE_INT   0x10
#define DSPO_LINE_DONE_INT     0x20
#define DSPO_UV_LINE_DONE_INT  0x40
#define DSPO_UV_BUF_UF_INT     0x80
#define DSPO_UV_BUF_OF_INT     0x100
#define DSPO_Y_BUF_UF_INT      0x200
#define DSPO_Y_BUF_OF_INT      0x400

//program define

//dma or aidb select define
#define DMA_DATA_PATH     0
#define AIDB_DATA_PATH    1

//data seq define
//                        bt656       bt1120
#define CbYCrY    0x0     //pass      pass
#define CrYCbY    0x1     //pass      pass
#define YCbYCr    0x2     //pass      pass
#define YCrYCb    0x3     //pass      pass

//embedded sync fmt define
#define Embedded_SYNC_BT1120   0     //pass
#define Embedded_SYNC_BT656    1     //pass

//data src define
#define YUV444_TO_PG       0     //pass
#define YUV422_TO_PG       1     //pass

//progress or interlace define
#define PROGRESS      0     //pass
#define INTERLACE     1

//output data width define
#define DATA_8Bit     0      //pass
#define DATA_16Bit    1      //pass

//interlace mode option define
#define INTER_BT1120   0     //pass
#define INTER_BT656    1     //pass

//sync signal polarity
//                              bt1120    bt656
#define HIGH_ACTIVE    0        //pass    pass
#define LOW_ACTIVE     1        //pass    pass

//clk invert bt1120 pass;bt656 pass


//dma descriptor mode define
#define DRAM_GET_MODE     0
#define REG_SET_MODE      1     //pass

//dspo work mode define
#define Normal_Mode       0     //pass
#define Tri_Mode          1     //pass

//dma block size select define
#define BYTES_256      0x0
#define BYTES_512      0x1
#define BYTES_1024     0x2
#define BYTES_2048     0x3

//dma input data type define
//                                   bt656       bt1120
#define ARGB888          0x0         //pass      pass
#define YUV444           0x1         //pass      pass
#define YUV422_YUYV      0x4         //pass      pass
#define YUV422_UYVY      0x5         //pass      pass
#define YUV422_YVYU      0x6         //pass      pass
#define YUV422_VYUY      0x7         //pass      pass
#define YUV422_Y_UVUV    0x8         //pass      pass
#define YUV422_Y_VUVU    0xa         //pass      pass
#define YUV420_Y_UVUV    0xc         //pass      pass
#define YUV420_Y_VUVU    0xe         //pass      pass

struct csi_dma_des {
    u32 config;
    u32 pkt_type;
    u32 source_format;
    u32 buff0_addr;
    u32 buff1_addr;
    u32 buff0_line_stride;
    u32 buff1_line_stride;
    u32 next_des_addr;
    //	struct csi_dma_des *virt_next;	/*Next CPU virtual structure address*/
};

#endif /* DSPO_PARAM_H_ */
