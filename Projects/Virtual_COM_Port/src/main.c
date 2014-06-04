/* Author             : lin jian sen
* Version            : V1.0
* Date               : 08/26/2013
* Description        :main
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/

#include "led.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
#include "usart.h"
//#include "exti.h"
#include "rtc.h"
#include "timer.h"
//#include "pwm.h"
#include "oled.h"
//#include "hc05.h"
#include "usmart.h"		 	 
#include "string.h"	
#include "gps.h" 
#include "usart2.h"		
#include "sys.h"
#include "dma.h"
/*oled  */
//#include "stm32l1xx_lib.h"
#include "menutal.h"
#include "usb_desc.h"
#include "hw_config.h"
//#include "stm32l1xx_rcc.h"
#include "stm32l1xx_rcc.h"
#include "spi.h"
#include "flash.h"
#include "oledhz.h"	   
#include "Myiic.h"
#include "Bmp085.h"
#include "stm_eeprom.h"
#include "menutal.h"
//#include "tsl_user.h"
#include "usb_lib.h"
#include "pwm.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/


const u8 *COMPILED_TIME=__TIME__;//»ñµÃ±àÒëÊ±¼ä
const u8 *COMPILED_DATE=__DATE__;//»ñµÃ±àÒëÈÕÆÚ

extern const u8 BMP_logo[];
extern system_flag system_flag_table;
static __IO u32 TimingDelay;

extern u8 system_power_flag ;

#define SYSTEM_CLOCK_CNT 24

#define MY_GITHUB 0
/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 1 ms.
  * @retval None
  */
void Delay(__IO u32 nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}


void change_clk_need_int(u8 mode)
{
 if(mode == 0)
	delay_init(16);	     //ÑÓÊ±³õÊ¼»¯
else
	delay_init(32);	     //ÑÓÊ±³õÊ¼»¯

#if EN_USART1  	
	uart_init(115200);
#endif

 if(mode == 0)	
   	USART2_Init(16,38400);	//³õÊ¼»¯´®¿Ú2  
  else
  	USART2_Init(32,38400);	//³õÊ¼»¯´®¿Ú2  
  // 	printf("\n\r Flash_Index :%d",Flash_Index);
//	Timerx_Init(99,3199);//10Khz  ¼ÆÊý4999 --- 500ms
 if(mode == 0)
	Timerx_Init(99,1599);//10Khz  ¼ÆÊý4999 --- 500ms	
else
	Timerx_Init(99,3199);//10Khz  ¼ÆÊý4999 --- 500ms	

	SPI_Flash_Init();   //SPI FLASH ³õÊ¼»¯ 
//	PWM_Init();
	ADC_Config();	
}

 int main(void)
 {	
	u16 i;
	extern u32  Flash_Index;		
	RCC_ClocksTypeDef  RCC_ClockFreq;
	bool StanbyWakeUp ;
	extern u8 rtc_set_flag;
//	extern long  most_Altitude ;
  /* NVIC configuration */
//     delay_init(16);	     //ÑÓÊ±³õÊ¼»¯
	 NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
	__enable_irq();	
	delay_init(32);	     //ÑÓÊ±³õÊ¼»¯
	
	LED_Init();
  /* Setup SysTick Timer for 1 msec interrupts  */
  //if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
   // while (1);
  }	
#if EN_USART1  
	uart_init(115200);
#endif

#if 1
	/* Check if the StandBy flag is set */
	 printf(" PWR_FLAG_SB:%d\r\n",PWR_GetFlagStatus(PWR_CSR_WUF));
	 
	if (PWR_GetFlagStatus(PWR_CSR_WUF) != RESET)
	{
	/* System resumed from STANDBY mode */
	/* Clear StandBy flag */

	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	//PWR_ClearFlag(PWR_FLAG_SB); 
	/* set StandbyWakeup indicator*/
	StanbyWakeUp = TRUE;
	//LED0 =0;
	} 
	else
	{
	/* Reset StandbyWakeup indicator*/
	StanbyWakeUp = FALSE;    
	//LED0 =1;
	} 
#endif
//	Stm32_Clock_Init(9);	//ÏµÍ³Ê±ÖÓÉèÖÃ	
   //   
 //      NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 

//	printf("\n\r SYSCLK_Frequency2 :%d",RCC_ClockFreq.SYSCLK_Frequency);
//	board_io_init();
//	printf("\n\r SYSCLK_Frequency1 :%d",RCC_ClockFreq.SYSCLK_Frequency);
//	printf("\n\r SYSCLK_Frequency3 :%d",RCC_ClockFreq.SYSCLK_Frequency)	
	KEY_Init();				//³õÊ¼»¯ÓëLEDÁ¬½ÓµÄÓ²¼þ½Ó¿Ú
	SPI_Flash_Init();   //SPI FLASH ³õÊ¼»¯ 
//	printf("\n\r SYSCLK_Frequency4 :%d",RCC_ClockFreq.SYSCLK_Frequency);


#if 1//EN_USART1_RX
	usmart_dev.init(32); 	//³õÊ¼»¯USMART 	
#endif	
	

//  printf(" lest_Altitude:%x\r\n",most_Altitude);
#if 0		
	 RCC_GetClocksFreq(&RCC_ClockFreq);
//	 SysTick_Config(RCC_ClockFreq.HCLK_Frequency / 1000);
	printf("\n\r SYSCLK_Frequency :%d",RCC_ClockFreq.SYSCLK_Frequency);
	printf("\n\r HCLK_Frequency :%d",RCC_ClockFreq.HCLK_Frequency);
	printf("\n\r PCLK1_Frequency :%d",RCC_ClockFreq.PCLK1_Frequency);
	printf("\n\r PCLK2_Frequency :%d",RCC_ClockFreq.PCLK2_Frequency);
	#endif	



	IIC_Init();
	OLED_Init();			//³õÊ¼»¯OLED     
//       while(1);
	My_RTC_Init();

	//OLED_ShowString(32,5,"GPS RECORD");
	//OLED_ShowString(48,32,"V-1000"); 
//      BMP_logo
//	stm_read_eerpom(MENU_POWER_STATE_INDEX_ADDRES ,&Flash_Index);
//	printf("\n\r power :%x",Flash_Index);
	if(0)//Flash_Index != 0x12345678)
	{
	Movie_Show_Img(13, 10,BMP_logo);
	OLED_ShowString(60,30,"0.8"); 
	OLED_ShowString12(0,52,COMPILED_DATE); 
	OLED_ShowString12(76,52,COMPILED_TIME); 
	OLED_Refresh_Gram();	 
	}
//	else
//	system_power_flag = 1;
//	MYDMA_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)USART1_TX_BUF,USART2_MAX_RECV_LEN);//DMA1Í¨µÀ4,ÍâÉèÎª´®¿Ú1,´æ´¢Æ÷ÎªSendBuff,³¤¶È5200.

  #if 1
	SPI_Flash_WAKEUP();
	while(SPI_Flash_ReadID()!=FLASH_ID)//¼ì²â²»µ½W25X16
		{	   
		i=SPI_Flash_ReadID();
	//	OLED_Refresh_Gram();	
		printf("\r ID:%d",i);
		SPI_Flash_WAKEUP();
		//OLED_ShowString(0,32,"W25X16 Check Failed!");
		delay_ms(500);
		//OLED_ShowString(0,48,"   Please Check!    ");
		delay_ms(500);
//		LED0=!LED0;//DS0ÉÁË¸
		}
	#endif
	
	SPIx_SetSpeed(SPI_SPEED_2);

	stm_read_eerpom(CUURENT_FLASH_ADDRER ,&Flash_Index);
	printf("\n\r Flash_Index :%d",Flash_Index);
	if(Flash_Index >= 0x7ff000)
		Flash_Index = 0;
     // stm_write_eerpom(HOME_NUMBER1_INDE, 0);
//	stm_write_eerpom(HOME_NUMBER2_INDE, 0);
//	stm_write_eerpom(HOME_NUMBER3_INDE, 0);		
   	USART2_Init(32,38400);	//³õÊ¼»¯´®¿Ú2  
  // 	printf("\n\r Flash_Index :%d",Flash_Index);
//	Timerx_Init(99,3199);//10Khz  ¼ÆÊý4999 --- 500ms
	Timerx_Init(99,3199);//10Khz  ¼ÆÊý4999 --- 500ms
	check_eeprom_init();
	Send_to_goto_lowpower();
	Init_BMP085();  
//	PWM_Init();
	ADC_Config();	
	Button_key_perss(0);		
	#ifndef NO_BMP085				
		BMP085_get_process();
	#endif
	restore_pass();	

	//delay_ms(1200);
	//delay_ms(1200);
	//delay_ms(1200);	
	OLED_Clear();	

	#if 1				
		Set_System();
		Set_USBClock();

		USB_Init();
	  #endif
	  
	if(USB_CONNECT == 1)
		{

		system_flag_table.charger_connected = 1;
	//	USB_Interrupts_Config();
		LED0 = 1;
		}
	else
		system_flag_table.charger_connected = 0;

	///else	
		{
		Choose_power_mode(MCU_RUN);
		change_clk_need_int(0);
		}
//	GPS_power_ctrl(0);
//	delay_ms(1500);	     //ÑÓÊ±³õÊ¼»
//	gps_dma_usb();
//utton_key_perss(1);	

	rtc_set_flag = 1;

	while(1) 
	{
	   Timer_task();
	   main_porcess();
	}	  
  
 }





#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


