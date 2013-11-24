#ifndef __pwmRotor4_h__
#define __pwmRotor4_h__

#include <board.h>

#define PWM1  TIM2->CCR1
#define PWM2  TIM2->CCR2
#define PWM3  TIM2->CCR4    //猞猁更改 20131111 交换了3和4号电机
#define PWM4  TIM2->CCR3

typedef struct {
	int (* initialize)(void);
	int (* frequency)(int freq, float percentage);
	int (* setFreq)(int freq);
	int (* setMotor1F)(float percentage);
	int (* setMotor2F)(float percentage);
	int (* setMotor3F)(float percentage);
	int (* setMotor4F)(float percentage);
	int (* setMotor1I)(int percentage);
	int (* setMotor2I)(int percentage);
	int (* setMotor3I)(int percentage);
	int (* setMotor4I)(int percentage);
}PWMROTOR4_T;                                   //声明结构体类型PWMROTOR4_T

extern PWMROTOR4_T pwmRotor4;                     //PWMROTOR4是类型为PWMROTOR4_T的结构体变量


#endif
