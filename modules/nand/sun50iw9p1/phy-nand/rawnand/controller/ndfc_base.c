/* SPDX-License-Identifier: GPL-2.0 */
/*
 ************************************************************************************************************************
 *                                                      eNand
 *                                           Nand flash driver scan module
 *
 *                             Copyright(C), 2008-2009, SoftWinners Microelectronic Co., Ltd.
 *                                                  All Rights Reserved
 *
 * File Name : nand_chip_op.c
 *
 * Author :
 *
 * Version : v0.1
 *
 * Date : 2013-11-20
 *
 * Description :
 *
 * Others : None at present.
 *
 *
 *
 ************************************************************************************************************************
 */
#define _NC_C_

#include "ndfc_base.h"
#include "../../../nfd/nand_osal_for_linux.h"
#include "../../nand_errno.h"
#include "../../nand_nftl.h"
#include "../rawnand.h"
#include "../rawnand_chip.h"
#include "../rawnand_cfg.h"
#include "../rawnand_debug.h"
#include "../../version.h"
#include "ndfc_internal.h"
#include "ndfc_ops.h"
#include "../rawnand.h"
#include <asm/io.h>
#include <linux/sched.h>

#define RANDOM_SEED_DEFAULT (0x4a80)
struct nand_controller_info *g_nctri;
/*struct nand_controller_info g_nctri_data[2] = {0};*/

int ndfc_ver;
const u32 default_random_seed = 0x4a80;
const u32 random_seed[128] = {
    //0        1      2       3        4      5        6       7       8       9
    0x2b75, 0x0bd0, 0x5ca3, 0x62d1, 0x1c93, 0x07e9, 0x2162, 0x3a72, 0x0d67, 0x67f9,
    0x1be7, 0x077d, 0x032f, 0x0dac, 0x2716, 0x2436, 0x7922, 0x1510, 0x3860, 0x5287,
    0x480f, 0x4252, 0x1789, 0x5a2d, 0x2a49, 0x5e10, 0x437f, 0x4b4e, 0x2f45, 0x216e,
    0x5cb7, 0x7130, 0x2a3f, 0x60e4, 0x4dc9, 0x0ef0, 0x0f52, 0x1bb9, 0x6211, 0x7a56,
    0x226d, 0x4ea7, 0x6f36, 0x3692, 0x38bf, 0x0c62, 0x05eb, 0x4c55, 0x60f4, 0x728c,
    0x3b6f, 0x2037, 0x7f69, 0x0936, 0x651a, 0x4ceb, 0x6218, 0x79f3, 0x383f, 0x18d9,
    0x4f05, 0x5c82, 0x2912, 0x6f17, 0x6856, 0x5938, 0x1007, 0x61ab, 0x3e7f, 0x57c2,
    0x542f, 0x4f62, 0x7454, 0x2eac, 0x7739, 0x42d4, 0x2f90, 0x435a, 0x2e52, 0x2064,
    0x637c, 0x66ad, 0x2c90, 0x0bad, 0x759c, 0x0029, 0x0986, 0x7126, 0x1ca7, 0x1605,
    0x386a, 0x27f5, 0x1380, 0x6d75, 0x24c3, 0x0f8e, 0x2b7a, 0x1418, 0x1fd1, 0x7dc1,
    0x2d8e, 0x43af, 0x2267, 0x7da3, 0x4e3d, 0x1338, 0x50db, 0x454d, 0x764d, 0x40a3,
    0x42e6, 0x262b, 0x2d2e, 0x1aea, 0x2e17, 0x173d, 0x3a6e, 0x71bf, 0x25f9, 0x0a5d,
    0x7c57, 0x0fbe, 0x46ce, 0x4939, 0x6b17, 0x37bb, 0x3e91, 0x76db};
const u32 random_seed_new[67] = {
    //0              1           2           3           4          5           6           7           8           9           10          11          12          13          14          15
    0xab11cc9b, 0x2e1da61b, 0xf8372be7, 0xf2cdf49f, 0xd3d22f7f, 0x5be09d1c, 0xf98e137e, 0x6f670345, 0xd91b5d72, 0xedec5375, 0x8a50e1d2, 0x167637db, 0x3988baed, 0x5075b36e, 0x27ea9ab7, 0x49d5a266,
    0x948e296e, 0xbfd518f3, 0xdf01ec4a, 0x686e1bb1, 0xfa7ee2c9, 0x7397c076, 0xd275da40, 0xcf428cef, 0xe1ab245b, 0x57e26a5e, 0x2ac04837, 0x99a115a6, 0xd3c92d47, 0x196d8a26, 0x9a1a1aa9, 0xb8259e70,
    0x547badc1, 0xd3ed8ca8, 0xa34a4e94, 0xe8bc32f0, 0x3063210f, 0x4a5808d8, 0x94772728, 0x7d832e2b, 0xbe1626a2, 0xf046bea7, 0x8c451487, 0xe8bcd15f, 0x555fdde8, 0x86006513, 0x4df771ed, 0xe391fbee,
    0xdbc1fd98, 0x164cfd33, 0x2eae0831, 0x57077929, 0x72cfee1f, 0x7b2a7fcc, 0x39d47304, 0xafa49758, 0xc9c5e5a3, 0x45a5fa49, 0xe544e572, 0x7e8f78a6, 0x42074860, 0x6a183bbc, 0x4d797600, 0x03be9b49,
    0xb80dfafb, 0xf2dad720, 0x9aaaa2c7};

const u8 rand_factor_1st_spare_data[][128] = {
	{ /*0 index*/
		0xf5, 0xf1, 0xa4, 0x74, 0xd2, 0x8b, 0x85, 0xb6, 0x5c, 0xd6,
		0x7d, 0x69, 0x75, 0x81, 0xf3, 0x82, 0x87, 0x2a, 0x49, 0x25,
		0xa8, 0xa9, 0x3e, 0xd9, 0xbf, 0xec, 0x2e, 0x34, 0x85, 0x00,
		0x2a, 0x07, 0x5d, 0x92, 0x8f, 0x55, 0xba, 0x82, 0x2e, 0x4e,
		0xeb, 0x0c, 0x44, 0x5d, 0x1a, 0xc6, 0x79, 0xb2, 0x9f, 0x92,
		0x27, 0x24, 0xe3, 0xc1, 0xa5, 0x0c, 0xe8, 0x10, 0x76, 0xc5,
		0xe3, 0x7f, 0x69, 0x9f, 0x65, 0xfd, 0x9a, 0x79, 0x95, 0xef,
		0x81, 0xcc, 0x16, 0xdb, 0x14, 0x07, 0x91, 0x76, 0x53, 0x9e,
		0x9f, 0xb7, 0xfb, 0x94, 0x93, 0xdd, 0xbb, 0xc8, 0x47, 0x0e,
		0x0e, 0x33, 0x9e, 0x40, 0x8d, 0x68, 0xf1, 0xf5, 0xcf, 0x21,
		0xeb, 0x79, 0xac, 0x4d, 0x2a, 0xe2, 0x28, 0xac, 0x6e, 0x96,
		0x50, 0x79, 0x9c, 0xe1, 0x26, 0xc7, 0x3e, 0x6f, 0x05, 0x01,
		0x5b, 0x7e, 0x2b, 0xa5, 0xf9, 0x59, 0x10, 0xcd
	},
	{ /*1 index*/
		0xac, 0xb5, 0x63, 0x52, 0xab, 0x77, 0xf2, 0xb2, 0x6a, 0xf0,
		0xf5, 0xe7, 0xd3, 0x9e, 0x0c, 0xb9, 0x67, 0x84, 0xec, 0x99,
		0x56, 0xc0, 0x86, 0xfb, 0x4d, 0x55, 0xe7, 0x73, 0xd1, 0x4d,
		0xa3, 0x7e, 0xc4, 0xeb, 0x52, 0xc2, 0xd3, 0xfd, 0xaa, 0xba,
		0x75, 0x45, 0x68, 0x9b, 0xd8, 0xcc, 0x83, 0xb7, 0x41, 0x8e,
		0x6b, 0x56, 0x01, 0x87, 0xc4, 0x13, 0x13, 0x59, 0x88, 0xe0,
		0x54, 0x5b, 0xa2, 0x50, 0xf6, 0xb6, 0xbe, 0x85, 0xe2, 0x14,
		0x8a, 0x1b, 0xcb, 0x64, 0x05, 0x4f, 0x85, 0xb5, 0x68, 0x8d,
		0x25, 0x38, 0x64, 0x30, 0x46, 0xed, 0x29, 0x0b, 0x3f, 0xc9,
		0x8c, 0x79, 0xbc, 0x58, 0xb9, 0x3e, 0xca, 0xf1, 0x57, 0x91,
		0x64, 0xb5, 0x15, 0xd8, 0xdf, 0xc7, 0x65, 0x6e, 0x99, 0xeb,
		0x04, 0x81, 0x60, 0x86, 0xc6, 0x42, 0xa7, 0x48, 0x87, 0x96,
		0x14, 0xc0, 0x06, 0xd7, 0xd3, 0xd6, 0x44, 0xbc
	},
	{ /*2 index*/
		0x47, 0x84, 0x3b, 0xa7, 0x1d, 0x27, 0xe3, 0xf6, 0xb9, 0x5e,
		0xe1, 0x6e, 0x67, 0xe0, 0x45, 0xa1, 0x22, 0x1f, 0x36, 0xdb,
		0xfe, 0x7e, 0x90, 0x1a, 0xb0, 0x8d, 0x5c, 0x57, 0xa3, 0xc0,
		0x5f, 0x82, 0x39, 0x2d, 0xe4, 0xbf, 0x33, 0xa1, 0x9c, 0xb4,
		0x8f, 0xc5, 0x33, 0x79, 0x0b, 0x52, 0x62, 0x35, 0xa8, 0xed,
		0x5a, 0x9b, 0x89, 0x10, 0x7b, 0x45, 0x0e, 0xcc, 0x26, 0x53,
		0x49, 0x60, 0xae, 0x68, 0xab, 0xc1, 0xeb, 0xe2, 0xef, 0x4c,
		0xe0, 0x15, 0x4e, 0x5b, 0xcf, 0x42, 0xac, 0xe6, 0x3d, 0xa8,
		0xa8, 0x76, 0x43, 0x6f, 0xed, 0x99, 0xb3, 0x16, 0x72, 0xc4,
		0x04, 0xd5, 0x68, 0x30, 0xa5, 0xae, 0xc4, 0x87, 0x14, 0xd8,
		0x4f, 0xe2, 0xbd, 0x35, 0x5f, 0x09, 0xde, 0xfd, 0xec, 0x2e,
		0x3c, 0xe2, 0x69, 0xc8, 0x9a, 0xd2, 0x50, 0x2c, 0x43, 0x80,
		0x3b, 0x20, 0x9f, 0x3b, 0x02, 0xba, 0x0c, 0x55
	},
	{ /*3 index*/
		0x3d, 0x77, 0x69, 0x7d, 0xbf, 0x66, 0x05, 0xf5, 0xef, 0xc4,
		0x87, 0xca, 0x1d, 0x68, 0xc5, 0xb2, 0xaa, 0x23, 0xcd, 0x2a,
		0xbe, 0xd0, 0x62, 0xc3, 0x35, 0xff, 0x4a, 0x65, 0x1c, 0x35,
		0x39, 0xa0, 0x93, 0x8f, 0x3d, 0x11, 0x1d, 0x81, 0x7f, 0x73,
		0x67, 0xf3, 0x6e, 0xab, 0x1a, 0xd5, 0xe1, 0xb6, 0x30, 0xa4,
		0xaf, 0x7e, 0xc0, 0x62, 0x13, 0xcd, 0x8d, 0x3a, 0xe6, 0x08,
		0xff, 0x3b, 0xf9, 0x3c, 0x06, 0xb6, 0x30, 0xe3, 0x09, 0x0f,
		0x67, 0xcb, 0xd7, 0x6b, 0x43, 0xb4, 0x63, 0xf7, 0xee, 0x65,
		0x1b, 0x92, 0x6b, 0x54, 0xf2, 0x8d, 0x5e, 0x87, 0x90, 0x56,
		0x65, 0xe2, 0x71, 0x3a, 0xb2, 0x90, 0x57, 0x04, 0x3e, 0x6c,
		0x6b, 0xf7, 0xcf, 0x9a, 0x18, 0x92, 0xab, 0xec, 0x6a, 0xcf,
		0x03, 0xe0, 0xe8, 0x62, 0xd2, 0xb1, 0x7a, 0x36, 0x22, 0x6e,
		0x4f, 0x50, 0x42, 0x1e, 0xdd, 0xde, 0x33, 0xb1
	},
	{ /*4 index*/
		0xf2, 0x23, 0xd3, 0xba, 0x49, 0x9a, 0x89, 0x86, 0x32, 0x38,
		0x08, 0xac, 0xea, 0x48, 0xf3, 0xf8, 0x99, 0x48, 0xd6, 0xdb,
		0xc0, 0x20, 0xec, 0x4b, 0xb4, 0x25, 0xb9, 0xfe, 0x79, 0x90,
		0xf8, 0x61, 0x52, 0x5d, 0x8b, 0xb0, 0xd5, 0xb8, 0x29, 0x37,
		0x24, 0x13, 0x95, 0x62, 0x87, 0xfd, 0xe9, 0x97, 0x7e, 0x4d,
		0x7b, 0xeb, 0x66, 0x8c, 0x63, 0xf3, 0xc4, 0xd5, 0x9a, 0x3d,
		0x76, 0x68, 0xbc, 0x2e, 0xff, 0xd0, 0x4f, 0x09, 0x8c, 0x75,
		0x08, 0x4f, 0x74, 0x7b, 0x14, 0x31, 0x3d, 0x0a, 0x91, 0xbe,
		0x3e, 0xa6, 0x71, 0x2c, 0xcd, 0xaa, 0xf5, 0x4e, 0x25, 0xd3,
		0xc3, 0xdf, 0xee, 0x94, 0xfb, 0x7c, 0x13, 0x62, 0x8f, 0x5a,
		0x74, 0x09, 0x31, 0x97, 0x38, 0x86, 0x18, 0x41, 0xcd, 0x5c,
		0x51, 0x49, 0x2e, 0xd6, 0xeb, 0x9d, 0xbc, 0x9d, 0xb1, 0xe0,
		0x53, 0x18, 0xe8, 0x93, 0xc1, 0xf3, 0x45, 0xff
	},
	{ /*5 index*/
		0x91, 0x66, 0x6e, 0xa1, 0xb0, 0xaa, 0xc3, 0xc7, 0xcc, 0x53,
		0x62, 0x57, 0x89, 0x2e, 0x13, 0x75, 0xbf, 0x19, 0xd5, 0x5f,
		0x70, 0x5c, 0x29, 0x11, 0x17, 0x80, 0xf7, 0xab, 0xc9, 0x17,
		0x12, 0xb8, 0xed, 0xa4, 0x51, 0x0c, 0xc9, 0x60, 0xe0, 0x65,
		0x2a, 0x05, 0xec, 0xff, 0x4b, 0x9f, 0x88, 0x36, 0x94, 0xbb,
		0x3c, 0x60, 0xd0, 0x29, 0x4d, 0x15, 0x65, 0xd3, 0xca, 0xc6,
		0xc0, 0x13, 0x42, 0x91, 0x42, 0x76, 0x54, 0x89, 0x06, 0xc4,
		0x2a, 0x17, 0x5e, 0x6f, 0x31, 0xb7, 0xe9, 0xc6, 0x8c, 0xab,
		0x8b, 0xed, 0xef, 0x3f, 0x85, 0xe5, 0xf8, 0xe2, 0xac, 0x7e,
		0x6b, 0x09, 0xa4, 0x13, 0x35, 0x6c, 0x7e, 0x83, 0x50, 0xad,
		0x2f, 0x86, 0x94, 0x2b, 0x0a, 0xed, 0x7f, 0x8d, 0xef, 0x54,
		0xc1, 0x88, 0xce, 0xa9, 0x1d, 0xb4, 0x23, 0xd6, 0xd9, 0x2c,
		0x74, 0x3c, 0x31, 0x48, 0x99, 0x18, 0xd5, 0x34
	},
	{ /*6 index*/
		0x85, 0x99, 0xdd, 0xb3, 0x36, 0xeb, 0x26, 0x22, 0x15, 0x52,
		0x86, 0x3d, 0x8f, 0xb6, 0x05, 0x82, 0x2a, 0xf6, 0x9e, 0x1b,
		0x50, 0x18, 0x8d, 0xf7, 0x37, 0x1b, 0x32, 0x00, 0xe2, 0x2c,
		0xc2, 0x28, 0xfd, 0x39, 0xa7, 0x74, 0x9f, 0x72, 0x1e, 0xd6,
		0x9b, 0xcd, 0x6f, 0x69, 0x22, 0x01, 0x4e, 0xee, 0x20, 0x75,
		0x23, 0x4f, 0x2a, 0xa5, 0xe9, 0x85, 0x93, 0x1f, 0xeb, 0x91,
		0x26, 0x6e, 0xf1, 0xdc, 0xc0, 0xdc, 0x34, 0xc6, 0xe5, 0x27,
		0x86, 0x74, 0xa7, 0x63, 0xcf, 0x54, 0xd1, 0x87, 0x6c, 0x30,
		0x50, 0xba, 0x64, 0x5d, 0x95, 0xbf, 0x47, 0xb4, 0x1b, 0xdd,
		0x11, 0x98, 0x4c, 0x2f, 0x83, 0x21, 0x8d, 0x69, 0x64, 0xfb,
		0x67, 0x86, 0x14, 0x2e, 0x92, 0xa2, 0x4a, 0xf0, 0x15, 0x39,
		0xfc, 0x36, 0x9c, 0x9e, 0x8f, 0x69, 0x31, 0xe9, 0xb4, 0x48,
		0x3d, 0x0a, 0x8e, 0x6d, 0x90, 0x45, 0xf3, 0x40
	},
	{ /*7 index*/
		0xac, 0xea, 0xec, 0x38, 0xf4, 0x3f, 0xd1, 0xd2, 0x95, 0xbd,
		0xa9, 0xfe, 0x26, 0x9c, 0xcd, 0xa7, 0xf0, 0x8a, 0xdf, 0x78,
		0x24, 0x39, 0xde, 0x4c, 0x4e, 0x20, 0xc6, 0x7f, 0xd6, 0x0e,
		0x8d, 0x72, 0x8d, 0xbb, 0x7c, 0x05, 0x16, 0xa8, 0xc8, 0xab,
		0x5f, 0xc3, 0x0d, 0x80, 0xb7, 0xa8, 0xe6, 0x96, 0x6f, 0xb3,
		0x51, 0x68, 0xdc, 0xde, 0xf5, 0xcf, 0x6b, 0x1d, 0x17, 0x92,
		0xd0, 0x8d, 0xf1, 0x6c, 0x31, 0x26, 0x3f, 0xe6, 0xc2, 0x13,
		0x9f, 0x0e, 0x78, 0x6c, 0x54, 0x76, 0x8e, 0x12, 0x65, 0x7f,
		0x67, 0xcd, 0x4c, 0xd0, 0xa3, 0x0b, 0x02, 0x49, 0x3d, 0xe0,
		0xef, 0x06, 0x7b, 0x4d, 0x57, 0xed, 0xe0, 0xa1, 0x3c, 0x3d,
		0x5c, 0xe2, 0x6f, 0x9f, 0x87, 0xcd, 0xa0, 0x65, 0x8c, 0xff,
		0x50, 0xe6, 0x54, 0xfe, 0x49, 0xb7, 0xd9, 0x9e, 0x5a, 0x1d,
		0xe7, 0x91, 0x94, 0xf6, 0x6a, 0xca, 0x1f, 0x17
	},
	{ /*8 index*/
		0x63, 0xea, 0x59, 0x75, 0x16, 0x0f, 0xda, 0x99, 0xcf, 0xfd,
		0xa2, 0x91, 0xe4, 0x76, 0xc3, 0x21, 0x1f, 0xc6, 0x28, 0x0b,
		0x3c, 0xca, 0xe5, 0x46, 0x96, 0x0b, 0x95, 0x40, 0xc9, 0x9d,
		0x91, 0x9e, 0x81, 0x52, 0x7a, 0xe7, 0xe8, 0x25, 0x08, 0x1e,
		0x2b, 0x15, 0xec, 0x2e, 0x59, 0x00, 0xb4, 0xcc, 0x58, 0x67,
		0xd9, 0x34, 0x1f, 0xfb, 0x8e, 0x23, 0x2d, 0xc8, 0x0f, 0xec,
		0x1a, 0xec, 0x84, 0x59, 0x90, 0xd9, 0x57, 0xd2, 0xcb, 0x5a,
		0x22, 0xa7, 0x7a, 0x29, 0x54, 0xbf, 0xdc, 0xe2, 0xed, 0x54,
		0x7c, 0xb3, 0x2b, 0x39, 0x2f, 0x30, 0xb2, 0xb7, 0xcb, 0x59,
		0x4c, 0xea, 0x75, 0xdc, 0x21, 0xd8, 0x65, 0xee, 0x2b, 0x83,
		0x2a, 0xe2, 0x4f, 0x5c, 0x2d, 0xb9, 0x37, 0x84, 0x0f, 0x52,
		0x41, 0x96, 0x69, 0xe8, 0xa4, 0x6e, 0xd4, 0xce, 0xf7, 0xf6,
		0x51, 0xc7, 0x64, 0xad, 0xec, 0xb3, 0x05, 0x70
	},
	{ /*9 index*/
		0x3d, 0xcf, 0x8d, 0xd2, 0xc7, 0x50, 0xdc, 0x9d, 0x2f, 0xb1,
		0xfe, 0x80, 0x1a, 0xe9, 0x15, 0xba, 0x04, 0xe7, 0x58, 0x62,
		0x1b, 0x92, 0x98, 0xb5, 0xb4, 0x58, 0x92, 0x20, 0x9e, 0xc4,
		0xa5, 0xa5, 0xa5, 0xf3, 0xa1, 0x43, 0x0e, 0xbe, 0x56, 0xff,
		0x78, 0x91, 0x05, 0xe0, 0xb6, 0x7e, 0x4a, 0x6e, 0x2c, 0x35,
		0xfc, 0x2e, 0x19, 0x18, 0xc7, 0x14, 0xef, 0x09, 0x4e, 0x6d,
		0xdc, 0x65, 0x44, 0xed, 0x14, 0xda, 0x50, 0xca, 0x11, 0x8d,
		0xe8, 0x44, 0xa2, 0xed, 0x3f, 0x66, 0x64, 0x8d, 0xeb, 0x20,
		0x2a, 0x15, 0x75, 0x9c, 0x39, 0x07, 0x81, 0x76, 0x51, 0x88,
		0x4c, 0x82, 0xe3, 0x35, 0xfe, 0x4d, 0x88, 0xf8, 0x51, 0x51,
		0xb9, 0xc9, 0x6c, 0x68, 0xa2, 0x95, 0x38, 0x2b, 0x25, 0xc0,
		0xfc, 0xca, 0xff, 0x40, 0x36, 0xf6, 0x5a, 0xe8, 0x7b, 0x89,
		0x8a, 0x2c, 0x6f, 0x86, 0x2f, 0x17, 0xc8, 0x0e
	},
	{ /*10 index*/
		0xe9, 0x0f, 0xfa, 0xa7, 0x4e, 0x04, 0x5b, 0xaa, 0x14, 0x81,
		0xf9, 0x6c, 0xcb, 0xe6, 0x91, 0x98, 0x08, 0x12, 0x1e, 0x87,
		0x51, 0xd7, 0x4b, 0xf2, 0x6e, 0x07, 0xef, 0x30, 0xd6, 0x69,
		0xac, 0xa8, 0xa0, 0x7d, 0xe3, 0x0a, 0xce, 0x9b, 0x86, 0x48,
		0x1f, 0xcf, 0x8d, 0x1c, 0xba, 0x80, 0xf7, 0xd5, 0x3a, 0xea,
		0x5a, 0x97, 0xc8, 0x43, 0x24, 0x19, 0x5d, 0x96, 0x84, 0x8d,
		0x0b, 0x8d, 0x63, 0xfa, 0x6c, 0xda, 0x3e, 0xdd, 0x97, 0xfb,
		0x19, 0x7a, 0xa3, 0xde, 0x7f, 0xf0, 0x59, 0x89, 0x0d, 0x3f,
		0xa1, 0xb5, 0xdf, 0x12, 0xdc, 0x54, 0xb5, 0xf6, 0x97, 0x3a,
		0x35, 0xcf, 0x67, 0x99, 0x98, 0x9a, 0x2b, 0x4c, 0xdf, 0xa1,
		0xdf, 0x89, 0x34, 0x39, 0x9d, 0xb2, 0x16, 0x23, 0xc4, 0x3d,
		0x30, 0xee, 0x6e, 0x4e, 0xfb, 0xac, 0xdf, 0x54, 0x06, 0x86,
		0xbc, 0x52, 0x6b, 0xfd, 0x0d, 0x35, 0x03, 0xa4
	},
	{ /*11 index*/
		0xd1, 0x14, 0xe5, 0x1d, 0xd2, 0x3c, 0x19, 0xe9, 0x1c, 0xb4,
		0x80, 0x60, 0x4b, 0xce, 0xcf, 0x73, 0x03, 0xca, 0xba, 0x69,
		0xcb, 0x2d, 0x2a, 0xf7, 0xf7, 0x7a, 0x2d, 0x18, 0xe8, 0x93,
		0x7b, 0x7b, 0x7b, 0x85, 0x38, 0xb1, 0x84, 0x30, 0xbe, 0x40,
		0x62, 0x2c, 0xc3, 0x48, 0xf6, 0x20, 0x77, 0xec, 0x9d, 0x97,
		0xc1, 0x5c, 0x0a, 0x4a, 0x52, 0xcf, 0x8c, 0x86, 0x34, 0xed,
		0x19, 0xeb, 0x73, 0xcd, 0x0f, 0xdb, 0xbc, 0x97, 0x4c, 0x25,
		0x8e, 0xb3, 0x39, 0xcd, 0x50, 0x2a, 0xeb, 0xa5, 0x8f, 0x58,
		0xdf, 0xcf, 0x67, 0xe9, 0x12, 0x02, 0xa0, 0xa6, 0x7c, 0xe6,
		0xf5, 0x21, 0x09, 0xd7, 0x40, 0xb5, 0x26, 0x42, 0x7c, 0xfc,
		0x32, 0x96, 0x2d, 0xee, 0xb9, 0xef, 0x92, 0x5f, 0x1b, 0x90,
		0x41, 0xd7, 0xc0, 0xb0, 0x56, 0x46, 0x7b, 0x4e, 0xa3, 0xe6,
		0x67, 0x9d, 0x6c, 0xa2, 0xdc, 0xce, 0x16, 0x04
	}
	/*
	   0xf5, 0xf1, 0xa4, 0x74, 0xd2, 0x8b, 0x85, 0xb6, 0x5c, 0xd6,
	   0x7d, 0x69, 0x75, 0x81, 0xf3, 0x82, 0x87, 0x2a, 0x49, 0x25,
	   0xa8, 0xa9, 0x3e, 0xd9, 0xbf, 0xec, 0x2e, 0x34, 0x85, 0x00,
	   0x2a, 0x07, 0x5d, 0x92, 0x8f, 0x55, 0xba, 0x82, 0x2e, 0x4e,
	   0xeb, 0x0c, 0x44, 0x5d, 0x1a, 0xc6, 0x79, 0xb2, 0x9f, 0x92,
	   0x27, 0x24, 0xe3, 0xc1, 0xa5, 0x0c, 0xe8, 0x10, 0x76, 0xc5,
	   0xe3, 0x7f, 0x69, 0x9f, 0x65, 0xfd, 0x9a, 0x79, 0x95, 0xef,
	   0x81, 0xcc, 0x16, 0xdb, 0x14, 0x07, 0x91, 0x76, 0x53, 0x9e,
	   0x9f, 0xb7, 0xfb, 0x94, 0x93, 0xdd, 0xbb, 0xc8, 0x47, 0x0e,
	   0x0e, 0x33, 0x9e, 0x40, 0x8d, 0x68, 0xf1, 0xf5, 0xcf, 0x21,
	   0xeb, 0x79, 0xac, 0x4d, 0x2a, 0xe2, 0x28, 0xac, 0x6e, 0x96,
	   0x50, 0x79, 0x9c, 0xe1, 0x26, 0xc7, 0x3e, 0x6f, 0x05, 0x01,
	   0x5b, 0x7e, 0x2b, 0xa5, 0xf9, 0x59, 0x10, 0xcd,
	   },
	   {
	   0xac, 0xb5, 0x63, 0x52, 0xab, 0x77, 0xf2, 0xb2, 0x6a, 0xf0,
	   0xf5, 0xe7, 0xd3, 0x9e, 0x0c, 0xb9, 0x67, 0x84, 0xec, 0x99,
	   0x56, 0xc0, 0x86, 0xfb, 0x4d, 0x55, 0xe7, 0x73, 0xd1, 0x4d,
	   0xa3, 0x7e, 0xc4, 0xeb, 0x52, 0xc2, 0xd3, 0xfd, 0xaa, 0xba,
	   0x75, 0x45, 0x68, 0x9b, 0xd8, 0xcc, 0x83, 0xb7, 0x41, 0x8e,
	   0x6b, 0x56, 0x01, 0x87, 0xc4, 0x13, 0x13, 0x59, 0x88, 0xe0,
	   0x54, 0x5b, 0xa2, 0x50, 0xf6, 0xb6, 0xbe, 0x85, 0xe2, 0x14,
	   0x8a, 0x1b, 0xcb, 0x64, 0x05, 0x4f, 0x85, 0xb5, 0x68, 0x8d,
	   0x25, 0x38, 0x64, 0x30, 0x46, 0xed, 0x29, 0x0b, 0x3f, 0xc9,
	   0x8c, 0x79, 0xbc, 0x58, 0xb9, 0x3e, 0xca, 0xf1, 0x57, 0x91,
	   0x64, 0xb5, 0x15, 0xd8, 0xdf, 0xc7, 0x65, 0x6e, 0x99, 0xeb,
	   0x04, 0x81, 0x60, 0x86, 0xc6, 0x42, 0xa7, 0x48, 0x87, 0x96,
	   0x14, 0xc0, 0x06, 0xd7, 0xd3, 0xd6, 0x44, 0xbc,
	   },
	   {
	   0x47, 0x84, 0x3b, 0xa7, 0x1d, 0x27, 0xe3, 0xf6, 0xb9, 0x5e,
	   0xe1, 0x6e, 0x67, 0xe0, 0x45, 0xa1, 0x22, 0x1f, 0x36, 0xdb,
	   0xfe, 0x7e, 0x90, 0x1a, 0xb0, 0x8d, 0x5c, 0x57, 0xa3, 0xc0,
	   0x5f, 0x82, 0x39, 0x2d, 0xe4, 0xbf, 0x33, 0xa1, 0x9c, 0xb4,
	   0x8f, 0xc5, 0x33, 0x79, 0x0b, 0x52, 0x62, 0x35, 0xa8, 0xed,
	   0x5a, 0x9b, 0x89, 0x10, 0x7b, 0x45, 0x0e, 0xcc, 0x26, 0x53,
	   0x49, 0x60, 0xae, 0x68, 0xab, 0xc1, 0xeb, 0xe2, 0xef, 0x4c,
	   0xe0, 0x15, 0x4e, 0x5b, 0xcf, 0x42, 0xac, 0xe6, 0x3d, 0xa8,
	   0xa8, 0x76, 0x43, 0x6f, 0xed, 0x99, 0xb3, 0x16, 0x72, 0xc4,
	   0x04, 0xd5, 0x68, 0x30, 0xa5, 0xae, 0xc4, 0x87, 0x14, 0xd8,
	   0x4f, 0xe2, 0xbd, 0x35, 0x5f, 0x09, 0xde, 0xfd, 0xec, 0x2e,
	   0x3c, 0xe2, 0x69, 0xc8, 0x9a, 0xd2, 0x50, 0x2c, 0x43, 0x80,
	   0x3b, 0x20, 0x9f, 0x3b, 0x02, 0xba, 0x0c, 0x55,
	   },
	   {
	   0x3d, 0x77, 0x69, 0x7d, 0xbf, 0x66, 0x05, 0xf5, 0xef, 0xc4,
	   0x87, 0xca, 0x1d, 0x68, 0xc5, 0xb2, 0xaa, 0x23, 0xcd, 0x2a,
	   0xbe, 0xd0, 0x62, 0xc3, 0x35, 0xff, 0x4a, 0x65, 0x1c, 0x35,
	   0x39, 0xa0, 0x93, 0x8f, 0x3d, 0x11, 0x1d, 0x81, 0x7f, 0x73,
	   0x67, 0xf3, 0x6e, 0xab, 0x1a, 0xd5, 0xe1, 0xb6, 0x30, 0xa4,
	   0xaf, 0x7e, 0xc0, 0x62, 0x13, 0xcd, 0x8d, 0x3a, 0xe6, 0x08,
	   0xff, 0x3b, 0xf9, 0x3c, 0x06, 0xb6, 0x30, 0xe3, 0x09, 0x0f,
	   0x67, 0xcb, 0xd7, 0x6b, 0x43, 0xb4, 0x63, 0xf7, 0xee, 0x65,
	   0x1b, 0x92, 0x6b, 0x54, 0xf2, 0x8d, 0x5e, 0x87, 0x90, 0x56,
	   0x65, 0xe2, 0x71, 0x3a, 0xb2, 0x90, 0x57, 0x04, 0x3e, 0x6c,
	   0x6b, 0xf7, 0xcf, 0x9a, 0x18, 0x92, 0xab, 0xec, 0x6a, 0xcf,
	   0x03, 0xe0, 0xe8, 0x62, 0xd2, 0xb1, 0x7a, 0x36, 0x22, 0x6e,
	   0x4f, 0x50, 0x42, 0x1e, 0xdd, 0xde, 0x33, 0xb1,
	   },
	   */
};

int ndfc_get_version(void)
{
	return ndfc_ver;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
#define AWBIT(n) (1U << (n))
#define GET_BIT(n, num) (((num) >> (n)) & 1U)

int gen_rand_num(unsigned short seed, int len, unsigned char *out)
{
	int i, cnt, bi;
	unsigned short pnsr, pns_out;

	if (len & 0x1) {
		RAWNAND_ERR("gen_rand_num, wrong input len %d, len must be even!!\n", len);
		return ERR_NO_35;
	}

	pnsr = seed;
	pns_out = 0;
	cnt = 0;

	for (bi = 0; bi < (len >> 1); bi++) {
		for (i = 0; i < 14; i++) {
			//pns_out[i]  = pnsr[i] ^ pnsr[i+1];
			pns_out &= (~AWBIT(i));
			pns_out |= ((GET_BIT(i, pnsr) ^ GET_BIT(i + 1, pnsr)) << i);

			//pns_out[14] = pnsr[14] ^ pnsr[0] ^ pnsr[1];
			pns_out &= (~AWBIT(14));
			pns_out |= ((GET_BIT(14, pnsr) ^ GET_BIT(0, pnsr) ^ GET_BIT(1, pnsr)) << 14);

			//pns_out[15] = pnsr[0]  ^ pnsr[2];
			pns_out &= (~AWBIT(15));
			pns_out |= ((GET_BIT(0, pnsr) ^ GET_BIT(2, pnsr)) << 15);
		}

		//update seed
		pnsr = pns_out >> 1;

		out[cnt++] = pns_out & 0xff;
		out[cnt++] = (pns_out >> 8) & 0xff;
	}

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int wait_reg_status_half_us(volatile u32 *reg, u32 mark, u32 value)
{
	int i, ret;
	volatile u32 data;

	for (i = 0; i < 0xfff; i++) {
		ret = ERR_TIMEOUT;
		data = readl(reg);
		if ((data & mark) == value) {
			ret = 0;
			break;
		}
	}
	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
s32 wait_reg_status(volatile u32 *reg, u32 mark, u32 value, u32 us)
{
	//volatile u32 data;
	int ret = 0;

	for (; us > 0; us--) {
		ret = wait_reg_status_half_us(reg, mark, value);
		if (ret == 0) {
			break;
		}
		cond_resched();
	}

	if (ret != 0)
		RAWNAND_ERR("nand wait_reg_status timeout, reg:%p, value:0x%08x\n",
				reg, (u32)readl(reg));

	return ret;
}

/*****************************************************************************
 *Name         : ndfc_print_dma
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_print_dma(struct nand_controller_info *nctri)
{
	unsigned char i;
	for (i = 0; i < 8; i++)
		RAWNAND_DBG("*****reg_data_dma_size_2x+%d(%p): 0x%08x*****\n", i,
				(nctri->nreg.reg_data_dma_size_2x + i), readl(nctri->nreg.reg_data_dma_size_2x + i));
}

/*****************************************************************************
 *Name         : ndfc_print_random
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_print_random(struct nand_controller_info *nctri)
{
	unsigned char i;
	for (i = 0; i < 8; i++)
		RAWNAND_DBG("*****reg_random_seed_x+%d(%p): 0x%08x*****\n", i,
				(nctri->nreg.reg_random_seed_x + i), readl(nctri->nreg.reg_random_seed_x + i));
}

/*****************************************************************************
 *Name         : _cal_random_seed
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
u32 _cal_random_seed(u32 page)
{
	return random_seed[page % 128];
}

/*****************************************************************************
 *Name         : _set_addr
 *Description  : set address register
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void _set_addr(struct nand_controller_info *nctri, u8 acnt, u8 *abuf)
{
	s32 i;
	u32 addr_low = 0, addr_high = 0;

	for (i = 0; i < acnt; i++) {
		if (i < 4)
			addr_low |= abuf[i] << (i * 8);
		else
			addr_high |= abuf[i] << ((i - 4) * 8);
	}

	writel(addr_low, nctri->nreg.reg_addr_low);
	writel(addr_high, nctri->nreg.reg_addr_high);
}

/*****************************************************************************
 *Name         : _disable_ecc
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_disable_ecc(struct nand_controller_info *nctri)
{
	writel((readl(nctri->nreg.reg_ecc_ctl) & (~NDFC_ECC_EN)),
			nctri->nreg.reg_ecc_ctl);
}

/*****************************************************************************
 *Name         : _enable_ecc
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_enable_ecc(struct nand_controller_info *nctri, u32 pipline, u32 randomize)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_ecc_ctl);

	if (pipline == 1)
		cfg |= NDFC_ECC_PIPELINE;
	else
		cfg &= ~NDFC_ECC_PIPELINE;

	//after erased, all data is 0xff, but ecc is not 0xff,if random open, disable exception
	if (randomize)
		cfg &= ~NDFC_ECC_EXCEPTION;
	else
		cfg |= NDFC_ECC_EXCEPTION;

	cfg |= NDFC_ECC_EN;

	writel(cfg, nctri->nreg.reg_ecc_ctl);
}

/*****************************************************************************
 *Name         : _enable_ldpc_ecc
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_enable_ldpc_ecc(struct nand_controller_info *nctri, u32 pipline)
{
	writel((readl(nctri->nreg.reg_ecc_ctl) | NDFC_ECC_EN),
			nctri->nreg.reg_ecc_ctl);
}

/*****************************************************************************
 *Name         : _repeat_mode_enable
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_repeat_mode_enable(struct nand_controller_info *nctri)
{
	u32 reg_val = 0;

	reg_val = readl(nctri->nreg.reg_ctl);

	if (reg_val & NDFC_DDR_TYPE) {
		reg_val |= NDFC_DDR_REPEAT_ENABLE;
		writel(reg_val, nctri->nreg.reg_ctl);
	}
}

/*****************************************************************************
 *Name         : _repeat_mode_disable
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_repeat_mode_disable(struct nand_controller_info *nctri)
{
	u32 reg_val = 0;

	reg_val = readl(nctri->nreg.reg_ctl);
	if (reg_val & NDFC_DDR_TYPE) {
		reg_val &= ~NDFC_DDR_REPEAT_ENABLE;
		writel(reg_val, nctri->nreg.reg_ctl);
	}
}

/*****************************************************************************
 *Name         : _wait_cmdfifo_free
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
s32 ndfc_wait_cmdfifo_free(struct nand_controller_info *nctri)
{
	s32 ret;
	ret = wait_reg_status(nctri->nreg.reg_sta, NDFC_CMD_FIFO_STATUS, 0, 0x0f);
	if (ret != 0) {
		RAWNAND_ERR("ndfc wait cmdfifo free error!\n");
		show_nctri(nctri);
	}

	return ret;
}

/*****************************************************************************
 *Name         : _wait_cmdfifo_free
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
s32 ndfc_wait_fsm_ready(struct nand_controller_info *nctri)
{
	s32 ret;
	ret = wait_reg_status(nctri->nreg.reg_sta, NDFC_STA, 0, 0xfffff);
	if (ret != 0) {
		RAWNAND_ERR("ndfc wait fsm ready error!\n");
	}

	return ret;
}

/*****************************************************************************
 *Name         : _wait_cmdfifo_free
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
s32 ndfc_wait_cmd_finish(struct nand_controller_info *nctri)
{
	s32 ret;
	ret = wait_reg_status(nctri->nreg.reg_sta, NDFC_CMD_INT_FLAG, NDFC_CMD_INT_FLAG, 0xffffff);
	if (ret != 0) {
		RAWNAND_ERR("ndfc wait cmd finish error!\n");
		show_nctri(nctri);
		writel(NDFC_CMD_INT_FLAG, nctri->nreg.reg_sta);
		return ret;
	}

	// bit2, bit 1, bit 0 are cleared after writing '1'. other bits are read only.
	// here, we only clear bit 1.
	writel(NDFC_CMD_INT_FLAG, nctri->nreg.reg_sta);

	return 0;
}

/*****************************************************************************
 *Name         : _select_chip
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_select_chip(struct nand_controller_info *nctri, u32 chip)
{
	u32 cfg;

	cfg = *nctri->nreg.reg_ctl;
	cfg &= ~NDFC_CE_SEL;
	switch (chip) {
	case 0:
		cfg |= NDFC_SEL_CE_0;
		break;
	case 1:
		cfg |= NDFC_SEL_CE_1;
		break;
	case 2:
		cfg |= NDFC_SEL_CE_2;
		break;
	case 3:
		cfg |= NDFC_SEL_CE_3;
		break;
	case 4:
		cfg |= NDFC_SEL_CE_4;
		break;
	case 5:
		cfg |= NDFC_SEL_CE_5;
		break;
	case 6:
		cfg |= NDFC_SEL_CE_6;
		break;
	case 7:
		cfg |= NDFC_SEL_CE_7;
		break;
	case 8:
		cfg |= NDFC_SEL_CE_8;
		break;
	case 9:
		cfg |= NDFC_SEL_CE_9;
		break;
	case 10:
		cfg |= NDFC_SEL_CE_10;
		break;
	case 11:
		cfg |= NDFC_SEL_CE_11;
		break;
	case 12:
		cfg |= NDFC_SEL_CE_12;
		break;
	case 13:
		cfg |= NDFC_SEL_CE_13;
		break;
	case 14:
		cfg |= NDFC_SEL_CE_14;
		break;
	case 15:
		cfg |= NDFC_SEL_CE_15;
		break;
	default:
		RAWNAND_ERR("select chip ce err no the ce:%u\n", chip);
	}
	writel(cfg, nctri->nreg.reg_ctl);

	//ddr nand
	if (cfg & NDFC_NF_TYPE) {
		writel(nctri->ddr_timing_ctl[0], nctri->nreg.reg_timing_ctl);
	}
#ifdef FPGA_PLATFORM
	else {
		writel(0, nctri->nreg.reg_timing_ctl); /*fix EDO*/
	}
#endif
}

/*****************************************************************************
 *Name         : _select_chip
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_select_rb(struct nand_controller_info *nctri, u32 rb)
{
	u32 cfg;

	cfg = readl(nctri->nreg.reg_ctl);
	cfg &= ~NDFC_RB_SEL;
	switch (rb) {
	case 0:
		cfg |= NDFC_SEL_RB0;
		break;
	case 1:
		cfg |= NDFC_SEL_RB1;
		break;
	case 2:
		cfg |= NDFC_SEL_RB2;
		break;
	case 3:
		cfg |= NDFC_SEL_RB3;
		break;
	default:
		RAWNAND_ERR("select rb err, no the rb:%u\n", rb);
	}
	writel(cfg, nctri->nreg.reg_ctl);
}


/*****************************************************************************
 *Name         : _get_selected_rb_no
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
u32 ndfc_get_selected_rb_no(struct nand_controller_info *nctri)
{
	return ((readl(nctri->nreg.reg_ctl) & NDFC_RB_SEL) >> 3);
}

/*****************************************************************************
 *Name         : _get_rb_sta
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
s32 ndfc_get_rb_sta(struct nand_controller_info *nctri, u32 rb)
{
	u32 rb_check = 0;

	if (rb == 0)
		rb_check = NDFC_RB_STATE0;
	if (rb == 1)
		rb_check = NDFC_RB_STATE1;

	if ((readl(nctri->nreg.reg_sta) & rb_check) != 0) {
		return 1;
	} else {
		return 0;
	}
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
u32 ndfc_get_page_size(struct nand_controller_info *nctri)
{
	return (readl(nctri->nreg.reg_ctl) >> 8) & 0xf;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
int ndfc_set_page_size(struct nand_controller_info *nctri, u32 page_size)
{
	u32 val = 0;
	val = readl(nctri->nreg.reg_ctl);
	val &= ~NDFC_PAGE_SIZE;
	val |= ((page_size & 0xf) << 8);
	writel(val, nctri->nreg.reg_ctl);
	return 0;
}

/*****************************************************************************
 *Name         : _set_ecc_mode
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_set_ecc_mode(struct nand_controller_info *nctri, u8 ecc_mode)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_ecc_ctl);
	cfg &= ~NDFC_ECC_MODE;
	cfg |= NDFC_ECC_MODE & (ecc_mode << 8); // cfg |= NDFC_ECC_MODE & (ecc_mode<<12); modifyfor AW1728 new nand controller
	writel(cfg, nctri->nreg.reg_ecc_ctl);
}

/*****************************************************************************
 *Name         : _set_boot0_ldpc_mode
 *Description  : set ldpc mode for burn or read boot0
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_boot0_ldpc_mode(struct nand_controller_info *nctri)
{
	u32 cfg;

	cfg = readl(nctri->nreg.reg_enc_ldpc_mode_set);
	cfg &= ~LDPC_MODE;
	cfg |= BOOT0_LDPC_MODE; //ldpc mode = 0
	cfg &= ~FW_EXTEND_ENCODE;
	cfg |= BOOT0_FW_EXTEND_ENCODE; //firmware 0:8B 1:16B
	writel(cfg, nctri->nreg.reg_enc_ldpc_mode_set);

	cfg = readl(nctri->nreg.reg_cor_ldpc_mode_set);
	cfg &= ~C0_LDPC_MODE;
	cfg |= BOOT0_C0_LDPC_MODE; //c0 ldpc mode = 0
	cfg &= ~C1_LDPC_MODE;
	cfg |= BOOT0_C1_LDPC_MODE; //c0 ldpc mode = 0
	cfg &= ~FW_EXTEND_DECODE;
	cfg |= BOOT0_FW_EXTEND_DECODE; //firmware 0:8B 1:16B
	writel(cfg, nctri->nreg.reg_cor_ldpc_mode_set);
}

/*****************************************************************************
 *Name         : _enable_encode
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_enable_encode(struct nand_controller_info *nctri)
{
	u32 cfg = 0;

	cfg = readl(nctri->nreg.reg_enc_ldpc_mode_set);
	cfg |= ENCODE; //decode = 0, encode =1
	writel(cfg, nctri->nreg.reg_enc_ldpc_mode_set);

	cfg = readl(nctri->nreg.reg_glb_cfg);
	cfg &= ~LDPC_OUTPUT_DISORDER_CTL; //open disorder
	writel(cfg, nctri->nreg.reg_glb_cfg);
}

/*****************************************************************************
 *Name         : _enable_decode
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_enable_decode(struct nand_controller_info *nctri)
{
	u32 cfg = 0;

	cfg = readl(nctri->nreg.reg_enc_ldpc_mode_set);
	cfg &= ~ENCODE; //decode = 0, encode =1
	writel(cfg, nctri->nreg.reg_enc_ldpc_mode_set);

	cfg = readl(nctri->nreg.reg_cor_ldpc_mode_set);
	cfg &= ~C0_DECODE_MODE; // C0_Decode_Mode, 0~4, 0: HB_Mode
	cfg &= ~C0_SCALE_MODE;  // C0_SCALE_Mode, 0: HB, 1: SB
	cfg &= ~C1_DECODE_MODE; // C1_Decode_Mode, 0~4, 0-HB_Mode
	cfg &= ~C1_SCALE_MODE;  // C1_SCALE_Mode, 0: HB, 1: SB
	writel(cfg, nctri->nreg.reg_cor_ldpc_mode_set);

	cfg = readl(nctri->nreg.reg_ldpc_ctl);
	cfg &= ~CH1_HB_LLR_VAL;
	cfg |= (0xa & 0xf) << 28; //default value
	writel(cfg, nctri->nreg.reg_ldpc_ctl);
}

/*****************************************************************************
 *Name         : _get_ecc_mode
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
s32 ndfc_get_ecc_mode(struct nand_controller_info *nctri)
{
	return ((readl(nctri->nreg.reg_ecc_ctl) & NDFC_ECC_MODE) >> 8);
}

/*****************************************************************************
 *Name         : _enable_randomize
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_set_rand_seed(struct nand_controller_info *nctri, u32 page_no)
{
	u32 cfg = 0, random_seed = 0;

	random_seed = _cal_random_seed(page_no);

	cfg = readl(nctri->nreg.reg_ecc_ctl);
	cfg &= ~NDFC_RANDOM_SEED;
	cfg |= (random_seed << 16);
	writel(cfg, nctri->nreg.reg_ecc_ctl);
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
void ndfc_set_new_rand_seed(struct nand_controller_info *nctri, u32 page_no)
{
	u32 i, j, index;
	//	struct nand_chip_info *nci = nctri->nci;

	index = page_no % 67;

	for (i = 0, j = 0; i < 16; i++, j++) {
		if ((index + i) > 66) {
			index = 0;
			j = 0;
		}

		writel(random_seed_new[index + j], nctri->nreg.reg_random_seed_x + i);
	}
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
void ndfc_set_default_rand_seed(struct nand_controller_info *nctri)
{
	u32 cfg = 0;

	cfg = readl(nctri->nreg.reg_ecc_ctl);
	cfg &= ~NDFC_RANDOM_SEED;
	cfg |= (RANDOM_SEED_DEFAULT << 16);
	writel(cfg, nctri->nreg.reg_ecc_ctl);
}

/*****************************************************************************
 *Name         : _enable_randomize
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_enable_randomize(struct nand_controller_info *nctri)
{
	u32 val = 0;
	val = readl(nctri->nreg.reg_ecc_ctl);
	val |= NDFC_RANDOM_EN;
	writel(val, nctri->nreg.reg_ecc_ctl);
}

/*****************************************************************************
 *Name         : _enable_randomize
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_disable_randomize(struct nand_controller_info *nctri)
{
	u32 cfg = 0;

	cfg = readl(nctri->nreg.reg_ecc_ctl);
	cfg &= ~NDFC_RANDOM_EN;
	cfg &= ~NDFC_RANDOM_SEED;
	cfg |= (RANDOM_SEED_DEFAULT << 16);
	writel(cfg, nctri->nreg.reg_ecc_ctl);
}

/*****************************************************************************
 *Name         : _enable_rb_int
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_enable_rb_b2r_int(struct nand_controller_info *nctri)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_int);
	cfg |= NDFC_B2R_INT_ENABLE;
	writel(cfg, nctri->nreg.reg_int);
}

/*****************************************************************************
 *Name         : _enable_rb_int
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_disable_rb_b2r_int(struct nand_controller_info *nctri)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_int);
	cfg &= ~NDFC_B2R_INT_ENABLE;
	writel(cfg, nctri->nreg.reg_int);
}
/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
void ndfc_clear_rb_b2r_int(struct nand_controller_info *nctri)
{
	writel(NDFC_RB_B2R, nctri->nreg.reg_sta);
}
/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
u32 ndfc_get_rb_b2r_int_sta(struct nand_controller_info *nctri)
{
	return (readl(nctri->nreg.reg_sta) & NDFC_RB_B2R);
}
/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
u32 ndfc_check_rb_b2r_int_occur(struct nand_controller_info *nctri)
{
	return ((readl(nctri->nreg.reg_sta) & NDFC_RB_B2R) && (readl(nctri->nreg.reg_int) & NDFC_B2R_INT_ENABLE));
}

/*****************************************************************************
 *Name         : _enable_rb_int
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_enable_cmd_int(struct nand_controller_info *nctri)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_int);
	cfg |= NDFC_CMD_INT_ENABLE;
	writel(cfg, nctri->nreg.reg_int);
}

/*****************************************************************************
 *Name         : _enable_rb_int
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_disable_cmd_int(struct nand_controller_info *nctri)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_int);
	cfg &= ~NDFC_CMD_INT_ENABLE;
	writel(cfg, nctri->nreg.reg_int);
}
/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
void ndfc_clear_cmd_int(struct nand_controller_info *nctri)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_sta);
	cfg |= NDFC_CMD_INT_FLAG;
	writel(cfg, nctri->nreg.reg_sta);
}
/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
u32 ndfc_get_cmd_int_sta(struct nand_controller_info *nctri)
{
	return (readl(nctri->nreg.reg_sta) & NDFC_CMD_INT_FLAG);
}
/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
u32 ndfc_check_cmd_int_occur(struct nand_controller_info *nctri)
{
	return ((readl(nctri->nreg.reg_sta) & NDFC_CMD_INT_FLAG) &&
			(readl(nctri->nreg.reg_int) & NDFC_CMD_INT_ENABLE));
}

/*****************************************************************************
 *Name         : _enable_rb_int
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_enable_dma_int(struct nand_controller_info *nctri)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_int);
	cfg |= NDFC_DMA_INT_ENABLE;
	writel(cfg, nctri->nreg.reg_int);
}
/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
void ndfc_disable_dma_int(struct nand_controller_info *nctri)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_int);
	cfg &= ~NDFC_DMA_INT_ENABLE;
	writel(cfg, nctri->nreg.reg_int);
}
/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
void ndfc_clear_dma_int(struct nand_controller_info *nctri)
{
	writel(NDFC_DMA_INT_FLAG, nctri->nreg.reg_sta);
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
u32 ndfc_get_dma_int_sta(struct nand_controller_info *nctri)
{
	return (readl(nctri->nreg.reg_sta) & NDFC_DMA_INT_FLAG);
}
/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
u32 ndfc_check_dma_int_occur(struct nand_controller_info *nctri)
{
	return ((readl(nctri->nreg.reg_sta) & NDFC_DMA_INT_FLAG) &&
			(readl(nctri->nreg.reg_int) & NDFC_DMA_INT_ENABLE));
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
s32 ndfc_wait_rb_ready_int(struct nand_controller_info *nctri, u32 rb)
{
	s32 ret;
	u32 rb_check = 0;

	if (rb == 0)
		rb_check = NDFC_RB_STATE0;
	if (rb == 1)
		rb_check = NDFC_RB_STATE1;

	if ((*nctri->nreg.reg_sta & rb_check) == 0) {
		if (nctri->rb_ready_flag == 1) {
			goto wait_rb_ready_int;
		}

		ndfc_enable_rb_b2r_int(nctri);

		if (nand_rb_wait_time_out(nctri->channel_id, &nctri->rb_ready_flag) == 0) {
			RAWNAND_ERR("_wait_rb ready_int int timeout, ch: 0x%x, sta: 0x%x\n", nctri->channel_id, readl(nctri->nreg.reg_sta));
		}
	}

wait_rb_ready_int:
	ret = wait_reg_status(nctri->nreg.reg_sta, rb_check, rb_check, 0xfffff);
	if (ret != 0) {
		RAWNAND_ERR("_wait_rb ready_int wait timeout, ch: 0x%x, sta: 0x%x\n", nctri->channel_id, readl(nctri->nreg.reg_sta));
		//return ret;
	}

	ndfc_disable_rb_b2r_int(nctri);

	ndfc_clear_rb_b2r_int(nctri);

	nctri->rb_ready_flag = 0;

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
s32 ndfc_wait_rb_ready(struct nand_controller_info *nctri, u32 rb)
{
	s32 ret;
	u32 rb_check = 0;

	if (rb == 0)
		rb_check = NDFC_RB_STATE0;
	if (rb == 1)
		rb_check = NDFC_RB_STATE1;

	ret = wait_reg_status(nctri->nreg.reg_sta, rb_check, rb_check, 0xff);

	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
s32 ndfc_wait_all_rb_ready(struct nand_controller_info *nctri)
{
	s32 i, ret = 0;

	for (i = 0; i < 2; i++) {
		ret |= ndfc_wait_rb_ready(nctri, i);
	}

	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
s32 ndfc_write_wait_rb_ready(struct nand_controller_info *nctri, u32 rb)
{
	if (nctri->write_wait_rb_mode == 0) {
		return ndfc_wait_rb_ready(nctri, rb);
	} else {
		return ndfc_wait_rb_ready_int(nctri, rb);
	}
}

/*****************************************************************************
 *Name         : ndfc_change_page_size
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
s32 ndfc_change_page_size(struct nand_controller_info *nctri)
{
	struct nand_chip_info *nci = nctri->nci; //get first nand flash chip

	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_ctl);

	cfg &= ~NDFC_PAGE_SIZE;
	if (nci->sector_cnt_per_page == 2)
		cfg |= 0x0 << 8; //1K
	else if (nci->sector_cnt_per_page == 4)
		cfg |= 0x1 << 8; //2K
	else if (nci->sector_cnt_per_page == 8)
		cfg |= 0x2 << 8; //4K
	else if (nci->sector_cnt_per_page == 16)
		cfg |= 0x3 << 8; //8K
	else if ((nci->sector_cnt_per_page > 16) && (nci->sector_cnt_per_page < 32))
		cfg |= 0x4 << 8; //12K
	else if (nci->sector_cnt_per_page == 32)
		cfg |= 0x4 << 8; //16K
	else if (nci->sector_cnt_per_page == 64)
		cfg |= 0x5 << 8; //32K
	else {
		RAWNAND_ERR("ndfc_change_page_size, wrong page size!\n");
		return ERR_NO_34;
	}
	writel(cfg, nctri->nreg.reg_ctl);

	writel((nci->sector_cnt_per_page << 9), nctri->nreg.reg_spare_area);

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :Command FSM
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
s32 _normal_cmd_io_send(struct nand_controller_info *nctri, struct _nctri_cmd *icmd)
{
	s32 ret = 0;
	u32 cmd_val = 0;
	//u32 rb = ndfc_get_selected_rb_no(nctri);
	u32 reg_val;

	//===================================
	//     configure cmd
	//===================================
	if (icmd->cmd_send) {
		cmd_val |= icmd->cmd & 0xff;
		cmd_val |= NDFC_SEND_CMD1;
		if (icmd->cmd_wait_rb)
			cmd_val |= NDFC_WAIT_FLAG;
	}

	//===================================
	//     configure address
	//===================================
	if (icmd->cmd_acnt) {
		_set_addr(nctri, icmd->cmd_acnt, icmd->cmd_addr);
		cmd_val |= ((icmd->cmd_acnt - 1) & 0x7) << 16;
		cmd_val |= NDFC_SEND_ADR;
	}

	if ((ndfc_wait_cmdfifo_free(nctri) != 0) || (ndfc_wait_fsm_ready(nctri) != 0)) {
		RAWNAND_ERR("_normal_cmd_io_send, wait cmd fifo free timeout!\n");
		return ERR_TIMEOUT;
	}

	//===================================
	//     configure data
	//===================================
	if (icmd->cmd_trans_data_nand_bus) {
		writel((icmd->cmd_mdata_len & 0x3ff), nctri->nreg.reg_ndfc_cnt);
		cmd_val |= NDFC_DATA_TRANS;
		if (icmd->cmd_direction) //write
			cmd_val |= NDFC_ACCESS_DIR;

		if (icmd->cmd_swap_data) {
			if (icmd->cmd_swap_data_dma) {
				cmd_val |= NDFC_DATA_SWAP_METHOD; //dma
				ndfc_dma_config_start(nctri, icmd->cmd_direction, icmd->cmd_mdata_addr, icmd->cmd_mdata_len);
			} else {
				//set AHB mode//zengyu
				reg_val = readl(nctri->nreg.reg_ctl);
				reg_val &= ~NDFC_DMA_METHOD;
				writel(reg_val, nctri->nreg.reg_ctl);
				if (icmd->cmd_direction) { //write
					memcpy((void *)nctri->nreg.reg_ram0_base, (void *)icmd->cmd_mdata_addr, icmd->cmd_mdata_len);
				}
			}
		}
	}
	//===================================
	//     send cmd id
	//===================================
	writel(cmd_val, nctri->nreg.reg_cmd);

	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :Command FSM
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
s32 _normal_cmd_io_wait(struct nand_controller_info *nctri, struct _nctri_cmd *icmd)
{
	s32 ret = 0;
	u32 rb = ndfc_get_selected_rb_no(nctri);
	//u32 reg_val;

	//wait cmd finish
	ret = ndfc_wait_cmd_finish(nctri);
	if (ret != 0) {
		RAWNAND_ERR("_normal_cmd io_wait, wait cmd finish timeout 0x%x!\n", icmd->cmd);
	}

	//check data
	if (icmd->cmd_swap_data) {
		if (icmd->cmd_swap_data_dma != 0) {
			ret |= ndfc_wait_dma_end(nctri, icmd->cmd_direction, icmd->cmd_mdata_addr, icmd->cmd_mdata_len);
			if (ret != 0) {
				RAWNAND_ERR("_normal_cmd io_wait, wait dma timeout!\n");
			}
		} else {
			//			//set AHB mode//zengyu
			//			reg_val = *nctri->nreg.reg_ctl;
			//			reg_val &= ~(0x1U<<14);
			//			*nctri->nreg.reg_ctl = reg_val;
			if (icmd->cmd_direction == 0) { //read
				memcpy((void *)icmd->cmd_mdata_addr, (void *)nctri->nreg.reg_ram0_base, icmd->cmd_mdata_len);
			}
		}
	}

	//check rb status
	if (icmd->cmd_wait_rb) {
		ret = ndfc_write_wait_rb_ready(nctri, rb);
		if (ret != 0) {
			RAWNAND_ERR("_normal cmd io_wait, sw wait rb b2r error 0x%x, wait mode %d!\n", *nctri->nreg.reg_sta, icmd->cmd_wait_rb);
		}
	}

	if (ret)
		return ERR_NO_33;
	else
		return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :Command FSM
 *Parameter    :
 *Return       :
 *Note         :
 *****************************************************************************/
s32 _batch_cmd_io_wait(struct nand_controller_info *nctri, struct _nctri_cmd_seq *cmd_list)
{
	s32 ret = 0;
	u32 val = 0;
	struct _nctri_cmd *icmd = &(cmd_list->nctri_cmd[0]);
	u32 rb = ndfc_get_selected_rb_no(nctri);

	//check data
	if (cmd_list->nctri_cmd[0].cmd_mdata_addr != 0) {
		ret |= ndfc_wait_dma_end(nctri, icmd->cmd_direction, icmd->cmd_mdata_addr, icmd->cmd_mdata_len);
		if (ret) {
			RAWNAND_ERR("_batch cmd io wait, wait dma timeout!\n");
			ndfc_print_save_reg(nctri);
			ndfc_print_reg(nctri);
			RAWNAND_ERR("wait dma timeout end!\n");
		}
	}

	//wait cmd finish
	ret = ndfc_wait_cmd_finish(nctri);
	if (ret) {
		RAWNAND_ERR("_batch cmd io wait, wait cmd finish timeout 0x%x!\n", icmd->cmd);
		ndfc_print_reg(nctri);
		ret = 0;
	}

	//check rb status
	if (nctri->current_op_type == 1) { //write
		if (nctri->write_wait_rb_before_cmd_io == 0) {
			ndfc_write_wait_rb_ready(nctri, rb);
		}
	} else {
		if (icmd->cmd_wait_rb) {
			//ndfc_wait_rb_ready(nctri,rb);
			ndfc_write_wait_rb_ready(nctri, rb);
		}
	}

	if (nctri->random_cmd2_send_flag) {
		val = readl(nctri->nreg.reg_read_cmd_set);
		val &= ~NDFC_RANDOM_CMD2;
		writel(val, nctri->nreg.reg_read_cmd_set);
		val = readl(nctri->nreg.reg_cmd);
		val &= ~NDFC_SEND_RAN_CMD2;
		writel(val, nctri->nreg.reg_cmd);
	}

	if (ret)
		return ERR_NO_31;
	else
		return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
void ndfc_clean_cmd_seq(struct _nctri_cmd_seq *cmd_list)
{
	memset(cmd_list, 0x0, sizeof(struct _nctri_cmd_seq));
	return;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
void print_cmd_seq(struct _nctri_cmd_seq *cmd_list)
{
	int i, j;
	RAWNAND_DBG("cmd_type:0x%x!\n", cmd_list->cmd_type);
	RAWNAND_DBG("ecc_layout:0x%x!\n", cmd_list->ecc_layout);
	//RAWNAND_ERR("row_addr_auto_inc:0x%x!\n", cmd_list->row_addr_auto_inc);
	RAWNAND_DBG("re_start_cmd:0x%x!\n", cmd_list->re_start_cmd);
	RAWNAND_DBG("re_end_cmd:0x%x!\n", cmd_list->re_end_cmd);
	RAWNAND_DBG("re_cmd_times:0x%x!\n", cmd_list->re_cmd_times);

	for (i = 0; i < MAX_CMD_PER_LIST; i++) {
		RAWNAND_DBG("==========0x%x===========\n", i);
		if (cmd_list->nctri_cmd[i].cmd_valid != 0) {
			RAWNAND_DBG("cmd:0x%x!\n", cmd_list->nctri_cmd[i].cmd);
			RAWNAND_DBG("cmd_send:0x%x!\n", cmd_list->nctri_cmd[i].cmd_send);
			RAWNAND_DBG("cmd_wait_rb:0x%x!\n", cmd_list->nctri_cmd[i].cmd_wait_rb);
			RAWNAND_DBG("cmd_acnt:0x%x!\n", cmd_list->nctri_cmd[i].cmd_acnt);
			RAWNAND_DBG("cmd_trans_data_nand_bus:0x%x!\n", cmd_list->nctri_cmd[i].cmd_trans_data_nand_bus);
			RAWNAND_DBG("cmd_swap_data:0x%x!\n", cmd_list->nctri_cmd[i].cmd_swap_data);
			RAWNAND_DBG("cmd_swap_data_dma:0x%x!\n", cmd_list->nctri_cmd[i].cmd_swap_data_dma);
			RAWNAND_DBG("cmd_direction:0x%x!\n", cmd_list->nctri_cmd[i].cmd_direction);
			RAWNAND_DBG("cmd_mdata_len:0x%x!\n", cmd_list->nctri_cmd[i].cmd_mdata_len);
			RAWNAND_DBG("cmd_mdata_addr:%p!\n", cmd_list->nctri_cmd[i].cmd_mdata_addr);
			for (j = 0; j < MAX_CMD_PER_LIST; j++)
				RAWNAND_DBG("cmd_addr[%d]:0x%08x!\n", j, cmd_list->nctri_cmd[i].cmd_addr[j]);
		}
	}
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
s32 ndfc_execute_cmd(struct nand_controller_info *nctri, struct _nctri_cmd_seq *cmd_list)
{
	s32 ret = 0;
	s32 c = 0;
	s32 rb;
	struct _nctri_cmd *pcmd;

	rb = ndfc_get_selected_rb_no(nctri);
	ndfc_wait_rb_ready(nctri, rb);

	if (cmd_list->cmd_type == CMD_TYPE_NORMAL) {
		for (c = 0; c < MAX_CMD_PER_LIST; c++) {
			pcmd = &cmd_list->nctri_cmd[c];
			if (!pcmd->cmd_valid) {
				//RAWNAND_DBG("ndfc_execute_cmd, no more cmd, total cmd %d\n", c);
				break;
			}
			ret = _normal_cmd_io_send(nctri, pcmd);
			if (ret) {
				RAWNAND_ERR("ndfc_execute_cmd, send normal cmd %d error!\n", c);
				return ret;
			}
			ret = _normal_cmd_io_wait(nctri, pcmd);
			if (ret) {
				RAWNAND_ERR("ndfc_execute_cmd, wait normal cmd %d error!\n", c);
				return ret;
			}
		}
	} else if (cmd_list->cmd_type == CMD_TYPE_BATCH) {
		ret = batch_cmd_io_send(nctri, cmd_list);
		if (ret) {
			RAWNAND_ERR("ndfc_execute_cmd, send batch cmd %d error!\n", c);
			return ret;
		}
		ret = _batch_cmd_io_wait(nctri, cmd_list);
		if (ret) {
			RAWNAND_ERR("ndfc_execute_cmd, wait batch cmd %d error!\n", c);
			return ret;
		}
	} else {
		RAWNAND_ERR("ndfc_execute_cmd, wrong cmd type, %u!\n", cmd_list->cmd_type);
		return ERR_NO_30;
	}

	return ret;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
s32 ndfc_get_spare_data(struct nand_controller_info *nctri, u8 *sbuf, u32 udata_cnt)
{
	s32 i;
	u32 cnt = 0;
	u32 val = 0;
	cnt = (udata_cnt / 4) + ((udata_cnt % 4) ? 1 : 0);

	if (sbuf == NULL) {
		RAWNAND_ERR("ndfc_get_spare_data, wrong input parameter!\n");
		return ERR_NO_29;
	}

	for (i = 0; i < cnt; i++) {
		val = readl(nctri->nreg.reg_user_data_base + i);
		sbuf[i * 4 + 0] = ((val >> 0) & 0xff);
		sbuf[i * 4 + 1] = ((val >> 8) & 0xff);
		sbuf[i * 4 + 2] = ((val >> 16) & 0xff);
		sbuf[i * 4 + 3] = ((val >> 24) & 0xff);
	}
	//RAWNAND_DBG("\n");

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
s32 ndfc_set_spare_data(struct nand_controller_info *nctri, u8 *sbuf, u32 udata_cnt)
{
	s32 i = 0;
	u32 val = 0;

	if (sbuf == NULL) {
		RAWNAND_ERR("ndfc set spare data, wrong input parameter!\n");
		return ERR_NO_28;
	}

	for (i = 0; i < udata_cnt; i = i + 4) {
		val = (sbuf[i + 3] << 24 | sbuf[i + 2] << 16 | sbuf[i + 1] << 8 | sbuf[i]);
		writel(val, (nctri->nreg.reg_user_data_base + (i >> 2)));
	}
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         : udata_cnt is 4 bytes aligned
 *****************************************************************************/
s32 ndfc_set_user_data_len(struct nand_controller_info *nctri)
{
	s32 i;
	u32 cfg = 0;

	for (i = 0; i < MAX_ECC_BLK_CNT; i++) {
		cfg |= nctri->nctri_cmd_seq.udata_len_mode[i] << ((i & 0x07) << 2); //4 bit indecate one ecc block mode. a 32 register include 8 ecc block mode.
		if (((i + 1) & 0x07) == 0) {
			writel(cfg, nctri->nreg.reg_user_data_len_base + (i >> 3));
			cfg = 0;
		}
	}

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         : udata_cnt is 4 bytes aligned
 *****************************************************************************/
s32 ndfc_set_user_data_len_cfg(struct nand_controller_info *nctri, u32 udata_cnt)
{
	u32 udata_len[10] = {0, 4, 8, 12, 16, 20, 24, 28, 32};
	u32 i, ecc_block_cnt, last_udata_len;
	u8 last_udata_len_mode = 0;

	ecc_block_cnt = (udata_cnt + MAX_UDATA_LEN_FOR_ECCBLOCK - 1) / MAX_UDATA_LEN_FOR_ECCBLOCK;
	//ecc_block_cnt = (udata_cnt + MAX_UDATA_LEN_FOR_ECCBLOCK ) / MAX_UDATA_LEN_FOR_ECCBLOCK;
	last_udata_len = udata_cnt % MAX_UDATA_LEN_FOR_ECCBLOCK;
	if (udata_cnt && (!last_udata_len))
		last_udata_len = 32;

	for (i = 0; i < 10; i++) {
		if (last_udata_len == udata_len[i]) {
			last_udata_len_mode = i;
			break;
		}
	}

	for (i = 0; i < MAX_ECC_BLK_CNT; i++) {
		if (i < (ecc_block_cnt - 1))
			nctri->nctri_cmd_seq.udata_len_mode[i] = 8;
		else if (i == (ecc_block_cnt - 1))
			nctri->nctri_cmd_seq.udata_len_mode[i] = last_udata_len_mode;
		else
			nctri->nctri_cmd_seq.udata_len_mode[i] = 0;
	}

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         : udata_cnt is 4 bytes aligned
 *****************************************************************************/
s32 ndfc_set_user_data_len_cfg_4bytesper1k(struct nand_controller_info *nctri, u32 udata_cnt)
{

	u32 i = 0;
	u32 ecc_block_cnt;

	ecc_block_cnt = (udata_cnt + UDATA_LEN_FOR_4BYTESPER1K - 1) / UDATA_LEN_FOR_4BYTESPER1K;

	for (i = 0; i < MAX_ECC_BLK_CNT; i++) {
		if (i < ecc_block_cnt) {
			nctri->nctri_cmd_seq.udata_len_mode[i] = 1;
		} else {
			nctri->nctri_cmd_seq.udata_len_mode[i] = 0;
		}
	}

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int ndfc_is_toggle_interface(struct nand_controller_info *nctri)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_ctl);
	if (cfg & NDFC_TOG_DDR_TYPE) {
		return 1;
	}
	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int ndfc_set_legacy_interface(struct nand_controller_info *nctri)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_ctl);
	cfg &= ~NDFC_NF_TYPE;
	writel(cfg, nctri->nreg.reg_ctl);

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int ndfc_set_toggle_interface(struct nand_controller_info *nctri)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_ctl);
	cfg |= NDFC_TOG_DDR_TYPE;
	writel(cfg, nctri->nreg.reg_ctl);

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
int ndfc_set_dummy_byte(struct nand_controller_info *nctri, int dummy_byte)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_efr);
	cfg &= (~NDFC_DUMMY_BYTE_CNT);
	cfg |= (dummy_byte & 0xff) << 16;

	writel(cfg, nctri->nreg.reg_efr);

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
void ndfc_enable_dummy_byte(struct nand_controller_info *nctri)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_efr);
	cfg |= (1 << 24);
	writel(cfg, nctri->nreg.reg_efr);
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
void ndfc_disable_dummy_byte(struct nand_controller_info *nctri)
{
	u32 cfg = 0;
	cfg = readl(nctri->nreg.reg_efr);
	cfg &= ~(1 << 24);
	writel(cfg, nctri->nreg.reg_efr);
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
void ndfc_get_nand_interface(struct nand_controller_info *nctri, u32 *pddr_type, u32 *psdr_edo, u32 *pddr_edo, u32 *pddr_delay)
{
	u32 cfg = 0;

	/* ddr type */
	cfg = readl(nctri->nreg.reg_ctl);
	*pddr_type = (cfg & NDFC_NF_TYPE) >> 18;
	if (nctri->type == NDFC_VERSION_V2)
		*pddr_type |= (((cfg >> 28) & 0x1) << 4);

	/* edo && delay */
	cfg = readl(nctri->nreg.reg_timing_ctl);
	*psdr_edo = (cfg >> 8) & 0x3;
	*pddr_edo = (cfg >> 8) & 0xf;
	*pddr_delay = cfg & NDFC_DC_CTL;

	return;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
s32 ndfc_change_nand_interface(struct nand_controller_info *nctri, u32 ddr_type, u32 sdr_edo, u32 ddr_edo, u32 ddr_delay)
{
	u32 cfg = 0;

	//printf("****ddr_type: %u, sdr_edo: %u, ddr_edo: %u, ddr_delay: %u\n", ddr_type, sdr_edo, ddr_edo, ddr_delay);
	/* ddr type */
	cfg = readl(nctri->nreg.reg_ctl);
	cfg &= ~NDFC_NF_TYPE;
	cfg |= (ddr_type & 0x3) << 18;
	if (nctri->type == NDFC_VERSION_V2) {
		cfg &= ~(0x1 << 28);
		cfg |= ((ddr_type >> 4) & 0x1) << 28;
	}
	writel(cfg, nctri->nreg.reg_ctl);

	/* edo && delay */
	cfg = readl(nctri->nreg.reg_timing_ctl);
	cfg &= ~((0xf << 8) | 0x3f);
	if (ddr_type == 0) {
		cfg |= (sdr_edo << 8);
	} else {
		cfg |= (ddr_edo << 8);
		cfg |= ddr_delay;
	}
	writel(cfg, nctri->nreg.reg_timing_ctl);

	/*
	   ndfc's timing cfg
	   1. default value: 0x95
	   2. bit-16, tCCS=1 for micron l85a, nvddr-100mhz
	   */
	/**nctri->nreg.reg_timing_cfg = 0x10095;*/
	writel(0x10095, nctri->nreg.reg_timing_cfg);

	return 0;
}

/*****************************************************************************
 *Name         : ndfc_emce_ctrl_setting
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_emce_ctrl_setting(struct nand_controller_info *nctri, u32 enable_flag)
{
	if (enable_flag)
		writel(1, nctri->nreg.reg_emce_ctl);
	else
		writel(0, nctri->nreg.reg_emce_ctl);
}

/*****************************************************************************
 *Name         : ndfc_emce_fac_compare_value_setting
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_emce_fac_compare_value_setting(struct nand_controller_info *nctri, u32 value)
{
	writel(value, nctri->nreg.reg_emce_iv_fac_cmp_val);
}

/*****************************************************************************
 *Name         : ndfc_emce_calculate_factor_setting_for_serial_addr
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_emce_calculate_factor_setting_for_serial_addr(struct nand_controller_info *nctri, u32 sector_addr, u32 sector_mapping)
{
	int i = 0;
	for (i = 0; i < 32; i++) {
		if ((sector_mapping >> i) & 0x01) {
			//need encryption sector
			writel(sector_addr + i, nctri->nreg.reg_emce_iv_cal_fac_base);
		} else {
			//not need encryption sector
			writel(readl(nctri->nreg.reg_emce_iv_fac_cmp_val),
					nctri->nreg.reg_emce_iv_cal_fac_base);
		}
	}
}

/*****************************************************************************
 *Name         : ndfc_emce_calculate_factor_setting_for_serial_addr
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void ndfc_emce_calculate_factor_setting(struct nand_controller_info *nctri, u32 *values, u32 offset_start, u32 lenth)
{
	int i = offset_start;
	if ((offset_start + lenth) > 32) {
		return;
	}

	for (i = offset_start; i < offset_start + lenth; i++) {
		writel(values[i], nctri->nreg.reg_emce_iv_cal_fac_base);
	}
}

/*****************************************************************************
 *Name         :
 *Description  :set defualt timming  config base on timming controller
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
void _set_ndfc_def_timing_param(struct nand_chip_info *nci)
{
	u32 reg_val, sync_mode;

	//reg_val = NDFC_READ_REG_CTL();
	reg_val = readl(nci->nctri->nreg.reg_ctl);
	sync_mode = (reg_val & NDFC_NF_TYPE) >> 18;
	/*sync_mode |= (((reg_val >> 28) & 0x1) << 4);*/

	if (sync_mode == 0) { //async

		reg_val = 0;
		reg_val |= 0x1 << 16; //tCCS
		reg_val |= 0x0 << 14; //tCLHZ
		reg_val |= 0x0 << 12; //tCS
		reg_val |= 0x0 << 11; //tCDQSS
		reg_val |= 0x1 << 8;  //tCAD
		reg_val |= 0x0 << 6;  //tRHW
		reg_val |= 0x0 << 4;  //tWHR
		reg_val |= 0x0 << 2;  //tADL
		reg_val |= 0x0;       //<<0;  //tWB
	} else if (sync_mode == 2) {  //onfi ddr
		reg_val = 0;
		reg_val |= 0x1 << 16; //tCCS
		reg_val |= 0x0 << 14; //tCLHZ
		reg_val |= 0x0 << 12; //tCS
		reg_val |= 0x0 << 11; //tCDQSS
		reg_val |= 0x1 << 8;  //tCAD
		reg_val |= 0x1 << 6;  //tRHW
		reg_val |= 0x0 << 4;  //tWHR
		reg_val |= 0x0 << 2;  //tADL
		reg_val |= 0x1;       //<<0;  //tWB
	} else if (sync_mode == 3) {  //toggle ddr
		reg_val = 0;
		reg_val |= 0x2 << 16; //tCCS
		reg_val |= 0x0 << 14; //tCLHZ
		reg_val |= 0x0 << 12; //tCS
		reg_val |= 0x0 << 11; //tCDQSS
		reg_val |= 0x4 << 8;  //tCAD
		reg_val |= 0x0 << 6;  //tRHW
		reg_val |= 0x0 << 4;  //tWHR
		reg_val |= 0x2 << 2;  //tADL
		reg_val |= 0x0;       //<<0;  //tWB
	} else {
		//fault
		RAWNAND_ERR("wrong interface , 0x%x\n", sync_mode);
	}

	//NDFC_WRITE_REG_TIMING_CFG(reg_val);
	writel(reg_val, nci->nctri->nreg.reg_timing_cfg);

	return;
}

/*****************************************************************************
 *Name         : _wait_dma_end
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
s32 ndfc_wait_dma_end(struct nand_controller_info *nctri, u8 rw, void *buff_addr, u32 len)
{
	s32 ret = 0;
	/*
	   while( (!(*nctri->nreg.reg_sta) & 0x4))
	   {
	   }
	 *nctri->nreg.reg_sta = 0x4;//songwj for debug,
	//NOTE: bu neng Or 0x04 Op
	*/

	if (nctri->write_wait_dma_mode != 0) {
		ndfc_enable_dma_int(nctri);
		if (nctri->dma_ready_flag == 1) {
			//goto wait_dma_ready_int;
			goto dma_int_end;
		}
		if (nand_dma_wait_time_out(nctri->channel_id, &nctri->dma_ready_flag) == 0) {
			RAWNAND_ERR("_wait_dma_ready_int int timeout, ch: 0x%x, sta: 0x%x\n",
					nctri->channel_id, readl(nctri->nreg.reg_sta));
		} else {
			goto dma_int_end;
		}
	}

	//wait_dma_ready_int:
	//nctri->dma_ready_flag = 0;
	ret = wait_reg_status(nctri->nreg.reg_sta, NDFC_DMA_INT_FLAG, NDFC_DMA_INT_FLAG, 0xffff);
	if (ret != 0) {
		RAWNAND_ERR("nand _wait_dma_end timeout, NandIndex: 0x%x, rw: 0x%x, status:0x%x\n",
				nctri->channel_id, (u32)rw, readl(nctri->nreg.reg_sta));
		//RAWNAND_DBG("DMA addr: 0x%x, DMA len: 0x%x\n", *nctri->nreg.reg_mbus_dma_addr, *nctri->nreg.reg_dma_cnt);
		//show_nctri(nctri);
		ndfc_print_save_reg(nctri);
		ndfc_print_reg(nctri);
		//return ret;
		ret = wait_reg_status(nctri->nreg.reg_sta, NDFC_DMA_INT_FLAG, NDFC_DMA_INT_FLAG, 0xffff);
		RAWNAND_DBG("ret !!!!:0x%x\n", ret);
		if (ret == 0) {
			ndfc_print_reg(nctri);
		}
	}

	ndfc_clear_dma_int(nctri);
	ndfc_disable_dma_int(nctri);

dma_int_end:

	writel((readl(nctri->nreg.reg_sta) & NDFC_DMA_INT_FLAG), nctri->nreg.reg_sta);
	nand_dma_unmap_single(&aw_ndfc, rw, (void *)(unsigned long)nctri->dma_addr, len);
	nand_invaild_dcache_region(rw, (unsigned long)nctri->dma_addr, len);

	return ret;
}

/*****************************************************************************
 *Name         : ndfc_check_read_data_sta
 *Description  :
 *Parameter    :
 *Return       : NULL
 *Note         :
 *****************************************************************************/
s32 ndfc_check_read_data_sta(struct nand_controller_info *nctri, u32 eblock_cnt)
{
	s32 i, flag;

	//check all '1' or all '0' status

	if (readl(nctri->nreg.reg_data_pattern_sta) == ((0x1U << (eblock_cnt - 1)) | ((0x1U << (eblock_cnt - 1)) - 1))) {
		flag = readl(nctri->nreg.reg_pat_id) & 0x01;
		for (i = 1; i < eblock_cnt; i++) {
			if (flag != ((readl(nctri->nreg.reg_pat_id) >> i) & 0x1))
				break;
		}

		if (i == eblock_cnt) {
			if (flag) {
				//RAWNAND_DBG("read data sta, all '1'\n");
				return 4; //all input bits are '1'
			} else {
				//RAWNAND_DBG("read data sta, all '0'\n");
				return 3; //all input bit are '0'
			}
		} else {
			RAWNAND_DBG("read data sta, some ecc blocks are all '1', others are all '0' %x %x\n", readl(nctri->nreg.reg_ecc_sta), readl(nctri->nreg.reg_pat_id));
			return 2;
		}
	} else if (readl(nctri->nreg.reg_data_pattern_sta) == 0x0) {
		return 0;
	} else {
		//RAWNAND_DBG("!!!!read data sta, only some ecc blocks are all '1' or all '1', 0x%x 0x%x\n", *nctri->nreg.reg_ecc_sta, *nctri->nreg.reg_pat_id);
		return 1;
	}
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
s32 ndfc_get_bad_blk_flag(u32 page_no, u32 dis_random, u32 slen, u8 *sbuf)
{
	u8 _1st_spare_byte[16];
	u32 i, len, num;

	num = page_no % 128;
	len = slen;

	if (len > 16)
		len = 16;

	//RAWNAND_DBG("ndfc get_bad_blk_flag, %d %d %d 0x%x\n", page_no, dis_random, slen, sbuf);
	if (sbuf == NULL) {
		RAWNAND_ERR("ndfc get_bad_blk_flag, input parameter error!\n");
		return ERR_NO_50;
	}

	for (i = 0; i < 16; i++) {
		_1st_spare_byte[i] = 0xff;
	}

	if (dis_random) {
		//  check some byte in spare data
		len = sizeof(rand_factor_1st_spare_data) / sizeof(rand_factor_1st_spare_data[0]);
		len = MIN(len, slen);
		for (i = 0; i < len; i++) {
			_1st_spare_byte[i] = sbuf[i] ^ rand_factor_1st_spare_data[i % len][num];
		}
	} else {
		for (i = 0; i < len; i++) {
			_1st_spare_byte[i] = sbuf[i];
		}
	}

	//if (_1st_spare_byte[0] == 0xff)
	if (is_nouse_page(_1st_spare_byte) == 1) {
		//RAWNAND_DBG("good page flag, page %d\n", page_no);
		//		if((page_no != 0)&&(page_no != 255))
		//		{
		//		    RAWNAND_DBG("blank page:%d\n",page_no);
		//		}

		return 0;
	} else {
		//RAWNAND_DBG("bad page flag, page %d\n", page_no);
		return ERR_NO_49;
	}
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
s32 ndfc_check_ecc(struct nand_controller_info *nctri, u32 eblock_cnt)
{
	u32 i, ecc_limit, cfg;
	u32 ecc_cnt_w[8];
	u8 ecc_cnt;
	//	u8 ecc_tab[16] = {16, 24, 28, 32, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80};
	u8 ecc_limit_tab[16] = {13, 20, 23, 27, 35, 39, 42, 46, 50, 54, 58, 62, 66, 68, 72};

	//	max_ecc_bit_cnt = ecc_tab[(*nctri->nreg.reg_ecc_ctl >> 8) & 0xff];
	ecc_limit = ecc_limit_tab[(*nctri->nreg.reg_ecc_ctl >> 8) & 0xff];

	//check ecc errors
	//cfg = (1<<eblock_cnt) - 1;

	ndfc_encode_select(nctri);

	if (nctri->channel_sel == 0) //BCH
		cfg = (1 << (eblock_cnt - 1)) | ((1 << (eblock_cnt - 1)) - 1);
	else //LDPC
		cfg = (1 << (eblock_cnt / 2 - 1)) | ((1 << (eblock_cnt / 2 - 1)) - 1);
	//RAWNAND_ERR("check ecc: %d 0x%x 0x%x\n", nctri->nci->randomizer, *nctri->nreg.reg_ecc_ctl, *nctri->nreg.reg_ecc_sta);
	if ((readl(nctri->nreg.reg_ecc_sta) & cfg) != 0) {
		return ERR_ECC;
	}

	//check ecc limit
	ecc_cnt_w[0] = readl(nctri->nreg.reg_err_cnt0);
	ecc_cnt_w[1] = readl(nctri->nreg.reg_err_cnt1);
	ecc_cnt_w[2] = readl(nctri->nreg.reg_err_cnt2);
	ecc_cnt_w[3] = readl(nctri->nreg.reg_err_cnt3);
	ecc_cnt_w[4] = readl(nctri->nreg.reg_err_cnt4);
	ecc_cnt_w[5] = readl(nctri->nreg.reg_err_cnt5);
	ecc_cnt_w[6] = readl(nctri->nreg.reg_err_cnt6);
	ecc_cnt_w[7] = readl(nctri->nreg.reg_err_cnt7);

	if (nctri->channel_sel == 0) {
		for (i = 0; i < eblock_cnt; i++) {
			ecc_cnt = (u8)(ecc_cnt_w[i >> 2] >> ((i % 4) << 3));

			if (ecc_cnt > ecc_limit) {
				//RAWNAND_DBG("ecc limit: 0x%x 0x%x 0x%x 0x%x   %d  %d  %d\n", ecc_cnt_w[0],ecc_cnt_w[1],ecc_cnt_w[2],ecc_cnt_w[3],ecc_cnt,ecc_limit,max_ecc_bit_cnt);
				return ECC_LIMIT;
			}
		}
	} else {
		for (i = 0; i < eblock_cnt; i += 2) {
			if (i % 4 == 0)
				ecc_cnt = (u16)(ecc_cnt_w[i >> 2]);
			else
				ecc_cnt = (u16)(ecc_cnt_w[i >> 2] >> 16);

			if (ecc_cnt > 256) { //LDPC most can fix 511 err bits
				//RAWNAND_DBG("ecc limit : ecc_cnt_w[%d] = 0x%x\n", (i>>2), ecc_cnt_w[i>>2],);
				return ECC_LIMIT;
			}
		}
	}

	return 0;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
s32 ndfc_update_ecc_sta_and_spare_data(struct _nand_physic_op_par *npo, s32 ecc_sta, unsigned char *sbuf)
{
	unsigned char *buf;
	unsigned int len;
	s32 ret_ecc_sta, ret1 = 0;
	struct nand_chip_info *nci = nci_get_from_nsi(g_nsi, npo->chip);

	if (npo->slen != 0) {
		buf = npo->sdata;
		len = npo->slen;
	} else {
		buf = sbuf;
		len = nci->sdata_bytes_per_page;
	}

	if (npo->slen > nci->sdata_bytes_per_page) {
		len = nci->sdata_bytes_per_page;
	}

	//check input data status
	ret1 = ndfc_check_read_data_sta(nci->nctri, nci->sector_cnt_per_page >> nci->ecc_sector);

	ret_ecc_sta = ecc_sta;

	if (nci->randomizer) {
		if (ecc_sta == ERR_ECC) {
			//RAWNAND_DBG("update ecc, %d %d %d 0x%x\n", npo->page, nci->randomizer, npo->slen, npo->sdata);
			if (ndfc_get_bad_blk_flag(npo->page, nci->randomizer, len, buf) == 0) {
				//when the 1st byte of spare data is 0xff after randomization, this page is blank
				//RAWNAND_DBG("randomizer blank page\n");
				memset(buf, 0xff, len);
				ret_ecc_sta = 0;
			}
		}

		if (ret1 == 3) {
			//all data bits are '0', no ecc error, this page is a bad page
			//RAWNAND_DBG("randomizer all bit 0\n");
			memset(buf, 0x0, len);
			ret_ecc_sta = ERR_ECC;
		}
	} else {
		if (ret1 == 3) {
			//all data bits are '0', don't do ecc(ecc exception), this page is a bad page
			//RAWNAND_DBG("no randomizer all bit 0\n");
			ret_ecc_sta = ERR_ECC;
		} else if (ret1 == 4) {
			//all data bits are '1', don't do ecc(ecc exception), this page is a good page
			//RAWNAND_DBG("no randomizer all bit 1\n");
			ret_ecc_sta = 0;
		}
	}

	return ret_ecc_sta;
}

/*****************************************************************************
 *Name         :
 *Description  :
 *Parameter    :
 *Return       : 0:ok  -1:fail
 *Note         :
 *****************************************************************************/
void do_nand_interrupt(u32 no)
{
	struct nand_controller_info *nctri = nctri_get(g_nctri, no);

	//	u32 rb = ndfc_get_selected_rb_no(nctri);
	ndfc_get_selected_rb_no(nctri);

	if (ndfc_check_rb_b2r_int_occur(nctri) != 0) {
		ndfc_clear_rb_b2r_int(nctri);

		ndfc_disable_rb_b2r_int(nctri);

		nctri->rb_ready_flag = 1;

		//        RAWNAND_DBG("do %d\n",no);

		nand_rb_wake_up(no);
	}

	if (nctri->write_wait_dma_mode != 0) {
		if (ndfc_check_dma_int_occur(nctri) != 0) {
			//		nctri->dma_ready_flag = 1;
			ndfc_clear_dma_int(nctri);
			//
			ndfc_disable_dma_int(nctri);
			//
			nctri->dma_ready_flag = 1;
			//
			nand_dma_wake_up(no);
		}
	}
}

/*
 *struct ndfc_df_func_ops ndfc_df_ops= {
 *#if defined(CONFIG_ARCH_SUN8IW18)
 *    .init_nctri = init_nctri_v2px,
 *    .save_nctri = save_nctri_v2px,
 *    .fill_nctri = fill_nctri_v2px,
 *    .recover_nctri = recover_nctri_v2px,
 *    .ndfc_print_reg = ndfc_print_reg_v2px,
 *    .ndfc_print_save_reg = ndfc_print_save_reg_v2px,
 *    .ndfc_soft_reset = ndfc_soft_reset_v2px,
 *    .ndfc_encode_select = ndfc_encode_select_v2px,
 *    .ndfc_encode_default = ndfc_encode_default_v2px,
 *    .ndfc_channel_select = ndfc_channel_select_v2px,
 *    .batch_cmd_io_send = batch_cmd_io_send_v2px,
 *    .ndfc_dma_config_start = ndfc_dma_config_start_v2px,
 *
 *#else
 *    .init_nctri = init_nctri_v1px,
 *    .save_nctri = save_nctri_v1px,
 *    .fill_nctri = fill_nctri_v1px,
 *    .recover_nctri = recover_nctri_v1px,
 *    .ndfc_print_reg = ndfc_print_reg_v1px,
 *    .ndfc_print_save_reg = ndfc_print_save_reg_v1px,
 *    .ndfc_soft_reset = ndfc_soft_reset_v1px,
 *    .ndfc_encode_select = ndfc_encode_select_v1px,
 *    .ndfc_encode_default = ndfc_encode_default_v1px,
 *    .ndfc_channel_select = ndfc_channel_select_v1px,
 *    .batch_cmd_io_send = batch_cmd_io_send_v1px,
 *    .ndfc_dma_config_start = ndfc_dma_config_start_v1px,
 *
 *#endif
 *};
 */
