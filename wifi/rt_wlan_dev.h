#ifndef __RT_WLAN_DEV_H__
#define __RT_WLAN_DEV_H__
#include <netif/ethernetif.h>
#include "lwipopts.h"
#include "wlan_types.h"
#define WLANMACADDRESSLENGTH (6)
#define	RT_IF_PROMISC	0x100		/* receive all packets		*/
#define	RT_IF_ALLMULTI	0x200		/* receive all multicast packets*/

typedef struct multi_addr_struct
{
	u8	multi_addr[MRVDRV_MAX_MULTICAST_LIST_SIZE][MRVDRV_ETH_ADDR_LEN];
	u8  addressnum;
}Multi_Addr_Struct;

struct rt_wlan_dev
{
    /* inherit from ethernet device */
    struct eth_device parent;
    void * 	priv ;

    unsigned int irq;
    char name[16];
    char dev_addr[6];
    unsigned int flags;	/* interface flags (BSD)	*/
    int mc_count;	/* Number of installed mcasts	*/
    struct multi_addr_struct *mc_list;	/* Multicast mac addresses	*/
};

int rt_hw_wlan_dev_init(void);
#endif
