#ifndef __RT_DEVICE_H__
#define __RT_DEVICE_H__

#include <rtthread.h>

#define RT_DEVICE(device)	((rt_device_t)device)

/* completion flag */
struct rt_completion
{
    rt_uint32_t flag;

    /* suspended list */
    rt_list_t suspended_list;
};

/* ring buffer */
struct rt_ringbuffer
{
    rt_uint16_t read_index, write_index;
    rt_uint8_t *buffer_ptr;
    rt_uint16_t buffer_size;
};

/* pipe device */
#define PIPE_DEVICE(device)	((struct rt_pipe_device*)(device))
struct rt_pipe_device
{
	struct rt_device parent;

	/* ring buffer in pipe device */
	struct rt_ringbuffer ringbuffer;

	/* suspended list */
	rt_list_t suspended_read_list;
	rt_list_t suspended_write_list;
};

/* data link implementation */
struct rt_data_node
{
	rt_list_t list;

	void* data_ptr;
	rt_size_t data_size;
};

struct rt_data_link
{
	rt_list_t list;

	rt_uint16_t hwm;
	rt_uint16_t lwm;

	rt_size_t total_size;

	void (*push_notify)(struct rt_data_link *link);
	void (*pop_notify )(struct rt_data_link *link);
};

/**
 * Completion
 */
void rt_completion_init(struct rt_completion* completion);
rt_err_t rt_completion_wait(struct rt_completion* completion,
                            rt_int32_t timeout);
void rt_completion_done(struct rt_completion* completion);

/**
 * DataLink for DeviceDriver
 */

/**
 * RingBuffer for DeviceDriver
 *
 * Please note that the ring buffer implementation of RT-Thread 
 * has no thread wait or resume feature.
 */
void rt_ringbuffer_init(struct rt_ringbuffer* rb,
                        rt_uint8_t *pool,
                        rt_uint16_t size);
rt_size_t rt_ringbuffer_put(struct rt_ringbuffer* rb,
                            const rt_uint8_t *ptr,
                            rt_uint16_t length);
rt_size_t rt_ringbuffer_putchar(struct rt_ringbuffer* rb,
                                const rt_uint8_t ch);
rt_size_t rt_ringbuffer_get(struct rt_ringbuffer* rb,
                            rt_uint8_t *ptr,
                            rt_uint16_t length);
rt_size_t rt_ringbuffer_getchar(struct rt_ringbuffer* rb, rt_uint8_t *ch);
rt_inline rt_uint16_t rt_ringbuffer_get_size(struct rt_ringbuffer* rb)
{
	RT_ASSERT(rb != RT_NULL);
	return rb->buffer_size;
}
rt_size_t rt_ringbuffer_get_datasize(struct rt_ringbuffer* rb);
#define rt_ringbuffer_available_size	rt_ringbuffer_get_datasize

/**
 * Pipe Device
 */
rt_err_t rt_pipe_create(const char* name, rt_size_t size);
void rt_pipe_destroy(struct rt_pipe_device* pipe);

/**
 * DataLinke
 */
void rt_data_link_init(struct rt_data_link* link, rt_uint16_t hwm, rt_uint16_t lwm,
	void (*push_notify)(struct rt_data_link* link),
	void (*pop_notify )(struct rt_data_link* link));
void rt_data_link_push(struct rt_data_link* link, struct rt_data_node* node, rt_int32_t timeout);
struct rt_data_node* rt_data_link_pop(struct rt_data_link* link, rt_int32_t timeout);
void rt_data_link_reset(struct rt_data_link* link);

#ifdef RT_USING_SPI
#include "drivers/spi.h"
#endif /* RT_USING_SPI */

#ifdef RT_USING_MTD_NOR
#include "drivers/mtd_nor.h"
#endif /* RT_USING_MTD_NOR */

#ifdef RT_USING_MTD_NAND
#include "drivers/mtd_nand.h"
#endif /* RT_USING_MTD_NAND */

#ifdef RT_USING_USB_DEVICE
#include "drivers/usb_device.h"
#endif /* RT_USING_USB_DEVICE */

#ifdef RT_USING_USB_HOST
#include "drivers/usb_host.h"
#endif /* RT_USING_USB_HOST */

#ifdef RT_USING_SERIAL
#include "drivers/serial.h"
#endif /* RT_USING_SERIAL */

#ifdef RT_USING_I2C
#include "drivers/i2c.h"
#include "drivers/i2c_dev.h"

#ifdef RT_USING_I2C_BITOPS
#include "drivers/i2c-bit-ops.h"
#endif /* RT_USING_I2C_BITOPS */

#endif /* RT_USING_I2C */

#ifdef RT_USING_PM
#include "drivers/pm.h"
#endif

#ifdef RT_USING_SDIO
#include "drivers/mmcsd_core.h"
#include "drivers/sd.h"
#include "drivers/sdio.h"
#endif

#endif /* __RT_DEVICE_H__ */

