/*******************************************************************************
File: lcdDrive.c                                                                

Description:
operation to Lcd1602.
--------------------------------------------------------------------------------
API
void delayus(u16 time)
Delay for one us when the time is one,
e.g.
delayus(500) means delay for 500us.
void delayms(u16 time)
Delay for one ms when the time is one,
e.g.
delayus(500) means delay for 500us.
*******************************************************************************/
#include"configuration.h"

/******************************************************************************/
u8 code disp1[] = "MultiDetected-";
u8 code disp2[] = "SYSTEM";
u8 code disp3[] = "Author";
u8 code disp4[] = "yomic & PuZiQian";
/*******************************************************************************
* Function Definitions
*******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Public Functions */
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Function LcdWriteByte

Description:
	-向LCD发送指令或数据，Rors为指令数据选择位
Requires:
	-rors == 0，dat == Data ;
	-rors == 1, dat == Cmd;
Promises:
	-
*/
void LcdWriteByte(u8 dat, bit RorS){
	LCD_EN = 0; 								//关使能端，不让写入或读出数据
	LCD_RW = 0; 								//选择写入
	delayus(10);      					//延时 25us
	if(RorS)	LCD_RS = 1;				//高电平写入数据
	else			LCD_RS = 0;				//低电平写入命令
	LCD_DB = dat;								//写入数据或命令
	LCD_EN = 1; 								//开使能端
	delayus(10);								//延时等待写入完成
	LCD_EN = 0;									//关使能
	delayms(1);									//延时等待Lcd响应
}

/*------------------------------------------------------------------------------
Function LcdInit

Description:LCD初始化，包含设定显示，清屏光标复位
	-
Requires:
	-    
Promises:
	- 
*/
void LcdInit(void){
  delayms(50);								//等待Lcd空闲
  LcdWriteByte(0x38,0); 			//8位数据位宽,两行,5*7字符点阵
	while(BusyCheck());					//等待操作完成
	LcdWriteByte(0x0C,0); 			//显示:开，光标:关，光标闪烁:关
	while(BusyCheck());					//等待操作完成
	LcdWriteByte(0x06,0); 			//输入数据后光标右移，数据不移动
	while(BusyCheck());					//等待操作完成
	LcdClear();									//清屏
	LcdCurRst();								//光标复位
}

/*------------------------------------------------------------------------------
Function LcdClear

Description:发送清屏指令，包含延时
	-
Requires:
	-    
Promises:
	- 
*/
void LcdClear(void){
	LcdWriteByte(0x01,0);				//清屏指令
	delayms(50);
}

/*------------------------------------------------------------------------------
Function LcdCurRst

Description:发送光标复位指令，包含延时
	-
Requires:
	-    
Promises:
	- 
*/
void LcdCurRst(void){
	LcdWriteByte(0x02,0);				//光标复位指令
	delayms(50);
}

/*------------------------------------------------------------------------------
Function LcdSetPos

Description:设置光标位置
	-px为列，py为行
Requires:
	-py为0，在第一行显示；py为1，在第二行显示    
Promises:
	- 
*/
void LcdSetPos(u8 px, u8 py){
	if(py){
		LcdWriteByte(px|0x80 + 0x40, 0); 	//第二行显示
	}else{
		LcdWriteByte(px|0x80, 0); 				//第一行显示
	}
}

/*------------------------------------------------------------------------------
Function BusyCheck

Description:忙检测指令，可以用延时代替
	-速度匹配
Requires:
	-    
Promises:
	- 
*/
bit BusyCheck(void){
	LCD_RW = 1;													//读取
	LCD_RS = 0;													//指令
	delayus(50);												
	LCD_DB = LCD_DB << 1;								//忙标志左移
	return CY;													//读取标志
}

/*------------------------------------------------------------------------------
Function LcdCGRAM

Description:写入自定义字符
	-CGRAM一共64个字节，一个5*8字符占8个字节，所以最多能写入8个字符
Requires:
	-    
Promises:
	- 
*/
void LcdCGRAM(u8 dat[8], u8 u8addr){
	u8 temp;																			//设定计数器
	u8addr = u8addr % 8;													//8个数一个循环
	for(temp = 0; temp < 8; temp++){
		LcdWriteByte(u8addr << 3|0x40 + temp, 0);		//选择写入位置
		LcdWriteByte(dat[temp], 1);									//选择写入内容
	}
}

/*------------------------------------------------------------------------------
Function LcdDispNum

Description:显示数字
	-px,py为位置，Data为数字
Requires:
	-    
Promises:
	- 
*/
void LcdDispNum(u8 px, u8 py, u16 Data){
	LcdSetPos(px,py);															//设定位置
	NumConvertChar(Data);													//数字转字符
}

/*------------------------------------------------------------------------------
Function NumConvertChar

Description:数字转化为字符
	-为上面的显示数字提供辅助
Requires:
	-    
Promises:
	- 
*/
void NumConvertChar(u16 Data){
	u8 temp, i;																		
	u8 output[5];																	//转化后的结果
	for(i = 5; i > 0; i--){												
		temp = Data % 10;														//将每位都提取出来
		Data = Data / 10;				 										
		if(Data == 0 && temp == 0){									
			output[i-1] = ' ';												//高位的零删除掉
		}else{																			
			output[i-1] = temp + 0x30;								//数字转化为ASCII码
		}																						
	}
	for(i = 0; i < 5; i++){
		LcdWriteByte(output[i], 1);									//显示字符
	}		
}

/*------------------------------------------------------------------------------
Function LcdDispNum

Description:显示数字
	-px,py为位置，Data为数字
Requires:
	-    
Promises:
	- 
*/
void LcdDispStr(u8 px, u8 py, u8 *u8Str){
	u8 u8Index = 0;
	u8 u8Message[20] = {0};
	LcdSetPos(px,py);
	while(*u8Str != 0){
		u8Message[u8Index] = *u8Str;
		u8Str++;
		u8Index++;
	}
	u8Message[u8Index] = 0;
	for(u8Index = 0;u8Message[u8Index] != 0; u8Index++){
		LcdWriteByte(u8Message[u8Index],1);
	}
}

/*----------------------------------------------------------------------------*/
/* Private Functions */
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Function LcdBasic

Description:固定显示的内容
	-
Requires:
	-    
Promises:
	- 
*/
void LcdBasic(void){		
	u8 temp;
	for(temp = 0; temp < 8; temp++){
		LcdCGRAM(LcdData[temp], temp);							//装载CGRAM内容
	}
	LcdSetPos(0,0);
	LcdWriteByte(4,1);
	LcdSetPos(15,0);
	LcdWriteByte(5,1);
	LcdSetPos(0,1);
	LcdWriteByte(6,1);
	LcdSetPos(15,1);
	LcdWriteByte(7,1);
	/*
	LcdSetPos(2,0);
	LcdWriteByte('O',1);
	LcdWriteByte('b',1);
	LcdWriteByte('1',1);
	LcdWriteByte(':',1);
	LcdSetPos(11,0);
	LcdWriteByte('m',1);
	LcdWriteByte('m',1);
	LcdSetPos(2,1);
	LcdWriteByte('O',1);
	LcdWriteByte('b',1);
	LcdWriteByte('2',1);
	LcdWriteByte(':',1);
	LcdSetPos(11,1);
	LcdWriteByte('m',1);
	LcdWriteByte('m',1);
	*/
}

/*------------------------------------------------------------------------------
Function LcdStart

Description:开机界面
	-
Requires:
	-    
Promises:
	- 
*/
void LcdStart(void){
	u8 i,j = 0;
	LcdDispStr(1,0,disp1);			//显示MULTIDETECTED
	LcdDispStr(5,1,disp2);			//显示SYSTEM
	delayms(1000);
	LcdClear();
	LcdDispStr(5,0,disp3);
	delayms(500);
	LcdWriteByte(0x0F,0); 			//显示:开，光标:开，光标闪烁:开
	while(BusyCheck());					//等待操作完成
	for(;disp4[i] != 0; i++){
		LcdSetPos(j,1);
		j++;
		LcdWriteByte(disp4[i],1);
		delayms(100);
	}
	LcdWriteByte(0x0C,0); 			//显示:开，光标:关，光标闪烁:关
	delayms(1000);
	LcdClear();
}

