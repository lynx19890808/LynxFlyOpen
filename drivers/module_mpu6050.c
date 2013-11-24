/*********************************************************************************
*                                山猫飞控（Lynx）
*                                 for LynxFly
*
* Version   	: V1.0
* By        	: Lynx@sia 84693469@qq.com
*
* For       	: Stm32f405RGT6
* Mode      	: Thumb2
* Description   : mpu6050驱动 抄袭来的
*
*				
* Date          : 2013.XX.XX
*******************************************************************************/
/*==============================================================================================*/
/*==============================================================================================*/
#include "stm32f4_i2c_dma.h"
#include "module_mpu6050.h"
#include <rtthread.h>
/*==============================================================================================*/
/*==============================================================================================*
**函數 : MPU6050_Init
**功能 : 初始化MPU6050
**輸入 : None
**輸出 : None
**使用 : MPU6050_Init()
**==============================================================================================*/
/*==============================================================================================*/
void MPU6050_Init( void )
{
  u8 MPU6050_Init_Data[6] = {
      0x01, /* MPU6050_PWR_MGMT_1 */
      0x03, /* MPU6050_CONFIG */
      0x18, /* MPU6050_GYRO_CONFIG +-2000dps */
      0x08, /* MPU6050_ACCEL_CONFIG +-4G */
      0x32, /* MPU6050_INT_PIN_CFG */
      0x00	/* MPU6050_USER_CTRL */
		};
  I2C_DMA_WriteReg(MPU6050_I2C_ADDR, MPU6050_PWR_MGMT_1,   MPU6050_Init_Data,   1); rt_thread_delay(rt_tick_from_millisecond(10));
  I2C_DMA_WriteReg(MPU6050_I2C_ADDR, MPU6050_CONFIG,       MPU6050_Init_Data+1, 1); rt_thread_delay(rt_tick_from_millisecond(10));
  I2C_DMA_WriteReg(MPU6050_I2C_ADDR, MPU6050_GYRO_CONFIG,  MPU6050_Init_Data+2, 1); rt_thread_delay(rt_tick_from_millisecond(10));
  I2C_DMA_WriteReg(MPU6050_I2C_ADDR, MPU6050_ACCEL_CONFIG, MPU6050_Init_Data+3, 1); rt_thread_delay(rt_tick_from_millisecond(10));
  I2C_DMA_WriteReg(MPU6050_I2C_ADDR, MPU6050_INT_PIN_CFG,  MPU6050_Init_Data+4, 1); rt_thread_delay(rt_tick_from_millisecond(10));
  I2C_DMA_WriteReg(MPU6050_I2C_ADDR, MPU6050_USER_CTRL,    MPU6050_Init_Data+5, 1); rt_thread_delay(rt_tick_from_millisecond(10));
}
/*==============================================================================================*/
/*==============================================================================================*/
