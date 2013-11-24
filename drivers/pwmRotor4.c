/*
 * File						: pwmRotor4.c
 * Description		: This file is ...
 * Author					: lynx@sia  84693469@qq.com
 * Copyright			:
 *
 * History
 **--------------------
 * Rev						: 0.00
 * Date						: 10/19/2013
 *
 * create.
 *
 * Rev						: 0.00
 * Date						: 11/11/2013
 *
 * Exchanged the motor 3 and 4.
 **--------------------
 */

//----------------- Include files ------------------------//

#include "pwmRotor4.h"
//----------------- Define -------------------------------//

//----------------- Function Prototype -------------------//

static int initialize(void);
static int frequency(int freq, float percentage);
static int setFreq(int freq);
static int setMotor1F(float percentage);
static int setMotor2F(float percentage);
static int setMotor3F(float percentage);
static int setMotor4F(float percentage);
static int setMotor1I(int percentage);
static int setMotor2I(int percentage);
static int setMotor3I(int percentage);
static int setMotor4I(int percentage);
//----------------- Variable -----------------------------//

static int PWM_Motor_Min = 0;
static int PWM_Motor_Max = 1050-1;   // 默认周期 = 1ms, 21MHz分频到20kHz，这里定义在头文件的PWM_Motor_Max中 减一是精准定时的需要 注意 这里是初始值，随后多次更改需要重新设置
//在使用高频的情况下（基准频率21M） 可设置的最低频率低于340Hz

PWMROTOR4_T pwmRotor4 = {
	.initialize = initialize,
	.frequency = frequency,
	.setFreq = setFreq,
	.setMotor1F = setMotor1F,
	.setMotor2F = setMotor2F,
	.setMotor3F = setMotor3F,
	.setMotor4F = setMotor4F,
	.setMotor1I = setMotor1I,
	.setMotor2I = setMotor2I,
	.setMotor3I = setMotor3I,
	.setMotor4I = setMotor4I
};
//----------------- Function -----------------------------//

/*
 * Name						: initialize
 * Description		: This file is ...
 * Author					: lynx@sia  84693469@qq.com
 * Copyright			:
 *
 * History
 **--------------------
 * Rev						: 0.00
 * Date						: 10/19/2013
 *
 * create.
 **--------------------
 */
static int
initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM2);


	/* TIM2 PWM1 PA0 */	/* TIM2 PWM2 PA1 */	/* TIM2 PWM3 PA2 */	/* TIM2 PWM4 PA3 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	TIM_DeInit(TIM2);

/************************** PWM Output **************************************/
	/* 設定 TIM2 TIM3 TIM4 Time Base */
	TIM_TimeBaseStruct.TIM_Period = (u16)(PWM_Motor_Max);     // 週期 = 1ms, 20kHz，这里定义在头文件的PWM_Motor_Max中 包括减一
	TIM_TimeBaseStruct.TIM_Prescaler = (u16)(4-1);             // 除頻4 = 21MHz
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;		// 上數
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);

	/* 設定 TIM2 TIM3 TIM4 TIM8 OC */
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;							// 配置為 PWM1 模式
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	// 致能 OC
	TIM_OCInitStruct.TIM_Pulse = PWM_Motor_Min;									// 設置跳變值
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;			// 當計數值小於 PWM_Motor_Min 時為高電平
	TIM_OC1Init(TIM2, &TIM_OCInitStruct);												// 初始化 TIM2 OC1
	TIM_OC2Init(TIM2, &TIM_OCInitStruct);												// 初始化 TIM2 OC2
	TIM_OC3Init(TIM2, &TIM_OCInitStruct);												// 初始化 TIM2 OC3
	TIM_OC4Init(TIM2, &TIM_OCInitStruct);												// 初始化 TIM2 OC4
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);						// 致能 TIM2 OC1 預裝載
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);						// 致能 TIM2 OC2 預裝載
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);						// 致能 TIM2 OC3 預裝載
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);						// 致能 TIM2 OC4 預裝載
	
	//让电机不动作
	PWM1 = PWM_Motor_Min;
	PWM2 = PWM_Motor_Min;
	PWM3 = PWM_Motor_Min;
	PWM4 = PWM_Motor_Min;

	/* 啟動 */
	TIM_ARRPreloadConfig(TIM2, ENABLE);													// 致能 TIM2 重載寄存器ARR
	TIM_Cmd(TIM2, ENABLE);																			// 致能 TIM2

	return 0;
}
/*
 * Name						: frequency
 * Description		: This file is ...
 * Author					: lynx@sia  84693469@qq.com
 * Copyright			:
 *
 * History 2012/11/3改成了用float的形式来增加用户体验
 **--------------------
 * Rev						: 0.00
 * Date						: 10/19/2013
 *
 * create.
 **--------------------
 */
static int
frequency(int freq, float percentage)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	float Fpercentage;
	
	if (freq == 0 || percentage <= 0) {       //这里是调试关灯用的
		//让电机不动作
		PWM1 = PWM_Motor_Min;
		PWM2 = PWM_Motor_Min;
		PWM3 = PWM_Motor_Min;
		PWM4 = PWM_Motor_Min;
		return 0;
	} else {	
		//规范输入的数值范围
		if(percentage > 100)
			percentage = 100;
	
		Fpercentage = percentage/100.0f;          //转化为小数[0,1)
		
		PWM_Motor_Max = (21000000/freq-1); //设置自动重装载的值
		/* 設定 TIM2 TIM3 TIM4 Time Base */
		TIM_TimeBaseStruct.TIM_Period = (u16)(PWM_Motor_Max);     // 重新设置周期
		TIM_TimeBaseStruct.TIM_Prescaler = (u16)(4-1);             // 除頻4 = 21M
		TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;		// 上數
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);
		
		Fpercentage *= PWM_Motor_Max;
		
		PWM1 = Fpercentage;
		PWM2 = Fpercentage;
		PWM3 = Fpercentage;
		PWM4 = Fpercentage;
		
		/* 啟動 */
		TIM_ARRPreloadConfig(TIM2, ENABLE);													// 致能 TIM2 重載寄存器ARR
		TIM_Cmd(TIM2, ENABLE);																			// 致能 TIM2
		
	}

	return 1;
}

static int setFreq(int freq)
{	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	if (freq < 1) {       //这里是调试关灯用的
		//让电机不动作
		PWM1 = PWM_Motor_Min;
		PWM2 = PWM_Motor_Min;
		PWM3 = PWM_Motor_Min;
		PWM4 = PWM_Motor_Min;
		return 0;
	} else {			
		PWM_Motor_Max = (21000000/freq-1); //设置自动重装载的值
		/* 設定 TIM2 TIM3 TIM4 Time Base */
		TIM_TimeBaseStruct.TIM_Period = (u16)(PWM_Motor_Max);     // 重新设置周期
		TIM_TimeBaseStruct.TIM_Prescaler = (u16)(4-1);             // 除頻4 = 21M 
		TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;		// 上數
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);

		/* 啟動 */
		TIM_ARRPreloadConfig(TIM2, ENABLE);													// 致能 TIM2 重載寄存器ARR
		TIM_Cmd(TIM2, ENABLE);																			// 致能 TIM2
		
	}

	return 1;
}

static int setMotor1F(float percentage)
{
	float Fpercentage;
	
	//规范输入的数值范围 使用前请先初始化并至少设定一次频率
	if(percentage < 0)
		percentage = 0;
	else if(percentage > 100)
		percentage = 100;

	Fpercentage = percentage/100.0f;
	Fpercentage *= PWM_Motor_Max;
		
	PWM1 = Fpercentage;  //比较值按输入的百分数求出

	return 1;
}
static int setMotor2F(float percentage)
{
	float Fpercentage;
	
	//规范输入的数值范围 使用前请先初始化并至少设定一次频率
	if(percentage < 0)
		percentage = 0;
	else if(percentage > 100)
		percentage = 100;

	Fpercentage = percentage/100.0f;
	Fpercentage *= PWM_Motor_Max;
		
	PWM2 = Fpercentage;  //比较值按输入的百分数求出

	return 1;
}
static int setMotor3F(float percentage)
{
	float Fpercentage;
	
	//规范输入的数值范围 使用前请先初始化并至少设定一次频率
	if(percentage < 0)
		percentage = 0;
	else if(percentage > 100)
		percentage = 100;

	Fpercentage = percentage/100.0f;
	Fpercentage *= PWM_Motor_Max;
		
	PWM3 = Fpercentage;  //比较值按输入的百分数求出

	return 1;
}
static int setMotor4F(float percentage)
{
	float Fpercentage;
	
	//规范输入的数值范围 使用前请先初始化并至少设定一次频率
	if(percentage < 0)
		percentage = 0;
	else if(percentage > 100)
		percentage = 100;

	Fpercentage = percentage/100.0f;
	Fpercentage *= PWM_Motor_Max;
		
	PWM4 = Fpercentage;  //比较值按输入的百分数求出

	return 1;
}

static int setMotor1I(int percentage)
{
	int ITemp;
	
	//规范输入的数值范围 使用前请先初始化并至少设定一次频率
	if(percentage < 0)
		percentage = 0;
	else if(percentage > 100)
		percentage = 100;
	
	ITemp = (PWM_Motor_Max*percentage)/100;
		
	PWM1 = ITemp;  //比较值按输入的百分数求出

	return 1;
}
static int setMotor2I(int percentage)
{
	int ITemp;
	
	//规范输入的数值范围 使用前请先初始化并至少设定一次频率
	if(percentage < 0)
		percentage = 0;
	else if(percentage > 100)
		percentage = 100;

	ITemp = (PWM_Motor_Max*percentage)/100;
		
	PWM2 = ITemp;  //比较值按输入的百分数求出

	return 1;
}
static int setMotor3I(int percentage)
{
	int ITemp;
	
	//规范输入的数值范围 使用前请先初始化并至少设定一次频率
	if(percentage < 0)
		percentage = 0;
	else if(percentage > 100)
		percentage = 100;

	ITemp = (PWM_Motor_Max*percentage)/100;
		
	PWM3 = ITemp;  //比较值按输入的百分数求出

	return 1;
}
static int setMotor4I(int percentage)
{
	int ITemp;
	
	//规范输入的数值范围 使用前请先初始化并至少设定一次频率
	if(percentage < 0)
		percentage = 0;
	else if(percentage > 100)
		percentage = 100;

	ITemp = (PWM_Motor_Max*percentage)/100;
		
	PWM4 = ITemp;  //比较值按输入的百分数求出
	
	return 1;
}
