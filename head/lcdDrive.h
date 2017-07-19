/*******************************************************************************
* File: lcdDrive.h                                                               
* Description:
* LCD1602驱动程序，此项包含内容较多，函数详细介绍在.c文件，此处只给出接口，实现
*	全部在lcdDrive.c中介绍.
*******************************************************************************/
#ifndef _LCDDRIVE_H_
#define _LCDDRIVE_H_

/******************************************************************************/
#include"configuration.h"

/*******************************************************************************
Type Definitions
*******************************************************************************/

/*******************************************************************************
* Interface
*******************************************************************************/
#define LCD_DB   P2  			    	//数据或指令接口 
sbit    LCD_RS = P0^7; 			    //1：数据 0：指令
sbit    LCD_RW = P0^6; 			    //1：读   0：写
sbit    LCD_EN = P0^5;  		    //使能端高电平有效

/*----------------------------------------------------------------------------*/
/*Existing variables*/
extern u8 code LcdData[][8];

/*******************************************************************************
* Function Declarations
*******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Public functions */
bit	 BusyCheck     	(void);										//忙检测，确认上一次指令执行完成，可用延时代替
void LcdInit        (void);	                	//LCD初始化
void LcdCurRst			(void);						    		//光标复位
void LcdClear       (void);                 	//清全屏
void LcdSetPos    	(u8 px, u8 py);						//设定字符显示位置
void LcdWriteByte 	(u8 dat, bit RorS);				//LCD写命令或数据
void LcdCGRAM     	(u8 dat[8], u8 u8addr); 	//向CGRAM写入自定义字符
void LcdDispNum			(u8 px, u8 py, u16 Data);	//显示函数，在指定行列显示指定数字
void NumConvertChar	(u16 Data);								//将数字转化为字符显示在指定列的固定位置
void LcdDispStr			(u8 px, u8 py, u8 *u8Str);//显示一串字符串，长度不超过20个字符
/*----------------------------------------------------------------------------*/
/* Private functions */
void LcdBasic				(void);										//背景固定显示
void LcdStart				(void);										//开机界面
/*******************************************************************************
* Constants
*******************************************************************************/
//指令设置（若不另加说明）：需令LCD_RS = 0；LCD_RW = 0 ;
// 清屏
// DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0          
//  0   0   0   0   0   0   0   1
#define CLRLCD 		0x01          	// Clear LCD1602.       清屏

// 光标复位
// DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0          
//  0   0   0   0   0   0   1   *
#define CURRST 		0x02          	// Cursor restoration.  光标复位

// 输入方式设置                  	// I:Increase, D:Decrease. AC:address counter. 地址寄存器。   
// DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0          
//  0   0   0   0   0   1 I/D   S
#define MODEINC 	0x06          	// AC Increase,Not Shift. 操作后，AC自增，画面不动。  
#define MODEDEC 	0x04          	// AC Decrease,Not Shift. 操作后，AC自减，画面不动。
#define MODEINS 	0x07          	// AC Increase,Shift. 操作后，AC自增，画面平移。
#define MODEDES 	0x05          	// AC Decrease,Shift. 操作后，AC自减，画面平移。

// 显示开关控制  Display Setup
// DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
//  0   0   0   0   1   D   C   B
// D: Display Switch.  显示开关。
// C: Cursor Switch.   光标开关。置位时显示光标。
// B: Blink Switch.    闪烁控制。置位时光标闪烁。
// 将下列相应功能宏相或后赋值操作。
#define DISSET 		0x08          	//显示开关：关，光标开关：关，光标闪烁：关
#define SETDIS  	0x0C          	//显示开关：开，光标开关：关，光标闪烁：关
#define SETCUR  	0x0E          	//显示开关：开，光标开关：开，光标闪烁：关
#define SETBLI  	0x0F          	//显示开关：开，光标开关：开，光标闪烁：开

// 光标/画面位移(不改变DDRAM内容)
// DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
//  0   0   0   1  S/C R/L  *   *
// S/C: Screen or Cursor.       
// R/L: Right or left.
#define CURLFT    0x10
#define CURRIT    0x14
#define DISLFT    0x18
#define DISRIT    0x1C

// 功能设置 
// DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
//  0   0   1  DL   N   F   *   *
// DL = 1，8位数据接口； DL = 0，4位数据接口。
// N  = 1，两行显示；    N = 0，单行显示。
// F  = 1，5*10点阵字符；F = 0，5*7点阵字符。

// CGRAM 地址设置，character generator RAM  (0~3FH)
// DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
//  0   1  A5  A4  A3  A2  A1  A0
//CGRAM初始地址为0x40

// DDRAM 地址设置
// DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
//   1  A6  A5  A4  A3  A2  A1  A0
// N = 0,单行显示 A6 ~ A0 = 00H ~ 4FH
// N = 1,首行     A6 ~ A0 = 00H ~ 27H
//       次行     A6 ~ A0 = 40H ~ 67H

// 读BF、AC值(BF:Busy Flag.)
// RS R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
//  0   1  BF  A6  A5  A4  A3  A2  A1  A0

// 写数据
// RS R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
//  0   1  BF  A6  A5  A4  A3  A2  A1  A0


#endif

/*----------------------------------------------------------------------------*/
/* End of File */
/*-----------------------------------------------------------------------------*/
