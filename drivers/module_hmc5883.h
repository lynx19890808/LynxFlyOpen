/* #include "module_hmc5883.h" */

#ifndef __MODULE_HMC5883_H
#define __MODULE_HMC5883_H

#include <board.h>
/*=====================================================================================================*/
/*=====================================================================================================*/
#define HMC5883_I2C_ADDR				    ((u8)0x3C)

#define	HMC5883_REG_CONF_A          ((u8)0x00)
#define	HMC5883_REG_CONF_B          ((u8)0x01)
#define	HMC5883_REG_MODE            ((u8)0x02)
#define	HMC5883_REG_DATA_X_H        ((u8)0x03)
#define	HMC5883_REG_DATA_X_L        ((u8)0x04)
#define	HMC5883_REG_DATA_Y_H        ((u8)0x05)
#define	HMC5883_REG_DATA_Y_L        ((u8)0x06)
#define	HMC5883_REG_DATA_Z_H        ((u8)0x07)
#define	HMC5883_REG_DATA_Z_L        ((u8)0x08)
#define	HMC5883_REG_STATUS          ((u8)0x09)
#define	HMC5883_REG_IDENTF_A        ((u8)0x0A)
#define	HMC5883_REG_IDENTF_B        ((u8)0x0B)
#define	HMC5883_REG_IDENTF_C        ((u8)0x0C)
/*=====================================================================================================*/
/*=====================================================================================================*/
void HMC5883_Init( void );
/*=====================================================================================================*/
/*=====================================================================================================*/
#endif
