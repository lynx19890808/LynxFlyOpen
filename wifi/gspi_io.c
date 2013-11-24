#include "stm32f4xx.h"
#include "rtthread.h"
#include "gspi_io.h"
#include <drivers/spi.h>

struct rt_spi_device *rt_spi_device;

int g_dummy_clk_ioport = 0;
static struct rt_semaphore gspi_lock;

/*
WIFI_RST: PC4
WIFI_INT: PC5
*/
static void gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* GPIO Periph clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    /* Configure WIFI_RST */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	rt_thread_delay(1);
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	rt_thread_delay(1);
}

static int gspi_acquire_io(void)
{
	rt_sem_take(&gspi_lock, RT_WAITING_FOREVER);
	return 0;
}

static void gspi_release_io(void)
{
	rt_sem_release(&gspi_lock);
	return;
}

static char transmittembuf[2048];
int gspi_write_data_direct(u8 * data, u16 reg, u16 n)
{
	gspi_acquire_io();

	/* N bytes are sent wrt to 16 bytes, convert it to 8 bytes */
	n = (n * 2);
	reg |= 0x8000;

	rt_memcpy(transmittembuf, &reg, sizeof(u16));
	rt_memcpy(transmittembuf + sizeof(u16), data, (n - 2));
	if ((n % 4) != 0)
	{
		rt_memcpy(transmittembuf + n, 0x00, 2);
		n = n + 2;
	}

	rt_spi_send(rt_spi_device, transmittembuf, n / 2);

	gspi_release_io();

	return 0;
}

int gspi_write_reg(u16 reg, u16 val)
{
	gspi_write_data_direct((u8 *) &val, reg, 2);

	return 0;
}

int gspi_write_data(u16 * data, u16 size)
{
	gspi_write_data_direct((u8 *) &data[1], data[0], 2);

	return 0;
}

static char rectembuf[2048 + 1024];
int gspi_read_data_direct(u8 * data, u16 reg, u16 n)
{
	u16 wlanreg;

	if (gspi_acquire_io())
	{
		return -1;
	}

	n = ((n + g_dummy_clk_ioport) * 2);

	wlanreg = reg;
	rt_spi_send_then_recv(rt_spi_device, &wlanreg, 1, rectembuf, n / 2);

	rt_memcpy(data, rectembuf + (g_dummy_clk_ioport) * 2,
			(n - (g_dummy_clk_ioport) * 2));

	gspi_release_io();
	return 0;
}

int gspi_read_data(u16 * data, u16 size)
{
	return gspi_read_data_direct((u8 *) &data[1], data[0], 2);
}

int gspi_read_reg(u16 reg, u16 * val)
{
	gspi_read_data_direct((u8 *) val, reg, 2);
	return 0;
}

extern void sbi_interrupt(int vector);
/*config GPIO as wlan interrupt */
void EXTI1_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIOC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PC5 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Connect EXTI Line0 to PC5 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource5);

	/* Configure EXTI Line5 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line5 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void disable_wlan_interrupt(void)
{
	NVIC_DisableIRQ(EXTI9_5_IRQn);
}

void enable_wlan_interrupt(void)
{
	NVIC_EnableIRQ(EXTI9_5_IRQn);
}

extern void wlan_interrupt(void);
void EXTI9_5_IRQHandler(void)
{
	/* perform wlan interrupt */
	wlan_interrupt();
	disable_wlan_interrupt();

	/* Clear the EXTI line pending bit */
	EXTI_ClearITPendingBit(EXTI_Line5);
}

int gspi_register_irq(int * irqnum)
{
	NVIC_DisableIRQ(EXTI9_5_IRQn);
	EXTI1_Config();
	EXTI_ClearITPendingBit(EXTI_Line5);
	*irqnum = EXTI9_5_IRQn;
	return GSPI_OK;
}

void gspi_irq_clear(void)
{
	EXTI_ClearITPendingBit(EXTI_Line5);
}

static int gspihost_init_hw(void)
{
	rt_spi_device = (struct rt_spi_device *) rt_device_find("spi21");
	if (rt_spi_device == RT_NULL)
	{
		return -1;
	}

	/* config spi */
	{
		struct rt_spi_configuration cfg;
		cfg.data_width = 16;
		cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB;
		cfg.max_hz = 20 * 1000 * 1000;
		rt_spi_configure(rt_spi_device, &cfg);
	}

	gpio_init();

	return 0;
}

int gspihost_init(void)
{
	int ret = RT_EOK;
	ret = rt_sem_init(&gspi_lock, "wifi_gspi", 1, RT_IPC_FLAG_FIFO);
	if (ret != RT_EOK)
		return -ret;

	ret = gspihost_init_hw();
	return ret;
}
