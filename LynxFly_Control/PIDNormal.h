#ifndef _PIDNORMAL_H_
#define _PIDNORMAL_H_


#define I_PID_CHANNEL_ROLL 2  //定义通道的值
#define I_PID_CHANNEL_PITCH 1
#define I_PID_CHANNEL_YAW 3


void PIDNormalClear(void);
float PIDNormal(float CDataYr[], float CDataYE[], float CDataYG[], int Channel, float SampleTime);
float PDNormal(float CDataYr[], float CDataYE[], float CDataYG[], int Channel, float SampleTime);
float PINormal(float CDataYr[], float CDataY[], int Channel, float SampleTime);







#endif

