/********************************************************************************
File: main.c                                                                

Description:
Container for the firmware.  
********************************************************************************/
#include "configuration.h"
/*******************************************************************************/
sbit Start 	= P3^2;												//接外部中断0
sbit Modify	= P3^3;												//接外部中断1
sbit State1	= P1^0;												//接LED1
sbit State2	= P1^1;												//接LED2
/*******************************************************************************/
/* New variables */
u8	u8SensorAddr	  = 0;									//当前地址位置，跑地址映射表的计数器
u8  u8UARTRecCount  = 1;									//接收计数器，用来接收正确结果
u8  u8OverTimeCount = 0;									//超时计数器，用来掉线重连
u8  u8AddressFlag		= 0;									//地址标记，用来更新地址映射表
u8  u8ScanTime			= 0;									//扫描计时器
u16 u16UartResult   = 0;									//返回结果
u8  u8SensorQueue[16]= {0};								//当前地址队列，存储地址映射表

bit	NewRec 					= 1;									//接收标记
bit SendEnd 				= 1; 									//发送标记
bit StartFlag				= 0;									//启动/停止标记
bit ModifyFlag			= 0;									//修改地址标记
u8 i;
/*******************************************************************************/
/* fuction declaration */
void SM_Idle(void);
void SM_Error(void);

/*******************************************************************************/
/*main process*/
void main(){
	/* Application initialization */
	LcdInit();				//LCD初始化
	LcdStart();				//开机界面
	LcdBasic();				//固定显示内容
	UARTInit();				//串口初始化
	ES 	= 1;					//开串口中断
	EA 	= 1;					//开总中断
	EX0 = 1;					//开外部中断0
	IT0 = 1;					//下降沿触发
	EX1 = 1;					//开外部中断1
	IT1 = 1;					//下降沿触发
	sensorInit();	
/*-----------------------------------------------------------------------------*/	
/* Super loop */ 
	while(1){
		if(StartFlag){	
			// nomal state
			if(NewRec){
					SM_Idle();										
			}
			// error state
			if(u8OverTimeCount == 4){												//超时重传
				SM_Error();
			}
			// Security arrangement
			delayms(20);																		//延时20ms
			if(u8ScanTime == 100){													//定时更新地址映射表
				u8ScanTime = 0;
				sensorInit();																	
				allocAddress();																
			}
			u8OverTimeCount++;															//超时计数器自增
			u8ScanTime++;																		//扫描计数器自增
		}else if(ModifyFlag){
			State2 = 0;																			//修改地址LED亮	
			for(i = 0; u8SensorQueue[i]; i++){
				setAddress(u8SensorQueue[i], InitAddress);		//修改地址
			}	
			State2 = 1;																			//修改地址LED灭
			ModifyFlag = 0;																	//修改标志清零
		}
		/*
		//Display on LCD   2
		if(u8SensorAddr%2){
			LcdDispNum(6,1,u16UartResult);
		}else{
			LcdDispNum(6,0,u16UartResult);
		}
		*/	
		// Display on LCD  4 
		switch(u8SensorAddr%4){
			case 0:		LcdDispNum(2,0,u16UartResult);		break;						
			case 1:		LcdDispNum(8,0,u16UartResult);		break;
			case 2:		LcdDispNum(2,1,u16UartResult);		break;
			default:	LcdDispNum(8,1,u16UartResult);				
		}
		
	}	
}

/*-----------------------------------------------------------------------------*/
/*Interupt service subprogram*/
void serInt (void) interrupt 4   																	//串口中断
{  
	if(RI == 1){  																									//如果收到
		RI = 0;																												//清除RI中断标志		
		if(u8UARTRecCount == 1){
			if(SBUF <= 0x14){																						//判断，如果数据错误则丢弃
				u16UartResult = SBUF << 8;																//第一次接收高位
				u8UARTRecCount++;																					//接受计数器自增
			}
		}else{
			u16UartResult = u16UartResult + SBUF;												//第二次接收低位
			u8UARTRecCount--;																						//接收计数器清零
			NewRec = 1;																									//接收完成置1
		}			
	}else{        																									//如果送毕  
		TI = 0;      																									//清除TI中断标志 
		SendEnd = 1;  																								//发送标志置1
	}  
}

/*-----------------------------------------------------------------------------*/
/*key1 stop/start*/
void key1 (void) interrupt 0   																		//按键1中断
{ 
	if(Start == 0){
		delayms(20);																									//延时去抖
		if((Start == 0) && Modify){
			StartFlag = ~StartFlag;																			//启动标志翻转
			State1 = ~State1;																						//对应LED亮灭
		}
	}
}

/*-----------------------------------------------------------------------------*/
/*key2 modify the address*/
void key2 (void) interrupt 2   																		//按键2中断
{ 
	if(Modify == 0){
		delayms(20);																									//延时去抖
		if((Modify == 0) && (StartFlag == 0)){		
			ModifyFlag = 1;																							//修改标志置1
		}
	}
}
/*-----------------------------------------------------------------------------*/
/*nomal state*/
void SM_Idle(void){
	if(u8SensorQueue[u8SensorAddr]){
		NewRec = 0;																								//清除变量值，准备接受下一次中断
		u8OverTimeCount = 0;																			//超时计数器，防止卡死
		activeSensor(u8SensorQueue[u8SensorAddr]);								//激活当前传感器
		u8SensorAddr++;																						//切换到下一传感器
		if(u8SensorQueue[u8SensorAddr] == 0){
				u8SensorAddr = 0;																			//
		}
	}																							
}

/*-----------------------------------------------------------------------------*/
/*error state*/
void SM_Error(void){
	/*复位*/
	if(u8SensorAddr){
		u8SensorAddr--;
	}
	u8OverTimeCount = 0;
	NewRec = 1;	
	/*如果多次查询无果则从地址表中删去该项目*/
	if(u8AddressFlag == 4){
		u8 i, j = 0;
		for(i = 0; u8SensorQueue[i]; i++){
			sentCommand(u8SensorQueue[i], ConnectTest);													//测试传感器是否连接
			delayms(4);		
			if(u16UartResult != 529){
				for(j = i; u8SensorQueue[j]; j++){																//删除离线的设备
					u8SensorQueue[j] = u8SensorQueue[j+1];
				}			
			}
			u16UartResult = 0;																						
		}	
	}
	u8AddressFlag++;
}

/*----------------------------------------------------------------------------*/
/* End of File */
/*----------------------------------------------------------------------------*/
