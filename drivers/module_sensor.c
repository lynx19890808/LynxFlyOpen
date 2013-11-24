/*
 * FILE								: module_sensor.c
 * DESCRIPTION				: --
 * Author							: Lynx  84693469@qq.com
 * Copyright					:
 *
 * History
 * --------------------
 * Rev								: 0.00
 * Date								: 10/20/2013
 *
 * create.
 * --------------------
 */
//-----------------Include files-------------------------
#include "stm32f4_i2c_dma.h"
#include "module_sensor.h"
#include "module_mpu6050.h"
#include "module_ak8975.h"
#include "module_ms5611.h"
#include <rtthread.h>
//---------------- Function prototype -------------------

static int initialize(void);
static int read_imu_fast(void);
static int read_imu_slow(void);

//-----------------Variable------------------------------

SYS_IMU_SENSOR_T imu_sensor = {
	.initialize = initialize,
	.read_imu_fast = read_imu_fast,
	.read_imu_slow = read_imu_slow
};

//-----------------Function------------------------------

/*
 * Name										: initialize
 * Description						: ---
 * Author									: lynx@sia.
 *
 * History
 * ----------------------
 * Rev										: 0.00
 * Date										: 10/20/2013
 *
 * create.
 * ----------------------
 */
static int initialize(void)
{
	MPU6050_Init();  //Keep MPU-6050 initialized first
	AK8975_initialize();  //Do nothing infact
	//MS5611_Init(&imu_sensor.Bar);
	
	imu_sensor.Gyr.RawToTrue = 16.3835f;   //2000dps--16.4LSB
	imu_sensor.Acc.RawToTrue = 8192.0f;   //4g--8192LSB  
	imu_sensor.Mag.RawToTrue = 3.332f;   //1229uT--3.3319772172497965825874694873881LSB
	return 0;
}

/*
 * Name										: read_imu_fast
 * Description						: ---
 * Author									: lynx@sia.
 *
 * History
 * ----------------------
 * Rev										: 0.00
 * Date										: 10/20/2013
 *
 * create.
 * ----------------------
 */
static int read_imu_fast(void)
{
	u8 IMU_Buf[20] = {0};
	
	I2C_DMA_ReadReg(MPU6050_I2C_ADDR, MPU6050_ACCEL_XOUT_H, IMU_Buf,   14);
	// 	MS5611_ReadADC(&imu_sensor.Bar);
	// 	MS5611_Calculate(&imu_sensor.Bar);
	
	imu_sensor.Acc.X = (s16)((IMU_Buf[0]  << 8) | IMU_Buf[1]);
	imu_sensor.Acc.Y = (s16)((IMU_Buf[2]  << 8) | IMU_Buf[3]);
	imu_sensor.Acc.Z = (s16)((IMU_Buf[4]  << 8) | IMU_Buf[5]);
	//	Tmp   = (s16)((IMU_Buf[6]  << 8) | IMU_Buf[7]);
	imu_sensor.Gyr.X = (s16)((IMU_Buf[8]  << 8) | IMU_Buf[9]);
	imu_sensor.Gyr.Y = (s16)((IMU_Buf[10] << 8) | IMU_Buf[11]);
	imu_sensor.Gyr.Z = (s16)((IMU_Buf[12] << 8) | IMU_Buf[13]);
	
	//计算真值
	imu_sensor.Acc.TrueX = imu_sensor.Acc.X/imu_sensor.Acc.RawToTrue;
	imu_sensor.Acc.TrueY = imu_sensor.Acc.Y/imu_sensor.Acc.RawToTrue;
	imu_sensor.Acc.TrueZ = imu_sensor.Acc.Z/imu_sensor.Acc.RawToTrue;
	//计算真值
	imu_sensor.Gyr.TrueX = imu_sensor.Gyr.X/imu_sensor.Gyr.RawToTrue;
	imu_sensor.Gyr.TrueY = imu_sensor.Gyr.Y/imu_sensor.Gyr.RawToTrue;
	imu_sensor.Gyr.TrueZ = imu_sensor.Gyr.Z/imu_sensor.Gyr.RawToTrue;
	
	return 0;
}

/*
 * Name										: read_imu_slow
 * Description						: ---
 * Author									: lynx@sia.
 *
 * History
 * ----------------------
 * Rev										: 0.00
 * Date										: 10/20/2013
 *
 * create.
 * ----------------------
 */
static int read_imu_slow(void)
{
	//注意这里把ak8975坐标轴对齐到了mpu6050上 X=Y Y=X Z=-Z
	AK8975_getHeadingNoSleep(&imu_sensor.Mag.Y, &imu_sensor.Mag.X, &imu_sensor.Mag.Z);  
	imu_sensor.Mag.Z = -imu_sensor.Mag.Z;
	
	//计算真值
	imu_sensor.Mag.TrueX = imu_sensor.Mag.X/imu_sensor.Mag.RawToTrue;
	imu_sensor.Mag.TrueY = imu_sensor.Mag.Y/imu_sensor.Mag.RawToTrue;
	imu_sensor.Mag.TrueZ = imu_sensor.Mag.Z/imu_sensor.Mag.RawToTrue;
	
	return 0;
}












