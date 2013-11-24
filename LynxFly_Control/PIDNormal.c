/*********************************************************************************
*                                山猫飞控（Lynx）
*                             for LynxFly under GPLv2
*
* COPYRIGHT (C) 2012 - 2013, Lynx 84693469@qq.com
*
* Version   	: V1.0
* By        	: Lynx@sia 84693469@qq.com
*
* For       	: Stm32f405RGT6
* Mode      	: Thumb2
* Description   : PID算法封装
*
*				
* Date          : 2013.XX.XX
*******************************************************************************/
#include "PIDNormal.h"

// 定义PID的系数
// #define F_PID_PHI_KP 0 //4   //*57.29578   //Kp*弧度角度比例 横滚角
// #define F_PID_PHI_KI 0 //0.3 //8.1     //*57.29578   //Ki
// #define F_PID_PHI_KD 0 //17     //*57.29578   //Kd

// #define F_PID_THETA_KP 4   //*57.29578   //Kp*弧度角度比例 俯仰角
// #define F_PID_THETA_KI 0.3 //8.1     //*57.29578   //Ki
// #define F_PID_THETA_KD 17     //*57.29578   //Kd

// #define F_PID_PSI_KP 0 //8.5   //*57.29578   //Kp*弧度角度比例 偏航角
// #define F_PID_PSI_KI 0     //*57.29578   //Ki
// #define F_PID_PSI_KD 8.5 //0     //*57.29578   //Kd


// //按照Kp 1/Ti Td的重定义（按照标准PID公式）
// //注意这里定义的是Ti的倒数而不是Ti，为了调参数不出现奇点
// #define F_PID_PHI_KP 5.5 //5 //Kp 横滚角
// #define F_PID_PHI_TII (1/0.8) //(1/0.09) //1/Ti 注意是Ti的倒数
// #define F_PID_PHI_TD 1.1 //0.8 //Td

// #define F_PID_THETA_KP 3.3  //5.5 //5 //4   //Kp 俯仰角  //这个是10Hz控制时的参数，控制效果略逊色于25Hz时
// #define F_PID_THETA_TII (1/0.25)  //(1/0.8) //(1/0.12) //(1/0.53) //1/Ti 注意是Ti的倒数
// #define F_PID_THETA_TD  0.5625  //1.1 //0.73 //0.17  //Td

// // #define F_PID_PSI_KP 15 //0.1   //Kp 偏航角
// // #define F_PID_PSI_TII (1/0.45)   //1/Ti 注意是Ti的倒数
// // #define F_PID_PSI_TD 0 //Td
// //为了自适应降低了10Hz时的控制增益
// #define F_PID_PSI_KP 1 //3 //0.1   //Kp 偏航角
// #define F_PID_PSI_TII (1/0.45)   //1/Ti 注意是Ti的倒数
// #define F_PID_PSI_TD 0 //Td

//-----------------------------------------------------------
//按照Kp 1/Ti Td的重定义（按照标准PID公式）
//注意这里定义的是Ti的倒数而不是Ti，为了调参数不出现奇点
//注意，使用PD型的控制器请务必改小摇杆控制的缩放量，使用PID控制时请放大该控制量
#define F_PID_PHI_KP (24.2393f) //(8.82f) //Kp 横滚角
#define F_PID_PHI_TII (1/2.0f) //(1/0.25f) //(1/2.6475f)) // 1/Ti 注意是Ti的倒数
#define F_PID_PHI_TD (0.17004f) //(0.66187f) //Td

#define F_PID_THETA_KP (24.2393f) //(8.82f)   //Kp 俯仰角  //这个是10Hz控制时的参数，控制效果略逊色于25Hz时
#define F_PID_THETA_TII (1/2.0f) //(1/0.25f) // 1/Ti 注意是Ti的倒数
#define F_PID_THETA_TD (0.17004f) //(1.3f)   //Td

//为了自适应降低了10Hz时的控制增益
#define F_PID_PSI_KP (20.3175f) //3 //0.1   //Kp 偏航角
#define F_PID_PSI_TII (1/0.19651f) //(1/0.45)   //1/Ti 注意是Ti的倒数
#define F_PID_PSI_TD 0 //Td
//-----------------------------------------------------------

#define F_PID_BOUNDARY_I_PHI (50/(F_PID_PHI_KP*F_PID_PHI_TII))        //设置积分上限，防止积分太多回不来，也限制了转动的角度
#define F_PID_BOUNDARY_I_THETA (50/(F_PID_THETA_KP*F_PID_THETA_TII))        //设置积分上限，防止积分太多回不来，也限制了转动的角度
#define F_PID_BOUNDARY_I_PSI (50/(F_PID_PSI_KP*F_PID_PSI_TII))        //设置积分上限，防止积分太多回不来，也限制了转动的角度

//PID的积分全局变量
static float EIPhi = 0;
static float EITheta = 0;
static float EIPsi = 0;

void PIDNormalClear(void) {
	EIPhi = 0;
	EITheta = 0;
	EIPsi = 0;
}

float PIDNormal(float CDataYr[], float CDataYE[], float CDataYG[], int Channel, float SampleTime) {
	
	float EP = 0;
	float EI = 0;
	float ED = 0;
	
	float TempIClc = 0;
	float ControlOut = 0;
	
	EP = CDataYr[0]-CDataYE[0];  //本次的误差
	
	ED = -CDataYG[0];   //用陀螺仪的测量值代替微分
	
	//积分的处理需要额外的一些考虑
	TempIClc = EP*SampleTime;//累加积分并考虑采样时间（一阶采样积分）
	if(Channel == I_PID_CHANNEL_ROLL){
		if((TempIClc>=0 && EIPhi<F_PID_BOUNDARY_I_PHI)||(TempIClc<0 && EIPhi>(-F_PID_BOUNDARY_I_PHI))){
			EIPhi += TempIClc;
			EI = EIPhi;
		}
		//生成控制输出
		ControlOut = F_PID_PHI_KP*(EP+F_PID_PHI_TII*EI+F_PID_PHI_TD*ED);
	}else if(Channel == I_PID_CHANNEL_PITCH){
		if((TempIClc>=0 && EITheta<F_PID_BOUNDARY_I_THETA)||(TempIClc<0 && EITheta>(-F_PID_BOUNDARY_I_THETA))){
			EITheta += TempIClc;	
			EI = EITheta;
		}
		//生成控制输出
		ControlOut = F_PID_THETA_KP*(EP+F_PID_THETA_TII*EI+F_PID_THETA_TD*ED);
	}else if(Channel == I_PID_CHANNEL_YAW){
		if((TempIClc>=0 && EIPsi<F_PID_BOUNDARY_I_PSI)||(TempIClc<0 && EIPsi>(-F_PID_BOUNDARY_I_PSI))){
			EIPsi += TempIClc;
			EI = EIPsi;
		}
		//生成控制输出
		ControlOut = F_PID_PSI_KP*(EP+F_PID_PSI_TII*EI+F_PID_PSI_TD*ED);
	}else{
		EI = 0;  //这情况理论上不肯能出现
		//生成控制输出
		ControlOut = 0;
	}
	
	return ControlOut;  //返回该通道的值
}

float PDNormal(float CDataYr[], float CDataYE[], float CDataYG[], int Channel, float SampleTime) {
	
	float EP = 0;
	float ED = 0;
	
	float ControlOut = 0;
	
	EP = CDataYr[0]-CDataYE[0];  //本次的误差
	
	ED = -CDataYG[0];   //用陀螺仪的测量值代替微分
	
	if(Channel == I_PID_CHANNEL_ROLL){
		//生成控制输出
		ControlOut = F_PID_PHI_KP*(EP+F_PID_PHI_TD*ED);
	}else if(Channel == I_PID_CHANNEL_PITCH){
		//生成控制输出
		ControlOut = F_PID_THETA_KP*(EP+F_PID_THETA_TD*ED);
	}else if(Channel == I_PID_CHANNEL_YAW){
		//生成控制输出
		ControlOut = F_PID_PSI_KP*(EP+F_PID_PSI_TD*ED);
	}else{
		//生成控制输出
		ControlOut = 0;
	}
	
	return ControlOut;  //返回该通道的值
}

float PINormal(float CDataYr[], float CDataY[], int Channel, float SampleTime) {
	
	float EP = 0;
	float EI = 0;
	
	float TempIClc = 0;
	float ControlOut = 0;
	
	EP = CDataYr[0]-CDataY[0];  //本次的误差
	
	//积分的处理需要额外的一些考虑
	TempIClc = EP*SampleTime;//累加积分并考虑采样时间（一阶采样积分）
	if(Channel == I_PID_CHANNEL_ROLL){
		if((TempIClc>=0 && EIPhi<F_PID_BOUNDARY_I_PHI)||(TempIClc<0 && EIPhi>(-F_PID_BOUNDARY_I_PHI))){
			EIPhi += TempIClc;
			EI = EIPhi;
		}
		//生成控制输出
		ControlOut = F_PID_PHI_KP*(EP+F_PID_PHI_TII*EI);
	}else if(Channel == I_PID_CHANNEL_PITCH){
		if((TempIClc>=0 && EITheta<F_PID_BOUNDARY_I_THETA)||(TempIClc<0 && EITheta>(-F_PID_BOUNDARY_I_THETA))){
			EITheta += TempIClc;	
			EI = EITheta;
		}
		//生成控制输出
		ControlOut = F_PID_THETA_KP*(EP+F_PID_THETA_TII*EI);
	}else if(Channel == I_PID_CHANNEL_YAW){
		if((TempIClc>=0 && EIPsi<F_PID_BOUNDARY_I_PSI)||(TempIClc<0 && EIPsi>(-F_PID_BOUNDARY_I_PSI))){
			EIPsi += TempIClc;
			EI = EIPsi;
		}
		//生成控制输出
		ControlOut = F_PID_PSI_KP*(EP+F_PID_PSI_TII*EI);
	}else{
		EI = 0;  //这情况理论上不肯能出现
		//生成控制输出
		ControlOut = 0;
	}
	
	return ControlOut;  //返回该通道的值
}








