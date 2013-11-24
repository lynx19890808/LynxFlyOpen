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

#ifndef _AK8975_H_
#define _AK8975_H_

#include <board.h>

#define AK8975_ADDRESS_00         ((u8)0x0C) // default for InvenSense MPU-9150 
#define AK8975_ADDRESS_01         ((u8)0x0D)
#define AK8975_ADDRESS_10         ((u8)0x0E) // default for InvenSense MPU-6050 evaluation board
#define AK8975_ADDRESS_11         ((u8)0x0F)
#define AK8975_DEFAULT_ADDRESS    AK8975_ADDRESS_00

#define AK8975_RA_WIA             ((u8)0x00)
#define AK8975_RA_INFO            ((u8)0x01)
#define AK8975_RA_ST1             ((u8)0x02)
#define AK8975_RA_HXL             ((u8)0x03)
#define AK8975_RA_HXH             ((u8)0x04)
#define AK8975_RA_HYL             ((u8)0x05)
#define AK8975_RA_HYH             ((u8)0x06)
#define AK8975_RA_HZL             ((u8)0x07)
#define AK8975_RA_HZH             ((u8)0x08)
#define AK8975_RA_ST2             ((u8)0x09)
#define AK8975_RA_CNTL            ((u8)0x0A)
#define AK8975_RA_RSV             ((u8)0x0B) // RESERVED, DO NOT USE
#define AK8975_RA_ASTC            ((u8)0x0C)
#define AK8975_RA_TS1             ((u8)0x0D) // SHIPMENT TEST, DO NOT USE
#define AK8975_RA_TS2             ((u8)0x0E) // SHIPMENT TEST, DO NOT USE
#define AK8975_RA_I2CDIS          ((u8)0x0F)
#define AK8975_RA_ASAX            ((u8)0x10)
#define AK8975_RA_ASAY            ((u8)0x11)
#define AK8975_RA_ASAZ            ((u8)0x12)

#define AK8975_ST1_DRDY_BIT       0

#define AK8975_ST2_HOFL_BIT       3
#define AK8975_ST2_DERR_BIT       2

#define AK8975_CNTL_MODE_BIT      3
#define AK8975_CNTL_MODE_LENGTH   4

#define AK8975_MODE_POWERDOWN     ((u8)0x0)
#define AK8975_MODE_SINGLE        ((u8)0x1)
#define AK8975_MODE_SELFTEST      ((u8)0x8)
#define AK8975_MODE_FUSEROM       ((u8)0xF)

#define AK8975_ASTC_SELF_BIT      6

#define AK8975_I2CDIS_BIT         0


void AK8975_setAddr(uint8_t address);

void AK8975_initialize(void);
// bool AK8975_testConnection();

// WIA register
uint8_t AK8975_getDeviceID(void);

// INFO register
uint8_t AK8975_getInfo(void);

// ST1 register
// bool AK8975_getDataReady();

// H* registers
void AK8975_getHeading(int16_t *x, int16_t *y, int16_t *z);
void AK8975_getHeadingNoSleep(int16_t *x, int16_t *y, int16_t *z);
int16_t AK8975_getHeadingX(void);
int16_t AK8975_getHeadingY(void);
int16_t AK8975_getHeadingZ(void);

// ST2 register
// bool AK8975_getOverflowStatus();
// bool AK8975_getDataError();

// CNTL register
// uint8_t AK8975_getMode();
// void AK8975_setMode(uint8_t mode);
// void AK8975_reset();

// // ASTC register
// void AK8975_setSelfTest(bool enabled);

// // I2CDIS
// void AK8975_disableI2C(); // um, why...?

// ASA* registers
void AK8975_getAdjustment(int8_t *x, int8_t *y, int8_t *z);
void AK8975_setAdjustment(int8_t x, int8_t y, int8_t z);
uint8_t AK8975_getAdjustmentX(void);
void AK8975_setAdjustmentX(uint8_t x);
uint8_t AK8975_getAdjustmentY(void);
void AK8975_setAdjustmentY(uint8_t y);
uint8_t AK8975_getAdjustmentZ(void);
void AK8975_setAdjustmentZ(uint8_t z);


#endif /* _AK8975_H_ */
