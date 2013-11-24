#ifndef __BATTERY_H__
#define __BATTERY_H__

#include <board.h>

#define ADC1_DR_ADDRESS	((u32)0x4001204C)
#define ADC2_DR_ADDRESS	((u32)0x4001214C)
#define ADC3_DR_ADDRESS	((u32)0x4001224C)

#define ADC_Channels		2
#define ADC_Sample		5

typedef  struct{
	int (* initialize)(void);
	float (* read)(void);
	float value;
}SYS_BATTERY_T;

extern SYS_BATTERY_T battery;
extern vu16 ADC_DMA_Buf[ADC_Sample][ADC_Channels];


#endif //__BATTERY_H__
