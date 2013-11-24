#include <rtthread.h>
#include <netif/ethernetif.h>

#include "rt_wlan_dev.h"
#include "wlan_dev.h"
#include "sbi.h"

extern int WlanInitPhase2(void);
#define MAX_ADDR_LEN 6
extern int WlanInitPhase1(struct rt_wlan_dev* RTWlanDev);
struct rt_wlan_dev wlan_eth;

/* RT-Thread Device Interface */
/* initialize the interface */
static rt_err_t rt_wlan_dev_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t rt_wlan_dev_open(rt_device_t dev, rt_uint16_t oflag)
{
	rt_err_t  error=RT_EOK;

	if(dev==RT_NULL||dev->user_data==RT_NULL)
	{
		error=-RT_EEMPTY;
		goto done;
	}
	
	
done:
	return error;
}

static rt_err_t rt_wlan_dev_close(rt_device_t dev)
{

	return RT_EOK;
}

static rt_size_t rt_wlan_dev_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
	rt_set_errno(-RT_ENOSYS);
	return 0;
}

static rt_size_t rt_wlan_dev_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
	rt_set_errno(-RT_ENOSYS);
	return 0;
}

rt_err_t rt_wlan_dev_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
	rt_err_t  error=RT_EOK;
	struct rt_wlan_dev* netdev=(struct rt_wlan_dev*)(dev->user_data);
	WlanInfo *wlan=netdev->priv;
	WlanCard *card=wlan->card;
	char addressbuf[6]={0xff,0xff,0xff,0xff,0xff,0xff};
	char *myadd=(char *)args;
	
	if(dev==RT_NULL||dev->user_data==RT_NULL)
	{
		error=-RT_EEMPTY;
		goto done;
	}

	switch (cmd)
	{
	case NIOCTL_GADDR:
		
		if(rt_memcmp(card->MyMacAddress,addressbuf,6)==0)
		{
			error=-RT_ERROR;
			break;
		}
		else{
			rt_memcpy((u8*)args,card->MyMacAddress,6);
			rt_kprintf("my address %x,%x,%x,%x,%x,%x,%x\r\n",myadd[0],myadd[1],myadd[2],
				myadd[3],myadd[4],myadd[5]);
		}
		break;

	default :
		break;
	}
done:
	return error;
}

/* ethernet device interface */
rt_err_t rt_wlan_dev_tx( rt_device_t dev, struct pbuf* packet)
{
    struct rt_wlan_dev *wlandev=dev->user_data;
    WlanInfo *wlan = (WlanInfo*)wlandev->priv;
	WlanCard *card = wlan->card;
	rt_uint32_t level;

	if (packet == RT_NULL) return -RT_ERROR;

    SendSinglePacket(card, packet);

    level = rt_hw_interrupt_disable();
    card->HisRegCpy &= ~HIS_TxDnLdRdy;
    rt_hw_interrupt_enable(level);

    return RT_EOK;
}

/* reception packet. */
struct pbuf *rt_wlan_dev_rx(rt_device_t dev)
{
	struct rt_wlan_dev* netdev=(struct rt_wlan_dev*)(dev->user_data);
	WlanInfo*   wlan=netdev->priv;
	WlanCard * card=wlan->card;
	Rx_Pbuf_List *RxListHead;
	struct pbuf* p;
	rt_base_t level;

__again:
	p = RT_NULL;
	RxListHead = RT_NULL;

	level = rt_hw_interrupt_disable();
	if (card->RxList.next != &card->RxList)
	{
		RxListHead=card->RxList.next;
		p=RxListHead->p;
		card->RxList.next=RxListHead->next;
		RxListHead->next->pre=RxListHead->pre;
		card->RxQueueCount-- ;
	}
	rt_hw_interrupt_enable(level);

	if (RxListHead != RT_NULL)
	{
		rt_free(RxListHead);
		RxListHead = RT_NULL;
	}

	if ((p != RT_NULL) && (is_ieee802_11(p) == RT_TRUE))
		goto __again;

	if (p != RT_NULL)
	{
		hexdump("Rx Packet: \n", p->payload, p->len);
	}

    return p;
}

void rt_wlan_linkchange(rt_bool_t enable)
{
	eth_device_linkchange(&(wlan_eth.parent), enable);
}

int rt_wlan_init(void)
{
	rt_int32_t value=0;
	rt_err_t  error=RT_EOK;

	/*
	 * SRAM Tx/Rx pointer automatically return to start address,
	 * Packet Transmitted, Packet Received
	 */
	wlan_eth.parent.parent.init	   	= rt_wlan_dev_init;
	wlan_eth.parent.parent.open	   	= rt_wlan_dev_open;
	wlan_eth.parent.parent.close	= rt_wlan_dev_close;
	wlan_eth.parent.parent.read	   	= rt_wlan_dev_read;
	wlan_eth.parent.parent.write	= rt_wlan_dev_write;
	wlan_eth.parent.parent.control	= rt_wlan_dev_control;
	wlan_eth.parent.parent.user_data =(void *)&wlan_eth ;

	wlan_eth.parent.eth_rx	 = rt_wlan_dev_rx;
	wlan_eth.parent.eth_tx	 = rt_wlan_dev_tx;

	if (WlanInitPhase1(&wlan_eth) == 0)
	{
		WlanInitPhase2();
		WlanInitPhase3();
		eth_device_init(&(wlan_eth.parent), "w0");
	}

done:
	return error;
}
