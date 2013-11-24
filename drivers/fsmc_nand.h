/*
 * File      : fsmc_nand.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2012, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author         Notes
 * 2012-09-01     heyuanjie87    the first version
 */
 
#ifndef __FSMC_NAND_H__
#define __FSMC_NAND_H__

#include <rtthread.h>
#include <rtdevice.h>

 
 
#define CMD_AREA                   (uint32_t)(0x010000)  /* A16 = CLE  high */
#define ADDR_AREA                  (uint32_t)(0x020000)  /* A17 = ALE high */
#define DATA_AREA                  (uint32_t)(0x000000)

/* FSMC NAND memory command */
#define	NAND_CMD_READ_1             ((uint8_t)0x00)
#define	NAND_CMD_READ_TRUE          ((uint8_t)0x30)

#define	NAND_CMD_RDCOPYBACK         ((uint8_t)0x00)
#define	NAND_CMD_RDCOPYBACK_TRUE    ((uint8_t)0x35)

#define NAND_CMD_PAGEPROGRAM        ((uint8_t)0x80)
#define NAND_CMD_PAGEPROGRAM_TRUE   ((uint8_t)0x10)

#define NAND_CMD_COPYBACKPGM        ((uint8_t)0x85)
#define NAND_CMD_COPYBACKPGM_TRUE   ((uint8_t)0x10)

#define NAND_CMD_ERASE0             ((uint8_t)0x60)
#define NAND_CMD_ERASE1             ((uint8_t)0xD0)

#define NAND_CMD_READID             ((uint8_t)0x90)
#define NAND_CMD_STATUS             ((uint8_t)0x70)
#define NAND_CMD_RESET              ((uint8_t)0xFF)

#define NAND_CMD_CACHEPGM           ((uint8_t)0x80)
#define NAND_CMD_CACHEPGM_TRUE      ((uint8_t)0x15)

#define NAND_CMD_RANDOMIN           ((uint8_t)0x85)
#define NAND_CMD_RANDOMOUT          ((uint8_t)0x05)
#define NAND_CMD_RANDOMOUT_TRUE     ((uint8_t)0xE0)

#define NAND_CMD_CACHERD_START      ((uint8_t)0x00)
#define NAND_CMD_CACHERD_START2     ((uint8_t)0x31)
#define NAND_CMD_CACHERD_EXIT       ((uint8_t)0x34)

#define NAND_PAGE_SIZE    (2048)

 
 
struct stm32f4_nand
{
	struct rt_mtd_nand_device nand;
    rt_uint8_t id[5];
	struct rt_mutex lock;
    struct rt_completion comp;
};


void rt_nandflash_hw_init(void); 



#endif  
