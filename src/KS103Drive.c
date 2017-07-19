/*******************************************************************************
File: delay.c                                                                

Description:
Delay for a certain time.

--------------------------------------------------------------------------------
API
void sentCommand(u8 u8addr, u8 u8cmd)
sent command to a specific address,
e.g.
sentCommand(u8addr,u8cmd) means sent 8 bit command to 8 bit address.

void setAddress(u8 addr1, u8 addr2)
amend a new address to the sencor
e.g.
setAddress(0xee, 0xe8) means 
*******************************************************************************/
#include "configuration.h"
/******************************************************************************/

extern u16 u16UartResult;									
extern u8  u8SensorQueue[8];								
/*******************************************************************************
* Function Definitions
*******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Public Functions */
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Function sentComand

Description:向固定地址也就是固定的传感器和02寄存器发送命令
	-
Requires:地址和命令
	-    
Promises:
	- 
*/
void sentCommand(u8 u8addr, u8 u8cmd){
	UARTSend(u8addr);							//发送地址
	UARTSend(0x02);								//选择02寄存器
	UARTSend(u8cmd);							//发送命令
}
/*------------------------------------------------------------------------------
Function setAddress

Description:修改传感器地址,addr1为原始地址，addr2为修改后的地址
	-
Requires:初始地址和修改后地址
	-    
Promises:
	- 
*/
void setAddress(u8 addr1, u8 addr2){
	sentCommand(addr1, 0x9a);			//修改地址第一时序
	delayms(5);										//延时至少1ms
	sentCommand(addr1, 0x92);			//修改地址第二时序
	delayms(5);										//延时至少1ms
	sentCommand(addr1, 0x9e);			//修改地址第三时序
	delayms(5);										//延时至少1ms
	sentCommand(addr1, addr2);		//发送新的地址
	delayms(150);									//延时至少100ms
}
/*------------------------------------------------------------------------------
Function setDetectedCmd

Description:发送探测指令，包含延时
	-
Requires:
	-    
Promises:
	- 
*/
void setDetectedCmd(u8 u8cmd){
	UARTSend(u8cmd);							//UART发送
	delayus(20);									//至少延时20us
}
/*------------------------------------------------------------------------------
Function activeSensor

Description:
	-激活传感器，向传感器发送特定指令，来接收返回数据
Requires:
	-这里要注意的是对于同一个传感器，必须要等到传感器返回数据完成后才能发送
	新的探测指令    
Promises:
	- 
*/
void activeSensor(u8 u8addr)
{
	setDetectedCmd(u8addr);      	//选择传感器 
	setDetectedCmd(0x02);					//选择寄存器
	setDetectedCmd(DetectedCmd);				//发送指令
}

/*------------------------------------------------------------------------------
Function SensorInit

Description:
	-检测传感器地址，将检测到的地址拼凑成新的队列，维持一个动态的地址映射表
Requires:
	-   
Promises:
	- 
*/
void sensorInit(void){
	u8 i,j = 0;
	for(i = 0; i < 8; i++){
		u8SensorQueue[i] = 0;
	}
	for(i = 0; i < 19; i++){
		sentCommand(SensorAddress[i], ConnectTest);													//测试传感器是否连接
		delayms(4);
		if(u16UartResult == 529){	
			u16UartResult = 0;
			u8SensorQueue[j] = SensorAddress[i];
			j++;
		}	
	}
}

/*------------------------------------------------------------------------------
Function allocAddress

Description:
	-当插入传感器地址为默认地址的时候，系统将随机分配一个新的地址给该传感器，以减少
	冲突概率
Requires:
	-   
Promises:
	- 
*/
void allocAddress(void){
	u8 i,j = 0;
	sentCommand(InitAddress, ConnectTest);
	delayms(4);
	if(u16UartResult == 529){	
		u16UartResult = 0;
		for(i = 0; i < 19; i++){
			while(u8SensorQueue[j] != SensorAddress[i]){
				if(u8SensorQueue[j]){
					j++;
				}else{
					setAddress(InitAddress, SensorAddress[i]);
					u8SensorQueue[j] = SensorAddress[i];
					break;
				}					
			}	
		}
	}
}