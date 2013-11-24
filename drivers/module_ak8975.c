/*********************************************************************************
*                                山猫飞控（Lynx）
*                                 for LynxFly
*
* Version   	: V1.0
* By        	: Lynx@sia 84693469@qq.com
*
* For       	: Stm32f405RGT6
* Mode      	: Thumb2
* Description   : ak8975驱动 用linux驱动改的
*
*				
* Date          : 2013.XX.XX
*******************************************************************************/
// I2Cdev library collection - AK8975 I2C device class header file
// Based on AKM AK8975/B datasheet, 12/2009
// 8/27/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2011-08-27 - initial release
//     2013-10-20 - Lynx transplanted it on STM32

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#include <rtthread.h>
#include "stm32f4_i2c_dma.h"
#include "module_ak8975.h"

/** Default constructor, uses default I2C address.
 * @see AK8975_DEFAULT_ADDRESS
 */

u8 devAddr = AK8975_DEFAULT_ADDRESS<<1;   //这里经指点必须左移一位
u8 mode = 0;



/** Specific address constructor.
 * @param address I2C address
 * @see AK8975_DEFAULT_ADDRESS
 * @see AK8975_ADDRESS_00
 */
void AK8975_setAddr(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 * No specific pre-configuration is necessary for this device.
 */
void AK8975_initialize() {
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
// bool AK8975_testConnection() {
// 		u8 buffer[1];
//     if (I2C_DMA_ReadReg(devAddr, AK8975_RA_WIA, buffer,1) == 1) {
//         return (buffer[0] == 0x48);
//     }
//     return false;
// }

// WIA register

uint8_t AK8975_getDeviceID() {
		u8 buffer[1];
    I2C_DMA_ReadReg(devAddr, AK8975_RA_WIA, buffer,1);
    return buffer[0];
}

// INFO register

uint8_t AK8975_getInfo() {
		u8 buffer[1];
    I2C_DMA_ReadReg(devAddr, AK8975_RA_INFO, buffer,1);
    return buffer[0];
}

// ST1 register

// bool AK8975_getDataReady() {
// 		u8 buffer[1];
//     I2Cdev_readBit(devAddr, AK8975_RA_ST1, AK8975_ST1_DRDY_BIT, buffer);
//     return buffer[0];
// }

// H* registers
void AK8975_getHeading(int16_t *x, int16_t *y, int16_t *z) {
		u8 buffer[6];
		u8 AK8975_MODE_Data[4] = {
				AK8975_MODE_POWERDOWN, 
				AK8975_MODE_SINGLE,
				AK8975_MODE_SELFTEST,
				AK8975_MODE_FUSEROM,
			};

    I2C_DMA_WriteReg(devAddr, AK8975_RA_CNTL, AK8975_MODE_Data+1,1);
    rt_thread_delay(rt_tick_from_millisecond(10));
    I2C_DMA_ReadReg(devAddr, AK8975_RA_HXL, buffer, 6);
    *x = (((int16_t)buffer[1]) << 8) | buffer[0];
    *y = (((int16_t)buffer[3]) << 8) | buffer[2];
    *z = (((int16_t)buffer[5]) << 8) | buffer[4];
}
//每次读取上一回的数据，由用户自己设定延迟，请保证每次读取的间隔都大于10ms
void AK8975_getHeadingNoSleep(int16_t *x, int16_t *y, int16_t *z) {
		u8 buffer[6];
		u8 AK8975_MODE_Data[4] = {
				AK8975_MODE_POWERDOWN, 
				AK8975_MODE_SINGLE,
				AK8975_MODE_SELFTEST,
				AK8975_MODE_FUSEROM,
			};

    I2C_DMA_ReadReg(devAddr, AK8975_RA_HXL, buffer, 6);
    *x = (((int16_t)buffer[1]) << 8) | buffer[0];
    *y = (((int16_t)buffer[3]) << 8) | buffer[2];
    *z = (((int16_t)buffer[5]) << 8) | buffer[4];
			
		I2C_DMA_WriteReg(devAddr, AK8975_RA_CNTL, AK8975_MODE_Data+1,1);
}
int16_t AK8975_getHeadingX() {
		u8 buffer[2];
	u8 AK8975_MODE_Data[4] = {
				AK8975_MODE_POWERDOWN, 
				AK8975_MODE_SINGLE,
				AK8975_MODE_SELFTEST,
				AK8975_MODE_FUSEROM,
			};
    I2C_DMA_WriteReg(devAddr, AK8975_RA_CNTL, AK8975_MODE_Data+1,1);
    rt_thread_delay(rt_tick_from_millisecond(10));
    I2C_DMA_ReadReg(devAddr, AK8975_RA_HXL, buffer, 2);
    return (((int16_t)buffer[1]) << 8) | buffer[0];
}
int16_t AK8975_getHeadingY() {
		u8 buffer[2];
	u8 AK8975_MODE_Data[4] = {
				AK8975_MODE_POWERDOWN, 
				AK8975_MODE_SINGLE,
				AK8975_MODE_SELFTEST,
				AK8975_MODE_FUSEROM,
			};
    I2C_DMA_WriteReg(devAddr, AK8975_RA_CNTL, AK8975_MODE_Data+1,1);
    rt_thread_delay(rt_tick_from_millisecond(10));
    I2C_DMA_ReadReg(devAddr, AK8975_RA_HYL, buffer, 2);
    return (((int16_t)buffer[1]) << 8) | buffer[0];
}
int16_t AK8975_getHeadingZ() {
		u8 buffer[2];
	u8 AK8975_MODE_Data[4] = {
				AK8975_MODE_POWERDOWN, 
				AK8975_MODE_SINGLE,
				AK8975_MODE_SELFTEST,
				AK8975_MODE_FUSEROM,
			};
    I2C_DMA_WriteReg(devAddr, AK8975_RA_CNTL, AK8975_MODE_Data+1,1);
    rt_thread_delay(rt_tick_from_millisecond(10));
    I2C_DMA_ReadReg(devAddr, AK8975_RA_HZL, buffer, 2);
    return (((int16_t)buffer[1]) << 8) | buffer[0];
}

// ST2 register
// bool AK8975_getOverflowStatus() {
// 		u8 buffer[1];
//     I2Cdev_readBit(devAddr, AK8975_RA_ST2, AK8975_ST2_HOFL_BIT, buffer);
//     return buffer[0];
// }
// bool AK8975_getDataError() {
// 		u8 buffer[1];
//     I2Cdev_readBit(devAddr, AK8975_RA_ST2, AK8975_ST2_DERR_BIT, buffer);
//     return buffer[0];
// }

// CNTL register
// uint8_t AK8975_getMode() {
// 		u8 buffer[1];
//     I2Cdev_readBits(devAddr, AK8975_RA_CNTL, AK8975_CNTL_MODE_BIT, AK8975_CNTL_MODE_LENGTH, buffer);
// 		mode = 0;
//     return buffer[0];
// }
// void AK8975_setMode(uint8_t mode) {
//     I2Cdev_writeBits(devAddr, AK8975_RA_CNTL, AK8975_CNTL_MODE_BIT, AK8975_CNTL_MODE_LENGTH, mode);
// }
// void AK8975_reset() {
//     I2Cdev_writeBits(devAddr, AK8975_RA_CNTL, AK8975_CNTL_MODE_BIT, AK8975_CNTL_MODE_LENGTH, AK8975_MODE_POWERDOWN);
// }

// // ASTC register
// void AK8975_setSelfTest(bool enabled) {
//     I2Cdev_writeBit(devAddr, AK8975_RA_ASTC, AK8975_ASTC_SELF_BIT, enabled);
// }

// // I2CDIS
// void AK8975_disableI2C() {
//     I2Cdev_writeBit(devAddr, AK8975_RA_I2CDIS, AK8975_I2CDIS_BIT, true);
// }

// ASA* registers
void AK8975_getAdjustment(int8_t *x, int8_t *y, int8_t *z) {
		u8 buffer[3];
    I2C_DMA_ReadReg(devAddr, AK8975_RA_ASAX, buffer, 3);
    *x = buffer[0];
    *y = buffer[1];
    *z = buffer[2];
}
void AK8975_setAdjustment(int8_t x, int8_t y, int8_t z) {
		u8 buffer[3];
    buffer[0] = x;
    buffer[1] = y;
    buffer[2] = z;
    I2C_DMA_WriteReg(devAddr, AK8975_RA_ASAX, buffer, 3);
}
uint8_t AK8975_getAdjustmentX() {
		u8 buffer[1];
    I2C_DMA_ReadReg(devAddr, AK8975_RA_ASAX, buffer,1);
    return buffer[0];
}
void AK8975_setAdjustmentX(uint8_t x) {
    I2C_DMA_WriteReg(devAddr, AK8975_RA_ASAX, &x,1);
}
uint8_t AK8975_getAdjustmentY() {
		u8 buffer[1];
    I2C_DMA_ReadReg(devAddr, AK8975_RA_ASAY, buffer,1);
    return buffer[0];
}
void AK8975_setAdjustmentY(uint8_t y) {
    I2C_DMA_WriteReg(devAddr, AK8975_RA_ASAY, &y,1);
}
uint8_t AK8975_getAdjustmentZ() {
		u8 buffer[1];
    I2C_DMA_ReadReg(devAddr, AK8975_RA_ASAZ, buffer,1);
    return buffer[0];
}
void AK8975_setAdjustmentZ(uint8_t z) {
    I2C_DMA_WriteReg(devAddr, AK8975_RA_ASAZ, &z,1);
}
