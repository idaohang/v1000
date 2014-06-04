 #include "sys.h"
#include "rtc.h" 
#include "delay.h"
#include "usart.h" 
#include "menutal.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//RTCʵʱʱ�� ��������		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/30 
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.1�޸�˵��
//�޸���RTC_Init������Ƶ������Ч��bug
//�޸���RTC_Get������һ��bug
//////////////////////////////////////////////////////////////////////////////////	 
  
//Mini STM32������
//RTCʵʱʱ�� ��������			 
//����ԭ��@ALIENTEK
//2010/6/6

	   
//tm timer;//ʱ�ӽṹ�� 	   
//ʵʱʱ������
//��ʼ��RTCʱ��,ͬʱ���ʱ���Ƿ�������
//BKP->DR1���ڱ����Ƿ��һ�����õ�����
//����0:����
//����:�������
#define RTC_CLOCK_SOURCE_LSE   /* LSE used as RTC source clock */
//#define RTC_CLOCK_SOURCE_LSI   /* LSE used as RTC source clock */
__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;

__IO uint32_t CaptureNumber = 0, PeriodValue = 0;


RTC_TimeTypeDef RTC_TimeStructure;
RTC_DateTypeDef RTC_DateStructure;
RTC_TimeTypeDef  RTC_TimeStampStructure;
RTC_DateTypeDef  RTC_TimeStampDateStructure;

__IO uint32_t LsiFreq = 0;

/**
  * @brief  Configures TIM10 to measure the LSI oscillator frequency. 
  * @param  None
  * @retval LSI Frequency
  */
u32 GetLSIFrequency(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
  TIM_ICInitTypeDef  TIM_ICInitStructure;
  RCC_ClocksTypeDef  RCC_ClockFreq;

  /* Enable the LSI oscillator ************************************************/
  RCC_LSICmd(ENABLE);
  
  /* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}

  /* TIM10 configuration *******************************************************/ 
  /* Enable TIM10 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
  
  /* Reset TIM10 registers */
  TIM_DeInit(TIM10);

  /* Configure TIM10 prescaler */
  TIM_PrescalerConfig(TIM10, 0, TIM_PSCReloadMode_Immediate);

  /* Connect LSI clock to TIM10 Input Capture 1 */
  TIM_RemapConfig(TIM10, TIM10_LSE);

  /* TIM10 configuration: Input Capture mode ---------------------
     The reference clock(LSE or external) is connected to TIM10 CH1
     The Rising edge is used as active edge,
     The TIM10 CCR1 is used to compute the frequency value 
  ------------------------------------------------------------ */
  TIM_ICInitStructure.TIM_Channel     = TIM_Channel_1;
  TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_ICInit(TIM10, &TIM_ICInitStructure);

  /* Enable the TIM10 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable TIM10 counter */
  TIM_Cmd(TIM10, ENABLE);

  /* Reset the flags */
  TIM10->SR = 0;
    
  /* Enable the CC4 Interrupt Request */  
  TIM_ITConfig(TIM10, TIM_IT_CC1, ENABLE);


  /* Wait until the TIM10 get 2 LSI edges (refer to TIM10_IRQHandler() in 
    stm32l1xx_it.c file) ******************************************************/
  while(CaptureNumber != 2)
  {
  }
  /* Deinitialize the TIM10 peripheral registers to their default reset values */
  TIM_DeInit(TIM10);


  /* Compute the LSI frequency, depending on TIM10 input clock frequency (PCLK1)*/
  /* Get SYSCLK, HCLK and PCLKx frequency */
  RCC_GetClocksFreq(&RCC_ClockFreq);

  /* Get PCLK1 prescaler */
  if ((RCC->CFGR & RCC_CFGR_PPRE1) == 0)
  { 
    /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
    return ((RCC_ClockFreq.PCLK1_Frequency / PeriodValue) * 8);
  }
  else
  { /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
    return (((2 * RCC_ClockFreq.PCLK1_Frequency) / PeriodValue) * 8) ;
  }
}

/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
void RTC_Config(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_RTCAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
/* The RTC Clock may varies due to LSI frequency dispersion. */
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);
//	LsiFreq = GetLSIFrequency();
  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  SynchPrediv = 0x120;
  AsynchPrediv = 0x7F;

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;
    
#else
  #error Please select the RTC Clock source inside the main.c file
#endif /* RTC_CLOCK_SOURCE_LSI */

  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

  /* Enable The TimeStamp */
//  RTC_TimeStampCmd(RTC_TimeStampEdge_Falling, ENABLE);    
}

//�Ƚ������ַ���ָ�����ȵ������Ƿ����
//����:s1,s2Ҫ�Ƚϵ������ַ���;len,�Ƚϳ���
//����ֵ:1,���;0,�����
u8 str_cmpx(u8*s1,u8*s2,u8 len)
{
	u8 i;
	for(i=0;i<len;i++)if((*s1++)!=*s2++)return 0;
	return 1;	   
}


extern const u8 *COMPILED_DATE;//��ñ�������
extern const u8 *COMPILED_TIME;//��ñ���ʱ��
const u8 Month_Tab[12][3]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"}; 
//�Զ�����ʱ��Ϊ������ʱ��   

//void show_data
void Auto_Time_Set(void)
{
	u8 temp[3];
	u8 i,mon;
	u8 date;
	u16 year;
	u8 sec,min,hour;
	for(i=0;i<3;i++)temp[i]=COMPILED_DATE[i];   
	for(i=0;i<12;i++)if(str_cmpx((u8*)Month_Tab[i],temp,3))break;	
	mon=i+1;//�õ��·�
	if(COMPILED_DATE[4]==' ')date=COMPILED_DATE[5]-'0'; 
	else date=10*(COMPILED_DATE[4]-'0')+COMPILED_DATE[5]-'0';  
	year=/*1000*(COMPILED_DATE[7]-'0')+100*(COMPILED_DATE[8]-'0')*/+10*(COMPILED_DATE[9]-'0')+COMPILED_DATE[10]-'0';	   
	hour=10*(COMPILED_TIME[0]-'0')+COMPILED_TIME[1]-'0';  
	min=10*(COMPILED_TIME[3]-'0')+COMPILED_TIME[4]-'0';  
	sec=10*(COMPILED_TIME[6]-'0')+COMPILED_TIME[7]-'0';  
//	RTC_Set(year,mon,date,hour,min,sec)	;
	RTC_TimeStructure.RTC_Hours =hour; 
	RTC_TimeStructure.RTC_Minutes = min;
	RTC_TimeStructure.RTC_Seconds = sec;
    

	RTC_DateStructure.RTC_Date = date;
	RTC_DateStructure.RTC_Month = mon;
	RTC_DateStructure.RTC_Year = (u8)(year&0xff);	
	RTC_DateStructure.RTC_WeekDay =   RTC_Get_Week(year+2000,mon, date);
	printf("Auto_Time_Set %d-%d-%d  %d:%d:%d\n",RTC_DateStructure.RTC_Year ,mon,date,hour,min,sec);
} 


/**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_TimeRegulate(void)
{
//  uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;

  printf("\n\r==============Time Settings=====================================\n\r");
  RTC_TimeStructure.RTC_H12     = RTC_H12_AM;

#if 0  
  printf("  Please Set Hours\n\r");
  while (tmp_hh == 0xFF)
  {
    tmp_hh = USART_Scanf(0, 23);
    RTC_TimeStructure.RTC_Hours = tmp_hh;
  }
  printf(":  %0.2d\n\r", tmp_hh);
  
  printf("  Please Set Minutes\n\r");
  while (tmp_mm == 0xFF)
  {
    tmp_mm = USART_Scanf(0, 59);
    RTC_TimeStructure.RTC_Minutes = tmp_mm;
  }
  printf(":  %0.2d\n\r", tmp_mm);
  
  printf("  Please Set Seconds\n\r");
  while (tmp_ss == 0xFF)
  {
    tmp_ss = USART_Scanf(0, 59);
    RTC_TimeStructure.RTC_Seconds = tmp_ss;
  }
  printf(":  %0.2d\n\r", tmp_ss);
#endif


  /* Configure the RTC time register */
  if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) == ERROR)
  {
    printf("\n\r>> !! RTC Set Time failed. !! <<\n\r");
  } 
  else
  {
    printf("\n\r>> !! RTC Set Time success. !! <<\n\r");
    RTC_TimeShow();
    /* Indicator for the RTC configuration */
    RTC_WriteBackupRegister(RTC_BKP_DR1, 0x32F0);
  }
  
//  tmp_hh = 0xFF;
//  tmp_mm = 0xFF;
//  tmp_ss = 0xFF;

  printf("\n\r==============Date Settings=====================================\n\r");

  printf("  Please Set WeekDay (01-07)\n\r");

#if 0  
  while (tmp_hh == 0xFF)
  {
    tmp_hh = USART_Scanf(1, 7);
    RTC_DateStructure.RTC_WeekDay = tmp_hh;
  }
  printf(":  %0.2d\n\r", tmp_hh);
  tmp_hh = 0xFF;
  printf("  Please Set Date (01-31)\n\r");
  while (tmp_hh == 0xFF)
  {
    tmp_hh = USART_Scanf(1, 31);
    RTC_DateStructure.RTC_Date = tmp_hh;
  }
  printf(":  %0.2d\n\r", tmp_hh);
  
  printf("  Please Set Month (01-12)\n\r");
  while (tmp_mm == 0xFF)
  {
    tmp_mm = USART_Scanf(1, 12);
    RTC_DateStructure.RTC_Month = tmp_mm;
  }
  printf(":  %0.2d\n\r", tmp_mm);
  
  printf("  Please Set Year (00-99)\n\r");
  while (tmp_ss == 0xFF)
  {
    tmp_ss = USART_Scanf(0, 99);
    RTC_DateStructure.RTC_Year = tmp_ss;
  }
  printf(":  %0.2d\n\r", tmp_ss);
#endif

	
/* Configure the RTC date register */
  if(RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure) == ERROR)
  {
    printf("\n\r>> !! RTC Set Date failed. !! <<\n\r");
  } 
  else
  {
    printf("\n\r>> !! RTC Set Date success. !! <<\n\r");
    RTC_DateShow();
    /* Indicator for the RTC configuration */
    RTC_WriteBackupRegister(RTC_BKP_DR1, 0x32F0);
  }
  printf("\n\r  RTC_BKP_DR1 : %x\n\r",RTC_ReadBackupRegister(RTC_BKP_DR1));
}
//rtc_set(13,09,30,09,19,28,1)
void  rtc_set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec,u8 week)
{


	printf("rtc_set %d-%d-%d  %d:%d:%d %d\n",syear ,smon,sday,hour,min,sec,week);
	RTC_TimeStructure.RTC_Hours =hour; 
	RTC_TimeStructure.RTC_Minutes = min;
	RTC_TimeStructure.RTC_Seconds = sec;


	
	RTC_DateStructure.RTC_WeekDay =  week;//RTC_Get_Week(syear, smon, sday);
	RTC_DateStructure.RTC_Date = sday;
	RTC_DateStructure.RTC_Month = smon;
	RTC_DateStructure.RTC_Year = syear;	
	RTC_TimeRegulate();
}

/**
  * @brief  Display the current time on the Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_TimeShow(void)
{
  /* Get the current Time and Date */
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  //printf("\n\r============== Current Time Display ============================\n\r");
  //printf("\n\r  The current time (Hour-Minute-Second) is :  %0.2d:%0.2d:%0.2d \n\r", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
  /* Unfreeze the RTC DR Register */
  (void)RTC->DR;
}

/**
  * @brief  Display the current date on the Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_DateShow(void)
{
  /* Get the current Date */
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

  
//  printf("\n\r============== Current Date Display ============================\n\r");
//  printf("\n\r  The current date (WeekDay-Date-Month-Year) is :  %0.2d-%0.2d-%0.2d-%0.2d \n\r", RTC_DateStructure.RTC_WeekDay, RTC_DateStructure.RTC_Date, RTC_DateStructure.RTC_Month, RTC_DateStructure.RTC_Year);
}


/**
  * @brief  Display the current TimeStamp (time and date) on the Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_TimeStampShow(void)
{
  /* Get the current TimeStamp */
  RTC_GetTimeStamp(RTC_Format_BIN, &RTC_TimeStampStructure, &RTC_TimeStampDateStructure);
  printf("\n\r==============TimeStamp Display (Time and Date)=================\n\r");
  printf("\n\r  The current time stamp time (Hour-Minute-Second) is :  %0.2d:%0.2d:%0.2d \n\r", RTC_TimeStampStructure.RTC_Hours, RTC_TimeStampStructure.RTC_Minutes, RTC_TimeStampStructure.RTC_Seconds);
  printf("\n\r  The current timestamp date (WeekDay-Date-Month) is :  %0.2d-%0.2d-%0.2d \n\r", RTC_TimeStampDateStructure.RTC_WeekDay, RTC_TimeStampDateStructure.RTC_Date, RTC_TimeStampDateStructure.RTC_Month);
}


void My_RTC_Init(void)
{
RTC_InitTypeDef RTC_InitStructure;

	//����ǲ��ǵ�һ������ʱ��
  /* Output a message on Hyperterminal using printf function */
  printf("\n\r  *********************** RTC Time Stamp Example *********************** %d\n\r",RTC_ReadBackupRegister(RTC_BKP_DR1));

  if (RTC_ReadBackupRegister(RTC_BKP_DR1) != 0x32F0)
  {
    /* RTC configuration  */
    RTC_Config();

    /* Configure the RTC data register and RTC prescaler */
    RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
    RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
   
    /* Check on RTC init */
    if (RTC_Init(&RTC_InitStructure) == ERROR)
    {
      printf("\n\r        /!\\***** RTC Prescaler Config failed ********/!\\ \n\r");
    }

    /* Configure the time register */
    Auto_Time_Set();
    RTC_TimeRegulate(); 



  
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      printf("\r\n Power On Reset occurred....\n\r");
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      printf("\r\n External Reset occurred....\n\r");
    }

    printf("\r\n No need to configure RTC....\n\r");
    
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_RTCAccessCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();

    /* Clear the RTC Alarm Flag */
    RTC_ClearFlag(RTC_FLAG_ALRAF);

    /* Clear the EXTI Line 17 Pending bit (Connected internally to RTC Alarm) */
  //  EXTI_ClearITPendingBit(EXTI_Line17);

    /* Display the RTC Time/Date and TimeStamp Time/Date */
    RTC_TimeShow();
    RTC_DateShow();
  //  RTC_TimeStampShow();
  }
#if 0//defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
  LsiFreq = GetLSIFrequency();
  printf("\n\r     LsiFreq :%d! \n\r",LsiFreq);
  /* Calendar Configuration */
  RTC_InitStructure.RTC_AsynchPrediv = 0x7;
  RTC_InitStructure.RTC_SynchPrediv	=  0x1001;
  //RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
  //RTC_InitStructure.RTC_SynchPrediv	=  (LsiFreq/128) - 1;
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  RTC_Init(&RTC_InitStructure);  
  #endif

  //RTC_CoarseCalibConfig(RTC_CalibSign_Negative, 0x1f);
  //RTC_CoarseCalibCmd(ENABLE);
}


#if 0


//RTC�жϷ�����		 
//const u8* Week[2][7]=
//{
//{"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"},
//{"��","һ","��","��","��","��","��"}
//};					    
//RTCʱ���ж�
//ÿ�봥��һ��   
void RTC_IRQHandler(void)
{							    
	if(RTC->CRL&0x0001)//�����ж�
	{							
	 	RTC_TimeShow();
		//printf("CRL:%d\n",RTC->CRL);	
 	}
	if(RTC->CRL&0x0002)//�����ж�
	{
		//printf("Alarm!\n");	
		RTC->CRL&=~(0x0002);//�������ж�
 		//���Ӵ���
	} 				  								 
    RTC->CRL&=0X0FFA;         //�������������жϱ�־
	while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������		   							 	   	 
}


//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
u8 Is_Leap_Year(u16 year)
{			  
	if(year%4==0) //�����ܱ�4����
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400���� 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	 			   
//����ʱ��
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.
//�·����ݱ�											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�	  
//ƽ����·����ڱ�
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)	//��������ݵ��������
	{
		if(Is_Leap_Year(t))seccount+=31622400;//�����������
		else seccount+=31536000;			  //ƽ���������
	}
	smon-=1;
	for(t=0;t<smon;t++)	   //��ǰ���·ݵ����������
	{
		seccount+=(u32)mon_table[t]*86400;//�·����������
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//����2�·�����һ���������	   
	}
	seccount+=(u32)(sday-1)*86400;//��ǰ�����ڵ���������� 
	seccount+=(u32)hour*3600;//Сʱ������
    seccount+=(u32)min*60;	 //����������
	seccount+=sec;//�������Ӽ���ȥ
													    
	//����ʱ��
    RCC->APB1ENR|=1<<28;//ʹ�ܵ�Դʱ��
    RCC->APB1ENR|=1<<27;//ʹ�ܱ���ʱ��
	PWR->CR|=1<<8;    //ȡ��������д����
	//���������Ǳ����!
	RTC->CRL|=1<<4;   //�������� 
	RTC->CNTL=seccount&0xffff;
	RTC->CNTH=seccount>>16;
	RTC->CRL&=~(1<<4);//���ø���
	while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������  	
	return 0;	    
}

//�Ƚ������ַ���ָ�����ȵ������Ƿ����
//����:s1,s2Ҫ�Ƚϵ������ַ���;len,�Ƚϳ���
//����ֵ:1,���;0,�����
u8 str_cmpx(u8*s1,u8*s2,u8 len)
{
	u8 i;
	for(i=0;i<len;i++)if((*s1++)!=*s2++)return 0;
	return 1;	   
}
extern const u8 *COMPILED_DATE;//��ñ�������
extern const u8 *COMPILED_TIME;//��ñ���ʱ��
const u8 Month_Tab[12][3]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"}; 
//�Զ�����ʱ��Ϊ������ʱ��   
void Auto_Time_Set(void)
{
	u8 temp[3];
	u8 i;
	u8 mon,date;
	u16 year;
	u8 sec,min,hour;
	for(i=0;i<3;i++)temp[i]=COMPILED_DATE[i];   
	for(i=0;i<12;i++)if(str_cmpx((u8*)Month_Tab[i],temp,3))break;	
	mon=i+1;//�õ��·�
	if(COMPILED_DATE[4]==' ')date=COMPILED_DATE[5]-'0'; 
	else date=10*(COMPILED_DATE[4]-'0')+COMPILED_DATE[5]-'0';  
	year=1000*(COMPILED_DATE[7]-'0')+100*(COMPILED_DATE[8]-'0')+10*(COMPILED_DATE[9]-'0')+COMPILED_DATE[10]-'0';	   
	hour=10*(COMPILED_TIME[0]-'0')+COMPILED_TIME[1]-'0';  
	min=10*(COMPILED_TIME[3]-'0')+COMPILED_TIME[4]-'0';  
	sec=10*(COMPILED_TIME[6]-'0')+COMPILED_TIME[7]-'0';  
	RTC_Set(year,mon,date,hour,min,sec)	;
	//printf("%d-%d-%d  %d:%d:%d\n",year,mon,date,hour,min,sec);
} 


#endif

#if 0
//�õ���ǰ��ʱ��
//����ֵ:0,�ɹ�;����:�������.
u8 RTC_Get(void)
{
	static u16 daycnt=0;
	u32 timecount=0; 
	u32 temp=0;
	u16 temp1=0;	  
	   
		 

	temp=timecount/86400;   //�õ�����(��������Ӧ��)
	if(daycnt!=temp)//����һ����
	{	  
		daycnt=temp;
		temp1=1970;	//��1970�꿪ʼ
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))//������
			{
				if(temp>=366)temp-=366;//�����������
				else break;  
			}
			else temp-=365;	  //ƽ�� 
			temp1++;  
		}   
		timer.w_year=temp1;//�õ����
		temp1=0;
		while(temp>=28)//������һ����
		{
			if(Is_Leap_Year(timer.w_year)&&temp1==1)//�����ǲ�������/2�·�
			{
				if(temp>=29)temp-=29;//�����������
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
				else break;
			}
			temp1++;  
		}
		timer.w_month=temp1+1;//�õ��·�
		timer.w_date=temp+1;  //�õ����� 
	}
	temp=timecount%86400;     //�õ�������   	   
	return temp;
}	
#endif


const u8  table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�	  
//ƽ����·����ڱ�
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//������������������� 
//����ֵ�����ں�																						 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// ���Ϊ21����,�������100  
	if (yearH>19)yearL+=100;
	// ����������ֻ��1900��֮���  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
} 

//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
u8 Is_Leap_Year(u16 year)
{			  
	if(year%4==0) //�����ܱ�4����
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400���� 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	




//�õ���ǰ��ʱ��
//����ֵ:0,�ɹ�;����:�������.
u8 RTC_Get(u8 flag)
{
//	extern tm my_timer;
	u16 temp1=0;	  
	if(flag)   
		{
		my_timer.w_date +=1;  //�õ����� 
		if(Is_Leap_Year(my_timer.w_year)&&my_timer.w_month==1)
			{
			temp1 = 29;
			}
		else
			temp1 = mon_table[my_timer.w_month];
		
		if(my_timer.w_date>temp1 )
			{
			my_timer.w_date=1;//ƽ��
			my_timer.w_month++;//�õ��·�
			if(my_timer.w_month>12)
				{
				my_timer.w_month = 1;
				my_timer.w_year++;
				}
			}
			
		}
	else
		{
		my_timer.w_date -=1;  //�õ����� 
		if(my_timer.w_date == 0)
			{
			my_timer.w_month--;//�õ��·�

		if(Is_Leap_Year(my_timer.w_year)&&my_timer.w_month==1)
			{
			temp1 = 29;
			}
		else
			temp1 = mon_table[my_timer.w_month];

			
			my_timer.w_date= temp1;//ƽ��

			if(my_timer.w_month == 0)
				{
				my_timer.w_month = 12;
				my_timer.w_year--;
				}
			}

		}
		
	my_timer.week=RTC_Get_Week(my_timer.w_year,my_timer.w_month,my_timer.w_date);//��ȡ����   
	return 0;
}	

