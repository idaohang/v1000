#include "sys.h"
#include "timer.h"
#include "led.h"
#include "gps.h"
#include "string.h"	
#include "usart.h"
#include "usart2.h"
#include "menutal.h"
#include "bmp085.h"
#include "dma.h"
#include "key.h"
#include "rtc.h"
#include "oled.h"
#include "pwm.h"
#include "myadc.h"
#include "hw_config.h"

///////////////////
///////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//通用定时器 驱动代码			   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/12/03
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reservedadc
////////////////////////////////////////////////////////////////////////////////// 	  
u8 timer_flag =0; 
u8 user_key =0;
u8 flash_baoyu_flag = 0;
u8 USART1_TX_BUF[USART2_MAX_RECV_LEN]; 					//串口1,发送缓存区
extern u8 baoyun_flag;
extern const unsigned char Bmp_wanner[];


nmea_msg gpsx; 											//GPS信息
//const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode字符串 
extern system_flag system_flag_table;
extern u8 system_power_flag ;
extern u16 To_lower_timercnt ;
extern u8 enter_perss;

static u16 timer_1s_cnt = 0,save_tsp_s_timer = 0;
u16 flash_led_low_power_timer = 0;
u8 button_100_time = 0;
u8 guji_1s_time = 0;
u8 flash_led_flag = 0;
u32 save_tsp_tp_timer = 0;
static u16 wait_timer_cnt;
u32 average_speed  = 0;
u8 recored_run_flag;
extern u32 tatol_time ;
extern u8 guji_mode ;	
extern u8 rtc_set_flag;
extern uint8_t  USB_connect_State;
extern   __IO uint32_t EndOfTransfer ;
extern uint32_t USART_Rx_ptr_in ;
u16 usb_delay_timer_poweron = 0;
u16 usb_delay_timer_poweroff = 0;
extern const unsigned char Bmp_stanby_1[];
//定时器3中断服务程序	 
void TIM3_IRQHandler(void)
{ 	


	if(TIM3->SR&0X0001)//溢出中断
	{
	if(enter_perss == 3)
		{
		wait_timer_cnt++;
		if(wait_timer_cnt == 300)
			{
			wait_timer_cnt = 0;
			enter_perss =0;
			OLED_Clear();
			}
		}
	else if (enter_perss == 4)
		{			
		if(wait_timer_cnt == 200)
			{
			enter_perss =0;
			menu_enter_flash(wait_timer_cnt);
			wait_timer_cnt = 0;
			}
		else
			menu_enter_flash(wait_timer_cnt);

		wait_timer_cnt++;
		}


		 if((system_power_flag == SYSTEM_POWER_LOW_2)&&(system_flag_table.run_mode == 0))
		 	{
		 	flash_led_low_power_timer ++;
			if(flash_led_flag == 0)
				{
			 	if(flash_led_low_power_timer == 350)
			 		{
				 	flash_led_low_power_timer = 0;
				 	//LED0=1;
				 	Movie_Show_Img(58,24,Bmp_stanby_1);
					OLED_Refresh_Gram();//更新显示
					flash_led_flag =1;
			 		}
				}
			else if(flash_led_flag == 1)
				{
			 	if(flash_led_low_power_timer == 10)
			 		{
				 	flash_led_low_power_timer = 0;
				 	//LED0=0;
//				 	OLED_Clear();
					OLED_Fill(58, 24, 70, 36, 0);
					OLED_Refresh_Gram();//更新显示
					flash_led_flag = 2;
			 		}
				}
			else if(flash_led_flag == 2)
				{
			 	if(flash_led_low_power_timer == 20)
			 		{
				 	flash_led_low_power_timer = 0;
				 	//LED0=0;
//				 	OLED_Clear();
					Movie_Show_Img(58,24,Bmp_stanby_1);
					OLED_Refresh_Gram();//更新显示
					flash_led_flag = 3;
			 		}
				}
			else if(flash_led_flag == 3)
				{
			 	if(flash_led_low_power_timer == 10)
			 		{
				 	flash_led_low_power_timer = 0;
				 	//LED0=0;
//				 	OLED_Clear();
					OLED_Fill(58, 24, 70, 36, 0);
					OLED_Refresh_Gram();//更新显示
					flash_led_flag = 0;
			 		}
				}			
		 	}
		 else
		 	{
	 			flash_led_flag = 0;
			 	flash_led_low_power_timer = 0;
		 	}
	#if 1
       	if(timer_1s_cnt == 100)
       		{	

       		}
		else
			timer_1s_cnt++;

       	if(save_tsp_tp_timer == 90000)
       		{	
       		}
		else
			save_tsp_tp_timer++;

       	if(save_tsp_s_timer == 15000)
       		{		
       		}
		else
			save_tsp_s_timer++;		

		if(button_100_time)
			{
			button_100_time --;
			if(button_100_time == 0)
				Button_key_perss(0);
			}
	#endif	

	 if(guji_mode == RECORED_START_DOING)
	 	{
	 	guji_1s_time++;
		if(guji_1s_time == 100)
			{
			guji_1s_time  = 0;
			tatol_time += 1;	
			}

	 	}
		timer_flag = 1;

	}		
//	 TimingDelay_Decrement(); 
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}
//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!

void Timerx_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;//TIM3时钟使能    
 	TIM3->ARR=arr;  //设定计数器自动重装值//刚好1ms    
	TIM3->PSC=psc;  //预分频器7200,得到10Khz的计数时钟
	//这两个东东要同时设置才可以使用中断
	TIM3->DIER|=1<<0;   //允许更新中断				
//	TIM3->DIER|=1<<6;   //允许触发中断	   
	TIM3->CR1|=0x01;    //使能定时器3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);//抢占1，子优先级3，组2									 
}

extern void change_clk_need_int(u8 mode);
void Timer_task(void)
{
static u8 timer_cnt = 0;
static u16 timer_bmp085_cnt = 0;
extern u8 upload;

static u8 static_rtc_cnt = 0;
extern u8 send_status ;
extern u8 send_status_flag;
extern u32 Flash_Index;
//static u8 oled_rtc_seconds = 0;
//extern RTC_TimeTypeDef RTC_TimeStructure;
  RCC_ClocksTypeDef RCC_Clocks;
	
	if(timer_flag)
	{
		timer_flag = 0;


   		{

		     if(timer_cnt == 5)
		     	{
			user_key=KEY_Scan();
			menu_keyprocess(user_key);		
			user_key = 0;
			timer_cnt = 0;
			write_flash();


		if(USB_CONNECT == 1)
		{
			if(usb_delay_timer_poweron)
			{
			printf("\r\usb_delay_timer_poweron:%d",usb_delay_timer_poweron); 
				usb_delay_timer_poweron--;
				if(usb_delay_timer_poweron == 0)
					{
					system_flag_table.usb_standy_poweron_flag = 1;
					menu_keyprocess(5);	
					LED0 = 1;
					}
			}
			usb_delay_timer_poweroff =0;
		}
		else
			{
			usb_delay_timer_poweron = 0;
			if(usb_delay_timer_poweroff)
			{
			//printf("\r\usb_delay_timer_poweroff:%d",usb_delay_timer_poweroff); 
				usb_delay_timer_poweroff--;
				if(usb_delay_timer_poweroff == 0)
					{

//					di_and_flash_empet(0);					
					Choose_power_mode(MCU_STBY);	
					system_power_flag = SYSTEM_POWER_STANBY;
					LED0 = 0;
					}
			}	
		}
		
		if(USB_CONNECT == 1)
			{
			//printf("\r\nUSB_CONNECT:1"); 

	
#if 1			
			if((system_flag_table.charger_connected == 0))
				{
										
					{

					if((system_flag_table.usb_powerdown_flash == 1)&&(system_power_flag == SYSTEM_POWER_STANBY))					
						{
						//menu_keyprocess(5);	
						usb_delay_timer_poweron = 30;
						Choose_power_mode(MCU_RUN_USB);
						change_clk_need_int(1);
						}
					else
					//	LED0 = 1;
						{
						Choose_power_mode(MCU_RUN_USB);
						change_clk_need_int(1);
					//	printf("\r\n make 1");					
					//	printf("\r\n make 3");
						USB_Interrupts_Config();
						menu_keyprocess(0xff);	
						OLED_Clear();
						OLED_Display_On();						
						LED0 = 1;	
						}
						
					}
					printf("\r\n change to 32 mhz"); 	

				}
#endif			
			system_flag_table.charger_connected = 1;
		
			}
		else
			{
		
			if(system_flag_table.charger_connected == 1)
				{
				USB_Interrupts_Config_disable();
				system_flag_table.charger_connected = 0;
				LED0 = 0;
			       if(system_flag_table.usb_standy_poweron_flag == 1)
			       	{
			       	system_flag_table.usb_standy_poweron_flag = 0;
					usb_delay_timer_poweroff = 30;
					//Choose_power_mode(MCU_STBY);	
					//menu_keyprocess(0xff);	
				   	}
				   
				if((system_power_flag == 2)&&(system_flag_table.run_mode == 0))
					{
					Choose_power_mode(MCU_LP_RUN);	
					//menu_keyprocess(0xff);	
					}
				else if(system_power_flag == 1)
					{
					Choose_power_mode(MCU_RUN);	
					change_clk_need_int(0);

					}
					menu_keyprocess(0xff);	
				}


			}			
		     	}
			 else
			 	{
			 	timer_cnt ++;
			 	}


				
   		}
	
			     if(timer_bmp085_cnt == 20)
				{
				timer_bmp085_cnt = 0;
				//printf("\r\n timer_bmp085_cnt\n"); 
				#ifndef NO_BMP085				
					BMP085_get_process();
				#endif
				//printf("\r\n BMP085_get_process\n"); 
				}
			else
				timer_bmp085_cnt ++;		
			
			if(static_rtc_cnt == 50)
				{
				static_rtc_cnt = 0;
				RTC_TimeShow();
				RTC_DateShow()	;

				
					
			//	LED0=!LED0;//DS0闪烁
				}
			else
		   		static_rtc_cnt++;	
	}
}

void main_porcess(void)
{
u16 i = 0,rxlen = 0;
extern u8 gps_printf_status ;
		if(system_power_flag == SYSTEM_POWER_STANBY)
			{

			if(timer_1s_cnt == 100)
				{
				timer_1s_cnt = 0;

				 vddmv_adc_proess();
				//printf("vddmv_adc_proess \r\n"); 
				if((USB_CONNECT == 0)&&(usb_delay_timer_poweron == 0)&&(KEY2 == 0))
					{
						system_power_flag = SYSTEM_POWER_STANBY;
						Choose_power_mode(MCU_STBY);	
					}
				}
			  	return;
			}
			   

			
//if(system_flag_table.run_mode == 1)

	if(USART2_RX_STA&0X8000)		//接收到一次数据了
		{
			//显示信息
		rxlen=USART2_RX_STA&0X7FFF;	//得到数据长度
		for(i=0;i<rxlen;i++)
			{
			USART1_TX_BUF[i]=USART2_RX_BUF[i];	  
			if(gps_printf_status == 0)
				{
				USART_To_USB_Send_Data(USART1_TX_BUF[i]);
				
				}
			}
	//	printf("%s,",USART1_TX_BUF);
		USART2_RX_STA=0;		   	//启动下一次接收
		USART1_TX_BUF[i]=0;			//自动添加结束符
		GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//分析字符串
		//printf("%s,",USART1_TX_BUF);
   //    printf("longitude :%d \r\n",gpsx.longitude);	//显示UTC日期
     //  printf("latitude :%d \r\n",gpsx.latitude);	//显示UTC日期

		if(average_speed == 0xffff)
			average_speed = gpsx.speed;
		else		
			average_speed =(average_speed+gpsx.speed)/2;
		

		
		if(0)
			{
			printf("\nDMA DATA:"); 	    
			USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);           //使能串口1的DMA发送       
			MYDMA_Enable(DMA1_Channel4);//开始一次DMA传输！	  
			}	
		guji_Distance();		
		menu_3d_seq();



       
		if(rtc_set_flag == 1)
			{			
//			printf("\n gpsx.svnum :%d",gpsx.svnum );
			if(gpsx.svnum >= 2)
            
				{
					rtc_set_flag = 0;
				//     printf("\ncheck_time:");
				   check_time();
				}
			}
		else
			{
			if(gpsx.svnum < 2)
				{
					rtc_set_flag = 1;
				}
			
			}
}

  if(save_tsp_tp_timer == 90000)
  	{	
  	save_tsp_tp();
	save_tsp_tp_timer = 0;
  	}
  
  if(save_tsp_s_timer == 15000)
  	{
  	
  	save_tsp_s();
	save_tsp_s_timer = 0;
  	}
  
	if(timer_1s_cnt == 100)
		{
	
			timer_1s_cnt = 0;
			//D0=!LED0;//DS0闪烁
			//printf("\r\n led dso "); 
			if(baoyun_flag==1)
			{
			Button_key_perss(1);
			button_100_time = 5;
			if(flash_baoyu_flag == 0)
				{
				flash_baoyu_flag = 1;
				 Movie_Show_Img(61,0,Bmp_wanner);	
				 OLED_Refresh_Gram();//更新显示
				}
			else
				{
				flash_baoyu_flag = 0;
				OLED_Fill(61, 0, 84, 15, 0);
				OLED_Refresh_Gram();//更新显示
				}	
			}
			
			if(To_lower_timercnt)
			{
				To_lower_timercnt--;
				if(To_lower_timercnt == 0)
				{
				OLED_Fill(0, 0, 127, 63, 0);
				
				if((system_flag_table.run_mode == 0)&&(usb_delay_timer_poweron == 0))
					{
					//delay_init(4);	     //延时初始
					if(system_power_flag == SYSTEM_POWER_STANBY)
						{
						if(USB_CONNECT  == 0)
							{

							 Choose_power_mode(MCU_STBY);
							}
						}
					else	
						{
						system_power_flag = 3;
						if(USB_CONNECT  == 0)
							{							
							 Choose_power_mode(MCU_LP_RUN);
							}
						}
					}
				else
					system_power_flag = 2;
				OLED_Refresh_Gram();
			}
         	}
	//	RCC_GetClocksFreq(&RCC_Clocks);	
	//	 printf("Frequency %d \r\n",RCC_Clocks.SYSCLK_Frequency);
		 vddmv_adc_proess();

		}
if(system_flag_table.run_mode == 1)
{
	if((enter_perss !=4)&&(recored_run_flag == 1))
		{
			recored_run_flag = 0;
			Recording_guji();
			
		}

}

if(send_status_flag == 1)
{
	test_read_flash_and_printf();
	send_status_flag = 0;
}




}










