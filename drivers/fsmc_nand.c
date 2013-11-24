/*
 * File      : fsmc_nand.c
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
 
#include "fsmc_nand.h"
#include "board.h"


#define NAND_DEBUG    rt_kprintf
#define NAND_BANK     ((uint32_t)0x80000000)
static struct stm32f4_nand _device;


rt_inline nand_cmd(rt_uint8_t cmd)
{
    /* 写命令到命令区 */
    *(volatile rt_uint8_t *)(NAND_BANK | CMD_AREA) = cmd;
}

rt_inline nand_addr(rt_uint8_t addr)
{
	/* 写地址到地址区 */
    *(vu8 *)(NAND_BANK | ADDR_AREA) = addr;  
}

rt_inline nand_read8(void)
{

    return (*(volatile rt_uint8_t*)(NAND_BANK | DATA_AREA));
}

rt_inline nand_write8(rt_uint8_t data)
{

    *(volatile rt_uint8_t*)(NAND_BANK | DATA_AREA) = data;
}

rt_inline nand_waitready(void)
{
    while( GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6) == 0);
}

static rt_uint8_t nand_readstatus(void)
{
    nand_cmd(NAND_CMD_STATUS);
    return (nand_read8());
} 

static void gpio_nandflash_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 开启GPIO时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | \
                           RCC_AHB1Periph_GPIOG, ENABLE);

    /**** 初始化NandFlash要用到的GPIO ****/

    /* D2，D3（数据位） */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
    /* NOE, NWE */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
    /* SRAM片选，NE1 */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);

    /* A16,A17 config as fsmc */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
    /* D0 D1（数据位） */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

    /* 配置GPIOE */
    /* D4 D5 D6 D7（数据位） */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);

    /* 配置GPIOG */
    /* 6. FSMC_INT2 configuration */
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource6, GPIO_AF_FSMC);
    /* NandFlash片选，NE2 */
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource9 , GPIO_AF_FSMC);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | \
                                  GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_9 | \
                                  GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | \
                                  GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | \
                                  GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

static void fsmc_nandflash_init(void)
{
    FSMC_NANDInitTypeDef FSMC_NANDInitStructure;
    FSMC_NAND_PCCARDTimingInitTypeDef  p;
 
    /**** 配置FSMC ****/

    /* 开启FSMC的时钟 */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

    p.FSMC_SetupTime     = 0x1;
    p.FSMC_WaitSetupTime = 0x3;
    p.FSMC_HoldSetupTime = 0x2;
    p.FSMC_HiZSetupTime  = 0x1;

    FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank3_NAND;
    FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Disable;
    FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
    FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Enable;
    FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_2048Bytes;
    FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0x00;
    FSMC_NANDInitStructure.FSMC_TARSetupTime = 0x00;
    FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &p;
    FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &p;

    FSMC_NANDInit(&FSMC_NANDInitStructure);

    /* FSMC NAND Bank Cmd Test */
    FSMC_NANDCmd(FSMC_Bank3_NAND, ENABLE);
}

static rt_err_t nandflash_readid(struct rt_mtd_nand_device *mtd)
{
    nand_cmd(NAND_CMD_READID);
	nand_addr(0);
	
	_device.id[0] = nand_read8();
	_device.id[1] = nand_read8();
	_device.id[2] = nand_read8();
	_device.id[3] = nand_read8();
	NAND_DEBUG("ID[%X,%X]\n",_device.id[0], _device.id[1]);
	if (_device.id[0] == 0xEC && _device.id[1] == 0xDA)
	{   
		return (RT_EOK);
	}
	
	return (RT_ERROR);
}

static rt_err_t nandflash_readpage(struct rt_mtd_nand_device* device, rt_off_t page,
                                   rt_uint8_t *data, rt_uint32_t data_len,
                                   rt_uint8_t *spare, rt_uint32_t spare_len)                        
{
    rt_uint32_t index;
	volatile rt_uint32_t ecc;
	
	RT_ASSERT(data_len < 2049);
	RT_ASSERT(spare_len < 65);

	/* 读主数据区 */
	if (data && data_len)
	{   
	    /* 读数据 */
	    nand_cmd(NAND_CMD_READ_1);
		/* 页内地址 */
		nand_addr(0);
		nand_addr(0);
	    /* 寻址页 */
		nand_addr(page);
		nand_addr(page >> 8);
		nand_addr((page >> 16) & 1);
 
		nand_cmd(NAND_CMD_READ_TRUE);
		nand_waitready();
		/* 生成ECC */
		FSMC_NANDECCCmd(FSMC_Bank3_NAND,ENABLE);
		for (index = 0; index < data_len; index ++)
		{
		
		    data[index] = nand_read8();
		}
		
		ecc = FSMC_GetECC(FSMC_Bank3_NAND);
        FSMC_NANDECCCmd(FSMC_Bank3_NAND,DISABLE);
		NAND_DEBUG("ECC%X\n",ecc);
	}
	/* 读额外数据区 */
	if (spare && spare_len)
	{
	    /* 读数据 */
	    nand_cmd(NAND_CMD_READ_1);
		/* 页内地址，从2048开始 */
		nand_addr(0);
		nand_addr(0x08);
	    /* 寻址页 */
		nand_addr(page);
		nand_addr(page >> 8);
		nand_addr((page >> 16) & 1);
 
		nand_cmd(NAND_CMD_READ_TRUE);
		nand_waitready();
		
		for (index = 0; index < spare_len; index ++)
		{
		
		    spare[index] = nand_read8();
		}	       
	}
    
	return (RT_EOK);
}

static rt_err_t nandflash_writepage(struct rt_mtd_nand_device* device, rt_off_t page,
                                    rt_uint8_t *data, rt_uint32_t data_len,
                                    rt_uint8_t *spare, rt_uint32_t spare_len)
{
    rt_uint32_t index;
	rt_err_t result;
	
	RT_ASSERT(data_len < 2049);
	RT_ASSERT(spare_len < 65);
	
	result = RT_ERROR;
	
	rt_mutex_take(&_device.lock, RT_WAITING_FOREVER);
	/* 写主数据区 */
	if (data && data_len)
	{
	    nand_cmd(NAND_CMD_PAGEPROGRAM);
		
		nand_addr(0);
		nand_addr(0);
		
		nand_addr(page);
		nand_addr(page >> 8);
		nand_addr((page >> 16) & 0x01);
		
		for (index = 0; index < data_len; index ++)
		{
		    nand_write8(data[index]);
		}
		nand_cmd(NAND_CMD_PAGEPROGRAM_TRUE);
		
		nand_waitready();	
		if (nand_readstatus() & 0x01 == 1)
		    result = -RT_MTD_EIO;
			
		result = RT_MTD_EOK;	
	}
	/* 写spare区 */
	if (spare && spare_len)
	{
	    nand_cmd(NAND_CMD_PAGEPROGRAM);
		
		nand_addr(0);
		nand_addr(0x08);
		
		nand_addr(page);
		nand_addr(page >> 8);
		nand_addr((page >> 16) & 0x01);
		
		for (index = 0; index < spare_len; index ++)
		{
		    nand_write8(spare[index]);
		}
		nand_cmd(NAND_CMD_PAGEPROGRAM_TRUE);
		
		nand_waitready();	
		if (nand_readstatus() & 0x01 == 1)
		    result = -RT_MTD_EIO;
			
		result = RT_MTD_EOK;			
	}
	
	rt_mutex_release(&_device.lock);

    return (RT_EOK);
} 

rt_err_t nandflash_eraseblock(struct rt_mtd_nand_device* device, rt_uint32_t block)
{
    rt_uint32_t page;
	rt_err_t result;
	
	result = RT_EOK;
	page = block * 2048;
	rt_mutex_take(&_device.lock, RT_WAITING_FOREVER);
	
    nand_cmd(NAND_CMD_ERASE0);
	
	nand_addr(page);
	nand_addr(page >> 8);
	nand_addr((page >> 16) & 0x01);
	
	nand_cmd(NAND_CMD_ERASE1);
	
	nand_waitready();
	
	if (nand_readstatus() & 0x01 == 1)
	    result = -RT_MTD_EIO;
	rt_mutex_release(&_device.lock);		
	
    return (result);
}

static rt_err_t nandflash_pagecopy(struct rt_mtd_nand_device *device, rt_off_t src_page, rt_off_t dst_page)
{
    rt_err_t result;
	
	result = RT_MTD_EOK;
    rt_mutex_take(&_device.lock, RT_WAITING_FOREVER);
	
    /* 读数据到nand内部数据寄存器 */
    nand_cmd(NAND_CMD_RDCOPYBACK);
	
	nand_addr(0);
	nand_addr(0);
    /* 列地址 */
	nand_addr(src_page);
    nand_addr(src_page >> 8);
	nand_addr((src_page >> 16) & 0x01);
	
	nand_cmd(NAND_CMD_RDCOPYBACK_TRUE);
	
	nand_waitready();
	
	/* 写数据到另一个页 */
	nand_cmd(NAND_CMD_COPYBACKPGM);
	
	nand_addr(0);
	nand_addr(0);
	nand_addr(dst_page);
    nand_addr(dst_page >> 8);
	nand_addr((dst_page >> 16) & 0x01);	
	
	nand_cmd(NAND_CMD_COPYBACKPGM_TRUE);
	
	nand_waitready();
	if ((nand_readstatus() & 0x01) == 0x01)
	    result = -RT_MTD_EIO;
	
	rt_mutex_release(&_device.lock);
		
	return (result);
}

static rt_err_t nandflash_checkblock(struct rt_mtd_nand_device* device, rt_uint32_t block)
{
    return (RT_EOK);
}

static rt_err_t nandflash_markbad(struct rt_mtd_nand_device* device, rt_uint32_t block)
{
    return (RT_EOK);
}

static struct rt_mtd_nand_driver_ops ops = 
{
    nandflash_readid,
	nandflash_readpage,
	nandflash_writepage,
	nandflash_pagecopy,
	nandflash_eraseblock,
	nandflash_checkblock,
	nandflash_markbad
};

void rt_nandflash_hw_init(void)
{
    /* nandflash GPIO初始化 */
	gpio_nandflash_init();
    /* 初始化NandFlash控制器 */
    fsmc_nandflash_init();
	/* 初始化互斥锁 */
    rt_mutex_init(&_device.lock, "nand", RT_IPC_FLAG_FIFO);

    _device.nand.page_size   = 2048;
    _device.nand.pages_per_block = 64;
	_device.nand.block_total = 512; //2048
    _device.nand.oob_size    = 64;
	_device.nand.oob_free    = 60;
    _device.nand.block_start = 0;
    _device.nand.block_end   = 255;
    _device.nand.ops         = &ops;

    rt_mtd_nand_register_device("nand0", &_device.nand);
}
#include <math.h>
#include <finsh.h>
static uint8_t TxBuffer[NAND_PAGE_SIZE];
static uint8_t RxBuffer[NAND_PAGE_SIZE];
static rt_uint8_t spare[64];
void ntest(void)
{
    int i;
    /* 检测NAND Flash */
    nandflash_readid(0);
   
    /* Erase the Block */
	if (nandflash_eraseblock(0,0) != RT_MTD_EOK)
	{
	    rt_kprintf("擦出失败\n");
		return;
	}

    /* Write data to FSMC NAND memory */
    /* Clear the buffer */
    for (i = 0; i < 2048; i++)
	    TxBuffer[i] = i*i - i;
    memset(RxBuffer, 0, sizeof(RxBuffer));
    memset(spare, 0x33, 64);

    nandflash_writepage(0,0,TxBuffer,2048, spare, 64);
	memset(spare, 0x0, 64);

    /* Read back the written data */
    nandflash_readpage(0,0,RxBuffer, 2048,spare,64);


    if( memcmp( (char*)TxBuffer, (char*)RxBuffer, NAND_PAGE_SIZE ) == 0 )
    {
        NAND_DEBUG("Nand Flash is OK \r\n");
    }
    else
    {
        NAND_DEBUG("Nand Flash is error \r\n");
    }
}

void nread(void)
{
    int index;
	
	rt_memset(RxBuffer, 0, 2048);
	rt_memset(spare, 0, 64);
 
	
	nandflash_readpage(0,0,RxBuffer, 2048,spare,64);
// 	for (index = 0; index < 2048; index ++)
// 	{
// 	    rt_kprintf("0x%X,",RxBuffer[index]);
// 	}
// 	rt_kprintf("spare\n");
// 	for (index = 0; index < 64; index ++)
// 	{
// 	    rt_kprintf("[%X]", spare[index]);
// 	}
// 	rt_kprintf("\n\n");
}
FINSH_FUNCTION_EXPORT(nread, nand read test);
FINSH_FUNCTION_EXPORT(ntest, nand test);
 
