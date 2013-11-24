/*
 * FILE								: receiver.c
 * DESCRIPTION				: This file is RC receiver driver.目前还没有启用该功能
 * Author							: Lynx@sia 84693469@qq.com
 * Copyright					:
 *
 * History
 * --------------------
 * Rev								: 0.00
 * Date								: 11/03/2013
 *
 * create.
 * --------------------
 */

//---------------- Include files ------------------------//
#include "receiver.h"


#define TIMPSC 180    //定义预分频实际系数（未减一），改变时手动重新定义通道界限
#define TIMARR_M 400     //定义计时乘数，改变时手动重新定义通道界限，还要小心与下面相乘小于32768
#define TIMARR_C 30     //定义计时器循环周期（ms）
#define TIMARR_A (TIMARR_M*TIMARR_C)     //定义计时器实际的最大数值
#define STICK_M_LR_SUP 751  //定义各个通道的最大最小计时器值，注意这个值在计时器乘数或者预分频系数变时要手动改变
#define STICK_M_LR_MID 585  
#define STICK_M_LR_INF 422  
#define STICK_P_SUP 751  
#define STICK_P_MID 582  
#define STICK_P_INF 422  
#define STICK_M_BF_SUP 748  
#define STICK_M_BF_MID 589  
#define STICK_M_BF_INF 421  
#define STICK_T_LR_SUP 756  
#define STICK_T_LR_MID 589  
#define STICK_T_LR_INF 427  

#define SWITCH_ENABLE_SUP 790   //遥控器左上角那个大开关通道5，我设定用来使能遥控器，这几个参数是随便设置的
#define SWITCH_ENABLE_MID_UP 620    //当做是施密特触发器吧
#define SWITCH_ENABLE_MID 589  
#define SWITCH_ENABLE_MID_DOWN 550  
#define SWITCH_ENABLE_INF 400  

#define STICK_ERR_SUP 800  
#define STICK_ERR_INF 380  

//---------------- Function Prototype -------------------//

static int initialize(void);
// static int TIM3initialize(int ms);
// static int TIM3enable(void);
// static int TIM3disable(void);

//---------------- Variable -----------------------------//

receiver_T receiver = {
	.initialize = initialize,
	.ReceiverIntErr = 0,  //累计计时中断错误的次数
	.ReceiverOK = 0,  //接收机状态 0为无信号
	.StickMoveLR = 0,  //还原的各摇杆的位置
	.StickPower = 0,
	.StickMoveBF = 0,
	.StickTurnLR = 0,
	.SwitchEnable = 0
};

// static int FoundSthThisTime = 0;  //每次通道有数据就更新
// static int CH1HTimer = 0;
// static int CH2HTimer = 0;
// static int CH3HTimer = 0;
// static int CH4HTimer = 0;
// static int CH5HTimer = 0;

// static int CH6HTimer = 0;
// static int CH7HTimer = 0;
// static int CH8HTimer = 0;


//-----------------Function------------------------------//

/*
 * Name										: initialize
 * Description						: ---
 * Author									: Lynx@sia 84693469@qq.com.
 *
 * History
 * ----------------------
 * Rev										: 0.00
 * Date										: 03/05/2012
 *
 * create.
 * ----------------------
 */
static int
initialize(void)
{
	//由于现在接收机还没启动，所以这里直接初始化为不使用
	receiver.ReceiverOK = 0;
	receiver.SwitchEnable = 0;
	
	
	
// 	//PB9---------------------------------------
// 	//PB9 as interrupt
// 	GPIOB->CRH.B.MODE9 = 0;  //输入模式
// 	GPIOB->CRH.B.CNF9 = 2; //上下拉输入
// 	GPIOB->ODR.B.IO9 = 1; //上拉输入
// 	
// 	//开外部中断 PB9
// 	AFIO->EXTICR3.B.EXTI9 = 1; //开PB9中断引脚

// 	EXTI->IMR.B.MR9 = 1; //开放 PB9 线上的中断请求
// 	EXTI->RTSR.B.TR9 = 1; //上升沿触发
// 	EXTI->FTSR.B.TR9 = 1; //下降沿触发
// 	//PB8---------------------------------------
// 	//PB8 as interrupt
// 	GPIOB->CRH.B.MODE8 = 0;  //输入模式
// 	GPIOB->CRH.B.CNF8 = 2; //上下拉输入
// 	GPIOB->ODR.B.IO8 = 1; //上拉输入
// 	
// 	//开外部中断 PB8
// 	AFIO->EXTICR3.B.EXTI8 = 1; //开PB9中断引脚

// 	EXTI->IMR.B.MR8 = 1; //开放 PB9 线上的中断请求
// 	EXTI->RTSR.B.TR8 = 1; //上升沿触发
// 	EXTI->FTSR.B.TR8 = 1; //下降沿触发
// 	//PB7---------------------------------------
// 	//PB7 as interrupt
// 	GPIOB->CRL.B.MODE7 = 0;  //输入模式
// 	GPIOB->CRL.B.CNF7 = 2; //上下拉输入
// 	GPIOB->ODR.B.IO7 = 1; //上拉输入
// 	
// 	//开外部中断 PB7
// 	AFIO->EXTICR2.B.EXTI7 = 1; //开PB9中断引脚

// 	EXTI->IMR.B.MR7 = 1; //开放 PB9 线上的中断请求
// 	EXTI->RTSR.B.TR7 = 1; //上升沿触发
// 	EXTI->FTSR.B.TR7 = 1; //下降沿触发
// 	//PB6---------------------------------------
// 	//PB6 as interrupt
// 	GPIOB->CRL.B.MODE6 = 0;  //输入模式
// 	GPIOB->CRL.B.CNF6 = 2; //上下拉输入
// 	GPIOB->ODR.B.IO6 = 1; //上拉输入
// 	
// 	//开外部中断 PB6
// 	AFIO->EXTICR2.B.EXTI6 = 1; //开PB9中断引脚

// 	EXTI->IMR.B.MR6 = 1; //开放 PB9 线上的中断请求
// 	EXTI->RTSR.B.TR6 = 1; //上升沿触发
// 	EXTI->FTSR.B.TR6 = 1; //下降沿触发

// 	NVIC->ISER1.B.EXTI9_5 = 1;
// 	NVIC->IPR6.B.EXTI9_5 = 0;
// 	//PB5---------------------------------------
// 	//PB5 as interrupt
// 	GPIOB->CRL.B.MODE5 = 0;  //输入模式
// 	GPIOB->CRL.B.CNF5 = 2; //上下拉输入
// 	GPIOB->ODR.B.IO5 = 1; //上拉输入
// 	
// 	//开外部中断 PB6
// 	AFIO->EXTICR2.B.EXTI5 = 1; //开PB9中断引脚

// 	EXTI->IMR.B.MR5 = 1; //开放 PB9 线上的中断请求
// 	EXTI->RTSR.B.TR5 = 1; //上升沿触发
// 	EXTI->FTSR.B.TR5 = 1; //下降沿触发

// 	NVIC->ISER1.B.EXTI9_5 = 1;
// 	NVIC->IPR6.B.EXTI9_5 = 0;
// 	
// 	TIM3initialize(TIMARR_C);  //打开TIMER3，设定计时为前面define的100ms
// 	TIM3enable();  //使能
// 	
	return 0;
}

/*
 * Name										: EXTI9_5_IRQHandler
 * Description						: ---
 * Author									: Lynx@sia 84693469@qq.com.
 *
 * History
 * ----------------------
 * Rev										: 0.00
 * Date										: 03/05/2012
 *
 * create.
 * ----------------------
 */
// void EXTI9_5_IRQHandler(void)
// {
// 	//几个用于计算越渡时间的计时静态变量
// 	static int CH1UP = 0;
// 	static int CH1DOWN = 0;
// 	static int CH2UP = 0;
// 	static int CH2DOWN = 0;
// 	static int CH3UP = 0;
// 	static int CH3DOWN = 0;
// 	static int CH4UP = 0;
// 	static int CH4DOWN = 0;
//  	static int CH5UP = 0;
//  	static int CH5DOWN = 0;
// // 	static int CH6UP = 0;
// // 	static int CH6DOWN = 0;
// // 	static int CH7UP = 0;
// // 	static int CH7DOWN = 0;
// // 	static int CH8UP = 0;
// // 	static int CH8DOWN = 0;
// 	
// 	//在上来时立刻就记录用的计时值（为了最大限度的减少误差时间）
// 	int FastSave;  //这里不给初值是为了让OS函数保证为第一个触发
// 	OSIntEnter();
// 	
// 	FastSave = TIM3->CNT;  //快速记录时间
// 	
// 	//这里每一if都查询了一次，为了在中断被阻塞两个IO的中断同时进入的时候还能保证最低限度的读取成功
// 	
// 	if(EXTI->PR.B.PR9){
// 		if(GPIOB->IDR.B.IO9) {  //高电平说明是上升沿
// 			CH1UP = FastSave; //给相应的通道赋值
// 		}else{  //下降沿
// 			CH1DOWN = FastSave; //给相应的通道赋值
// 			CH1HTimer = (TIMARR_A+CH1DOWN-CH1UP)%TIMARR_A;   //在下降沿时计算高电平越渡的时间
// 			FoundSthThisTime++;
// 		}
// 		EXTI->PR.B.PR9 = 1; //CLEAR注意STM32清除是给1而查询也是1 囧
// 	}
// 	if(EXTI->PR.B.PR8){
// 		if(GPIOB->IDR.B.IO8) {  //高电平说明是上升沿
// 			CH2UP = FastSave; //给相应的通道赋值
// 		}else{  //下降沿
// 			CH2DOWN = FastSave; //给相应的通道赋值
// 			CH2HTimer = (TIMARR_A+CH2DOWN-CH2UP)%TIMARR_A;   //在下降沿时计算高电平越渡的时间
// 			FoundSthThisTime++;
// 		}
// 		EXTI->PR.B.PR8 = 1; //CLEAR注意STM32清除是给1而查询也是1 囧
// 	}
// 	if(EXTI->PR.B.PR7){
// 		if(GPIOB->IDR.B.IO7) {  //高电平说明是上升沿
// 			CH3UP = FastSave; //给相应的通道赋值
// 		}else{  //下降沿
// 			CH3DOWN = FastSave; //给相应的通道赋值
// 			CH3HTimer = (TIMARR_A+CH3DOWN-CH3UP)%TIMARR_A;   //在下降沿时计算高电平越渡的时间
// 			FoundSthThisTime++;
// 		}
// 		EXTI->PR.B.PR7 = 1; //CLEAR注意STM32清除是给1而查询也是1 囧
// 	}
// 	if(EXTI->PR.B.PR6){
// 		if(GPIOB->IDR.B.IO6) {  //高电平说明是上升沿
// 			CH4UP = FastSave; //给相应的通道赋值
// 		}else{  //下降沿
// 			CH4DOWN = FastSave; //给相应的通道赋值
// 			CH4HTimer = (TIMARR_A+CH4DOWN-CH4UP)%TIMARR_A;   //在下降沿时计算高电平越渡的时间
// 			FoundSthThisTime++;
// 		}
// 		EXTI->PR.B.PR6 = 1; //CLEAR注意STM32清除是给1而查询也是1 囧
// 	}
// 	if(EXTI->PR.B.PR5){
// 		if(GPIOB->IDR.B.IO5) {  //高电平说明是上升沿
// 			CH5UP = FastSave; //给相应的通道赋值
// 		}else{  //下降沿
// 			CH5DOWN = FastSave; //给相应的通道赋值
// 			CH5HTimer = (TIMARR_A+CH5DOWN-CH5UP)%TIMARR_A;   //在下降沿时计算高电平越渡的时间
// 			FoundSthThisTime++;
// 		}
// 		EXTI->PR.B.PR5 = 1; //CLEAR注意STM32清除是给1而查询也是1 囧
// 	}

// 	
// 	OSIntExit();
// }
/*
 * Name				: TIM3initialize
 * Description		: ---
 * Author			: Lynx@sia 84693469@qq.com.
 *
 * History
 * ----------------------
 * Rev	: 0.00
 * Date	: 2013.11.03
 *
 * create.
 * ----------------------
 */
// static int TIM3initialize(int ms)
// {
// 	TIM3->CR1.B.ARPE = 1;
// 	// TIM3->CR1.B.URS = 1;

// 	TIM3->CR2.B.MMS = 1;  //CEN TRIG

// 	TIM3->PSC = TIMPSC - 1;

// 	TIM3->ARR = ms * TIMARR_M - 1;

// 	TIM3->DIER.B.TIE = 1;
// 	TIM3->DIER.B.UIE = 1;

// 	TIM3->CR1.B.DIR = 0;  //up

// 	TIM3->CNT = 0;

// 	TIM3->CR1.B.CEN = 0;
// 	//timer3.timeout_flag = 0;

// 	return 0;
// }

/*
 * Name				: TIM3enable
 * Description		: ---
 * Author			: Lynx@sia 84693469@qq.com.
 *
 * History
 * ----------------------
 * Rev	: 0.00
 * Date	: 2013.11.03
 *
 * create.
 * ----------------------
 */
// static int TIM3enable(void)
// {
// 	TIM3->CNT = 0;

// 	TIM3->CR1.B.CEN = 1;

//  	return 0;
// }

/*
 * Name				: TIM3disable
 * Description		: ---
 * Author			: Lynx@sia 84693469@qq.com.
 *
 * History
 * ----------------------
 * Rev	: 0.00
 * Date	: 2013.11.03
 *
 * create.
 * ----------------------
 */
// static int TIM3disable(void)
// {
// 	TIM3->CR1.B.CEN = 0;

// 	return 0;
// }

// void TIM3_IRQHandler(void)
// {
// 	int TMPStickMoveLR;
// 	int TMPStickPower;
// 	int TMPStickMoveBF;
// 	int TMPStickTurnLR;
// 	int TMPSwitchEnable;
// 	OSIntEnter();
// 	
// 	if(FoundSthThisTime){
// 		TMPStickMoveLR = CH1HTimer;  //还原的各摇杆的位置
// 		TMPStickPower = CH2HTimer;
// 		TMPStickMoveBF = CH3HTimer;
// 		TMPStickTurnLR = CH4HTimer;
// 		TMPSwitchEnable = CH5HTimer;
// 		
// 		if(TMPStickMoveLR>STICK_ERR_INF && TMPStickMoveLR<STICK_ERR_SUP){
// 			TMPStickMoveLR = (TMPStickMoveLR-STICK_M_LR_MID)*100/(STICK_M_LR_SUP-STICK_M_LR_MID);
// 			if(TMPStickMoveLR>99){
// 				receiver.StickMoveLR = 99;
// 			}else if(TMPStickMoveLR<-99){
// 				receiver.StickMoveLR = -99;
// 			}else{
// 				receiver.StickMoveLR = TMPStickMoveLR;
// 			}
// 		}else{
// 			receiver.ReceiverIntErr++;
// 		}
// 		
// 		if(TMPStickMoveBF>STICK_ERR_INF && TMPStickMoveBF<STICK_ERR_SUP){
// 			TMPStickMoveBF = (TMPStickMoveBF-STICK_M_BF_MID)*100/(STICK_M_BF_SUP-STICK_M_BF_MID);
// 			if(TMPStickMoveBF>99){
// 				receiver.StickMoveBF = 99;
// 			}else if(TMPStickMoveBF<-99){
// 				receiver.StickMoveBF = -99;
// 			}else{
// 				receiver.StickMoveBF = TMPStickMoveBF;
// 			}
// 		}else{
// 			receiver.ReceiverIntErr++;
// 		}
// 		
// 		if(TMPStickTurnLR>STICK_ERR_INF && TMPStickTurnLR<STICK_ERR_SUP){
// 			TMPStickTurnLR = (TMPStickTurnLR-STICK_T_LR_MID)*100/(STICK_T_LR_SUP-STICK_T_LR_MID);
// 			if(TMPStickTurnLR>99){
// 				receiver.StickTurnLR = 99;
// 			}else if(TMPStickTurnLR<-99){
// 				receiver.StickTurnLR = -99;
// 			}else{
// 				receiver.StickTurnLR = TMPStickTurnLR;
// 			}
// 		}else{
// 			receiver.ReceiverIntErr++;
// 		}
// 		
// 		if(TMPStickPower>STICK_ERR_INF && TMPStickPower<STICK_ERR_SUP){
// 			TMPStickPower = (TMPStickPower-STICK_P_INF)*100/(STICK_P_SUP-STICK_P_INF);   //注意这里与前面的差别，这里是开关量
// 			if(TMPStickPower>99){
// 				receiver.StickPower = 99;
// 			}else if(TMPStickPower<0){
// 				receiver.StickPower = 0;
// 			}else{
// 				receiver.StickPower = TMPStickPower;
// 			}
// 		}else{
// 			receiver.ReceiverIntErr++;
// 		}
// 		
// 		if(TMPSwitchEnable>STICK_ERR_INF && TMPSwitchEnable<STICK_ERR_SUP){
// 			//注意这里与前面的差别
// 			if(TMPSwitchEnable>SWITCH_ENABLE_MID_UP){  //这里防止它在中间位置来回跳
// 				receiver.SwitchEnable = 1;
// 			}else if(TMPSwitchEnable<SWITCH_ENABLE_MID_DOWN){
// 				receiver.SwitchEnable = 0;
// 			}
// 		}else{
// 			//receiver.SwitchEnable = 0;    这句被我注释掉了，虽然很想留下，但是留下会造成严重的断续控制的问题，不得已啊
// 			receiver.ReceiverIntErr++;
// 		}
// 		
// 		receiver.ReceiverOK = 1;  //接收机状态正常
// 		FoundSthThisTime = 0; //clear
// 	}else{
// 		receiver.ReceiverOK = 0;
// 		receiver.SwitchEnable = 0;
// 		FoundSthThisTime = 0; //clear
// 	}

// 	TIM3->SR.B.UIF = 0;
// 	TIM3->SR.B.TIF = 0;
// 	OSIntExit();
// }

