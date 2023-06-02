/* SPDX-License-Identifier: GPL-2.0 */
/*****************************************************************************/
#define _BUILD_PHY_PARTITION_C_
/*****************************************************************************/

#include "build_phy_partition_v2.h"
#include "../nand-partition/build_phy_partition.h"
#include "../nand-partition/phy.h"
#include "../nand_nftl.h"
#include "../nand_physic_interface.h"
/*****************************************************************************/

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/
/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/
void print_partition(struct _partition *partition)
{
	int i;

	NFTL_DBG("[ND]partition->size :%d\n", partition->size);
	NFTL_DBG("[ND]partition->cross_talk :%d\n", partition->cross_talk);
	NFTL_DBG("[ND]partition->attribute :%d\n", partition->attribute);

	NFTL_DBG("[ND]partition->start.Chip_NO :%d\n", partition->start.Chip_NO);
	NFTL_DBG("[ND]partition->start.Block_NO :%d\n", partition->start.Block_NO);
	NFTL_DBG("[ND]partition->end.Chip_NO :%d\n", partition->end.Chip_NO);
	NFTL_DBG("[ND]partition->end.Block_NO :%d\n", partition->end.Block_NO);

	for (i = 0; i < MAX_PART_COUNT_PER_FTL; i++) {
		//NFTL_DBG("[ND]partition->nand_disk[i].name :%s\n",partition->nand_disk[i].name);
		NFTL_DBG("[ND]partition->nand_disk[%d].size :%d\n", i, partition->nand_disk[i].size);
		NFTL_DBG("[ND]partition->nand_disk[%d].type :%d\n", i, partition->nand_disk[i].type);
	}
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :
*Return       :
*Note         :
*****************************************************************************/
void change_partition(struct _partition *partition)
{
	uint32 size = 0, i;

	for (i = 0; i < MAX_PART_COUNT_PER_FTL; i++) {
		if ((partition->nand_disk[i].size != 0) && (partition->nand_disk[i].type != 0xffffffff)) {
			size += partition->nand_disk[i].size;
		}
		if ((partition->nand_disk[i].size == 0) && (partition->nand_disk[i].type != 0xffffffff)) {
			partition->nand_disk[i].size = partition->size - size;
			break;
		}
	}
}
int get_max_free_block_num(struct _nand_info *nand_info, struct _nand_phy_partition *phy_partition)
{
	int max_free_block_num, sector_per_block;
	uint32 total_sector;

	sector_per_block = phy_partition->PageNumsPerBlk * phy_partition->SectorNumsPerPage;
	total_sector = sector_per_block * nand_info->BlkPerChip;
	total_sector *= nand_info->ChipNum;

	if (total_sector <= 0x64000) { //less than 200MB {

		max_free_block_num = 40;
	} else if (total_sector <= 0x96000) { //less than 300MB {

		max_free_block_num = 85;
	} else if (total_sector <= 0x12c000) { //less than 600MB {

		if (sector_per_block >= 2048) {
			max_free_block_num = 40;
		} else if (sector_per_block >= 1024) {
			max_free_block_num = 80;
		} else {
			max_free_block_num = 180;
		}
	} else if (total_sector <= 0x258000) { //less than 1200MB{

		if (sector_per_block >= 1024) {
			max_free_block_num = 170;
		} else {
			max_free_block_num = 320;
		}
	} else if (total_sector <= 0xa00000) { //less than 5GB {

		if (sector_per_block >= 16384) {
			max_free_block_num = 50;
		} else if (sector_per_block >= 8182) {
			max_free_block_num = 75;
		} else if (sector_per_block >= 4096) {
			max_free_block_num = 170;
		} else {
			max_free_block_num = 300;
		}
	} else if (total_sector <= 0x1400000) { //less than 10GB{

		if (sector_per_block >= 16384) {
			max_free_block_num = 75;
		} else if (sector_per_block >= 8182) {
			max_free_block_num = 160;
		} else if (sector_per_block >= 4096) {
			max_free_block_num = 300;
		} else {
			max_free_block_num = 600;
		}
	} else if (total_sector <= 0x2800000) { //less than 20GB {

		if (sector_per_block >= 32768) {
			max_free_block_num = 70;
		} else if (sector_per_block >= 16384) {
			max_free_block_num = 150;
		} else if (sector_per_block >= 8182) {
			max_free_block_num = 160;
		} else {
			max_free_block_num = 240;
		}
	} else { //32G {

		if (sector_per_block >= 32768) {
			max_free_block_num = 140;
		} else if (sector_per_block >= 16384) {
			max_free_block_num = 250;
		} else {
			max_free_block_num = 500;
		}
	}

	return max_free_block_num;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :logic_size:sectors
*Return       :
*Note         :
*****************************************************************************/
struct _nand_phy_partition *mp_build_phy_partition(struct _nand_info *nand_info, struct _partition *part)
{
	struct _nand_phy_partition *phy_partition;
	struct _nand_phy_partition *p;
	uint32 total_blocks, temp1, temp2, factory_bad_count, new_bad_count;
	unsigned short nDieNum, nBlkNum, PartitionNO, last_phy_partition;
	int max_free_block_num;

	phy_partition = (struct _nand_phy_partition *)nftl_malloc(sizeof(struct _nand_phy_partition));
	if (phy_partition == NULL) {
		NFTL_ERR("[NE]%s:malloc fail for phy_partition\n", __func__);
		return NULL;
	}

	phy_partition->next_phy_partition = NULL;
	if (nand_info->phy_partition_head == NULL) {
		nand_info->phy_partition_head = phy_partition;
		phy_partition->PartitionNO = 0;
		phy_partition->StartBlock.Chip_NO = nand_info->no_used_block_addr.Chip_NO;
		phy_partition->StartBlock.Block_NO = nand_info->no_used_block_addr.Block_NO;
	} else {
		PartitionNO = 1;
		p = nand_info->phy_partition_head;
		while (p->next_phy_partition != NULL) {
			p = p->next_phy_partition;
			PartitionNO++;
		}
		p->next_phy_partition = phy_partition;
		phy_partition->PartitionNO = PartitionNO;

		phy_partition->StartBlock.Chip_NO = p->EndBlock.Chip_NO;
		phy_partition->StartBlock.Block_NO = p->EndBlock.Block_NO + 1;

		while (phy_partition->StartBlock.Block_NO >= nand_info->BlkPerChip) {
			phy_partition->StartBlock.Chip_NO++;
			phy_partition->StartBlock.Block_NO -= nand_info->BlkPerChip;
			if (phy_partition->StartBlock.Chip_NO >= nand_info->ChipNum) {
				NFTL_ERR("[NE]no block2 %d\n", phy_partition->StartBlock.Chip_NO);
				return NULL;
			}
		}
	}

	phy_partition->Attribute = part->attribute;
	phy_partition->CrossTalk = part->cross_talk;
	phy_partition->page_read = page_read;
	phy_partition->page_write = page_write;
	phy_partition->block_erase = block_erase;

	phy_partition->nand_info = nand_info;
	phy_partition->SectorNumsPerPage = nand_info->SectorNumsPerPage;
	phy_partition->BytesUserData = nand_info->BytesUserData;
	phy_partition->PageNumsPerBlk = nand_info->PageNumsPerBlk;
	phy_partition->FullBitmapPerPage = phy_partition->SectorNumsPerPage;

	phy_partition->EndBlock.Chip_NO = phy_partition->StartBlock.Chip_NO;
	phy_partition->EndBlock.Block_NO = phy_partition->StartBlock.Block_NO;

	/////////////////////////////////////////////////////////////////

	if (part->size != 0xffffffff) {
		phy_partition->TotalSectors = part->size;
		temp1 = part->size / phy_partition->SectorNumsPerPage;
		temp1 /= phy_partition->PageNumsPerBlk;
		total_blocks = temp1 + temp1 / phy_partition->PageNumsPerBlk; //logic block num

		NFTL_DBG("[NE]phy_partition Attribute %d\n", phy_partition->Attribute);
		if (phy_partition->Attribute == 0) {

			temp2 = temp1 / (NORM_RESERVED_BLOCK_RATIO + nand_info->capacity_level * 3);
		} else {
			temp2 = temp1 / (SYS_RESERVED_BLOCK_RATIO + nand_info->capacity_level * 3);
		}

		if (temp2 < nand_info->mini_free_block_first_reserved) {
			temp2 = nand_info->mini_free_block_first_reserved;
		}

		max_free_block_num = get_max_free_block_num(nand_info, phy_partition);
		if (temp2 > max_free_block_num) {
			temp2 = max_free_block_num;
		}
		total_blocks += temp2; // add nftl free block

		phy_partition->FreeBlock = MIN_PHY_RESERVED_BLOCK_V2;

		total_blocks += phy_partition->FreeBlock; // add phy free block
		last_phy_partition = 0;
	} else {
		total_blocks = 0xffffffff;
		last_phy_partition = 1;
	}

	/////////////////////////////////////////////////////////////////

	phy_partition->TotalBlkNum = 0;
	phy_partition->factory_bad_block = nftl_malloc(FACTORY_BAD_BLOCK_SIZE);
	if (phy_partition->factory_bad_block == NULL) {
		NFTL_ERR("[NE]%s:malloc fail for factory_bad_block\n", __func__);
		return NULL;
	}

	memset(phy_partition->factory_bad_block, 0xff, FACTORY_BAD_BLOCK_SIZE);

	phy_partition->new_bad_block = nftl_malloc(PHY_PARTITION_BAD_BLOCK_SIZE);
	if (phy_partition->new_bad_block == NULL) {
		NFTL_ERR("[NE]%s:malloc fail for new_bad_block\n", __func__);
		return NULL;
	}

	memset(phy_partition->new_bad_block, 0xff, PHY_PARTITION_BAD_BLOCK_SIZE);

	factory_bad_count = 0;
	new_bad_count = 0;
	nDieNum = phy_partition->StartBlock.Chip_NO;
	nBlkNum = phy_partition->StartBlock.Block_NO;
	while (total_blocks != 0) {
		if (nand_info->FirstBuild == 1) {
			if (BlockCheck(nDieNum, nBlkNum) == 0) {
				total_blocks--;
			} else {
				phy_partition->factory_bad_block[factory_bad_count].Chip_NO = nDieNum;
				phy_partition->factory_bad_block[factory_bad_count].Block_NO = nBlkNum;
				factory_bad_count++;
				if (factory_bad_count == FACTORY_BAD_BLOCK_SIZE / sizeof(struct _nand_super_block)) {
					NFTL_ERR("[NE]too much bad block %d\n", factory_bad_count);
					nftl_free(phy_partition);
					return NULL;
				}
			}
		} else {
			if (is_factory_bad_block(nand_info, nDieNum, nBlkNum) == 1) {
				NFTL_DBG("[ND]factory bad block:%d,%d PartitionNO:%d\n", nDieNum, nBlkNum, phy_partition->PartitionNO);
				phy_partition->factory_bad_block[factory_bad_count].Chip_NO = nDieNum;
				phy_partition->factory_bad_block[factory_bad_count].Block_NO = nBlkNum;
				factory_bad_count++;
				if (factory_bad_count == FACTORY_BAD_BLOCK_SIZE / sizeof(struct _nand_super_block)) {
					NFTL_ERR("[NE]too much factory bad block %d\n", factory_bad_count);
					nftl_free(phy_partition);
					return NULL;
				}
			} else if (is_new_bad_block(nand_info, nDieNum, nBlkNum) == 1) {
				NFTL_DBG("[ND]new bad block:%d,%d PartitionNO:%d\n", nDieNum, nBlkNum, phy_partition->PartitionNO);
				phy_partition->new_bad_block[new_bad_count].Chip_NO = nDieNum;
				phy_partition->new_bad_block[new_bad_count].Block_NO = nBlkNum;
				new_bad_count++;
				if (new_bad_count == PHY_PARTITION_BAD_BLOCK_SIZE / sizeof(struct _nand_super_block)) {
					NFTL_ERR("[NE]too much new bad block %d\n", factory_bad_count);
					nftl_free(phy_partition);
					return NULL;
				}
			} else {
				total_blocks--;
			}
		}
		phy_partition->EndBlock.Chip_NO = nDieNum;
		phy_partition->EndBlock.Block_NO = nBlkNum;
		phy_partition->TotalBlkNum++;
		nBlkNum++;
		if (nBlkNum == nand_info->BlkPerChip) {
			nBlkNum = 0;
			nDieNum++;
			if (nDieNum == nand_info->ChipNum) {
				break;
			}
		}
	}

	if (last_phy_partition == 1) {
		total_blocks = phy_partition->TotalBlkNum - factory_bad_count;

		phy_partition->FreeBlock = MIN_PHY_RESERVED_BLOCK_V2;
		total_blocks -= phy_partition->FreeBlock;

		if (phy_partition->Attribute == 0) {
			temp2 = total_blocks / (NORM_RESERVED_BLOCK_RATIO + nand_info->capacity_level * 3);
		} else {
			temp2 = total_blocks / (SYS_RESERVED_BLOCK_RATIO + nand_info->capacity_level * 3);
		}

		if (temp2 < nand_info->mini_free_block_first_reserved) {
			temp2 = nand_info->mini_free_block_first_reserved;
		}

		max_free_block_num = get_max_free_block_num(nand_info, phy_partition);
		if (temp2 > max_free_block_num) {
			temp2 = max_free_block_num;
		}
		total_blocks -= temp2;

		temp1 = total_blocks * phy_partition->PageNumsPerBlk;
		temp1 *= phy_partition->SectorNumsPerPage;
		phy_partition->TotalSectors = temp1;
		part->size = phy_partition->TotalSectors;
	}

	if (nand_info->FirstBuild == 1) {
		put_factory_bad_block(nand_info, phy_partition);
	}

	phy_partition->disk = nand_info->partition[phy_partition->PartitionNO].nand_disk;

	part->start.Chip_NO = phy_partition->StartBlock.Chip_NO;
	part->start.Block_NO = phy_partition->StartBlock.Block_NO;

	part->end.Chip_NO = phy_partition->EndBlock.Chip_NO;
	part->end.Block_NO = phy_partition->EndBlock.Block_NO;

	change_partition(part);
	print_partition(part);

	print_phy_partition(phy_partition);
	print_all_bad_block(phy_partition);

	return phy_partition;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :logic_size:sectors
*Return       :
*Note         :
*****************************************************************************/
struct _nand_phy_partition *launch_build_phy_partition(struct _nand_info *nand_info, struct _partition *part)
{
	struct _nand_phy_partition *phy_partition;
	struct _nand_phy_partition *p;
	uint32 temp1, factory_bad_count, new_bad_count;
	unsigned short nDieNum, nBlkNum, PartitionNO;
	//    int max_free_block_num;

	phy_partition = (struct _nand_phy_partition *)nftl_malloc(sizeof(struct _nand_phy_partition));
	if (phy_partition == NULL) {
		NFTL_ERR("[NE]%s:malloc fail for phy_partition\n", __func__);
		return NULL;
	}

	phy_partition->next_phy_partition = NULL;
	if (nand_info->phy_partition_head == NULL) {
		nand_info->phy_partition_head = phy_partition;
		phy_partition->PartitionNO = 0;
	} else {
		PartitionNO = 1;
		p = nand_info->phy_partition_head;
		while (p->next_phy_partition != NULL) {
			p = p->next_phy_partition;
			PartitionNO++;
		}
		p->next_phy_partition = phy_partition;
		phy_partition->PartitionNO = PartitionNO;
	}

	phy_partition->StartBlock.Chip_NO = part->start.Chip_NO;
	phy_partition->StartBlock.Block_NO = part->start.Block_NO;
	phy_partition->EndBlock.Chip_NO = part->end.Chip_NO;
	phy_partition->EndBlock.Block_NO = part->end.Block_NO;
	phy_partition->TotalSectors = part->size;
	phy_partition->Attribute = part->attribute;
	phy_partition->CrossTalk = part->cross_talk;
	phy_partition->page_read = page_read;
	phy_partition->page_write = page_write;
	phy_partition->block_erase = block_erase;
	phy_partition->disk = part->nand_disk;

	phy_partition->nand_info = nand_info;
	phy_partition->SectorNumsPerPage = nand_info->SectorNumsPerPage;
	phy_partition->BytesUserData = nand_info->BytesUserData;
	phy_partition->PageNumsPerBlk = nand_info->PageNumsPerBlk;
	phy_partition->FullBitmapPerPage = phy_partition->SectorNumsPerPage;
	phy_partition->FreeBlock = MIN_PHY_RESERVED_BLOCK_V2;
	phy_partition->TotalBlkNum = 0;
	phy_partition->GoodBlockNum = 0;

	phy_partition->factory_bad_block = nftl_malloc(FACTORY_BAD_BLOCK_SIZE);
	if (phy_partition->factory_bad_block == NULL) {
		NFTL_ERR("[NE]%s:malloc fail for factory_bad_block\n", __func__);
		return NULL;
	}

	memset(phy_partition->factory_bad_block, 0xff, FACTORY_BAD_BLOCK_SIZE);

	phy_partition->new_bad_block = nftl_malloc(PHY_PARTITION_BAD_BLOCK_SIZE);
	if (phy_partition->new_bad_block == NULL) {
		NFTL_ERR("[NE]%s:malloc fail for new_bad_block\n", __func__);
		return NULL;
	}

	memset(phy_partition->new_bad_block, 0xff, PHY_PARTITION_BAD_BLOCK_SIZE);

	factory_bad_count = 0;
	new_bad_count = 0;
	nDieNum = phy_partition->StartBlock.Chip_NO;
	nBlkNum = phy_partition->StartBlock.Block_NO;
	while (1) {
		if (is_factory_bad_block(nand_info, nDieNum, nBlkNum) == 1) {
			NFTL_DBG("[ND]factory bad block:%d,%d PartitionNO:%d\n", nDieNum, nBlkNum, phy_partition->PartitionNO);
			phy_partition->factory_bad_block[factory_bad_count].Chip_NO = nDieNum;
			phy_partition->factory_bad_block[factory_bad_count].Block_NO = nBlkNum;
			factory_bad_count++;
			if (factory_bad_count == FACTORY_BAD_BLOCK_SIZE / sizeof(struct _nand_super_block)) {
				NFTL_ERR("[NE]too much factory bad block %d\n", factory_bad_count);
				nftl_free(phy_partition);
				return NULL;
			}
		} else if (is_new_bad_block(nand_info, nDieNum, nBlkNum) == 1) {
			NFTL_DBG("[ND]new bad block:%d,%d PartitionNO:%d\n", nDieNum, nBlkNum, phy_partition->PartitionNO);
			phy_partition->new_bad_block[new_bad_count].Chip_NO = nDieNum;
			phy_partition->new_bad_block[new_bad_count].Block_NO = nBlkNum;
			new_bad_count++;
			if (new_bad_count == PHY_PARTITION_BAD_BLOCK_SIZE / sizeof(struct _nand_super_block)) {
				NFTL_ERR("[NE]too much new bad block %d\n", factory_bad_count);
				nftl_free(phy_partition);
				return NULL;
			}
		} else {
			phy_partition->GoodBlockNum++;
		}
		phy_partition->TotalBlkNum++;

		if ((nDieNum == phy_partition->EndBlock.Chip_NO) && (nBlkNum == phy_partition->EndBlock.Block_NO)) {
			break;
		}
		nBlkNum++;
		if (nBlkNum == nand_info->BlkPerChip) {
			nBlkNum = 0;
			nDieNum++;
			if (nDieNum == nand_info->ChipNum) {
				break;
			}
		}
	}

	//check
	temp1 = phy_partition->TotalSectors / phy_partition->SectorNumsPerPage;
	temp1 /= phy_partition->PageNumsPerBlk;

	if (phy_partition->GoodBlockNum > temp1) {
		if ((phy_partition->GoodBlockNum - temp1) < MIN_NAND_Free_BLOCK_NUM_RUNNING) {
			NFTL_ERR("[ND]not enough block:%d,%d!\n", phy_partition->GoodBlockNum, temp1);
			nftl_free(phy_partition);
			return NULL;
		}
	} else {
		NFTL_ERR("[ND]not enough block:%d,%d!!\n", phy_partition->GoodBlockNum, temp1);
		nftl_free(phy_partition);
		return NULL;
	}
	print_partition(part);

	print_phy_partition(phy_partition);
	print_all_bad_block(phy_partition);

	return phy_partition;
}

/*****************************************************************************
*Name         :
*Description  :
*Parameter    :logic_size:sectors
*Return       :
*Note         :
*****************************************************************************/
struct _nand_phy_partition *build_phy_partition_v2(struct _nand_info *nand_info, struct _partition *part)
{
	if (nand_info->FirstBuild == 1) {
		return mp_build_phy_partition(nand_info, part);
	} else {
		return launch_build_phy_partition(nand_info, part);
	}
}
