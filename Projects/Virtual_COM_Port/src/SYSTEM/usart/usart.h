#ifndef __USART_H
#define __USART_H
#include "stdio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/5/27
//版本：V1.3
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
////////////////////////////////////////////////////////////////////////////////// 

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			0		//使能（1）/禁止（0）串口1接收
#define EN_USART1 1

extern u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大63个字节.末字节为换行符 
extern u16 USART_RX_STA;         //接收状态标记	
#if EN_USART1


//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
#else


#define  printf   	

#endif


#endif
