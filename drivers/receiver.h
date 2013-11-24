/*------------------------------------------------------
FILE NAME   : receiver.h
DESCRIPTION : event file header
VERSION     : 0.0.0 (C)Lynx
AUTHOR      : Lynx
CREATE DATE : 2013-11-3
MODIFY DATE :
LOGS        :-\
--------------------------------------------------------*/
#ifndef __receiver_h__
#define __receiver_h__

#include <board.h>

//------------------- Define ---------------------------//
//#define receiverA	1
//#define receiverB	2
//#define receiverC	3

 

//----------------- Typedef------------------------------//
typedef struct{
		int (* initialize)(void);
	
	
		int ReceiverIntErr;  //累计计时中断错误的次数
		int ReceiverOK;  //接收机状态 0为无信号
		int StickMoveLR;  //还原的各摇杆的位置
		int StickPower;
		int StickMoveBF;
		int StickTurnLR;
		int SwitchEnable;
}receiver_T;
 
//----------------- Extern ------------------------------//
extern receiver_T receiver;

#endif //__receiver_h__
