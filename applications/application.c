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
* Description   : RT-Thread 用户初始化线程
*
*				
* Date          : 2013.XX.XX
*******************************************************************************/
/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <stdio.h>
#include <board.h>
#include <rtthread.h>

#include <battery.h>   //电池头文件
#include <pwmRotor4.h>   //电机控制头文件
#include <stm32f4_i2c_dma.h>   //DMA形式的I2C头文件

//用户线程的相关引用
#include "..\LynxFly\global_define.h"
#include "..\LynxFly\TaskNotify.h"
#include "..\LynxFly\TaskComm.h"
#include "..\LynxFly\TaskControl.h"
#include "..\LynxFly\TaskIMU.h"

#include "cpuusage.h"  //加载CPU使用情况函数

#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#endif

#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif

#ifdef RT_USING_DFS
#include <dfs_init.h>
#include <dfs_fs.h>
#include <dfs_elm.h>
#endif

//一大堆除警告用的引用 避免警告而又懒得大改而已
void rt_platform_init(void);
int rt_wlan_init(void);
void lwip_sys_init(void);
int WlanDirectConnect(void);
int ConnectSSID(char *connssid);
int wlan_pm_exit(void);

//电池电量显示的几个相关参量
int BatteryTimeToSleep = 0;   //休眠时间为一个0~20的数
float BatteryVTop = 4.1f;   //电池满电的电压
float BatteryVButtom = 3.0f;   //电池完全空电的电压
float BatteryVNow = 0;     //电池当前的电压

extern char    *Default_SSID;   //调用wlan_main中的默认SSID，来修改该默认值

//控制电源启动的全局变量，通电以后飞机不启动进打开充电线程，收到开机命令时才开始启动各线程与部件
int PowerOnSign = 0;

void rt_init_thread_entry(void* parameter)
{
	finsh_system_init();
	finsh_set_device(RT_CONSOLE_DEVICE_NAME);
	/* initialize the device file system */
	dfs_init();
	/* initialize the elm chan FatFS file systam*/
	elm_init();

	rt_platform_init();
	
	cpu_usage_init();   //调用初始化函数，加载CPU使用的钩子。

	/* Filesystem Initialization */
#ifdef RT_USING_DFS
	{
#ifdef RT_USING_DFS_ELMFAT
			/* mount sd card fat partition 1 as root directory */
			if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
			{
					rt_kprintf("File System initialized!\n");
			}
			else
					rt_kprintf("File System initialzation failed!\n");
#endif
	}
#endif
	
	pwmRotor4.initialize();   //初始化电机
	battery.initialize();  //初始化电池的测量
	I2C_Config();//启动I2C
	
	
	//未按动开机键前程序始终停留在这里，这里目前是一个通过晃动板子查看电池电量的方，很有趣吧 晃晃看灯的占空比
	while(KEY == 0 && PowerOnSign == 0)
	{
		LED_R_ON();
		
		BatteryVNow = battery.read();
		BatteryTimeToSleep = (int)((BatteryVNow-BatteryVButtom)/(BatteryVTop-BatteryVButtom)*20);   //计算电池电量0~20
		
		if(BatteryTimeToSleep>20)  //越界时的一些规范化
			BatteryTimeToSleep = 20;
		else if(BatteryTimeToSleep<0)
			BatteryTimeToSleep = 0;
		
		rt_thread_delay(rt_tick_from_millisecond(BatteryTimeToSleep));   //延时
		
		if(BatteryTimeToSleep>0)  //如果电量不满则显示差别
		{
			LED_R_OFF();
			rt_thread_delay(rt_tick_from_millisecond(20-BatteryTimeToSleep));   //延时
			
		}
	}
	//默认情况下只有当按下启动按键时才正式开机，否则仅显示充电状态
	PowerOnSign = 1;
	
// // 	pwmRotor4.setMotor1I(100);  //吓测试电机用的
// // 	pwmRotor4.setMotor2I(100);
// // 	pwmRotor4.setMotor3I(100);
// // 	pwmRotor4.setMotor4I(100);
	
#ifdef RT_USING_LWIP
	{
		/* register ethernetif device */
		eth_system_device_init();

		rt_wlan_init();
		rt_kprintf("wlan initialized\n");		
		
		//Default_SSID="LynxGround"; 
		//ConnectSSID(Default_SSID);  //仅靠搜索SSID连接的方法 同样需要指定正确的IP地址
		//WlanDirectConnect();  //原先的SSID与Mac地址直连方法 需要指定正确的IP地址
		
		Default_SSID="rtthread"; 
		StateFlag |= STATE_COMM_CONNECTING;  //进入连接状态
		//if(WlanDirectConnect())    //原先的SSID与Mac地址直连方法 需要指定正确的IP地址
		if(ConnectSSID(Default_SSID))  //仅靠搜索SSID连接的方法 同样需要指定正确的IP地址
		{
			StateFlag |= STATE_COMM_FAIL;  //连接失败，更改标志位
		}
		else
		{
			StateFlag &= ~STATE_COMM_FAIL;  //连接正常，取消通信失效标志
		}
		StateFlag &= ~STATE_COMM_CONNECTING;  //完成连接部分
		
		wlan_pm_exit();
		
		// init lwip system 注意一定要在wlan连接之后，否则会出wlan did not connect警告
		lwip_sys_init();
		rt_kprintf("TCP/IP initialized!\n");
	}
#endif
}

//山猫飞控的启动线程
void LynxFly_init(void* parameter)
{
	rt_err_t result;  //记录线程创建的结果
	
	rt_kprintf("LynxFly initializating now!\n");
	
	//初始化时一直卡在这里，直到获得开机指令才执行接下来的代码
	while(!PowerOnSign);
	
	rt_thread_delay(rt_tick_from_millisecond(500));  //睡眠半秒等待SD卡完成初始化，要不烦的很
	
	//------- 初始化LynxFly各线程间通信信号量----------
	result = rt_sem_init(&IMUVarSem , "IMUVarSem", 1, RT_IPC_FLAG_FIFO);
	if (result != RT_EOK)
			rt_kprintf("%7d-LynxFly-->Sem IMUVarSem ERROR\n",rt_tick_get());
	result = rt_sem_init(&ControlJSSem , "ControlJSSem", 1, RT_IPC_FLAG_FIFO);
	if (result != RT_EOK)
			rt_kprintf("%7d-LynxFly-->Sem ControlJSSem ERROR\n",rt_tick_get());
	result = rt_sem_init(&ControlPPRYChannel , "ControlPPRYChannel", 1, RT_IPC_FLAG_FIFO);
	if (result != RT_EOK)
			rt_kprintf("%7d-LynxFly-->Sem ControlPPRYChannel ERROR\n",rt_tick_get());
	
	
	//------- 启动LynxFly各线程----------

	//启动山猫飞控惯性导航线程
	result = rt_thread_init(&Thread_TaskIMU,
								 "TaskIMU",
								 TaskIMU,
								 RT_NULL,
								 &TaskIMU_Stack[0],
								 sizeof(TaskIMU_Stack),PRIORITY_TASK_IMU,rt_tick_from_millisecond(I_SAMPLETIME_CYCLE));
	if (result == RT_EOK)
	{
		rt_thread_startup(&Thread_TaskIMU);
		rt_kprintf("%7d-LynxFly-->TaskIMU Started!\n",rt_tick_get());
	}
	else
	{
		rt_kprintf("%7d-LynxFly-->TaskIMU Failed!\n",rt_tick_get());
	}
	
	//启动山猫飞控控制线程
	result = rt_thread_init(&Thread_TaskControl,
								 "TaskControl",
								 TaskControl,
								 RT_NULL,
								 &TaskControl_Stack[0],
								 sizeof(TaskControl_Stack),PRIORITY_TASK_CONTROL,rt_tick_from_millisecond(I_CONTROL_CYCLE));
	if (result == RT_EOK)
	{
		rt_thread_startup(&Thread_TaskControl);
		rt_kprintf("%7d-LynxFly-->TaskControl Started!\n",rt_tick_get());
	}
	else
	{
		rt_kprintf("%7d-LynxFly-->TaskControl Failed!\n",rt_tick_get());
	}
	
	//启动山猫飞控通知线程
	result = rt_thread_init(&Thread_TaskNotify,
								 "TaskNotify",
								 TaskNotify,
								 RT_NULL,
								 &TaskNotify_Stack[0],
								 sizeof(TaskNotify_Stack),PRIORITY_TASK_NOTIFY,rt_tick_from_millisecond(I_NOTIFY_CYCLE));
	if (result == RT_EOK)
	{
		rt_thread_startup(&Thread_TaskNotify);
		rt_kprintf("%7d-LynxFly-->TaskNotify Started!\n",rt_tick_get());
	}
	else
	{
		rt_kprintf("%7d-LynxFly-->TaskNotify Failed!\n",rt_tick_get());
	}
	
	//启动山猫飞控通信线程
	result = rt_thread_init(&Thread_TaskComm,
								 "TaskComm",
								 TaskComm,
								 RT_NULL,
								 &TaskComm_Stack[0],
								 sizeof(TaskComm_Stack),PRIORITY_TASK_COMM,rt_tick_from_millisecond(10));
	if (result == RT_EOK)
	{
		rt_thread_startup(&Thread_TaskComm);
		rt_kprintf("%7d-LynxFly-->TaskComm Started!\n",rt_tick_get());
	}
	else
	{
		rt_kprintf("%7d-LynxFly-->TaskComm Failed!\n",rt_tick_get());
	}
}


int rt_application_init()
{
  rt_thread_t tid;
	
	//定义系统初始状态
	StateFlag |= STATE_COMM_FAIL;  //定义通信模块初始为失效状态
	PowerOnSign = 0;  //定义初始情况下飞机不打开各部件功能仅充电

  tid = rt_thread_create("init",
	rt_init_thread_entry, RT_NULL,
	2048, RT_THREAD_PRIORITY_MAX/3, 20);

	if (tid != RT_NULL)
			rt_thread_startup(tid);
	
	
	tid = rt_thread_create("LynxFly_init",
	LynxFly_init, RT_NULL,
	1024, RT_THREAD_PRIORITY_MAX/2, 20);

	if (tid != RT_NULL)
			rt_thread_startup(tid);
	
	

	return 0;
}

/*@}*/
