#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"
#include "stdlib.h"	    
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//SSD1306 OLED 驱动IC驱动代码
//驱动方式:8080并口/4线串口
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  


//OLED模式设置
//0:4线串行模式
//1:并行8080模式
#define OLED_MODE 1
		    						  
//-----------------OLED端口定义----------------  					   
#define OLED_CS PBout(11)
//#define OLED_RST  PBout(14)//在MINISTM32上直接接到了STM32的复位脚！	
#define OLED_RS PBout(12)
#define OLED_WR PBout(13)		  
#define OLED_RD PBout(14)

//PB0~7,作为数据线
#define DATAOUT(x) GPIOB->ODR=(GPIOB->ODR&0xff00)|(x&0x00FF); //输出

//#define DATAOUT(DataValue) {GPIO_Write(GPIOB,(GPIO_ReadOutputData(GPIOB)&0xff00)|(DataValue&0x00FF));}  

//使用4线串行接口时使用 
#define OLED_SCLK PBout(0)
#define OLED_SDIN PBout(1)
		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
//OLED控制用函数
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const u8 *p);	 
void OLED_Fill_icon(u8 x1,u8 y1,u8 *icon)  ;
void Movie_Show_Img(u16 x,u16 y,const u8 *icon);
void OLED_ShowString_head(u8 x,u8 y,const u8 *p);
void OLED_ShowString12(u8 x,u8 y,const u8 *p);
void OLED_DrawVLine  (int x, int y0,  int y1) ;
void Movie_Show_Img_force1616(u16 x,u16 y,u8 index);
void Movie_Show_Img_3(u16 x,u16 y,const u8 *icon);
void OLED_ShowChar_head(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowString_24(u8 x,u8 y,const u8 *p);
void OLED_Refresh_Gram_low(void);
void OLED_DrawVLine_visual(int x, int y0,  int y1) ;
#endif  
	 



