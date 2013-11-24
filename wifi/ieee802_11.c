#include <rtthread.h>
#include <lwip/pbuf.h>
#include <netif/etharp.h>

#define ETHYPT_EAP 0x888e

rt_bool_t is_ieee802_11(struct pbuf* p)
{
	struct eth_hdr* ethhdr;
	rt_uint8_t hdbuf[6];
	rt_uint16_t type;
	rt_uint8_t *payload;

	ethhdr = p->payload;
	type = htons(ethhdr->type);

	if (type == ETHYPT_EAP)
	{
		payload = p->payload;
		pbuf_header(p, -((rt_int16_t)sizeof(struct eth_hdr)));
		rt_memcpy(hdbuf, payload + 6, 6);

		/* how to handle it? */
		return RT_TRUE;
	}

	return RT_FALSE;
}
