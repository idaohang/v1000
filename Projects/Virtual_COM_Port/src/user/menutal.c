/* Author           : jianjian
* Version            : V1.0
* Date               : 08/26/2013
* Description       ://C语言菜单设计
*/
#include "sys.h"
//#include "stm32l1xx.h"
#include "oled.h"
#include "gps.h"
#include "menutal.h"
#include "string.h"
//#include "bigfont.h"
#include "oledhz.h"	   
#include "Bmp085.h"
#include "usart.h"
#include "bigfont.h"
#include "rtc.h"
#include "Gongshi.h"
#include "stm_eeprom.h"
#include "usart2.h"
#include "flash.h"
#include "led.h"
#include "math.h"
#include "pwm.h"
#include "hw_config.h"

__align(4) u8 dtbuf[50];   								//打印缓存器
extern const u8 bmpdu[];
extern nmea_msg gpsx;
extern system_flag system_flag_table;
u8 rtc_set_flag= 1;
extern u8 ge,shi,bai,qian,wan,shiwan;           //显示变量
extern u32 average_speed;
extern long  temperature;
extern long  pressure;
extern long Altitude;											//海拔高度
u8 upload = 0;
u8 enter_perss= 0;
float time_zone = 8;
signed char home_menu_item_index = 0,menu_item_index = 0,guji_icon_index = 0;
extern RTC_TimeTypeDef RTC_TimeStructure;
extern RTC_DateTypeDef RTC_DateStructure;
extern u8 recored_run_flag;
extern const u8 BMP_logo[];
u8 old_div_cnt = 0xff,old_spped_div_cnt = 0xff,old_altiude_div_cnt = 0xff;
  
u8 menu_id_show_timer = 10;
#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_ENTER 2
#define KEY_LONG_ENTER 3
u8 key_type = 0xff;

extern const unsigned char Bmp_waitsinge[];
	
u8 system_power_flag = SYSTEM_POWER_STANBY;
u16 To_lower_timercnt = 0;


//aconst u8 *COMPILED_DATE=__DATE__;//获得编译日期



u32  Flash_Index = 0;
u16  Flash_write_buffer_Index = 0;
//u8    Flash_write_buffer[4096];
u32  Message_head_number = 0;
u32 tatol_time = 0;
u32 tatol_oled_time = 0;
u8 guji_mode =0;	
float guiji_distance = 0;		 

char guji_latitude1_flag,guji_longitude1_flag;
float guji_tp_long_dest,guji_tp_lati_dest;	
 


 extern uint8_t  USB_connect_State;
 
uc8 menu1[]  ={"1,实时信息页"};
uc8 menu2[]  ={"2,坐标页"};
uc8 menu3[]  ={"3,数据变化页"};
uc8 menu4[]  ={"4,里程信息页"};
uc8 menu5[]  ={"5,卫星信息"};
uc8 menu6[]  ={"6,晴雨表"};
uc8 menu7[]  ={"7.潮汐表"};
uc8 menu8[]  ={"8.设置菜单"};
uc8 menu8_a[]  ={"1,.坐标表示方式"};
uc8 menu8_b[] ={"2,距离单位"};
uc8 menu8_c[] ={"3,温度单位"};
uc8 menu8_d[] ={"4,记录间隔"};
uc8 menu8_e[] ={"5,自定义显示页面"};
uc8 menu8_f[] ={"6,更改系统语言"};
uc8 menu8_g[] ={"7,时区设置"};


int16_t temperature_pass[96];
int16_t speed_pass[96];
s32 pressure_pass[96];
s32 Altitude_pass[96];
s32  most_Altitude = 0;
s32 lest_Altitude = 0;
int16_t most_temperature = 0,lest_temperature = 0;
int16_t most_speed = 0,lest_speed = 0;
s32 most_pressure = 0,lest_pressure = 0;




#define MENU_MAIN 0
#define MENU_COORDINATES     1
#define MENU_REALTIME     2
#define MENU_TRANSFORMATION    3
#define MENU_MILEAGE       4
#define MENU_SATELLITE      5
#define MENU_SETTING_TEMPERATURRE     6
#define MENU_RIDE_TABLE     7
#define MENU_BAROMETER    8
#define MENU_SETTING      10
#define MENU_SETTING_COOR    9
#define MENU_SETTING_DISTANCE     11

#define MENU_SETTING_RECORD_SPACE      12
#define MENU_SETTING_DISPLAYMENU    13
#define MENU_SETTING_LANGUAGE   14
#define MENU_STTING_TIMEZONE   15

#define  MAX_KEYTABSTRUCT_NUM 12

u8 keyTabIndex = MENU_MAIN;
u8 ParMenuID_loop = MENU_MAIN;
u8 menu_change_index = MENU_TRANSFORMATION;
u8 menu_updown_flag = 0;
u8 Guijisave_BUF[4096];

extern const unsigned char Bmp_updwon[];
extern const unsigned char Bmp_ponit[];


const u8 timer_zone_Aarry[][7]=
	{
	"-12:00",   // 1
	"-11:00",   // 2
//	"-10:30",   // 3 
	"-10:00",   // 4    
	"-09:30",   //5
	"-09:00",
//	"-08:30",
	"-08:00",
	"-07:00",
	"-06:00",
	"-05:00",
	"-04:30",
	"-04:00",
	"-03:30",
	"-03:00",
	"-02:30",
	"-02:00",
	"-01:00",
//	"-00:44",
//	"-00:25",
	"+00:00",
//	"+00:20",
//	"+00:30",
	"+01:00",
	"+02:00",
	"+03:00",
	"+03:30",
	"+04:00",
	"+04:30",
//	"+04:51",
	"+05:00",
	"+05:30",
//	"+05:40",
	"+05:45",
	"+06:00",
	"+06:30",
	"+07:00",
//	"+07:20",
//	"+07:30",
	"+08:00",
//	"+08:30",
	"+08:45",
	"+09:00",
	"+09:30",
//	"+09:45",
	"+10:00",
	"+10:30",
	"+11:00",
	"+11:30",
	"+12:00",
	"+12:45",
	"+13:00",
	"+13:45",
	"+14:00",};
	
void menu_arruite_change_display(void);
void menu_real_crood_display(void);
	
//extern RTC_TimeTypeDef RTC_TimeStructure;
//extern const unsigned char Bmp_test_status[];


typedef struct
{
 u8 MenuID;      //本菜单状态索引号
 u8 ParMenuID;    //父菜单状态索引号
 uc8 *MenuName;  //菜单文本内容
 void (*CurrentOperate)(void); //本菜单的功能函数
 u8 subnumber;      //本菜单状态索引号
}KeyTabStruct;
//菜单表，


/*
#define MENU_SETTING_RUNMODEL_ADDRES (MENU_SETTING_INDEX_ADDRES+4)
#define MENU_SETTING_GUJI_BY_DISTANCE (MENU_SETTING_RUNMODEL_ADDRES+1)
#define MENU_SETTING_GUJI_BY_TIME (MENU_SETTING_GUJI_BY_DISTANCE+1)
#define MENU_SETTING_GUJI_BY_SPEED (MENU_SETTING_GUJI_BY_TIME+1)
#define MENU_SETTING_GUJI_RECOREDMETH (MENU_SETTING_GUJI_BY_SPEED+1)
#define MENU_SETTING_RECODED_AUTOFLAG (MENU_SETTING_GUJI_RECOREDMETH+1)

#define MENU_SETTING_UNIT_TIMER (MENU_SETTING_RECODED_AUTOFLAG+1)
#define MENU_SETTING_UNIT_DATA (MENU_SETTING_UNIT_TIMER+1)
#define MENU_SETTING_UNIT_COORD (MENU_SETTING_UNIT_DATA+1)
#define MENU_SETTING_UNIT_HW (MENU_SETTING_UNIT_COORD+1)
#define MENU_SETTING_UNIT_TMPE (MENU_SETTING_UNIT_HW+1)
#define MENU_SETTING_UNIT_PREE (MENU_SETTING_UNIT_TMPE+1)

#define MENU_SETTING_HEIGHT (MENU_SETTING_UNIT_PREE+1)
#define MENU_SETTING_WEIGHT (MENU_SETTING_HEIGHT+1)

#define MENU_SETTING_AJUST_ARR (MENU_SETTING_WEIGHT+1)
#define MENU_SETTING_SCREER_TIMER (MENU_SETTING_AJUST_ARR+1)
#define MENU_SETTING_TIMER_ZONE (MENU_SETTING_SCREER_TIMER+1)
#define MENU_SETTING_LANG (MENU_SETTING_TIMER_ZONE+1)
*/

//Monday Tuesday Wednesday Thursday Friday Saturday Sunday
const u8 week_word[7][3]={"SUN","MON","TUE","WED","THR","FRI","SAT"};

u8 baoyun_flag = 0;
u8 baoyun_flag_cnt = 0;
//s32 pressure_stat_baoyun = 0;
//s32 pressure_end_baoyun = 0;


u8 what_is_numbe_10(s32 number_vaule)
{
if(number_vaule <0)
{
	if(number_vaule<=-1000)
		return 5;
	if(number_vaule<= -100)
		return 4;

	if(number_vaule<=-10)
		return 3;
	if(number_vaule<0)
		return 2;

}
else
{
	if(number_vaule>=100000)
		return 6;
	if(number_vaule>=10000)
		return 5;
	if(number_vaule>=1000)
		return 4;
	if(number_vaule>=100)
		return 3;

	if(number_vaule>=10)
		return 2;
	if(number_vaule>=0)
		return 1;
	
}
	return 0;


}


void check_eeprom_init(void)
{
u32 check_data;
	stm_read_eerpom(MENU_SETTING_INDEX_ADDRES,&check_data);
		{
		printf("check_data:%x \r\n",check_data);
		 if(check_data != 0x200167)
		 	{
		 	stm_write_eerpom(MENU_SETTING_RUNMODEL_ADDRES,0); 
		 	stm_write_eerpom(MENU_SETTING_GUJI_BY_DISTANCE,0);
		 	stm_write_eerpom(MENU_SETTING_GUJI_BY_TIME,0);
		 	stm_write_eerpom(MENU_SETTING_GUJI_BY_SPEED,0);
		 	stm_write_eerpom(MENU_SETTING_GUJI_BY_SPEED_1,0);
		 	stm_write_eerpom(MENU_SETTING_GUJI_BY_SPEED_2,0);
		 	stm_write_eerpom(MENU_SETTING_GUJI_RECOREDMETH,0);
		 	stm_write_eerpom(MENU_SETTING_RECODED_AUTOFLAG,0);
		 	stm_write_eerpom(MENU_SETTING_UNIT_TIMER,0);
		 	stm_write_eerpom(MENU_SETTING_UNIT_DATA,0);
		 	stm_write_eerpom(MENU_SETTING_UNIT_COORD,0);
		 	stm_write_eerpom(MENU_SETTING_UNIT_HW,0);
		 	stm_write_eerpom(MENU_SETTING_UNIT_TMPE,0);
		 	stm_write_eerpom(MENU_SETTING_UNIT_PREE,0);
			
		 	stm_write_eerpom(MENU_SETTING_FALME,0);
		 	stm_write_eerpom(MENU_SETTING_HEIGHT,70);
		 	stm_write_eerpom(MENU_SETTING_WEIGHT,40);

		 	stm_write_eerpom(MENU_SETTING_AJUST_ARR,0);
		 	stm_write_eerpom(MENU_SETTING_SCREER_TIMER,0);
		 	stm_write_eerpom(MENU_SETTING_TIMER_ZONE,16);
		 	stm_write_eerpom(MENU_SETTING_LANG,0);
			
			
		 	stm_write_eerpom(MENU_SETTING_INDEX_ADDRES,0x200167);

		 	stm_write_eerpom(CHANGE_TMPE_VAULE_INDEX_ADDRES,0); 
		 	stm_write_eerpom(CHANGE_SPEED_VAULE_INDEX_ADDRES,0);
		 	stm_write_eerpom(CHANGE_ALTITUD_VAULE_INDEX_ADDRES,0);
			stm_write_eerpom(MENU_GUJI_MODE_INDEX_ADDRES ,0);	
			stm_write_eerpom(MENU_SETTING_POWERCTROL ,0);
			stm_write_eerpom(MENU_POWERDOWNTIME_INDEX_ADDRES ,0);
//			Flash_Index = 0;
//			stm_write_eerpom(CUURENT_FLASH_ADDRER ,Flash_Index);			
		 	}
	
		 	{

			stm_read_eerpom(MENU_SETTING_RUNMODEL_ADDRES ,&check_data);
			system_flag_table.run_mode= check_data;
			printf("system_flag_table.run_mode:%d \r\n",system_flag_table.run_mode);
			if(system_flag_table.run_mode)
					{
					GPS_power_ctrl(1);
					USART_Cmd(USART2, ENABLE);
					}
				else
					{
					GPS_power_ctrl(0);
					USART_Cmd(USART2, DISABLE);
					}
			stm_read_eerpom(MENU_SETTING_GUJI_BY_MODE ,&check_data);
			system_flag_table.guji_record.by_distance_time_flag= check_data;					

			stm_read_eerpom(MENU_SETTING_GUJI_BY_DISTANCE ,&check_data);

			system_flag_table.guji_record.by_distance_vaule_ft= check_data;					
			system_flag_table.guji_record.by_distance_vaule= check_data;		
			
			stm_read_eerpom(MENU_SETTING_GUJI_BY_TIME ,&check_data);
			system_flag_table.guji_record.by_time_vaule= check_data;					

			stm_read_eerpom(MENU_SETTING_GUJI_BY_SPEED ,&check_data);
			system_flag_table.guji_record.by_speed_vaule= check_data;					

			stm_read_eerpom(MENU_SETTING_GUJI_BY_SPEED_1 ,&check_data);
			system_flag_table.guji_record.by_speed_vaule_1= check_data;					

			stm_read_eerpom(MENU_SETTING_GUJI_BY_SPEED_2 ,&check_data);
			system_flag_table.guji_record.by_speed_vaule_2= check_data;					

			stm_read_eerpom(MENU_SETTING_GUJI_RECOREDMETH ,&check_data);
			system_flag_table.guji_record.recoed_meth= check_data;					

			stm_read_eerpom(MENU_SETTING_RECODED_AUTOFLAG ,&check_data);
			system_flag_table.guji_record.auto_recoed_flag= check_data;					

			stm_read_eerpom(MENU_SETTING_UNIT_TIMER ,&check_data);
			system_flag_table.myunit.TIMER= check_data;					

			stm_read_eerpom(MENU_SETTING_UNIT_DATA ,&check_data);
			system_flag_table.myunit.DATA= check_data;					

			stm_read_eerpom(MENU_SETTING_UNIT_COORD ,&check_data);
			system_flag_table.myunit.COORD= check_data;					

			stm_read_eerpom(MENU_SETTING_UNIT_HW ,&check_data);
			system_flag_table.myunit.H_W= check_data;					

			stm_read_eerpom(MENU_SETTING_UNIT_TMPE ,&check_data);
			system_flag_table.myunit.TEMP= check_data;					

			stm_read_eerpom(MENU_SETTING_UNIT_PREE ,&check_data);
			system_flag_table.myunit.PREE= check_data;					

			stm_read_eerpom(MENU_SETTING_FALME ,&check_data);
			system_flag_table.personal_da.flame= check_data;					
			stm_read_eerpom(MENU_SETTING_HEIGHT ,&check_data);
			system_flag_table.personal_da.height= check_data;					
			stm_read_eerpom(MENU_SETTING_HEIGHT_1 ,&check_data);
			system_flag_table.personal_da.height_ft= check_data;					
			stm_read_eerpom(MENU_SETTING_WEIGHT ,&check_data);
		 	system_flag_table.personal_da.weight = check_data+20;
			stm_read_eerpom(MENU_SETTING_WEIGHT_1 ,&check_data);
		 	system_flag_table.personal_da.weight_ft = ((check_data+40)*0.4536);
					

			stm_read_eerpom(MENU_SETTING_AJUST_FLAG ,&check_data);
			system_flag_table.ajust_Arr_flag = check_data;
			stm_read_eerpom(MENU_SETTING_AJUST_ARR ,&check_data);
			system_flag_table.ajust_arr = check_data;				
			stm_read_eerpom(MENU_SETTING_SCREER_TIMER ,&check_data);
			system_flag_table.screen_timer= check_data;
			stm_read_eerpom(MENU_SETTING_TIMER_ZONE ,&check_data);
			system_flag_table.time_zone = check_data;		
//	time_zone = ((timer_zone_Aarry[system_flag_table.time_zone][1]-'0')*10)+(timer_zone_Aarry[system_flag_table.time_zone][2]-'0');
//	time_zone += ((((timer_zone_Aarry[system_flag_table.time_zone][4]-'0')*10)+(timer_zone_Aarry[system_flag_table.time_zone][5]-'0'))/60);

			time_zone = ((((timer_zone_Aarry[system_flag_table.time_zone][4]-'0')*10)+(timer_zone_Aarry[system_flag_table.time_zone][5]-'0')));
			time_zone =time_zone/60;
			time_zone += ((timer_zone_Aarry[system_flag_table.time_zone][1]-'0')*10)+(timer_zone_Aarry[system_flag_table.time_zone][2]-'0');
			
			printf("time_zone:%f \r\n",time_zone);
			system_flag_table.index_timerzone = time_zone;
			stm_read_eerpom(MENU_SETTING_LANG ,&check_data);
			system_flag_table.lang= check_data;	

			stm_read_eerpom(CHANGE_TMPE_VAULE_INDEX_ADDRES ,&check_data);
			old_div_cnt = check_data;
			stm_read_eerpom(CHANGE_SPEED_VAULE_INDEX_ADDRES ,&check_data);
			old_spped_div_cnt = check_data;
			stm_read_eerpom(CHANGE_ALTITUD_VAULE_INDEX_ADDRES ,&check_data);
			old_altiude_div_cnt = check_data;		
			stm_read_eerpom(MENU_GUJI_MODE_INDEX_ADDRES ,&check_data);
			guji_icon_index = check_data;
			stm_read_eerpom(MENU_POWERDOWNTIME_INDEX_ADDRES ,&check_data);


			if(check_data&0x12000000)
				 system_flag_table.powerdown_timers = check_data&0xffff;
			else
				system_flag_table.powerdown_timers  = 1;
			stm_read_eerpom(MENU_SETTING_POWERCTROL ,&check_data);
			system_flag_table.usb_powerdown_flash = check_data;
		//	rtc_set(14, 02, 20, 02, 37, 25, 4);
		 	}
		 	
		}

			system_flag_table.batt_Status  =  3;
}


void menu_display_setting(void);

const KeyTabStruct KeyTab[MAX_KEYTABSTRUCT_NUM] =
 {
// 菜单ID                父菜单ID       菜单名        菜单功能函数
// MenuID,              ParMenuID,     MenuName,      CurFunction
	{MENU_MAIN, 0, menu1, menu_main_display,0},
	{MENU_COORDINATES, 0, menu2, menu_real_speed_display,0},
	{MENU_REALTIME, 0, menu3, menu_real_crood_display,0},
	{MENU_TRANSFORMATION,    0,   menu4,  menu_temp_change_display,2},
	{MENU_MILEAGE,    MENU_TRANSFORMATION,   menu5,  menu_Pressure_display ,0},
	{MENU_SATELLITE,  MENU_TRANSFORMATION,      menu6,  menu_arruite_change_display,0},
	{MENU_SETTING_TEMPERATURRE,  MENU_TRANSFORMATION,  menu8_d,menu_speed_change_display  ,0},
	{MENU_RIDE_TABLE,0,  menu8,      menu_3D_display,0},
	{MENU_BAROMETER, 0,  menu7,  menu_home_display,'H'},	
	{MENU_SETTING_COOR, 0,  menu8_b,  menu_guiji_display,'G'},
	{MENU_SETTING, 0,  menu8_a,  menu_chaoxi_display,'C'},
	{MENU_SETTING_DISTANCE, 0,  menu8_c,  menu_display_setting,'S'},

#if 0 


,
 {MENU_RIDE_TABLE,0,  menu7,      NULL},
 {MENU_SETTING,      0,   menu8,      NULL},
 {MENU_SETTING_RECORD_SPACE,   MENU_SETTING,    menu8_a,  NULL},
 {MENU_SETTING_DISTANCE,   MENU_SETTING,    menu8_b,  NULL},
 {MENU_SETTING_TEMPERATURRE,   MENU_SETTING,    menu8_c,  NULL},
 {MENU_SETTING_RECORD_SPACE,   MENU_SETTING,    menu8_d,  NULL},
 {MENU_SETTING_DISPLAYMENU,   MENU_SETTING,    menu8_e,  NULL},
 {MENU_SETTING_LANGUAGE,   MENU_SETTING,    menu8_f,  NULL},
 {MENU_STTING_TIMEZONE,   MENU_SETTING,    menu8_g,  NULL},
#endif

 };

void Send_to_goto_lowpower(void)
{

 if(system_flag_table.screen_timer == 0)
	To_lower_timercnt = 16;
else  if(system_flag_table.screen_timer == 1)
	To_lower_timercnt = 31;
else
	To_lower_timercnt = 0;

if(system_power_flag == SYSTEM_POWER_STANBY)
	To_lower_timercnt  = 1;
printf("Send_to_goto_lowpower:%d \r\n",To_lower_timercnt);
}

u8 get_prev(void)
{
	u8 prevKeyTabIndex  = 0;



	if(enter_perss == 1)
		{
		if(keyTabIndex== MENU_TRANSFORMATION)
			{
			prevKeyTabIndex = keyTabIndex+1;
			while(KeyTab[prevKeyTabIndex].ParMenuID ==MENU_TRANSFORMATION)
				{
				prevKeyTabIndex++;
				if(prevKeyTabIndex == MAX_KEYTABSTRUCT_NUM)
					break;
				}
			
				prevKeyTabIndex --;
			}
                else
			prevKeyTabIndex =keyTabIndex-1 ;
			
			 if(system_flag_table.run_mode == 0)
			 	{
                   		 if(keyTabIndex == MENU_TRANSFORMATION)
					prevKeyTabIndex = MENU_SATELLITE;	 	
			 	
			 	}
			 else
                   		 if(keyTabIndex == MENU_TRANSFORMATION)
					prevKeyTabIndex = MENU_SETTING_TEMPERATURRE;	 	
			 	
		}	
	else
		{
	
                   if(system_flag_table.run_mode == 0)
                   		{
                   		 if(keyTabIndex == menu_change_index)
					prevKeyTabIndex = MENU_MAIN;	 	
				 else if(keyTabIndex == MENU_MAIN)
					prevKeyTabIndex=MENU_SETTING_DISTANCE;
				 else if(keyTabIndex == MENU_SETTING_DISTANCE)
					prevKeyTabIndex = menu_change_index;	 	
                   		}
			else
				{
				if(keyTabIndex == 0)
					prevKeyTabIndex = MAX_KEYTABSTRUCT_NUM-1;
				else
					{
	                         if(keyTabIndex == MENU_RIDE_TABLE)
      		                   		{
      	      	             		prevKeyTabIndex = menu_change_index ;
             	            		}
					else if(keyTabIndex == menu_change_index)
						{
      	      	             		prevKeyTabIndex = MENU_REALTIME ;
						}
					else
						prevKeyTabIndex= keyTabIndex - 1; 
					}
				

          //        if((KeyTab[prevKeyTabIndex].ParMenuID == MENU_TRANSFORMATION)||(KeyTab[prevKeyTabIndex].ParMenuID  == MENU_MAIN))

		
			     		
					while(KeyTab[prevKeyTabIndex].ParMenuID > KeyTab[keyTabIndex].ParMenuID)
						{
						prevKeyTabIndex--;
						if(prevKeyTabIndex == 0)
							break;
						}
			     	
				if(prevKeyTabIndex == MENU_TRANSFORMATION)
					prevKeyTabIndex = menu_change_index;
				}
		}
	

//	KeyTab[prevKeyTabIndex].CurrentOperate();
	
	keyTabIndex = prevKeyTabIndex ; 
	
	return 1;
 
}



u8 get_next(void)
{
	u8 nextKeyTabIndex = 0;

	nextKeyTabIndex = keyTabIndex+1;
	if(enter_perss == 1)
		{	
		if(KeyTab[nextKeyTabIndex].ParMenuID!= MENU_TRANSFORMATION)
			{

			while(KeyTab[nextKeyTabIndex].MenuID!=MENU_TRANSFORMATION)
				{
				nextKeyTabIndex--;
				if(nextKeyTabIndex == 0)
					break;
				}
			}
			 if(system_flag_table.run_mode == 0)
			 	{
                   		 if(keyTabIndex == MENU_SATELLITE)
					nextKeyTabIndex = MENU_TRANSFORMATION;	 	
			 	
			 	}
			 else
			 	{
			 	     if(keyTabIndex == MENU_SETTING_TEMPERATURRE)
					nextKeyTabIndex = MENU_TRANSFORMATION;	 
			 	}
		
		}	
	else
		{


                   if(system_flag_table.run_mode == 0)
                   		{
                   		 if(keyTabIndex == menu_change_index)
					nextKeyTabIndex = MENU_SETTING_DISTANCE;	 	
				 else if(keyTabIndex == MENU_SETTING_DISTANCE)
					nextKeyTabIndex = MENU_MAIN;	 	
				 else if(keyTabIndex == MENU_MAIN)
					nextKeyTabIndex = menu_change_index;
                   		}
			else		
				{
	
			      if(nextKeyTabIndex>=MAX_KEYTABSTRUCT_NUM)
				  	nextKeyTabIndex = 0;
				if(keyTabIndex == menu_change_index)
					nextKeyTabIndex = MENU_RIDE_TABLE;

				while((KeyTab[nextKeyTabIndex].ParMenuID > KeyTab[keyTabIndex].ParMenuID))
					{
					nextKeyTabIndex++;
					if(nextKeyTabIndex == MAX_KEYTABSTRUCT_NUM)
						break;
					}	
				if(nextKeyTabIndex == MENU_TRANSFORMATION)
					nextKeyTabIndex = menu_change_index;
				}

			
		}

	//KeyTab[nextKeyTabIndex].CurrentOperate();	  	
	keyTabIndex = nextKeyTabIndex ;
	
 return 0;
}




u8 anayes_angle(float angle)
{

u8 temp_angle = 0;
	if((0<=angle)&&(angle<=22.5))
		{
		if(angle<=11.25)
			  temp_angle = 1;	 // zhengbei
		else
			  temp_angle = 2;	 // zhengbei
		}
	else if((22.5<angle)&&(angle<=67.5))
		{
		if(angle<=33.75)
			  temp_angle = 2;	 // dongbei		
		  else if(angle<=56.25)
			  temp_angle = 3;	 // dongbei
		  else
		  	  temp_angle = 4;	 // dongbei		
		}
	else if((67.5<angle)&&(angle<=112.5))
		{
		if(angle<=78.75)
			  temp_angle = 4;	 // dongbei		
		  else if(angle<=101.25)
			  temp_angle = 5;	 // dongbei
		  else
		  	  temp_angle = 6;	 // dongbei		

	//	  temp_angle = 6;	 // dongbei						
		}
	else if((112.5<angle)&&(angle<=157.5))
		{
		if(angle<=123.75)
			  temp_angle = 6;	 // dongbei		
		  else if(angle<=146.25)
			  temp_angle = 7;	 // dongbei
		  else
		  	  temp_angle = 8;	 // dongbei	
//		  temp_angle = 7;	 // dongbei						
		}
	else if((157.5<angle)&&(angle<=202.5))
		{
		if(angle<=168.75)
			  temp_angle = 8;	 // dongbei		
		  else if(angle<=191.25)
			  temp_angle = 9;	 // dongbei
		  else
		  	  temp_angle = 10;	 // dongbei			
//		  temp_angle = 1;	 // dongbei						
		}

	else if((202.5<angle)&&(angle<=247.5))
		{
		if(angle<=213.75)
			  temp_angle = 10;	 // dongbei		
		  else if(angle<=236.25)
			  temp_angle = 11;	 // dongbei
		  else
		  	  temp_angle = 12;	 // dongbei	
	//	  temp_angle = 4;	 // dongbei						
		}

	else if((247.5<angle)&&(angle<=292.5))
		{
		if(angle<=258.75)
			  temp_angle = 12;	 // dongbei		
		  else if(angle<=281.25)
			  temp_angle = 13;	 // dongbei
		  else
		  	  temp_angle = 14;	 // dongbei	
//		  temp_angle = 5;	 // dongbei						
		}
	else if((292.5<angle)&&(angle<=337.5))
		{
		if(angle<=303.75)
			  temp_angle = 14;	 // dongbei		
		  else if(angle<=326.25)
			  temp_angle = 15;	 // dongbei
		  else
		  	  temp_angle = 16;	 // dongbei			
//		  temp_angle = 2;	 // dongbei						
		}

	else if((337.5<angle)&&(angle<=360))
		{
		if(angle<=348.75)
			  temp_angle = 16;	 // dongbei		
		  else 
			  temp_angle = 1;	 // dongbei			
		 //  temp_angle = 3;	 // zhengbei						
		}	
	else
		temp_angle  = 0xff;

//	printf("angle:%f \r\n",angle);
	
return temp_angle;
}


const u8 input_jinweidu_post[18]={24,32,40,48,64,72,88,96,112,24,32,40,48,64,72,88,96,112};

void convert_show_en(float tp,u8 flag,u8 y,u8 mode)
{
	
	u8 du,fen,index;	
      float miao;
	extern const u8 bmpdu[];
	extern const u8 bmpmaio[];
	extern const u8 bmpfen[];



      if(system_flag_table.myunit.COORD == 0)
      	{
	      	miao = tp/1000000;
		if(mode == 1 )
			{
			if(miao<100)
				sprintf((char *)dtbuf,"%1c  %.6f",flag,miao);	//得到经度字符串	
			else	
				sprintf((char *)dtbuf,"%1c %.6f",flag,miao);	//得到经度字符串	
			}
		else
			{
			if(miao<100)
				sprintf((char *)dtbuf," %1c %.6f",flag,miao);	//得到经度字符串	
			else	
				sprintf((char *)dtbuf," %1c%.6f",flag,miao);	//得到经度字符串	

			}
			
			if(miao == 0)
				index = 104;	
			else
				index = 112;		 
		OLED_ShowString(16,y,dtbuf);	 	
		Movie_Show_Img(index,y,bmpdu);
      	}
     else if(system_flag_table.myunit.COORD == 1)
      	{
	      	du = (u8)(tp/1000000);
		miao = (tp/1000000 - du)*60 ;
		if(mode == 1 )
			{
			if(du >100)
				{
					sprintf((char *)dtbuf,"%1c %03d",flag,du);	//得到经度字符串	
				}
			else
				{
					sprintf((char *)dtbuf,"%1c  %02d",flag,du);	//得到经度字符串
					
				}
			}
		else
			{
			if(du >100)
				{
					sprintf((char *)dtbuf," %1c%03d",flag,du);	//得到经度字符串	
				}
			else
				{
					sprintf((char *)dtbuf," %1c %02d",flag,du);	//得到经度字符串
					
				}

			}		
		index = 56;
		OLED_ShowString(16,y,dtbuf);	 		
		index +=0;
		Movie_Show_Img(index,y,bmpdu);

		if(miao<10)
			sprintf((char *)dtbuf,"0%.3f",miao);	//得到经度字符串	
		else	
			sprintf((char *)dtbuf,"%.3f",miao);	//得到经度字符串	
		index += 8;
		OLED_ShowString(index,y,dtbuf);
		index =112;	
		Movie_Show_Img(index,y,bmpfen);

      	}

	else if(system_flag_table.myunit.COORD == 2)

		{
	//tp=gpsx.longitude;
		du = (u8)(tp/1000000);
		fen = (u8)((tp/1000000 - du)*60);
		miao = ((tp/1000000 - du)*60 -fen)*60;

		 if(miao >59.0)
		 	{
		 	miao = 0.0;
			fen++;
			if(fen>59)
				du++;
		 	}	
		 
		if(mode == 1 )
			{
			if(du >100)
				{
					sprintf((char *)dtbuf,"%1c %03d",flag,du);	//得到经度字符串	
				}
			else
				{
					sprintf((char *)dtbuf,"%1c  %02d",flag,du);	//得到经度字符串
					
				}
			}
		else
			{
			if(du >100)
				{
					sprintf((char *)dtbuf," %1c%03d",flag,du);	//得到经度字符串	
				}
			else
				{
					sprintf((char *)dtbuf," %1c %02d",flag,du);	//得到经度字符串
					
				}

			}	
			
		index = 56;
		//printf("\r\n du: %d  %d %d \n\n",du,fen,miao);
			OLED_ShowString(16,y,dtbuf);	 
		index +=0;
		Movie_Show_Img(index,y,bmpdu);
		index += 8;
		sprintf((char *)dtbuf,"%02d",fen);	//得到经度字符串
		OLED_ShowString(index,y,dtbuf);
		index +=16;	
		Movie_Show_Img(index,y,bmpfen);
		index += 8;
		if(miao<10.0)
			sprintf((char *)dtbuf,"0%.1f",miao);	//得到经度字符串
		else	
			sprintf((char *)dtbuf,"%.1f",miao);	//得到经度字符串
		OLED_ShowString(index,y,dtbuf);	
		index =120;
		Movie_Show_Img(index,y,bmpmaio);
		}
}

void convert_show_en_input(u8 *prt_c ,u8 y)
{
	
	u8 index;	

	extern const u8 bmpdu[];
	extern const u8 bmpmaio[];
	extern const u8 bmpfen[];


	sprintf((char *)dtbuf,"%1c%c%c%c",prt_c[0],prt_c[1],prt_c[2],prt_c[3]);	//得到经度字符串
	index = 56;

	//printf("\r\n du: %d  %d %d \n\n",du,fen,miao);
 	OLED_ShowString(24,y,dtbuf);	 
	index +=0;
	Movie_Show_Img(index,y,bmpdu);
	index += 8;
	sprintf((char *)dtbuf,"%c%c",prt_c[4],prt_c[5]);	//得到经度字符串
	OLED_ShowString(index,y,dtbuf);
	index +=16;	
	Movie_Show_Img(index,y,bmpfen);
	index += 8;
	sprintf((char *)dtbuf,"%c%c.%c",prt_c[6],prt_c[7],prt_c[8]);	//得到经度字符串
	OLED_ShowString(index,y,dtbuf);	
	index =120;
	Movie_Show_Img(index,y,bmpmaio);
}
void menu_real_crood_display(void)

{ 
	extern const u8 zhunquedu[];
	extern const u8 bmp_jinwei[];
	float tp;
       
 //	 sprintf((char *)dtbuf,"%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes);	//显示UTC日期
 //	OLED_ShowString_head(0,0,dtbuf);
// 	Movie_Show_Img(39,0,Bmp_test_status);
//	show_levev_space();
	OLED_Fill(0, 0, 127, 63, 0);
	menu_status_dispplay();
	if(system_flag_table.lang == 1)
		OLED_ShowString(0,48,"Accuracy:");
	else	
		{	
		Movie_Show_Img(0,48,zhunquedu);
		}
 	tp =gpsx.pdop;
 	tp =tp/100;
	 sprintf((char *)dtbuf,"%0.1f",tp);	//显示UTC日期
	OLED_ShowString(72, 48, dtbuf);
//	Movie_Show_Img(0,16,bmp_jinwei);
 //      printf("longitude :%d \r\n",gpsx.longitude);	//显示UTC日期
   //    printf("latitude :%d \r\n",gpsx.latitude);	//显示UTC日期

       if(gpsx.fixmode == 3)
		tp = gpsx.longitude;
      // printf("tp1 :%.6f \r\n",tp );	//显示UTC日期
	convert_show_en(tp,gpsx.ewhemi,32,1);
	if(gpsx.fixmode == 3)	  
		tp = gpsx.latitude;
      // printf("tp2 :%.6f \r\n",tp );	//显示UTC日期
	convert_show_en(tp,gpsx.nshemi,16,1);
	OLED_Refresh_Gram();
	
}

void check_time(void)
{
u16 gpsx_utc;	
u16 hour_timer,min_timer;
//u16 hour_timer_rtc,min_timer_rtc;
s16 diff_tone = 0.0;
u32 setting_tp = 0;


	printf("check_time\n");

     if((system_flag_table.run_mode == 1)&&(gpsx.svnum >= 2))
     		{
			if((gpsx.utc.year<2014)||(gpsx.utc.year==2080))
				{
				rtc_set_flag = 1;
				return;
				}
			my_timer.w_year = gpsx.utc.year -2000;
			my_timer.w_month =gpsx.utc.month;
			my_timer.w_date = gpsx.utc.date; 
			my_timer.week = RTC_Get_Week(gpsx.utc.year ,my_timer.w_month,my_timer.w_date);
			gpsx_utc = (gpsx.utc.hour*60+gpsx.utc.min);
			my_timer.sec = gpsx.utc.sec;

	if(timer_zone_Aarry[system_flag_table.time_zone][0] == '+')
		{
		hour_timer = ((timer_zone_Aarry[system_flag_table.time_zone][1]-'0')*10)+(timer_zone_Aarry[system_flag_table.time_zone][2]-'0');
		min_timer = ((timer_zone_Aarry[system_flag_table.time_zone][4]-'0')*10)+(timer_zone_Aarry[system_flag_table.time_zone][5]-'0');
		//printf("\n\r hour_timer : %d   min_timer :%d  ",hour_timer,min_timer); 
		if((gpsx_utc +(hour_timer*60+min_timer))>=(24*60))
			{
			  RTC_Get(1);
			   gpsx_utc  = (gpsx_utc + (hour_timer*60+min_timer) - (24*60));
			hour_timer = gpsx_utc/60;
			   min_timer  = gpsx_utc - hour_timer*60;
			}
		else
			{
			   gpsx_utc  = (gpsx_utc + (hour_timer*60+min_timer));
			hour_timer = gpsx_utc /60;
			   min_timer  = gpsx_utc - hour_timer*60;

			}
	//	printf("\n\r hour_timer : %d   min_timer :%d  ",hour_timer,min_timer); 
		}
	else if(timer_zone_Aarry[system_flag_table.time_zone][0] == '-')
		{
		hour_timer = ((timer_zone_Aarry[system_flag_table.time_zone][1]-'0')*10)+(timer_zone_Aarry[system_flag_table.time_zone][2]-'0');
		min_timer = ((timer_zone_Aarry[system_flag_table.time_zone][3]-'0')*10)+(timer_zone_Aarry[system_flag_table.time_zone][4]-'0');
		if(gpsx_utc > (hour_timer*60+min_timer))
			{
			   gpsx_utc  = (gpsx_utc - (hour_timer*60+min_timer));
			hour_timer = gpsx_utc/60;
			   min_timer  = gpsx_utc - hour_timer*60;
			}
		else
			{
			   gpsx_utc  = (gpsx_utc+(24*60) - (hour_timer*60+min_timer));
			hour_timer = gpsx_utc/60;
			   min_timer  = gpsx_utc - hour_timer*60;			
			RTC_Get(0);
			}
		}
	
	 }
	 else
	 	{
	 	my_timer.w_year = RTC_DateStructure.RTC_Year;
		my_timer.w_month =RTC_DateStructure.RTC_Month;
		my_timer.w_date = RTC_DateStructure.RTC_Date;
		my_timer.week = RTC_DateStructure.RTC_WeekDay;
		gpsx_utc = (RTC_TimeStructure.RTC_Hours *60+RTC_TimeStructure.RTC_Minutes);
		my_timer.sec = RTC_TimeStructure.RTC_Seconds ;
		diff_tone = gpsx_utc;
		diff_tone +=(s16)((time_zone - system_flag_table.index_timerzone)*60);

		if(diff_tone>=(24*60))
			{
			 RTC_Get(1);
			 diff_tone = diff_tone - (24*60);
			}
		if(diff_tone<0)
			{
			RTC_Get(0);
			 diff_tone = diff_tone + (24*60);
			}
		hour_timer = diff_tone/60;
		 min_timer  = diff_tone - hour_timer*60;
	 	}
		//printf("\n\r my_timer.w_year : %d   min_timer :%d  ",hour_timer,min_timer); 
		rtc_set(my_timer.w_year,my_timer.w_month,my_timer.w_date,hour_timer,min_timer,my_timer.sec,my_timer.week);
		stm_read_eerpom(MENU_FRIST_POWER_INDEX_ADDRES ,&setting_tp);
		     if((system_flag_table.run_mode == 1)&&(gpsx.svnum >= 2))
		     	{
				if(setting_tp !=0x12345678)
				{
				stm_write_eerpom(MENU_FRIST_POWER_INDEX_ADDRES ,0x12345678);					
				setting_tp = (my_timer.w_year<<16)|(my_timer.w_month<<8)|my_timer.w_date;
				stm_write_eerpom(MENU_FRIST_POWERDATE_INDEX_ADDRES ,setting_tp);		
				}
		 	}

	system_flag_table.index_timerzone = time_zone;	 
}

void menu_show_gujIcon_Status(u8 icon)
{
	extern const unsigned char Bmp_gujiiconsetp[];
	extern const unsigned char Bmp_gujiiconrun[];
	extern const unsigned char Bmp_gujiiconB[];
	extern const unsigned char Bmp_gujiiconcar[];
	extern const unsigned char Bmp_gujiiconfly[];
	extern const unsigned char Bmp_gujiiconCustom[];
	  
switch(icon)
	{
	case 0:
		Movie_Show_Img(85,0,Bmp_gujiiconsetp);
		break;
	case 1:
		Movie_Show_Img(85,0,Bmp_gujiiconrun);		
		break;
	case 2:
		Movie_Show_Img(85,0,Bmp_gujiiconB);
		break;
	case 3:
	      Movie_Show_Img(85,0,Bmp_gujiiconcar);
		break;
	case 4:
		Movie_Show_Img(85,0,Bmp_gujiiconfly);
		break;
	case 5:
		Movie_Show_Img(85,0,Bmp_gujiiconCustom);
		break;

	}
}

extern const unsigned char Bmp_am[];
extern const unsigned char Bm_pm[];

void menu_status_dispplay(void)
{

extern const unsigned char Bmp_chageing[];
extern const unsigned char Bmp_chage_ok[];
extern const unsigned char Bmp_batt_0[];
extern const unsigned char Bmp_batt_1[];
extern const unsigned char Bmp_batt_2[];
extern const unsigned char Bmp_batt_3[];

extern const unsigned char Bmp_singe_no[];
extern const unsigned char Bmp_singe_1[];
extern const unsigned char Bmp_singe_2[];
extern const unsigned char Bmp_singe_3[];

extern const unsigned char Bmp_bigam[];
extern const unsigned char Bmp_bigpm[];
extern const unsigned char Bmp_bigam_1[];
extern const unsigned char Bmp_bigpm_1[];
extern const unsigned char Bmp_wanner[];

extern u8 usb_delay_timer_poweron;

extern u8 flash_baoyu_flag ;

float tp;
u16 gpsx_utc;	
extern   int ddrunmv;

gpsx_utc = (RTC_TimeStructure.RTC_Hours*60+RTC_TimeStructure.RTC_Minutes);


if((system_power_flag == SYSTEM_POWER_LOW)&&(system_flag_table.run_mode == 0))
	{
	OLED_Fill(0, 19, 22, 44, 0);
	  if(system_flag_table.myunit.TIMER == 0)	 	
	 		{
	 		  if(gpsx_utc>=12*60)
	 			{	 			
				 if(gpsx_utc>=13*60)
			 		gpsx_utc= gpsx_utc - 12*60;
				 
				 if(gpsx_utc/60>=10)
				 	{
					 Movie_Show_Img(1,19,Bmp_bigpm_1);
				 	sprintf((char *)dtbuf,"%d:%02d:%02d",gpsx_utc/60-10,gpsx_utc%60,RTC_TimeStructure.RTC_Seconds);	//显示UTC日期
				 	}
				 else
				 	{
				 	Movie_Show_Img(1,19,Bmp_bigpm);
			 		sprintf((char *)dtbuf,"%d:%02d:%02d",gpsx_utc/60,gpsx_utc%60,RTC_TimeStructure.RTC_Seconds);	//显示UTC日期

				 	}
				 OLED_ShowString_24(16,19,dtbuf);	
			 	}
			 else
			 	{
				 if(gpsx_utc/60>=10)
				 	{
			 		sprintf((char *)dtbuf,"%d:%02d:%02d",gpsx_utc/60-10,gpsx_utc%60,RTC_TimeStructure.RTC_Seconds);	//显示UTC日期
					 Movie_Show_Img(1,19,Bmp_bigam_1);
					 }
				 else if(gpsx_utc < 60)
				 	{
				 	sprintf((char *)dtbuf,"%d:%02d:%02d",2,gpsx_utc%60,RTC_TimeStructure.RTC_Seconds);	//显示UTC日期
					 Movie_Show_Img(1,19,Bmp_bigam_1);
					 }
				 else
				 	{
			 		sprintf((char *)dtbuf,"%d:%02d:%02d",gpsx_utc/60,gpsx_utc%60,RTC_TimeStructure.RTC_Seconds);	//显示UTC日期
			 		 Movie_Show_Img(1,19,Bmp_bigam);
				 	}
				 OLED_ShowString_24(16,19,dtbuf);
			 	}
		 	}
		else
			{	
			 sprintf((char *)dtbuf,"%02d:%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);	//显示UTC日期
			 OLED_ShowString_24(0,19,dtbuf);

			// 	printf("%02d:%02d:%02d\r\n",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
			}    
		     	
	}
else if((system_power_flag == SYSTEM_POWER_STANBY)||(system_power_flag == SYSTEM_POWER_LOW_2))
	{
	 if((USB_CONNECT == 1)&&(usb_delay_timer_poweron == 0))
	 	{

			if(system_flag_table.charger_connected == 1)
			{
				Movie_Show_Img(43,1,Bmp_chageing);
			if(system_flag_table.batt_Status == 5)		
				Movie_Show_Img(43,1,Bmp_chage_ok);			 
			}
			else
				{
			 if(system_flag_table.batt_Status == 1)	
				Movie_Show_Img(43,1,Bmp_batt_1);
			else if(system_flag_table.batt_Status == 2)	
				Movie_Show_Img(43,1,Bmp_batt_2);
			else if(system_flag_table.batt_Status == 0)	
				Movie_Show_Img(43,1,Bmp_batt_0);
			else if(system_flag_table.batt_Status == 3)	
				Movie_Show_Img(43,1,Bmp_batt_3);
			else
				Movie_Show_Img(43,1,Bmp_batt_3);				
				}


	//		 sprintf((char *)dtbuf,"%d",ddrunmv);	//显示UTC日期
	 //OLED_ShowString12(60,0,dtbuf);
	 	}
	 else
	 	{
			OLED_Fill(0, 0, 127, 16, 0);
			
	 	}
	// if(system_power_flag == SYSTEM_POWER_LOW_2)
	/// 	{
	 	
	 //	}
	}
else
	{

	OLED_Fill(0, 0, 127, 16, 0);

#if 0	
	if(rtc_set_flag == 1)
		{
 		 sprintf((char *)dtbuf,"--:--");	//显示UTC日期
		 OLED_ShowString_head(0,0,dtbuf);	

		}
	else
#endif		
		{
		 if(system_flag_table.myunit.TIMER == 0)	 	
	 		{
	 		  if(gpsx_utc>=12*60)
	 			{
	 			 Movie_Show_Img(34,0,Bm_pm);
				 if(gpsx_utc>=13*60)
			 		gpsx_utc= gpsx_utc - 12*60;
		 		sprintf((char *)dtbuf,"%02d:%02d",gpsx_utc/60,gpsx_utc%60);	//显示UTC日期
				 OLED_ShowString_head(0,0,dtbuf);	
				
			 	}
			 else
			 	{
				 Movie_Show_Img(34,0,Bmp_am);
				 if(gpsx_utc>60)
			 		sprintf((char *)dtbuf,"%02d:%02d",gpsx_utc/60,gpsx_utc%60);	//显示UTC日期
			 	else
					sprintf((char *)dtbuf,"%02d:%02d",12,gpsx_utc%60);	//显示UTC日期
				 OLED_ShowString_head(0,0,dtbuf);	

			 	}
		 	}
		else
			{	
			 sprintf((char *)dtbuf,"%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes);	//显示UTC日期
			 OLED_ShowString_head(0,0,dtbuf);	
			 
			}
		}

		if(system_flag_table.charger_connected == 1)
		{
			Movie_Show_Img(43,1,Bmp_chageing);
		if(system_flag_table.batt_Status == 5)		
			Movie_Show_Img(43,1,Bmp_chage_ok);			 
		}
		else
			{
		 if(system_flag_table.batt_Status == 1)	
			Movie_Show_Img(43,1,Bmp_batt_1);
		else if(system_flag_table.batt_Status == 2)	
			Movie_Show_Img(43,1,Bmp_batt_2);
		else if(system_flag_table.batt_Status == 0)	
			Movie_Show_Img(43,1,Bmp_batt_0);
		else if(system_flag_table.batt_Status == 3)	
			Movie_Show_Img(43,1,Bmp_batt_3);
		else
			Movie_Show_Img(43,1,Bmp_batt_3);				
			}

	
	tp =gpsx.pdop;
	tp =tp/100;
	
if(system_flag_table.run_mode == 1)
{
       if(gpsx.fixmode >= 2)
       	{
		 if(tp <5)
			 Movie_Show_Img(61,0,Bmp_singe_3);	 
		 else if(tp <10)
			 Movie_Show_Img(61,0,Bmp_singe_2);	 
		 else if(tp <50)
			 Movie_Show_Img(61,0,Bmp_singe_1);	 
       	}
	   else
			 Movie_Show_Img(61,0,Bmp_singe_no);	
}

	if((flash_baoyu_flag == 1)&&(baoyun_flag == 0))
		{	
			 Movie_Show_Img(61,0,Bmp_wanner);	
		}
	   if(guji_mode == RECORED_START_DOING)
	   	{
	   	      
			 menu_show_gujIcon_Status(guji_icon_index);	
	   	}

	   show_levev_space();
	   
	}

}

void menu_main_display(void)
{
//u8 dtbuf_index = 0;

s16 gpsx_utc;
float temperature_F;
u16 year,month,data ,week,hour,min,sec;
 OLED_Fill(0, 0, 127, 63, 0);
	menu_status_dispplay();
//	 sprintf((char *)dtbuf,"%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes);	//显示UTC日期
//	OLED_ShowString_head(0,0,dtbuf);	
      if(1)//system_flag_table.run_mode == 0)
      		{
      		year = RTC_DateStructure.RTC_Year;
      		month = RTC_DateStructure.RTC_Month;
      		data = RTC_DateStructure.RTC_Date;
      		week = RTC_DateStructure.RTC_WeekDay;
      		hour = RTC_TimeStructure.RTC_Hours;
      		min = RTC_TimeStructure.RTC_Minutes;
      		sec =  RTC_TimeStructure.RTC_Seconds;
      		}
	//  printf("\r\n%d-%d-%d  %d  %d-%d-%d\r\n",year,month,data,week,hour,min,sec);
#if 0	  
//	else
		{
		year = gpsx.utc.year;
      		month = gpsx.utc.month;
      		data = gpsx.utc.date;
      		week = RTC_Get_Week(year,  month, data);
      		hour = gpsx.utc.hour;
      		min = gpsx.utc.min;
		sec =  gpsx.utc.sec;
		printf("\r\n%d-%d-%d  %d  %d-%d-%d\r\n",year,month,data,week,hour,min,sec);
		}
#endif	
//	if(year>2000)
//		year = year-2000;
	
	switch(system_flag_table.myunit.DATA)
		{
		case 0:
		 	sprintf((char *)dtbuf,"%02d-%02d-%02d",year,month,data);	//显示UTC日期
		 	break;
		case 1:
		 	sprintf((char *)dtbuf,"%02d-%02d-%02d",month,data,year);	//显示UTC日期
		 	break;
		case 2:
		 	sprintf((char *)dtbuf,"%02d-%02d-%02d",data,month,year);	//显示UTC日期
		 	break;

		}
	//printf("year2:%d\r\n",gpsx.utc.year);
	OLED_ShowString(0,48,dtbuf);		

	gpsx_utc = (hour*60+min);
	if(system_flag_table.myunit.TIMER == 1)
		{
		sprintf((char *)dtbuf,"%02d:%02d:%02d",hour,min,sec);	//显示UTC时间
	  	OLED_ShowString(0,32,dtbuf);	 	
		}
	else
		{
	 	  if(gpsx_utc>=12*60)
	 		{
	 		if(gpsx_utc>=13*60)
		 		 gpsx_utc= gpsx_utc - 12*60;
	 	 	 sprintf((char *)dtbuf,"%02d:%02d:%02d PM  ",gpsx_utc/60,gpsx_utc%60,sec);
			 OLED_ShowString(0,32,dtbuf);	
			 
		 	}
		 else
		 	{
		 	if(gpsx_utc<60)
				sprintf((char *)dtbuf,"%02d:%02d:%02d AM  ",12,gpsx_utc%60,sec);
					else
				sprintf((char *)dtbuf,"%02d:%02d:%02d AM  ",gpsx_utc/60,gpsx_utc%60,sec);
			 OLED_ShowString(0,32,dtbuf);	
		 	}

		}

	
//	rtc_timer = (RTC_TimeStructure.RTC_Hours*60+RTC_TimeStructure.RTC_Minutes);
#if 0
	if(gpsx_utc>rtc_timer)
		{
		comp_time = (gpsx_utc-rtc_timer)/60;
//		time_zone = comp_time;
		sprintf((char *)dtbuf,"+%02d:%02d",comp_time,(gpsx_utc-rtc_timer-(comp_time*60)));	//显示UTC时间
		}
	else

		{
		comp_time = (rtc_timer-gpsx_utc)/60;
//		time_zone = comp_time;
		sprintf((char *)dtbuf,"+%02d:%02d",comp_time,(rtc_timer-gpsx_utc-(comp_time*60)));	//显示UTC时间
		}
#endif			
      	sprintf((char *)dtbuf,"%c%c%c",week_word[week][0],week_word[week][1],week_word[week][2]);
	OLED_ShowString(88,48,dtbuf);	 	
	temperature_F=temperature;
	//temperature_F =temperature_F/10;
	if(system_flag_table.myunit.TEMP == 0)
		{
	//	conversion(temperature);
//		sprintf((char *)dtbuf,"%c%c.%c C",bai,shi,ge);	//得到经度字符串
                sprintf((char *)dtbuf,"%.1f C",temperature_F/10);	//得到经度字符串 
	//	OLED_ShowString((72-what_is_numbe_10(temperature)*8),16,dtbuf);	 	
	//	Movie_Show_Img((104-what_is_numbe_10(temperature)*8),16,bmpdu);
		}
	else if(system_flag_table.myunit.TEMP == 1)
		{
		temperature_F = 320+(1.8*temperature);
	//	conversion(temperature_F);
	//	if(temperature_F>=1000)
			{
			sprintf((char *)dtbuf,"%.1f F",temperature_F/10);	//得到经度字符串
	//		OLED_ShowString(56,16,dtbuf);	 	
//			Movie_Show_Img(88,16,bmpdu);
			}


			
	//		OLED_ShowString(56,16,dtbuf);	 	
	//		Movie_Show_Img(96,16,bmpdu);
		}

		OLED_ShowString((80-what_is_numbe_10(temperature_F/10)*8),16,dtbuf);	 	
		Movie_Show_Img(96,16,bmpdu);

      if(system_flag_table.myunit.H_W == 0)
      	{

	sprintf((char *)&dtbuf,"%dm",Altitude/100);	//得到经度字符串		
      	}
     else  if(system_flag_table.myunit.H_W >= 1)
      	{

		gpsx_utc = (floor)(Altitude/100/0.3048);

		sprintf((char *)&dtbuf,"%dft",gpsx_utc);	//得到经度字符串	
      	}

	OLED_ShowString(0,16,dtbuf);	

//	Movie_Show_Img(39,0,Bmp_test_status);
//	show_levev_space();
//	OLED_Refresh_Gram();
	
}


void menu_real_speed_display(void)
{

float tp;		

extern const u8 fangxianbmp[];
extern const u8 bmpfangxian90[];
extern const u8 bmpfangxian180[];
extern const u8 bmpfangxian270[];
extern const u8 fangxianbmp22[];
extern const u8 fangxianbmp45[];
extern const u8 fangxianbmp67[];
extern const u8 bmpfangxian112[];
extern const u8 bmpfangxian135[];
extern const u8 bmpfangxian157[];
extern const u8 bmpfangxian202[];
extern const u8 bmpfangxian225[];
extern const u8 bmpfangxian247[];
extern const u8 bmpfangxian270[];
extern const u8 bmpfangxian292[];
extern const u8 bmpfangxian314[];
extern const u8 bmpfangxian336[];
//extern const u8 donnanword[];

extern const u8 bmp_worddong[];
extern const u8 bmp_wordBei[];
extern const u8 bmp_wordXi[];
extern const u8 bmp_wordnan[];
s16 tmp_Att = 0;
//u8 dtbuf_index = 0;

//       RTC_TimeShow();
//	 sprintf((char *)dtbuf,"%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes);	//显示UTC日期
//	OLED_ShowString_head(0,0,dtbuf);
//	Movie_Show_Img(39,0,Bmp_test_status);
//	show_levev_space();
	OLED_Fill(0, 0, 127, 63, 0);
      menu_status_dispplay();
	tp=gpsx.speed;	   

	if(system_flag_table.myunit.H_W == 0)//||(system_flag_table.myunit.H_W == 1))
	 	sprintf((char *)dtbuf,"%.1fkm/h",tp/1000);		    		//得到速度字符串	 

#if 1
	else if(system_flag_table.myunit.H_W == 1)
	 	sprintf((char *)dtbuf,"%.3fmi/h",tp/1609);		    		//得到速度字符串	 
#endif
	else if(system_flag_table.myunit.H_W == 2)
	 	sprintf((char *)dtbuf,"%.1fkt/h",tp/1852);		    		//得到速度字符串	 
 	OLED_ShowString(48,16,dtbuf);	 
	
      if(system_flag_table.myunit.H_W == 0)
      		{
		sprintf((char *)&dtbuf,"%dm",Altitude/100);	//得到经度字符串		
      		}
     else  if(system_flag_table.myunit.H_W >= 1)
      		{
      		tmp_Att= (floor)(Altitude/100/0.3405);
		sprintf((char *)&dtbuf,"%dft",tmp_Att);	//得到经度字符串	
      		}
	
	OLED_ShowString(48,32,dtbuf);
	
	tp=gpsx.angle;
	tp /=1000;

	if(tp <10)
		sprintf((char *)dtbuf,"   %d",(u16)tp);		    		//得到速度字符串	 
	else if(tp <100)
		sprintf((char *)dtbuf,"  %d",(u16)tp);		    		//得到速度字符串	 
	else
		sprintf((char *)dtbuf," %d",(u16)tp);		    		//得到速度字符串	 
		
 	OLED_ShowString(80,48,dtbuf);	 
	Movie_Show_Img(120,48,bmpdu);

	switch(anayes_angle(tp))
		{
		case 1:  //zhengbei
		//	Movie_Show_Img(0,16,fangxianbmp);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"N");
		else	
			Movie_Show_Img(48,48,bmp_wordBei);
			break;
		case 2: //zhengdong
		//	Movie_Show_Img(0,16,fangxianbmp22);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"NNE");
		else	
			{
			Movie_Show_Img(48,48,bmp_wordBei);
			Movie_Show_Img(64,48,bmp_wordBei);
			Movie_Show_Img(80,48,bmp_worddong);
			}
			break;
		case 3: //zhengnan
		//	Movie_Show_Img(0,16,fangxianbmp45);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"EN");
		else	
			{

			Movie_Show_Img(48,48,bmp_worddong);
			Movie_Show_Img(64,48,bmp_wordBei);
			}

			break;
		case 4:
		//	Movie_Show_Img(0,16,fangxianbmp67);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"ENE");
		else	
			{

			Movie_Show_Img(48,48,bmp_worddong);
			Movie_Show_Img(64,48,bmp_wordBei);
			Movie_Show_Img(80,48,bmp_worddong);
			}
			break;
		case 5:  //zhengbei
		//	Movie_Show_Img(0,16,bmpfangxian90);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"E");
		else	
			{
			Movie_Show_Img(48,48,bmp_worddong);
			}
			break;
		case 6: //zhengdong
		//	Movie_Show_Img(0,16,bmpfangxian112);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"ESE");
		else	
			{
		
			Movie_Show_Img(48,48,bmp_worddong);
			Movie_Show_Img(64,48,bmp_wordnan);
			Movie_Show_Img(80,48,bmp_worddong);
			}
			break;
		case 7: //zhengnan
		//	Movie_Show_Img(0,16,bmpfangxian135);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"ES");
		else	
			{		
			Movie_Show_Img(48,48,bmp_worddong);
			Movie_Show_Img(64,48,bmp_wordnan);
			}

			break;
		case 8:
		//	Movie_Show_Img(0,16,bmpfangxian157);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"SSE");
		else	
			{
		
			Movie_Show_Img(48,48,bmp_wordnan);
			Movie_Show_Img(64,48,bmp_wordnan);
			Movie_Show_Img(80,48,bmp_worddong);
			}
			break;
		case 9:  //zhengbei
		//	Movie_Show_Img(0,16,bmpfangxian180);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"S");
		else	
			{		
			Movie_Show_Img(48,48,bmp_wordnan);
			}
			break;
		case 10: //zhengdong
		//	Movie_Show_Img(0,16,bmpfangxian202);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"SSW");
		else	
			{
		
			Movie_Show_Img(48,48,bmp_wordnan);
			Movie_Show_Img(64,48,bmp_wordnan);
			Movie_Show_Img(80,48,bmp_wordXi);
			}
			break;
		case 11: //zhengnan
		//	Movie_Show_Img(0,16,bmpfangxian225);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"WS");
		else	
			{		
			Movie_Show_Img(48,48,bmp_wordXi);
			Movie_Show_Img(64,48,bmp_wordnan);
			}

			break;
		case 12:
		//	Movie_Show_Img(0,16,bmpfangxian247);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"WSW");
		else	
			{		
			Movie_Show_Img(48,48,bmp_wordXi);
			Movie_Show_Img(64,48,bmp_wordnan);
			Movie_Show_Img(80,48,bmp_wordXi);
			}
			break;
		case 13:  //zhengbei
		//	Movie_Show_Img(0,16,bmpfangxian270);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"W");
		else	
			{		
			Movie_Show_Img(48,48,bmp_wordXi);
			}
			break;
		case 14: //zhengdong
		//	Movie_Show_Img(0,16,bmpfangxian292);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"WNW");
		else	
			{		
			Movie_Show_Img(48,48,bmp_wordXi);
			Movie_Show_Img(64,48,bmp_wordBei);
			Movie_Show_Img(80,48,bmp_wordXi);
			}
			break;
		case 15: //zhengnan
		//	Movie_Show_Img(0,16,bmpfangxian314);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"WN");
		else	
			{
		
			Movie_Show_Img(48,48,bmp_wordXi);
			Movie_Show_Img(64,48,bmp_wordBei);
			}
			break;
		case 16:
		//	Movie_Show_Img(0,16,bmpfangxian336);
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,48,"NNW");
		else	
			{
		
			Movie_Show_Img(48,48,bmp_wordBei);
			Movie_Show_Img(64,48,bmp_wordBei);
			Movie_Show_Img(80,48,bmp_wordXi);
			}
			break;			
		}	
	tp = 360 -tp;
	switch(anayes_angle(tp))
		{
		case 1:  //zhengbei
			Movie_Show_Img(0,16,fangxianbmp);
	//		Movie_Show_Img(48,48,bmp_wordBei);
			break;
		case 2: //zhengdong
			Movie_Show_Img(0,16,fangxianbmp22);
			//Movie_Show_Img(48,48,bmp_wordBei);
			//Movie_Show_Img(64,48,bmp_wordBei);
			//Movie_Show_Img(80,48,bmp_worddong);
			break;
		case 3: //zhengnan
			Movie_Show_Img(0,16,fangxianbmp45);
			//Movie_Show_Img(48,48,bmp_worddong);
			//Movie_Show_Img(64,48,bmp_wordBei);


			break;
		case 4:
			Movie_Show_Img(0,16,fangxianbmp67);
			//Movie_Show_Img(48,48,bmp_worddong);
			//Movie_Show_Img(64,48,bmp_wordBei);
			//
			//Movie_Show_Img(80,48,bmp_worddong);

			break;
		case 5:  //zhengbei
			Movie_Show_Img(0,16,bmpfangxian90);
			//Movie_Show_Img(48,48,bmp_worddong);
			break;
		case 6: //zhengdong
			Movie_Show_Img(0,16,bmpfangxian112);
			//Movie_Show_Img(48,48,bmp_worddong);
			//Movie_Show_Img(64,48,bmp_wordnan);
			//Movie_Show_Img(80,48,bmp_worddong);

			break;
		case 7: //zhengnan
			Movie_Show_Img(0,16,bmpfangxian135);
			//Movie_Show_Img(48,48,bmp_worddong);
			//Movie_Show_Img(64,48,bmp_wordnan);


			break;
		case 8:
			Movie_Show_Img(0,16,bmpfangxian157);
			//Movie_Show_Img(48,48,bmp_wordnan);
			//Movie_Show_Img(64,48,bmp_wordnan);
			//Movie_Show_Img(80,48,bmp_worddong);

			break;
		case 9:  //zhengbei
			Movie_Show_Img(0,16,bmpfangxian180);
			//Movie_Show_Img(48,48,bmp_wordnan);
			break;
		case 10: //zhengdong
			Movie_Show_Img(0,16,bmpfangxian202);
			//Movie_Show_Img(48,48,bmp_wordnan);
			//Movie_Show_Img(64,48,bmp_wordnan);
			//Movie_Show_Img(80,48,bmp_wordXi);

			break;
		case 11: //zhengnan
			Movie_Show_Img(0,16,bmpfangxian225);
			//Movie_Show_Img(48,48,bmp_wordXi);
			//Movie_Show_Img(64,48,bmp_wordnan);


			break;
		case 12:
			Movie_Show_Img(0,16,bmpfangxian247);
			//Movie_Show_Img(48,48,bmp_wordXi);
			//Movie_Show_Img(64,48,bmp_wordnan);
			//Movie_Show_Img(80,48,bmp_wordXi);
			break;
		case 13:  //zhengbei
			Movie_Show_Img(0,16,bmpfangxian270);
			//Movie_Show_Img(48,48,bmp_wordXi);
			break;
		case 14: //zhengdong
			Movie_Show_Img(0,16,bmpfangxian292);
			//Movie_Show_Img(48,48,bmp_wordXi);
			//Movie_Show_Img(64,48,bmp_wordBei);
			//Movie_Show_Img(80,48,bmp_wordXi);

			break;
		case 15: //zhengnan
			Movie_Show_Img(0,16,bmpfangxian314);
			//Movie_Show_Img(48,48,bmp_wordXi);
			//Movie_Show_Img(64,48,bmp_wordBei);

			break;
		case 16:
			Movie_Show_Img(0,16,bmpfangxian336);
			//Movie_Show_Img(48,48,bmp_wordBei);
			//Movie_Show_Img(64,48,bmp_wordBei);
			//Movie_Show_Img(80,48,bmp_wordXi);

			break;			
		}

//	OLED_Refresh_Gram();
	
}


extern const unsigned char Bmp_24hour_downline[];
extern const unsigned char Bmp_speed_word[];
extern const unsigned char Bmp_temp_word[];
extern const unsigned char Bmp_attide_word[];
extern const unsigned char Bmp_pree_word[];
extern const unsigned char Bmp_4hour_downline[];
extern const unsigned char Bmp_change_upline[];

 

void menu_temp_change_display(void)
{
	//extern const u8 Temp_change[];
	u8 i;
	signed char y;
	static signed char old_y1;
	float tp,tp_most,tp_lest,temperature_F;	
	int temp = 0;
	OLED_Fill(0, 0, 127, 63, 0);

	if(system_flag_table.lang == 1)
		OLED_ShowString(30,0,"TEMP");
	else	
		{
		Movie_Show_Img(30, 0,Bmp_temp_word);
		}
	Movie_Show_Img(30, 17,Bmp_change_upline);
	Movie_Show_Img(24, 56,Bmp_24hour_downline);	
	
	if(enter_perss == 1)
		{
			Movie_Show_Img(0,0,Bmp_updwon);				
		}
	else
		{
		if(keyTabIndex == menu_change_index)
			Movie_Show_Img(0,0,Bmp_ponit);				
		}


             if(old_div_cnt == 0)
             	{
			most_temperature = temperature;
			lest_temperature	 = temperature;				
             	}	
	if(system_flag_table.myunit.TEMP == 0)
		{
//		conversion(temperature);
//(81-what_is_numbe_10(temperature)*8)
		temperature_F = temperature;
		sprintf((char *)dtbuf,"%.1f C",temperature_F/10);	//得到经度字符串
		OLED_ShowString((65),0,dtbuf);
		Movie_Show_Img((80+what_is_numbe_10(temperature_F/10)*8),0,bmpdu);
		
	//	conversion(most_temperature);
//		sprintf((char *)dtbuf,"%c%c.%c",bai,shi,ge);	//most
		sprintf((char *)dtbuf,"%d",temperature/10);	//得到经度字符串
		//	most = bai*100+shi*10+ge;
		OLED_ShowString12(0,16,dtbuf);
	//	conversion(lest_temperature);
	//	sprintf((char *)dtbuf,"%c%c.%c",bai,shi,ge);	//lest
		sprintf((char *)dtbuf,"%d",temperature/10);	//得到经度字符串
		OLED_ShowString12(0,46,dtbuf);
	
		}
	else
		{
		temperature_F = 320+(1.8*temperature);
	//	conversion(temperature_F);
	#if 0
		if(temperature_F>=1000)
			{
			sprintf((char *)dtbuf,"%.1f F",temperature_F);	//得到经度字符串
			OLED_ShowString(65,0,dtbuf);	
			Movie_Show_Img(104,0,bmpdu);
			}
		else
			{
			sprintf((char *)dtbuf,"%.1f F",temperature_F);	//得到经度字符串
			OLED_ShowString(65,0,dtbuf);
			Movie_Show_Img(96,0,bmpdu);
			}
	#endif	 	
		sprintf((char *)dtbuf,"%.1f F",temperature_F/10);	//得到经度字符串
		OLED_ShowString((65),0,dtbuf);
		Movie_Show_Img((80+what_is_numbe_10(temperature_F/10)*8),0,bmpdu);
			 	
		temp = (int)(320+(1.8*most_temperature));
		#if 0
		conversion(temperature_F);
		if(temperature_F>=1000)
			{
			sprintf((char *)dtbuf,"%c%c%c",qian,bai,shi);	//得到经度字符串
			}
		else
			{
			sprintf((char *)dtbuf,"%c%c.%c ",bai,shi,ge);	//得到经度字符串
			}
		OLED_ShowString12(0,16,dtbuf);	

		temperature_F = 320+(1.8*lest_temperature);
		conversion(temperature_F);
		if(temperature_F>=1000)
			{
			sprintf((char *)dtbuf,"%c%c%c",qian,bai,shi);	//得到经度字符串
			}
		else
			{
			sprintf((char *)dtbuf,"%c%c.%c ",bai,shi,ge);	//得到经度字符串
			}
		OLED_ShowString12(0,46,dtbuf);	
		#endif
		sprintf((char *)dtbuf,"%d",temp/10);	//得到经度字符串
		OLED_ShowString12(0,16,dtbuf);		
		temp = (int)(320+(1.8*lest_temperature));
		sprintf((char *)dtbuf,"%d",temp/10);	//得到经度字符串
		OLED_ShowString12(0,46,dtbuf);
		}

//	lest = bai*100+shi*10+ge;
       i = 95;

//	OLED_Fill(32, 17, 125, 54, 0);	
		while(i != (95-old_div_cnt)  && (old_div_cnt != 0xff))
		{

			tp_most = most_temperature;
			tp_lest =  lest_temperature;
			tp =temperature_pass[i] ;
			
			tp = (tp- ((tp_most+tp_lest)/2))*40/(tp_most-tp_lest);		
			
			y = (signed char)(tp);	
		       if(y==20)
			   	y =19;
		//	printf("\r temperature_pass[%d]= %d i= %d , y = %d, tp = %.1f",i,temperature_pass[i],i,y,tp);
		
	              if(35-y >56)
				  y = -21;
	              if(35-y <17)
				  y = 18;

			OLED_DrawPoint(i+30,35-y,1);	
#if 1
			if((i!=95)&&(old_div_cnt>0))
				{
				if(old_y1<=y)
					OLED_DrawVLine(i+31,35-y,35-old_y1);				
				else if(old_y1>y)
					OLED_DrawVLine(i+31,35-old_y1,35-y);				
			
				}
#endif			
	   	    old_y1 = y;
                   i--;
		}


           if(old_div_cnt>=95)
           	{
            for(i=0;i<95;i++)
            		{
            		if(temperature_pass[i] == most_temperature)
						break;
            		}
		OLED_DrawVLine_visual(i+31,17,56);			
           	}


//	OLED_Refresh_Gram();
}


void menu_Pressure_display(void)
{
	//extern const u8 Pressure_change[];
	u8 i;
	signed char y;
	static signed char old_y1;
	float tp,tp_most,tp_lest,pressure_unit;	

	OLED_Fill(0, 0, 127, 63, 0);
//	Movie_Show_Img(26, 0,Pressure_change);
	if(system_flag_table.lang == 1)
		OLED_ShowString(22,0,"PRES");
	else	
		{
		Movie_Show_Img(22, 0,Bmp_pree_word);
		}
	Movie_Show_Img(30, 17,Bmp_change_upline);
	Movie_Show_Img(24, 56,Bmp_24hour_downline);
	if(enter_perss == 1)
		{
			Movie_Show_Img(0,0,Bmp_updwon);				
		}
	else
		{
		if(keyTabIndex == menu_change_index)
			Movie_Show_Img(0,0,Bmp_ponit);				
		}

	
	conversion(pressure);


	   if(old_div_cnt == 0)
             	{
             	most_pressure = pressure;
		lest_pressure	 = pressure;				
             	}
	if(system_flag_table.myunit.PREE == 0)
		{
//		conversion(pressure);
		pressure_unit = pressure;
		sprintf((char *)dtbuf,"%.1fhpa",pressure_unit/100);	//得到经度字符串
		OLED_ShowString(57,0,dtbuf);
	//	conversion(most_pressure);
		sprintf((char *)dtbuf,"%d",(int)(most_pressure/100));	//得到经度字符串
		OLED_ShowString12(0,16,dtbuf);
	//	conversion(lest_pressure);
		sprintf((char *)dtbuf,"%d",(int)(lest_pressure/100));	//得到经度字符串
		OLED_ShowString12(0,46,dtbuf);	
		}
	else if(system_flag_table.myunit.PREE == 1)
		{
	//	conversion(pressure);
	pressure_unit = pressure;
		sprintf((char *)dtbuf,"%.1fmba",pressure_unit/100);	//得到经度字符串
		OLED_ShowString(57,0,dtbuf);
		//conversion(most_pressure);
		sprintf((char *)dtbuf,"%d",(int)(most_pressure/100));	//得到经度字符串�
		OLED_ShowString12(0,16,dtbuf);
	//	conversion(lest_pressure);
		sprintf((char *)dtbuf,"%d",(int)(lest_pressure/100));	//得到经度字符串
		OLED_ShowString12(0,46,dtbuf);	
		}
	else if(system_flag_table.myunit.PREE == 2)
		{
			pressure_unit = pressure/1.333;
			//conversion(pressure_unit);
			sprintf((char *)dtbuf,"%.1fmmhg",pressure_unit/100);	//得到经度字符串
			OLED_ShowString(57,0,dtbuf);
			pressure_unit = most_pressure/1.333;
			//		conversion(pressure_unit);
			sprintf((char *)dtbuf,"%d",(int)(pressure_unit/100));	//得到经度字符串
			OLED_ShowString12(0,16,dtbuf);
			pressure_unit = lest_pressure/1.333;
			conversion(pressure_unit);
			sprintf((char *)dtbuf,"%d",(int)(pressure_unit/100));	//得到经度字符串
			OLED_ShowString12(0,46,dtbuf);		      
		}
	else if(system_flag_table.myunit.PREE == 3)
		{
			pressure_unit = pressure/33.864;
			sprintf((char *)dtbuf,"%.2fmmhg",pressure_unit/100);	//得到经度字符串	
			OLED_ShowString(57,0,dtbuf);
			pressure_unit = most_pressure/33.864;
			sprintf((char *)dtbuf,"%d",(int)(pressure_unit/100));	//得到经度字符串	
			OLED_ShowString12(0,16,dtbuf);
			pressure_unit = lest_pressure/33.864;
			sprintf((char *)dtbuf,"%d",(int)(pressure_unit/100));	//得到经度字符串
			OLED_ShowString12(0,46,dtbuf);	
		}
	      



        


	if(old_div_cnt == 0xff)	
		return;
	
       i = 95;
	//   	OLED_Fill(32, 17, 125, 54, 0);	
	while(i != (95-old_div_cnt))
		{
		tp_most = most_pressure;
		tp_lest =  lest_pressure;
		tp = pressure_pass[i];
		tp = (tp - ((tp_most+tp_lest)/2))*40/(tp_most-tp_lest);

			y = (signed char)(tp);		
		// 	y = (signed char)((pressure_pass[i] -(most_pressure + lest_pressure)/2)*15/(most_pressure-lest_pressure));			
		   if(y==20)
		   	y =19;
		   
	//	printf("\r pressure_pass[%d]= %d i= %d y = %d %.1f",i,pressure_pass[i],i,y,tp);
	              if(35-y >56)
				  y = -21;
	              if(35-y <17)
				  y = 18;	
		OLED_DrawPoint(i+30,35-y,1);			
#if 1		
		if((i!=95)&&(old_div_cnt>0))
			{
			if(old_y1<=y)
				OLED_DrawVLine(i+31,35-y,35-old_y1);				
			else if(old_y1>y)
				OLED_DrawVLine(i+31,35-old_y1,35-y);				
		
			}
#endif				
		 	
		   old_y1 = y;
                  i--;
		}
	
	           if(old_div_cnt>=95)
           	{
            for(i=0;i<95;i++)
            		{
            		if(pressure_pass[i] == most_pressure)
						break;
            		}
		OLED_DrawVLine_visual(i+31,17,56);			
           	}
//	OLED_Refresh_Gram();
}

void menu_arruite_change_display(void)
{
//	const unsigned char Bmp_pree_word[];
	u8 i=0;
	float tp,tp_most,tp_lest;	
	signed char y;
	static signed char old_y1;
	OLED_Fill(0, 0, 127, 63, 0);

	if(system_flag_table.lang == 1)
		OLED_ShowString(30,0,"ALTI");
	else	
		{
		Movie_Show_Img(30, 0,Bmp_attide_word);
		}
	Movie_Show_Img(30, 17,Bmp_change_upline);
	Movie_Show_Img(27, 56,Bmp_4hour_downline);
	if(enter_perss == 1)
		{
			Movie_Show_Img(0,0,Bmp_updwon);				
		}
	else
		{
		if(keyTabIndex == menu_change_index)
			Movie_Show_Img(0,0,Bmp_ponit);				
		}


	if(old_altiude_div_cnt == 0)
		{
		most_Altitude = Altitude;
		lest_Altitude = Altitude;
		}
		
      if(system_flag_table.myunit.H_W == 0)
      		{
			sprintf((char *)&dtbuf,"%ldm",Altitude/100);	//得到经度字符串
			OLED_ShowString(65,0,dtbuf);				

			sprintf((char *)dtbuf,"%ld",most_Altitude/100);		    		//得到速度字符串	 
			OLED_ShowString12((24-what_is_numbe_10(most_Altitude/100)*6),16,dtbuf);

			sprintf((char *)dtbuf,"%ld",lest_Altitude/100);		    		//得到速度字符串	 
			OLED_ShowString12((24-what_is_numbe_10(lest_Altitude/100)*6),46,dtbuf);	
		//	printf("\r\nlest_Altitude= %ld",lest_Altitude);	
      		}
     else  if(system_flag_table.myunit.H_W >= 1)
      		{
//      		      if((Altitude/100/0.3045)<10000)
//				sprintf((char *)&dtbuf,"%.1fft",(Altitude/100/0.3048));	//得到经度字符串
   //                    else		  
				sprintf((char *)&dtbuf,"%.0fft",(Altitude/100/0.3048));	//得到经度字符串					   	
			OLED_ShowString(65,0,dtbuf);							

			sprintf((char *)dtbuf,"%.0f",(most_Altitude/30.48));		    		//得到速度字符串	 
			OLED_ShowString12((24-(what_is_numbe_10(most_Altitude/30.48)*6)),16,dtbuf);

			sprintf((char *)dtbuf,"%.0f",lest_Altitude/30.48);		    		//得到速度字符串	 
			OLED_ShowString12((24-(what_is_numbe_10(lest_Altitude/30.48)*6)),46,dtbuf);	

      		}

        i = 95;
	//OLED_Fill(32, 17, 125, 54, 0);	
	while((i != (95-old_altiude_div_cnt))  && (old_altiude_div_cnt != 0xff))
		{
		// if(speed_pass[i])
		 	{
		 	//h = (most_pressure + lest_pressure)/2;
			//y = pressure_pass[i]-h;	
			tp_most = most_Altitude;
			//tp_most /= 100;
			tp_lest =  lest_Altitude;
			//tp_lest /=100;


				tp = (Altitude_pass[i] - (tp_most+tp_lest)/2)*40/(tp_most-tp_lest);
			
		 	y = (signed char)(tp);			
		       if(y==20)
			   	y =19;

	//	printf("\r\n speed_pass[i]= %d i= %d , y = %d",speed_pass[i],i,y);
	              if(37-y >56)
				  y = -19;
	              if(37-y <17)
				  y = 20;
				  
			OLED_DrawPoint(i+30,37-y,1);	
			if((i!=95)&&(old_altiude_div_cnt>0))
				{
				if(old_y1<=y)
					OLED_DrawVLine(i+31,37-y,37-old_y1);				
				else if(old_y1>y)
					OLED_DrawVLine(i+31,37-old_y1,37-y);								
				}
			old_y1 = y;			
				
			}
		 i --;
	}

           if(old_altiude_div_cnt>=95)
           	{
            for(i=0;i<95;i++)
            		{
            		if(Altitude_pass[i] == most_Altitude)
						break;
            		}
		OLED_DrawVLine_visual(i+31,17,56);			
           	}

	 
}




void menu_speed_change_display(void)
{
	float tp,tp_most,tp_lest;		
//	extern const u8 bmp_speed[];
	u8 i;
	signed char y;
	static signed char old_y1;
	OLED_Fill(0, 0, 127, 63, 0);

	if(system_flag_table.lang == 1)
		OLED_ShowString(30,0,"SPD");
	else	
		{	
		Movie_Show_Img(24, 0,Bmp_speed_word);
		}
	Movie_Show_Img(30, 17,Bmp_change_upline);
	Movie_Show_Img(27, 56,Bmp_4hour_downline);
	tp=gpsx.speed;	   


	if(enter_perss == 1)
		{
			Movie_Show_Img(0,0,Bmp_updwon);				
		}
	else
		{
		if(keyTabIndex == menu_change_index)
			Movie_Show_Img(0,0,Bmp_ponit);				
		}

	if(old_spped_div_cnt == 0)
		{
		most_speed = tp;
		lest_speed = tp;
		}


	if(system_flag_table.myunit.H_W == 0)
		{
			sprintf((char *)dtbuf,"%dkm/h",(u32)tp/1000);		    		//得到速度字符串	 
			OLED_ShowString(70,0,dtbuf);	 
			tp=most_speed;	   
			sprintf((char *)dtbuf,"%.1f",tp);		    		//得到速度字符串	 
			OLED_ShowString12(0,16,dtbuf);
			tp=lest_speed;	   
			sprintf((char *)dtbuf,"%.1f",tp);		    		//得到速度字符串	 
			OLED_ShowString12(0,46,dtbuf);	
		}
	else if(system_flag_table.myunit.H_W == 1)
		{
			sprintf((char *)dtbuf,"%dmi/h",(u32)tp/1609);		    		//得到速度字符串	 
			OLED_ShowString(70,0,dtbuf);	 
			tp=most_speed;	   
			sprintf((char *)dtbuf,"%.1f",tp/1.609);		    		//得到速度字符串	 
			OLED_ShowString12(0,16,dtbuf);
			tp=lest_speed;	   
			sprintf((char *)dtbuf,"%.1f",tp/1.609);		    		//得到速度字符串	 
			OLED_ShowString12(0,46,dtbuf);	

		}
	else if(system_flag_table.myunit.H_W == 2)
		{
			sprintf((char *)dtbuf,"%dkt/h",(u32)tp/1852);		    		//得到速度字符串	 
			OLED_ShowString(70,0,dtbuf);	 
			tp=most_speed;	   
			sprintf((char *)dtbuf,"%.1f",tp/1.852);		    		//得到速度字符串	 
			OLED_ShowString12(0,16,dtbuf);
			tp=lest_speed;	   
			sprintf((char *)dtbuf,"%.1f",tp/1.852);		    		//得到速度字符串	 
			OLED_ShowString12(0,46,dtbuf);	

		}

       i = 95;
	//OLED_Fill(32, 17, 125, 54, 0);	
		while(i != (95-old_spped_div_cnt)  && (old_spped_div_cnt != 0xff))
		{
		// if(speed_pass[i])
		 	{
		 	//h = (most_pressure + lest_pressure)/2;
			//y = pressure_pass[i]-h;	
			tp_most = most_speed;
			tp_lest =  lest_speed;
			tp = (speed_pass[i] - (tp_most+tp_lest)/2)*40/(tp_most-tp_lest);
			
		 	y = (signed char)(tp);			
		       if(y==20)
			   	y =19;

	              if(35-y >56)
				  y = -21;
	              if(35-y <17)
				  y = 18;
	//	printf("\r\n speed_pass[i]= %d i= %d , y = %d",speed_pass[i],i,y);
			OLED_DrawPoint(i+30,35-y,1);	
			if((i!=95)&&(old_spped_div_cnt>0))
				{
				if(old_y1<=y)
					OLED_DrawVLine(i+31,35-y,35-old_y1);				
				else if(old_y1>y)
					OLED_DrawVLine(i+31,35-old_y1,35-y);				
				old_y1 = y;
				}
				
			}
			   i--;

	}
           if(old_spped_div_cnt>=95)
           	{
            for(i=0;i<95;i++)
            		{
            		if(speed_pass[i] == most_speed)
						break;
            		}
		OLED_DrawVLine_visual(i+31,17,56);			
           	}

//	OLED_Refresh_Gram();

}



void menu_show_sl_number(u8 number,u8 index)
{
extern const u8 BMp_0[];
extern const u8 BMp_1[];
extern const u8 BMp_2[];
extern const u8 BMp_3[];
extern const u8 BMp_4[];
extern const u8 BMp_5[];
extern const u8 BMp_6[];
extern const u8 BMp_7[];
extern const u8 BMp_8[];
extern const u8 BMp_9[];
const u8 *sl_number[]={BMp_0,BMp_1,BMp_2,BMp_3,BMp_4,BMp_5,BMp_6,BMp_7,BMp_8,BMp_9};

//	Movie_Show_Img(16,32,BMp_0);
//	Movie_Show_Img(16,38,BMp_1);
//	printf("bmp_0");
if(number<=9)
	{
	Movie_Show_Img_3(index*10,55,BMp_0);
	Movie_Show_Img_3(index*10+4,55,sl_number[number]);
	}
else
	{
	if(number>99)
		{
		Movie_Show_Img_3(index*10,55,sl_number[9]);
		Movie_Show_Img_3(index*10+4,55,sl_number[9]);
		}
	else
		{
		Movie_Show_Img_3(index*10,55,sl_number[number/10]);
		Movie_Show_Img_3((index*10+4),55,sl_number[number%10]);
		}

	}

}
void menu_3d_seq(void)
{
	nmea_slmsg seq_buff;
	u8 i,j;
#if 0		
       i = gpsx.svnum -1;
	while(i>0&&flag==1)
	{
		flag=0;                  /* 姣忚稛鎺掑簭鍓嶆爣蹇梖lag缃�0 */ 
		for(j=0;j<i;j++) 
	   		if(gpsx.slmsg[j].num>gpsx.slmsg[j+1].num)
	   			{
				seq_buff = gpsx.slmsg[j];	
				gpsx.slmsg[j] = gpsx.slmsg[j+1];
				gpsx.slmsg[j+1] = seq_buff;
					flag=1;              /* 鏍囧織flag缃�1 */ 
				
	   			}
 
			
			
		i--;
	}
#endif
       if(gpsx.svnum >13)
	   	gpsx.svnum = 13;
	for (i = 0; i < gpsx.svnum; i++) {
        for (j = gpsx.svnum - 1; j > i; j--) {
             if(gpsx.slmsg[j].num<gpsx.slmsg[j-1].num) {
                seq_buff = gpsx.slmsg[j-1];
                gpsx.slmsg[j-1] =  gpsx.slmsg[j];
                gpsx.slmsg[j] = seq_buff;
            }
        }
    }
}
void menu_3D_display(void)
{
//	float tp;		
	extern const u8 BMPWaitdinwei[];
	extern const u8 BMP3Ddinwei[];
	extern const u8 BMP2Ddinwei[];
	//extern const u8 BMPweixing[];
	u8 i,j,din_wei_flag;

  
	  
//	Movie_Show_Img(39,0,Bmp_test_status);
//	show_levev_space();
	OLED_Fill(0, 0, 127, 63, 0);
	menu_status_dispplay();
//	sprintf((char *)dtbuf,"%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes);	//显示UTC日期
//	OLED_ShowString_head(0,0,dtbuf);

//	OLED_Fill(0, 16, 127, 63, 0);
	if(gpsx.fixmode == 3)
		{
		if(system_flag_table.lang == 1)
			OLED_ShowString(0,16,"3D Fix");
		else	
			Movie_Show_Img(0, 16,BMP3Ddinwei);
		}
	else if(gpsx.fixmode == 2)
		{
		if(system_flag_table.lang == 1)
			OLED_ShowString(0,16,"2D Fix");
		else	
			Movie_Show_Img(0, 16,BMP2Ddinwei);		
		}
	else
		{
		if(system_flag_table.lang == 1)
			OLED_ShowString(0,16,"Locating Sate.");
		else			
			Movie_Show_Img(0, 16,BMPWaitdinwei);		
		}


        if(gpsx.svnum>13)
		gpsx.svnum =13;
#if 1
	for(i=0;i<gpsx.svnum;i++)
		{
	//	menu_show_sl_number(gpsx.slmsg[i].num,i);
			{						
				menu_show_sl_number(gpsx.slmsg[i].num,i);
				din_wei_flag = 0;
				for(j=0;j<gpsx.posslnum;j++)
					{
						if(gpsx.slmsg[i].num== gpsx.possl[j])
							{
							if(gpsx.fixmode  >=2)
								din_wei_flag = 1;
							break;
							}
					}
				if(((gpsx.slmsg[i].sn*23)/46)>0)
					{
					if(((gpsx.slmsg[i].sn*23)/46)>22)
						OLED_Fill((i*10), 34, (i*10+6), 56, 1);	
					else
						OLED_Fill((i*10), (56-((gpsx.slmsg[i].sn*23)/46)), (i*10+6), 56, 1);	
					}
				if(din_wei_flag)
					{
					}
				else
					{
					if(((gpsx.slmsg[i].sn*23)/46)>2)
						{
						if(((gpsx.slmsg[i].sn*23)/46)>22)
							OLED_Fill((i*10), 35, (i*10+6), 55, 1);	
						else
							OLED_Fill((i*10+1), (57-((gpsx.slmsg[i].sn*23)/46)), (i*10+5), 55, 0);	
						}
					}
		
			}
		}
#endif

	
}



void show_the_fanxiang(float angle)
{

extern const u8 BMPdongbei[];
extern const u8 BMPxibei[];
extern const u8 BMPzhengbei[];
extern const u8 BMPxinan[];
extern const u8 Bmpzhengnan[];
extern const u8 Bmpzhengxi[];
extern const u8 Bmpzhengdong[];
extern const u8 Bmpdongnan[];




u8 temp_angle = 0;

//	temp_angle = (u16)angle;

temp_angle = anayes_angle(angle);
OLED_Fill(0, 16, 15, 63, 0);
OLED_Fill(16, 16, 47, 31, 0);
OLED_Fill(32, 32, 47,63, 0);
OLED_Fill(16, 48, 31,63, 0);

switch(temp_angle)
	{
	case 1:
		Movie_Show_Img(16, 16,BMPzhengbei);
		//printf("\r\n BMPdongbei ");
		break;
	case 2:
//	case 16:
		Movie_Show_Img(16, 16,BMPzhengbei);
		Movie_Show_Img(32 ,16,BMPdongbei);
		break;

     case 3:
//	case 15:
	  	Movie_Show_Img(32 ,16,BMPdongbei);
	  	break;
	case 4:
//	case 14:		
		Movie_Show_Img(32 ,16,BMPdongbei);
		Movie_Show_Img(32, 32,Bmpzhengdong);
		break;
	case 5:
//	case 13:		
		Movie_Show_Img(32, 32,Bmpzhengdong);
	//	Movie_Show_Img(32, 48,Bmpdongnan);
		break;
	case 6:
	//case 12:
		Movie_Show_Img(32, 32,Bmpzhengdong);
		Movie_Show_Img(32, 48,Bmpdongnan);
		break;
	case 7:
	//case 11:
		Movie_Show_Img(32, 48,Bmpdongnan);
		break;
	case 8:
	//case 10:		
		Movie_Show_Img(32, 48,Bmpdongnan);
		Movie_Show_Img(16, 48,Bmpzhengnan);
		break;
	case 9:
		Movie_Show_Img(16, 48,Bmpzhengnan);
		break;
	case 10:
	//case 8:	
		Movie_Show_Img(16, 48,Bmpzhengnan);
		Movie_Show_Img(0, 48,BMPxinan);
		break;
	case 11:
	//case 7:	
		Movie_Show_Img(0, 48,BMPxinan);
		//printf("\r\n BMPdongbei ");
		break;
	case 12:
	//case 6:		
		Movie_Show_Img(0, 48,BMPxinan);
		Movie_Show_Img(0, 32,Bmpzhengxi);
		
		break;

	case 13:
	//se 5:
		Movie_Show_Img(0, 32,Bmpzhengxi);
		break;
	case 14:
	//se 4:
		Movie_Show_Img(0, 32,Bmpzhengxi);
		Movie_Show_Img(0, 16,BMPxibei);
		break;
	case 15:
	//se 3:
		Movie_Show_Img(0, 16,BMPxibei);
		break;
	case 16:
	//case 2:
		Movie_Show_Img(0, 16,BMPxibei);
		Movie_Show_Img(16, 16,BMPzhengbei);
		break;
	}

//	printf("\r\n angle  %.3f temp_angle :%d,",angle,temp_angle);
}

//26.073861,119.286032	

void save_the_interest_place(u32 lat1, char latitude1_flag,u32 lng1, char longitude1_flag,u8 index)
{
u32 flag;
	flag = (latitude1_flag<<8)|longitude1_flag;
	stm_write_eerpom((HOME_WEIDU_ADDER+(3*index)), lat1);
	stm_write_eerpom((HOME_WEIDU_FLAG_ADDER+(3*index)), flag);
	stm_write_eerpom((HOME_JINDU_ADDER+(3*index)), lng1);
	//stm_write_eerpom(HOME_JINDU_FLAG_ADDER+(4*index), longitude1_flag);

}


void read_the_interest_place(u32 *lat1, char *latitude1_flag,u32 *lng1, char *longitude1_flag,u8 index)
{
u32 flag;
	stm_read_eerpom((HOME_WEIDU_ADDER+(3*index)), lat1);
	stm_read_eerpom((HOME_WEIDU_FLAG_ADDER+(3*index)), &flag);
	stm_read_eerpom((HOME_JINDU_ADDER+(3*index)), lng1);
//	stm_read_eerpom(HOME_JINDU_FLAG_ADDER+(4*index), longitude1_flag);

	*latitude1_flag = (char)((flag>>8)&0xff);
	*longitude1_flag = (char)(flag&0xff);
}


const u8 number_input[10]={'0','1','2','3','4','5','6','7','8','9'};
const u8 number_input_DU[10]={' ','1','2','3','4','5','6','7','8','9'};
const u8 char_weidu_input[2]={'N','S'};
const u8 char_jidu_input[2]={'E','W'};
static u8 input_memery[18];
//static u8 input_jidu_memery[9];
static u8 input_index;

void change_latlon_to_index(void)
{
	u32 coxtern1,coxtern2,coxtern3;
	u8 i,temp,const_index = 0;

	  stm_read_eerpom(HOME_NUMBER1_INDE, &coxtern1);
       stm_read_eerpom(HOME_NUMBER2_INDE, &coxtern2);
       stm_read_eerpom(HOME_NUMBER3_INDE, &coxtern3);

	  for(i=0;i<18;i++)
	  	{


	
        if(i == 0)
       	{
       	const_index =  (u8)((coxtern1>>24)&0xff);
		if(const_index == 'N')
			const_index = 0;
		else
			const_index = 1;
	
			
	   	temp = char_weidu_input[i];

       	}
	   else if(i == 9)
	   	{
	   	const_index =   (u8)((coxtern1>>16)&0xff);

		if(const_index == 'E')
			const_index = 0;
		else
			const_index = 1;
		
	   	temp = char_jidu_input[const_index];

	   	}
	 else if((i == 1)||(i == 10))
	 	{
	 	if(i == 1)
	 		const_index= (u8)((coxtern1>>8)&0xff);

	      if(i == 10)
		 	const_index= (u8)((coxtern1)&0xff);
		 
		if(const_index == ' ')
			const_index = 0;
		else
			const_index = const_index-'0';
		

	   	temp = number_input_DU[const_index];	  

	 	}
	  else
	  	{
	  	if(i<9)
		   	const_index = (u8)((coxtern2>>(28-(4*(i-2))))&0xf);	 
		else
			const_index = (u8)((coxtern3>>(28-(4*(i-11))))&0xf);	 
		printf("\r\n coxtern1  %x coxtern2 :%x  coxtern3 :%x ",coxtern1,coxtern2,coxtern3);
	       printf("\r\n const_index  %d  ",const_index);
	   	temp = number_input[const_index];	  
	  	}
	   

	       input_memery[i] = temp;

	  	}
	       input_memery[1] = ' ';
	   
	convert_show_en_input(input_memery,32);	
	convert_show_en_input(&input_memery[9],48);		

		
}
void menu_input_process(void)
{

	u8 temp,const_index = 0,flag_60;
	u32 coxtern1,coxtern2,coxtern3;


//	read_the_interest_place(&lat1,&latitude1_flag,&lng1,&longitude1_flag,save_pos);	
        stm_read_eerpom(HOME_NUMBER1_INDE, &coxtern1);
        stm_read_eerpom(HOME_NUMBER2_INDE, &coxtern2);
        stm_read_eerpom(HOME_NUMBER3_INDE, &coxtern3);

//	printf("\r\n coxtern1  %d coxtern2 :%d  coxtern3 :%d ",coxtern1,coxtern2,coxtern3);
	printf("\r\n coxtern1  %x coxtern2 :%x  coxtern3 :%x ",coxtern1,coxtern2,coxtern3);
      printf("\r\n const_index  %d  ",const_index);
	
	if(input_index>=18)
		input_index = 0;
	
        if(input_index == 0)
       	{
       	const_index =  (u8)((coxtern1>>24)&0xff);
		if(const_index == 'N')
			const_index = 0;
		else
			const_index = 1;
		if((key_type == KEY_UP)||(key_type == KEY_DOWN))
			{
			if(const_index == 1)
				const_index = 0;
			else
				const_index = 1;
			}		
			
	   	temp = char_weidu_input[const_index];
		coxtern1 &=0x00ffffff; 
		coxtern1 |= temp<<24;
		stm_write_eerpom(HOME_NUMBER1_INDE, coxtern1);
       	}
	   else if(input_index == 9)
	   	{
	   	const_index =   (u8)((coxtern1>>16)&0xff);

		if(const_index == 'E')
			const_index = 0;
		else
			const_index = 1;
		if((key_type == KEY_UP)||(key_type == KEY_DOWN))
			{
			if(const_index == 1)
				const_index = 0;
			else
				const_index = 1;
			}			
	   	temp = char_jidu_input[const_index];
		coxtern1 &=0xff00ffff; 
		coxtern1 |= temp<<16;
		stm_write_eerpom(HOME_NUMBER1_INDE, coxtern1);
	   	}
	 else if((input_index == 1)||(input_index == 10))
	 	{
	 	if(input_index == 1)
	 		const_index= (u8)((coxtern1>>8)&0xff);

	      if(input_index == 10)
		 	const_index= (u8)((coxtern1)&0xff);
		 
		if(const_index == ' ')
			const_index = 0;
		else
			const_index = const_index-'0';
		
		if(key_type == KEY_UP)
			{
			if(const_index == 0)
				const_index = 1;
			else
				const_index --;
			}
		else if(key_type == KEY_DOWN)
			{
			if(const_index == 1)
				const_index = 0;
			else
				const_index ++;
			}
	   	temp = number_input_DU[const_index];	  
		if(input_index == 1)
			{
			coxtern1 &=0xffff00ff; 
			coxtern1 |= temp<<8;		
			}
		else
			{
			coxtern1 &=0xffffff00; 
			coxtern1 |= temp;		

			}
		stm_write_eerpom(HOME_NUMBER1_INDE, coxtern1);
	 	}
	  else
	  	{
	  	if(input_index<9)
	  		{
		   	const_index = (u8)((coxtern2>>(28-(4*(input_index-2))))&0xf);	 

			if((input_index==4)||(input_index == 6))
				flag_60 =1;
			else
				flag_60 =0;

	  		}
		else
			{
			if((input_index==13)||(input_index == 15))
				flag_60 =1;
			else
				flag_60 =0;

			const_index = (u8)((coxtern3>>(28-(4*(input_index-11))))&0xf);	 
			}

		if(key_type == KEY_UP)
			{
			if(const_index == 0)
				{
				if(flag_60 == 1)
					const_index = 5;
				else	
					{		
					       if(input_index==2)
						   	const_index = 8;
						 else if(input_index==11)  
						 	const_index = 7;
						 else  	
							const_index = 9;
					}
				}
			else
				const_index --;
			}
		else if(key_type == KEY_DOWN)
			{
			if(flag_60 == 0)
				{
				 if((input_index==2)&&(const_index == 8))
				   	{
				   		const_index = 0;
				 	}
				else if((input_index==11)&(const_index == 7)) 
					{
						const_index = 0;
					}
				else if(const_index == 9)
					{						  						 
						const_index = 0;
					}
				else
						const_index ++;
				}
			else
				{
					if(const_index == 5)
						const_index = 0;
					else
						const_index ++;

				}
			}

		if(input_index<9)
	  		{
                   coxtern2  &= ~(((uint32_t)0x0000000f) << (28-(4*(input_index-2))));

                   //GPIOx->MODER |= (((uint32_t)GPIO_InitStruct->GPIO_Mode) << (pinpos * 2));
			//coxtern2 &=(0xf<<(28-(4*(input_index-2)))); 
			//coxtern2 = ~coxtern2;
			coxtern2 |= const_index<<(28-(4*(input_index-2)));	
			stm_write_eerpom(HOME_NUMBER2_INDE, coxtern2);
	  		}
		else
			{
			//coxtern3 &=(0xf<<(28-(4*(input_index-11)))); 
			//coxtern3 = ~coxtern3;
			coxtern3  &= ~(((uint32_t)0x0000000f) << (28-(4*(input_index-11))));
			coxtern3 |= const_index<<(28-(4*(input_index-11)));			
			stm_write_eerpom(HOME_NUMBER3_INDE, coxtern3);
			}
	   	temp = number_input[const_index];	  
	  	}
	   

	   	input_memery[input_index] = temp;

	   
	convert_show_en_input(input_memery,32);	
	convert_show_en_input(&input_memery[9],48);


	
}

void convert_u32_to_index(float lat1, char latitude1_flag,float lng1, char longitude1_flag)
{
	u8 du,fen,temp_vaule,temp_vaule1;	
      float miao;
	u32 coxtern1 =0 ,coxtern2 = 0,coxtern3 = 0;
	
	coxtern1 |= 	(latitude1_flag<<24)|(longitude1_flag<<16);
	du = (u8)(lat1/1000000);
	fen = (u8)((lat1/1000000 - du)*60);
	miao = ((lat1/1000000 - du)*60 -fen)*600;

	 printf("\r\n convert_u32_to_index  %.1f  ",miao);
	 if(miao >599.0)
	 	{
	 	miao = 0.0;
		fen++;
		if(fen>59)
			du++;
	 	}
	if(du<100)
		{
      		coxtern1 |= ' '<<8;	
		temp_vaule = du/10;
		 coxtern2 |= (temp_vaule<<28);
	//	  printf("\r\n temp_vaule  %d  ",temp_vaule);
		temp_vaule = du -(temp_vaule*10);
	//	 printf("\r\n temp_vaule  %d  ",temp_vaule);
		 coxtern2 |= (temp_vaule<<24);
		}
	else
		{
		temp_vaule = du/100;
      		coxtern1 |= ((temp_vaule+'0')<<8);	
		temp_vaule1 = ((du-temp_vaule*100)/10);
		 coxtern2 |= (temp_vaule1<<28);
		temp_vaule = (du -(temp_vaule*100)-(temp_vaule1*10));
		 coxtern2 |= (temp_vaule<<24);
		}

	temp_vaule = fen/10;
	 coxtern2 |= (temp_vaule<<20);
	temp_vaule = (fen -temp_vaule*10);
	 coxtern2 |= (temp_vaule<<16);
		 
	temp_vaule = (u8)(miao/100);
	//  printf("\r\n temp_vaule  %d  ",temp_vaule);
	 coxtern2 |= (temp_vaule<<12);
	temp_vaule1 = (u8)(miao -temp_vaule*100)/10;
//	  printf("\r\n temp_vaule1  %d  ",temp_vaule1);
	 coxtern2 |= (temp_vaule1<<8);
	temp_vaule1 = (u8)(miao -(temp_vaule*100) - (temp_vaule1*10));
//	  printf("\r\n temp_vaule1  %d  ",temp_vaule1);
	 coxtern2 |= (temp_vaule1<<4);



	du = (u8)(lng1/1000000);
	fen = (u8)((lng1/1000000 - du)*60);
	miao = ((lng1/1000000 - du)*60 -fen)*600;

	printf("\r\n convert_u32_to_index  %d %d %.1f  ",du,fen,miao);
	 if(miao >599.0)
	 	{
	 	miao = 0.0;
		fen++;
		if(fen>59)
			du++;
	 	}	
	if(du<100)
		{
      		coxtern1 |= ' ';	
		temp_vaule = (du/10);
		 coxtern3 |= (temp_vaule<<28);
		temp_vaule = (du -(temp_vaule*10));
		 coxtern3 |= (temp_vaule<<24);
		}
	else
		{
		temp_vaule = du/100;
		coxtern1 |= (temp_vaule+'0');	
	//	printf("\r\n temp_vaule  %d  ",temp_vaule);					
		temp_vaule1 = ((du-temp_vaule*100)/10);
	//	printf("\r\n temp_vaule1  %d  ",temp_vaule1);
		coxtern3 |= (temp_vaule1<<28);
		temp_vaule = (du -(temp_vaule*100)-(temp_vaule1*10));
		coxtern3 |= (temp_vaule<<24);
	//	printf("\r\n temp_vaule  %d  ",temp_vaule);
		}

	temp_vaule = fen/10;
	 coxtern3 |= (temp_vaule<<20);
	temp_vaule = fen -temp_vaule*10;
	 coxtern3 |= (temp_vaule<<16);
		 
	temp_vaule = (u8)(miao/100);
	//	  printf("\r\n temp_vaule  %d  ",temp_vaule);
	 coxtern3 |= temp_vaule<<12;
	temp_vaule1 = (u8)(miao -temp_vaule*100)/10;
	//	  printf("\r\n temp_vaule1  %d  ",temp_vaule1);
	 coxtern3 |= (temp_vaule1<<8);
	temp_vaule1 = (u8)(miao -temp_vaule*100 - temp_vaule1*10);
//		  printf("\r\n temp_vaule1  %d  ",temp_vaule1);
	 coxtern3 |= (temp_vaule1<<4);
	 
	stm_write_eerpom(HOME_NUMBER1_INDE, coxtern1);
	stm_write_eerpom(HOME_NUMBER2_INDE, coxtern2);
	stm_write_eerpom(HOME_NUMBER3_INDE, coxtern3);	
}


//#define TEST_TIME
static u8 home_flag = 0;	
#define CUSTOM_HOME_POST 12

void menu_home_display(void)
{
	float tp;		
	
	extern const u8 bmpyuji[];
	extern const u8 bmp_near_place[];
	extern const u8 bmp_choose_post[];
	extern const u8 bmp_choosed[];
	//extern const u8 bmp_destination_icon[];
	extern const u8 bmp_destination_updown[];
	extern const u8 Bmp_input_jiweidu[];
	extern const u8 Bmp_lock[];
	float tp_long,tp_lati,tp_long_dest,tp_lati_dest;	
      float distance  = 0.0,angle = 0.0;
	u32 lat1, lng1;
	char latitude1_flag,longitude1_flag;
	u16 timer_need;
//       u8 i;
	
	tp_long = gpsx.longitude;
	tp_long/=1000000;	 
	tp_lati = gpsx.latitude;
	tp_lati/=1000000;
	
//	Movie_Show_Img(16, 32,bmphome);

//	Movie_Show_Img(39, 0,Bmp_test_status);
//	show_levev_space();
//	 sprintf((char *)dtbuf,"%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes);	//显示UTC日期
//	OLED_ShowString_head(0,0,dtbuf);

	//convert_show_en(gpsx.longitude,gpsx.ewhemi,16);
	//convert_show_en(gpsx.latitude,gpsx.nshemi,32);
	OLED_Fill(0, 0, 127, 63, 0);
	menu_status_dispplay();
	if(home_menu_item_index+menu_item_index >=16)
		{
		home_menu_item_index= 0;
		menu_item_index = 0;
		}
	if(home_menu_item_index+menu_item_index <0)
		{
		home_menu_item_index = 15;
		menu_item_index = 0;
		}

		read_the_interest_place(&lat1,&latitude1_flag,&lng1,&longitude1_flag,home_menu_item_index+menu_item_index);
	if(enter_perss == 0)
		{
         

//		if(home_menu_item_index!=0)
//	      sprintf((char *)dtbuf,"%02d",home_menu_item_index+menu_item_index);		    		//得到速度字符串	
//		OLED_ShowString(16,32,dtbuf);
	//	Movie_Show_Img(16, 32,bmp_destination_icon);
		//OLED_Fill(0, 16, 127, 63, 0);
		Movie_Show_Img_force1616(16,32,home_menu_item_index+menu_item_index);
		if((latitude1_flag == 'N')||(latitude1_flag == 'S'))
		{
		tp_long_dest = lng1;
		tp_long_dest/=1000000;	 
		tp_lati_dest = lat1;
		tp_lati_dest/=1000000;
		
//		Movie_Show_Img(48, 16,bmpjuli);
		

	#if 0
	printf("\r\n lat1  %f",tp_lati_dest);      
	printf("\r\n latitude1_flag  %c",latitude1_flag);      
	printf("\r\n lng1  %f",tp_long_dest);      
	printf("\r\n longitude1_flag  %c",longitude1_flag);      


	printf("\r\n tp_lati  %f",tp_lati);      
	printf("\r\n gpsx.nshemi  %c",gpsx.nshemi);      
	printf("\r\n tp_long  %f",tp_long);      
	printf("\r\n gpsx.ewhemi  %c",gpsx.ewhemi);      
	#endif
	//distance = getDistanceVer1( tp_lati, tp_long, tp_lati_dest,  tp_long_dest);
	distance = getDistanceVer2( tp_lati, gpsx.nshemi, tp_long, gpsx.ewhemi, tp_lati_dest,  latitude1_flag, tp_long_dest,  longitude1_flag);
	//angle = get_angle( tp_lati_dest,  latitude1_flag, tp_long_dest,  longitude1_flag,tp_lati, gpsx.nshemi, tp_long, gpsx.ewhemi);
	angle = get_angle(tp_lati, gpsx.nshemi, tp_long, gpsx.ewhemi, tp_lati_dest,  latitude1_flag, tp_long_dest,  longitude1_flag);
	////angle = GetAngle( tp_lati_dest,  latitude1_flag, tp_long_dest,  longitude1_flag,tp_lati, gpsx.nshemi, tp_long, gpsx.ewhemi);
	tp=gpsx.angle;
	tp /=1000;
	angle = angle - tp;
	if(angle<0)
		angle= angle+360;
	


	}
		else
			return;

#ifndef TEST_TIME
	if(gpsx.fixmode <= 1)
		{
			if(system_flag_table.lang == 1)
				OLED_ShowString(16,16,"Locating Sate.");
			else
				Movie_Show_Img(32,16,Bmp_waitsinge);
		
		return;
		}
#endif	
	//printf("\r\n distance  %.1fkm",distance);      
	if(distance <=0.01)
		{
		if(system_flag_table.lang == 1)
			OLED_ShowString(48,16,"Arriving.");
		else	
			Movie_Show_Img(48,16,bmp_near_place);
		}
	else
		{

		if(system_flag_table.myunit.H_W == 0)
			{
			if(distance>=10 && distance<100)
				sprintf((char *)dtbuf,"%.2fkm",distance);		    		//得到速度字符串	
			else if (distance<10)
				sprintf((char *)dtbuf,"%.3fkm",distance);		    		//得到速度字符串	
			else
				sprintf((char *)dtbuf,"%ldkm",(u32)(distance));		    		//得到速度字符串
			}
		else if(system_flag_table.myunit.H_W == 1)
			{
			if((distance/1.609)>=10 && (distance/1.609)<100)
				sprintf((char *)dtbuf,"%.2fmi",distance/1.609);		    		//得到速度字符串	
			else if ((distance/1.609)<10)
				sprintf((char *)dtbuf,"%.3fmi",distance/1.609);		    		//得到速度字符串	
			else
				sprintf((char *)dtbuf,"%ldmi",(u32)(distance/1.609));		    		//得到速度字符串
			}
#if 1		
		else if(system_flag_table.myunit.H_W == 2)
			{
			if((distance/1.852)>=10 && (distance/1.852)<100)
				sprintf((char *)dtbuf,"%.2fkt",distance/1.852);		    		//得到速度字符串	
			else if ((distance/1.852)<10)
				sprintf((char *)dtbuf,"%.3fkt",distance/1.852);		    		//得到速度字符串	
			else
				sprintf((char *)dtbuf,"%ldkt",(u32)(distance/1.852));		    		//得到速度字符串			
	 	
			}
#endif
		OLED_ShowString(48,16,dtbuf);
		show_the_fanxiang(angle);					
		}
  
	 tp=gpsx.speed;	   
	 if(tp >1500000.0)
	 	tp =9999.0;
	 

 	if(tp>=3000)
		Movie_Show_Img(110,16,Bmp_lock);
	else
		OLED_Fill(110, 16, 127, 31, 0);	

	if(system_flag_table.myunit.H_W == 0)//||(system_flag_table.myunit.H_W == 1))
		{
		if(tp/1000>=100.0)
			sprintf((char *)dtbuf,"%.0fkm/h",tp/=1000);		    		//得到速度字符串	 
		else
		 	sprintf((char *)dtbuf,"%.1fkm/h",tp/=1000);		    		//得到速度字符串	 
		}
#if 1
	else if(system_flag_table.myunit.H_W == 1)
		{
		if(tp/1609>=100.0)
			sprintf((char *)dtbuf,"%.0fmi/h",tp/=1609);		    		//得到速度字符串	 
		else
		 	sprintf((char *)dtbuf,"%.1fmi/h",tp/=1609);		    		//得到速度字符串	 
		}
#endif
	else if(system_flag_table.myunit.H_W == 2)
		{
		if(tp/1852>=100.0)
			sprintf((char *)dtbuf,"%.0fkt/h",tp/=1852);		    		//得到速度字符串	 
		else
		 	sprintf((char *)dtbuf,"%.1fkt/h",tp/=1852);		    		//得到速度字符串	 	 
		}
		
	OLED_ShowString(48,32,dtbuf);	 

#ifdef TEST_TIME
	distance = 3.45;
	tp = 13112.1;
#endif	
	timer_need = (u16)((distance/tp)*60);
 //     tp = distance/(tp/1000);
  //    timer_need = (tp - floor(tp))*60;
	//printf("\r\n timer_need  %d ",timer_need);
	if((tp == 0.0)||(timer_need>=6000))
		{
			//tp = 99;
	    	timer_need  = 5999;	
		}
      sprintf((char *)dtbuf,"%02d:%02d",timer_need/60,timer_need%60);		    		//得到速度字符串	
//	printf("\r\n timer_need1  %d %d  ",timer_need/60,timer_need%60);

	OLED_ShowString(80,48,dtbuf);
	if(system_flag_table.lang == 1)
		OLED_ShowString(48,48,"ETE.");
	else	
		Movie_Show_Img(48, 48,bmpyuji);
	}
	else if(enter_perss >0)
		{	
	      //sprintf((char *)dtbuf,"%02d",home_menu_item_index+menu_item_index);		    		//得到速度字符串	
           //  if(enter_perss ==2) 
	//		OLED_Fill(0, 16, 127, 63, 0);
	//	if(gpsx.fixmode > 1)
			{
			if((latitude1_flag == 'N')||(latitude1_flag == 'S'))
				{
				convert_show_en(lng1,longitude1_flag,48,0);
				convert_show_en(lat1,latitude1_flag,32,0);
				tp_long_dest = lng1;
				tp_long_dest/=1000000;	 
				tp_lati_dest = lat1;
				tp_lati_dest/=1000000;		
				distance = getDistanceVer2( tp_lati, gpsx.nshemi, tp_long, gpsx.ewhemi, tp_lati_dest,  latitude1_flag, tp_long_dest,  longitude1_flag);			
				}
			else if(home_flag == 0)
				{
				if(home_flag != 3)	 
					Movie_Show_Img(0,24,bmp_destination_updown);

				Movie_Show_Img_force1616(0,32,home_menu_item_index+menu_item_index);
				return ;
				}
			}


      		switch(home_flag )
	      	{
	      	case 0:
			OLED_Fill(48, 16, 127, 31, 0);	
			
			if(distance <0.001)
				{
                   		 if(system_flag_table.lang == 1)
					OLED_ShowString(21,16,"Arriving.");
				else
					Movie_Show_Img(48,16,bmp_near_place);
				}
			else		
				{
				if(system_flag_table.myunit.H_W == 0)
					{
					if(distance>=10 && distance<100)
						sprintf((char *)dtbuf,"%.2fkm",distance);		    		//得到速度字符串	
					else if (distance<10)
						sprintf((char *)dtbuf,"%.3fkm",distance);		    		//得到速度字符串	
					else
						sprintf((char *)dtbuf,"%ldkm",(u32)(distance));		    		//得到速度字符串
					}
				else if(system_flag_table.myunit.H_W == 1)
					{
					if((distance/1.609)>=10 && (distance/1.609)<100)
						sprintf((char *)dtbuf,"%.2fmi",distance/1.609);		    		//得到速度字符串	
					else if ((distance/1.609)<10)
						sprintf((char *)dtbuf,"%.3fmi",distance/1.609);		    		//得到速度字符串	
					else
						sprintf((char *)dtbuf,"%ldmi",(u32)(distance/1.609));		    		//得到速度字符串
					}
				#if 1
				else if(system_flag_table.myunit.H_W == 2)
					{
					if((distance/1.852)>=10 && (distance/1.852)<100)
						sprintf((char *)dtbuf,"%.2fkt",distance/1.852);		    		//得到速度字符串	
					else if ((distance/1.852)<10)
						sprintf((char *)dtbuf,"%.3fkt",distance/1.852);		    		//得到速度字符串	
					else
						sprintf((char *)dtbuf,"%ldkt",(u32)(distance/1.852));		    		//得到速度字符串			
			 	
					}
#endif
                       
                         if(gpsx.fixmode >1)  // GPS定位到后才显示内容
					OLED_ShowString(48,16,dtbuf);	
				}
			break;
	      	case 1:
	      	//	 home_flag = 1; // 进入选择存储兴趣点模式
			//home_flag = 3;	// 进入输入模式	
                    if(system_flag_table.lang == 1)
				OLED_ShowString(21,16,"Choose Symbol");
			else			
				 Movie_Show_Img(22,16,bmp_choose_post);
	
			break;
	      	case 2:
			OLED_Fill(0, 16, 127, 63, 0);
		      	if((home_menu_item_index+menu_item_index >= CUSTOM_HOME_POST )&&((key_type == KEY_ENTER)||(key_type == KEY_LONG_ENTER)))
		      		{
		      		if((latitude1_flag != 'N')&&(latitude1_flag != 'S'))
		      			{
		      			latitude1_flag = 'N';
		      			longitude1_flag = 'E';
		      			}
				//printf("\r\n lat1  %d",lat1);  
				//printf("\r\n lng1  %d",lng1);  
				convert_u32_to_index(lat1,latitude1_flag,lng1,longitude1_flag);
                    if(system_flag_table.lang == 1)
				OLED_ShowString(16,16,"Enter Location");
			else		
				Movie_Show_Img(22,16,Bmp_input_jiweidu);			 
				//menu_input_process();
			//	convert_show_en(lng1,longitude1_flag,48);
			//	convert_show_en(lat1,latitude1_flag,32);
				sprintf((char *)dtbuf,"_");
				OLED_ShowString(24,32,dtbuf);
				change_latlon_to_index();
				input_index = 0;
				home_flag = 3;
	      			}		
			else
				{
				
				convert_show_en(gpsx.longitude,gpsx.ewhemi,48,0);
				convert_show_en(gpsx.latitude,gpsx.nshemi,32,0);
				home_menu_item_index =home_menu_item_index+menu_item_index;
				menu_item_index = 0;
				save_the_interest_place(gpsx.latitude, gpsx.nshemi, gpsx.longitude, gpsx.ewhemi,home_menu_item_index);			
				//printf("\r\n home_menu_item_index  %d",home_menu_item_index);   			  	
				if(system_flag_table.lang == 1)
					OLED_ShowString(21,16,"POI Marked");
				else		
					Movie_Show_Img(22,16,bmp_choosed);
				home_flag = 0;
				enter_perss =3;
				}
			break;

		case 3:
				//OLED_Fill(48, 16, 127, 32, 0);	
			       if(system_flag_table.lang == 1)
					OLED_ShowString(21,16,"Enter Location");
				else		
					Movie_Show_Img(22,16,Bmp_input_jiweidu);	
				if(key_type == KEY_LONG_ENTER)
					{
					OLED_Fill(0, 16, 127, 63, 0);
					if(input_memery[10] ==' ')	
						lng1 = (1000000*((input_memery[11]-'0')*10+(input_memery[12]-'0')));
					else
						lng1 = (1000000*((input_memery[10]-'0')*100+(input_memery[11]-'0')*10+(input_memery[12]-'0')));	


                                tp = 10*((input_memery[13]-'0')*10+(input_memery[14]-'0'));
					tp /=6;		
					//printf("\r\n feng   %.5fd",tp);  
					lng1 += tp*10000;											
					tp = 1000*((input_memery[15]-'0')*100+(input_memery[16]-'0')*10+(input_memery[17]-'0'));
					//printf("\r\n maio number  %.1f",tp);  
					tp /=36;
					//printf("\r\n maio   %.1fd",tp);  
					lng1 += tp;
					//printf("\r\n lng1 number  %d",lng1);  

					
					longitude1_flag= input_memery[9];
				//	printf("\r\n llongitude1_flag  %c",longitude1_flag);  

					if(input_memery[1] ==' ')						
						lat1 = (1000000*((input_memery[2]-'0')*10+(input_memery[3]-'0')));
					else
						lat1 = (1000000*((input_memery[1]-'0')*100+(input_memery[2]-'0')*10+(input_memery[3]-'0')));


                                tp = 10*((input_memery[4]-'0')*10+(input_memery[5]-'0'));
					tp /=6;			
					//printf("\r\n fen %.5fd",tp);
					lat1 += tp*10000;		
					tp = 1000*((input_memery[6]-'0')*100+(input_memery[7]-'0')*10+(input_memery[8]-'0'));
					tp /=36;
					//printf("\r\n maio  %.1fd",tp);
					lat1 += tp;
					//printf("\r\n lat1 number  %d",lat1);  
					latitude1_flag = input_memery[0];
					//printf("\r\n latitude1_flag  %c",latitude1_flag); 
					
					convert_show_en(lng1,longitude1_flag,48,0);
					convert_show_en(lat1,latitude1_flag,32,0);
					home_menu_item_index =home_menu_item_index+menu_item_index;
					menu_item_index = 0;
					save_the_interest_place(lat1,latitude1_flag,lng1,longitude1_flag,home_menu_item_index);	
					//printf("\r\n home_menu_item_index  %d",home_menu_item_index);   			  	
               		     if(system_flag_table.lang == 1)
						OLED_ShowString(21,16,"POI Marked");
					else		
						Movie_Show_Img(22,16,bmp_choosed);
					home_flag = 0;
					enter_perss =3;
					return;
					}
				if(key_type == KEY_ENTER)
				{
				sprintf((char *)dtbuf,"%c",input_memery[input_index]);
				if(input_index<9)
					OLED_ShowString(input_jinweidu_post[input_index],32,dtbuf);
				else
					OLED_ShowString(input_jinweidu_post[input_index],48,dtbuf);
				input_index++;
				if(input_index == 1)
					input_index  = 2;
				sprintf((char *)dtbuf,"_");
				if(input_index<9)
					OLED_ShowString(input_jinweidu_post[input_index],32,dtbuf);
				else
					OLED_ShowString(input_jinweidu_post[input_index],48,dtbuf);				
				}				
				menu_input_process();
			break;

	      	}

		if(home_flag != 3)	 
			Movie_Show_Img(0,24,bmp_destination_updown);
		
		Movie_Show_Img_force1616(0,32,home_menu_item_index+menu_item_index);
              

	}
	//angle = get_angle( tp_lati_dest,  latitude1_flag, tp_long_dest,  longitude1_flag,tp_lati, gpsx.nshemi, tp_long, gpsx.ewhemi);

		

}




tm my_timer;

int test_sunriset(u16 year,u8 month,u8 day,float lon,float lat) 
{ 
//	int year = 2009,month = 7,day = 3; 
//	float lon = 120.6553,lat = 28.06; 
	float sunrise,sunset; 
	float altit = -35.0/60.0,upper_limb = 1.0; 
 	//extern __align(4) u8 dtbuf[];   	
		
//	cout &lt;&lt; sunrise &lt;&lt; endl; 
 
	float zone = time_zone; 
 
	int hour,minute; 


	sunriset( year, month, day, lon, lat,  
		altit, upper_limb, &sunrise, &sunset );//核心算法，计算出日出日落时间保存在sunrise和sunset里 


	hour = sunrise+zone; 
	minute = (sunrise - hour+zone)*60; 
	//cout &lt;&lt; hour &lt;&lt; " " &lt;&lt; minute &lt;&lt; endl; 
//	 printf("\r\n sunrise  %d:%d",hour,minute);
      //24,16
      if(gpsx.fixmode!=3)
      		sprintf((char *)dtbuf,"--:--");	//显示UTC日期
	  else
      		sprintf((char *)dtbuf,"%02d:%02d",hour,minute);	//显示UTC日期
	OLED_ShowString(22,16,dtbuf);		

//    sprintf((char *)dtbuf,"%d:%d",hour,minute);	//显示UTC日期
//	OLED_ShowString(24,16,dtbuf);		

	hour = sunset+zone; 
	minute = (sunset - hour+zone)*60; 
	//cout &lt;&lt; hour &lt;&lt; " " &lt;&lt; minute &lt;&lt; endl; 
//	 printf("\r\n sunset  %d:%d",hour,minute);
      if(gpsx.fixmode!=3)
      		sprintf((char *)dtbuf,"--:--");	//显示UTC日期
	  else
	      	sprintf((char *)dtbuf,"%02d:%02d",hour,minute);	//显示UTC日期
	OLED_ShowString(83,16,dtbuf);		

	return 0; 
}



void menu_chaoxi_display(void)
{

	extern const unsigned char bmpchaoxi[];
	float tp_long,tp_lati;		   
//	u16 gpsx_utc,rtc_timer;	
	//gpsx_utc = (gpsx.utc.hour*60+gpsx.utc.min);
	//rtc_timer = (RTC_TimeStructure.RTC_Hours*60+RTC_TimeStructure.RTC_Minutes);

	tp_long = gpsx.longitude;
	tp_long/=1000000;	 
	tp_lati = gpsx.latitude;
	tp_lati/=1000000;

//	time_zone = (rtc_timer-gpsx_utc);
//	time_zone =	time_zone /60;
	 //printf("\r\n time_zone  %f ",time_zone);
	OLED_Fill(0, 0, 127, 63, 0);
	menu_status_dispplay();
	Movie_Show_Img(0, 16,bmpchaoxi);	
//	Movie_Show_Img(39, 0,Bmp_test_status);
//	show_levev_space();
//	sprintf((char *)dtbuf,"%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes);	//显示UTC日期
//	OLED_ShowString_head(0,0,dtbuf);	

	if(enter_perss == 0)
		{
		my_timer.w_year = RTC_DateStructure.RTC_Year+2000;
		my_timer.w_month =RTC_DateStructure.RTC_Month;
		my_timer.w_date = RTC_DateStructure.RTC_Date;
		my_timer.week = RTC_DateStructure.RTC_WeekDay;
		Movie_Show_Img(112, 48,Bmp_ponit);	
		}
	else
		{
		Movie_Show_Img(112, 48,Bmp_updwon);	
		my_timer.week = RTC_Get_Week(my_timer.w_year ,my_timer.w_month,my_timer.w_date);
		}

	sprintf((char *)dtbuf,"%02d-%02d-%02d",my_timer.w_year ,my_timer.w_month,my_timer.w_date);	//显示UTC日期
	OLED_ShowString(0,48,dtbuf);		    
	// printf("\r\n week  %d",my_timer.week-1);
	test_sunriset(my_timer.w_year ,my_timer.w_month,my_timer.w_date,tp_long,tp_lati);
	moon_test(tp_long,tp_lati,my_timer.w_year ,my_timer.w_month,my_timer.w_date,time_zone);
      	sprintf((char *)dtbuf,"%c%c%c",week_word[my_timer.week][0],week_word[my_timer.week][1],week_word[my_timer.week][2]);
	OLED_ShowString(86,48,dtbuf);		

}


 
void guji_Distance(void)
{

	
	 float tp_long,tp_lati;
	 float tp_distance = 0;
	 float tp =0.0;
	 static float tp_distacn_temp = 0;
	
//	stm_write_eerpom(GUJI_STRAT_WEIDU , gpsx.latitude);
//	stm_write_eerpom(GUJI_STRAT_WEIDU_FLAG, flag);
  //     stm_write_eerpom(GUJI_STRAT_JINDU, gpsx.longitude);
       
if((gpsx.fixmode ==3)&&(gpsx.latitude >0)&&(gpsx.longitude>0))
{
		tp_long = gpsx.longitude;
		tp_long/=1000000;	 
		tp_lati = gpsx.latitude;
		tp_lati/=1000000;	
		
	if(guji_mode == RECORED_START_DOING)
		{
     		if(Message_head_number >0)
 	    	{	
		if((guji_tp_lati_dest >0)&&(guji_latitude1_flag>0))
			tp_distance = getDistanceVer2( tp_lati, gpsx.nshemi, tp_long, gpsx.ewhemi, guji_tp_lati_dest,  guji_latitude1_flag, guji_tp_long_dest,  guji_longitude1_flag);
		else
			{			
			guji_tp_long_dest = tp_long;
			guji_tp_lati_dest = tp_lati;
			guji_latitude1_flag= gpsx.nshemi;
			guji_longitude1_flag = gpsx.ewhemi;			
     			}
		
		}
    		if(tp_distance>4)
    			{
			guji_tp_long_dest = tp_long;
			guji_tp_lati_dest = tp_lati;
			guji_latitude1_flag= gpsx.nshemi;
			guji_longitude1_flag = gpsx.ewhemi;	    			
			return;
    			}
				
				
	   switch(guji_icon_index)
	   	{
	   	case 0:
			tp = 0.005;
			break;
	   	case 1:
			tp = 0.010;
			break;
	   	case 2:
			tp = 0.020;
			break;
	   	case 3:
			tp = 0.050;
			break;
	   	case 4:
			tp = 0.100;
			break;
	   	case 5:
				if(system_flag_table.guji_record.by_distance_time_flag == 0)
					{

						
						if(system_flag_table.myunit.H_W >= 1)
							{
							tp =system_flag_table.guji_record.by_distance_vaule_ft;
							tp /=1000;
							tp *=0.3048;
							}
						else
							{
							tp =system_flag_table.guji_record.by_distance_vaule;
							tp /=1000;
							}
					}
				else
					{
					if((tatol_time-tatol_oled_time)>=system_flag_table.guji_record.by_time_vaule)
						{
						if(tp_distance>0.005)
							{
							guiji_distance += tp_distance;
							//	printf("\n\r guiji_distance : %.5f ",guiji_distance); 
							guji_tp_long_dest = tp_long;
							guji_tp_lati_dest = tp_lati;
							guji_latitude1_flag= gpsx.nshemi;
							guji_longitude1_flag = gpsx.ewhemi;
							}



						if(system_flag_table.myunit.H_W == 0)
							{
							if(gpsx.speed/1000<system_flag_table.guji_record.by_speed_vaule)
							         return;
							}
						else if(system_flag_table.myunit.H_W == 1)
							{
							if(gpsx.speed<(system_flag_table.guji_record.by_speed_vaule_1*1609))
							         return;
							}
						else if(system_flag_table.myunit.H_W == 2)
							{
							if(gpsx.speed<(system_flag_table.guji_record.by_speed_vaule_2*1852))
							         return;
							}

						tatol_oled_time = tatol_time;
						recored_run_flag = 1;
						}
					return;
					}
			break;

	   	}
//	  if(tp_distance>=0.005)
//			guiji_distance += tp_distance;
         if(tp_distance>0.005)
             	{
				guiji_distance += tp_distance;
			//	printf("\n\r guiji_distance : %.5f ",guiji_distance); 
				guji_tp_long_dest = tp_long;
				guji_tp_lati_dest = tp_lati;
				guji_latitude1_flag= gpsx.nshemi;
				guji_longitude1_flag = gpsx.ewhemi;
				//if(recored_run_flag == 0)
				tp_distacn_temp +=tp_distance;
				if((tp_distacn_temp>tp))
					{
					if(guji_icon_index == 5)
						{
						if(system_flag_table.myunit.H_W == 0)
							{
							if(gpsx.speed/1000<system_flag_table.guji_record.by_speed_vaule)
							         return;
							}
						else if(system_flag_table.myunit.H_W == 1)
							{
							if(gpsx.speed<(system_flag_table.guji_record.by_speed_vaule_1*1609))
							         return;
							}
						else if(system_flag_table.myunit.H_W == 2)
							{
							if(gpsx.speed<(system_flag_table.guji_record.by_speed_vaule_2*1852))
							         return;
							}
						
						}
					recored_run_flag = 1;
					tp_distacn_temp = 0;

					
					}
             	}
		}

}
}

void save_guiji_message(u8 guji_record_type)
{
	u8 one_shot_buffer[32];
	u16 pressure16 = pressure/10;
	u8 index = 0;
	
	//u32 lng1,lat1;
//	u32 flag;
	
//	one_shot_buffer[index++] = (u8)(Message_head_number>>24)&0xff;  // 1mb
	one_shot_buffer[index++] = (u8)(Message_head_number>>16)&0xff;  // 2mb
	one_shot_buffer[index++] = (u8)(Message_head_number>>8)&0xff;  // 3mb
	one_shot_buffer[index++] = (u8)(Message_head_number)&0xff;  // 4 mb

	one_shot_buffer[index++]  = guji_record_type;  // 5mb

	one_shot_buffer[index++]  = (gpsx.utc.year -2000);  // 6mb
	one_shot_buffer[index++]  = gpsx.utc.month;  // 7mb
	one_shot_buffer[index++]  = gpsx.utc.date;  // 8mb

	one_shot_buffer[index++]  = gpsx.utc.hour;  // 9mb
	one_shot_buffer[index++]  = gpsx.utc.min;  // 10mb
	one_shot_buffer[index++]  = gpsx.utc.sec;  // 11mb

	one_shot_buffer[index++]  = (u8)((gpsx.latitude>>24)&0xff);  // 12mb
	one_shot_buffer[index++]  = (u8)((gpsx.latitude>>16)&0xff);  // 13mb
	one_shot_buffer[index++]  = (u8)((gpsx.latitude>>8)&0xff);  // 14mb
	one_shot_buffer[index++] =  (u8)((gpsx.latitude)&0xff);  // 15mb



	one_shot_buffer[index++]  = (u8)((gpsx.longitude>>24)&0xff);  // 16mb
	one_shot_buffer[index++]  = (u8)((gpsx.longitude>>16)&0xff);  // 17mb
	one_shot_buffer[index++]  = (u8)((gpsx.longitude>>8)&0xff);  // 18mb
	one_shot_buffer[index++]  = (u8)((gpsx.longitude)&0xff);  // 19mb

	one_shot_buffer[index++]  = (u8)(Altitude>>24)&0xff;  // 20mb
	one_shot_buffer[index++] = (u8)(Altitude>>16)&0xff;  // 21mb
	one_shot_buffer[index++]  = (u8)(Altitude>>8)&0xff;  // 22mb
	one_shot_buffer[index++]  = (u8)(Altitude)&0xff;  // 23mb

	one_shot_buffer[index++]  = (u8)(((gpsx.speed/100)>>8)&0xff);  // 24mb
	one_shot_buffer[index++]  = (u8)(((gpsx.speed/100))&0xff);  // 25mb

	one_shot_buffer[index++]  = (u8)((gpsx.angle>>8)&0xff);  // 26mb
	one_shot_buffer[index++]  = (u8)((gpsx.angle)&0xff);  // 27mb



	one_shot_buffer[index++]  = (u8)(temperature>>8)&0xff;  // 28mb
	one_shot_buffer[index++]  = (u8)(temperature)&0xff;  // 29mb


//	one_shot_buffer[index++]  = (u8)(pressure>>24)&0xff;  // 30mb
//	one_shot_buffer[index++]  = (u8)(pressure>>16)&0xff;  // 31mb
	one_shot_buffer[index++]  = (u8)(pressure16>>8)&0xff;  // 32mb
	one_shot_buffer[index++]  = (u8)(pressure16)&0xff;  // 33mb


	one_shot_buffer[index++] = (u8)(gpsx.nshemi)&0xff;  // 15mb
	one_shot_buffer[index++] = (u8)(gpsx.ewhemi)&0xff;  // 15mb


//	Flash_write_buffer_Index++;
	memcpy(&USART2_TX_BUF[Flash_write_buffer_Index],one_shot_buffer,32);

      Flash_write_buffer_Index  += 32;
	


	// stm_write_eerpom(GUJI_STRAT_WEIDU , gpsx.latitude);
	 //stm_write_eerpom(GUJI_STRAT_WEIDU_FLAG, flag);
       // stm_write_eerpom(GUJI_STRAT_JINDU, gpsx.longitude);
	Message_head_number++;		

}

void write_flash(void)
{
	if(Flash_write_buffer_Index)
		{

		if(Flash_Index >= (MOST_FLASH_ADDRES-Flash_write_buffer_Index))
			{
			if(system_flag_table.guji_record.recoed_meth == 0)
				{
				Flash_Index = 0;
				}
			else
				guji_mode = RECORED_STOP;

			return;
			}

		SPI_Flash_Write(USART2_TX_BUF,Flash_Index, Flash_write_buffer_Index);	
		Flash_Index += Flash_write_buffer_Index;
		 stm_write_eerpom(CUURENT_FLASH_ADDRER ,Flash_Index);
		Flash_write_buffer_Index = 0;
		}
}

void  show_levev_space(void)
{
	float tp;
	u8 baifenbi;
	extern const unsigned char Bmp_100[];
//	if((KeyTab[keyTabIndex].MenuID != MENU_TRANSFORMATION)&&(KeyTab[keyTabIndex].ParMenuID != MENU_TRANSFORMATION))
		{
		tp = Flash_Index;
		tp = (1-(tp/MOST_FLASH_ADDRES))*100;
		//	printf("\n\r Recording_guji  save_place :%f",tp);
		baifenbi = (u8)tp;
		//printf("\n\r Recording_guji  save_place :%d",baifenbi);
		if(baifenbi == 100)
			{
			Movie_Show_Img(101,0,Bmp_100);
			}
		else
			{
			sprintf((char *)dtbuf,"%02d",baifenbi);		    		//得到速度字符串	
			OLED_ShowString(103,0,dtbuf);
			OLED_ShowChar(119, 0, '%', 16, 1);
			}

		}
}
void Recording_guji(void)
{
	u32 interst_pos_number;
	extern const unsigned char Bmp_waitsinge[];
	
	switch(guji_mode )
		{
		case RECORED_IDLE:

			break;
		case RECORED_START:			
			Message_head_number = 0;
			if((gpsx.fixmode ==3)&&(gpsx.latitude >0)&&(gpsx.longitude>0))
				{
			//	stm_write_eerpom(GUJI_STRAT_WEIDU , gpsx.latitude);
			//	stm_write_eerpom(GUJI_STRAT_WEIDU_FLAG, (gpsx.nshemi<<8)|gpsx.ewhemi);
			//	stm_write_eerpom(GUJI_STRAT_JINDU, gpsx.longitude);

				guji_tp_long_dest = gpsx.longitude;
				guji_tp_lati_dest = gpsx.latitude;
				guji_tp_long_dest /= 1000000;
				guji_tp_lati_dest /= 1000000;
				guji_latitude1_flag = 	gpsx.nshemi;
				guji_longitude1_flag = gpsx.ewhemi;
				Flash_write_buffer_Index = 0;
				save_guiji_message('S');
				//Flash_Index = 0;
				//tatol_time += 1;		
				tatol_oled_time = tatol_time;
				guji_mode = RECORED_START_DOING;
				interst_pos_number = 0;
				//	sprintf((char *)dtbuf,"%d",interst_pos_number);		    		//得到速度字符串	
				//	OLED_ShowString(17,32,dtbuf);			
				stm_write_eerpom(GUJI_INTERST_POS,interst_pos_number);
				}
			else
				{
				     if((keyTabIndex == MENU_SETTING_COOR)&&(system_power_flag == 1))
				     	{
					OLED_Fill(32, 16, 127, 32, 0);
					if(system_flag_table.lang == 1)
						OLED_ShowString(16,16,"Locating Sate.");
					else					
						Movie_Show_Img(32,16,Bmp_waitsinge);
				     	}
					recored_run_flag = 1;
				}

			break;
		case RECORED_START_DOING:
			if((gpsx.fixmode ==3)&&(gpsx.latitude >0)&&(gpsx.longitude>0))
				save_guiji_message('T');			
			else
				printf("\n\r gpsx.fixmode  %d  gpsx.latitude: %d  gpsx.longitude %d ",gpsx.fixmode,gpsx.latitude,gpsx.longitude); 	
			break;
		case RECORED_T:
			save_guiji_message('A');
			stm_read_eerpom(GUJI_INTERST_POS ,&interst_pos_number);
			//if(interst_pos_number>100)
			//	interst_pos_number = 0;
			interst_pos_number++;
		//	sprintf((char *)dtbuf,"%02d",interst_pos_number);		    		//得到速度字符串	
		//	OLED_ShowString(18,32,dtbuf);			
			stm_write_eerpom(GUJI_INTERST_POS,interst_pos_number);
			guji_mode = RECORED_START_DOING;
			break;
		case RECORED_STOP:
			save_guiji_message('P');
			guji_mode = RECORED_IDLE;
			write_flash();
                   stm_write_eerpom(CUURENT_FLASH_ADDRER ,Flash_Index);
			//if(Flash_write_buffer_Index<4096)
				{			
			//	SPI_Flash_Write(Flash_write_buffer,Flash_Index, Flash_write_buffer_Index);			
			//	Flash_Index += Flash_write_buffer_Index;
			//	Flash_write_buffer_Index = 0;
			//	return;
				}
			break;

		}
#if 0
		if(Flash_write_buffer_Index == (4096-(Flash_Index%4096)))
			{			
			SPI_Flash_Write(Flash_write_buffer,Flash_Index, Flash_write_buffer_Index);
			Flash_Index += Flash_write_buffer_Index;
			memset(Flash_write_buffer,0x00,4096);
			Flash_write_buffer_Index = 0;
			}
#endif


			
}





void menu_show_gujIcon(u8 icon)
{
	extern const unsigned char Bmp_gujiiconsetp[];
	extern const unsigned char Bmp_gujiiconrun[];
	extern const unsigned char Bmp_gujiiconB[];
	extern const unsigned char Bmp_gujiiconcar[];
	extern const unsigned char Bmp_gujiiconfly[];
      extern const unsigned char Bmp_gujiiconCustom[];
	  
switch(icon)
	{
	case 0:
		Movie_Show_Img(0,16,Bmp_gujiiconsetp);
		break;
	case 1:
		Movie_Show_Img(0,16,Bmp_gujiiconrun);		
		break;
	case 2:
		Movie_Show_Img(0,16,Bmp_gujiiconB);
		break;
	case 3:
	      Movie_Show_Img(0,16,Bmp_gujiiconcar);
		break;
	case 4:
		Movie_Show_Img(0,16,Bmp_gujiiconfly);
		break;
	case 5:
		Movie_Show_Img(0,16,Bmp_gujiiconCustom);
		break;

	}

//MENU_GUJI_MODE_INDEX_ADDRES
}


void menu_show_gujiword(u8 icon)
{

	extern const unsigned char Bmp_wordbuxing[];
	extern const unsigned char Bmp_wordrun[];
	extern const unsigned char Bmp_wordB[];
	extern const unsigned char Bmp_wordCar[];
	extern const unsigned char Bmp_feixing[];
      extern const unsigned char Bmp_wordcustom[];
	  
switch(icon)
	{
	case 0:
             if(system_flag_table.lang == 1)
			 OLED_ShowString(32,16,"Walking");	
		else	 	
			Movie_Show_Img(32,16,Bmp_wordbuxing);
		break;
	case 1:
             if(system_flag_table.lang == 1)
			 OLED_ShowString(32,16,"Running");	
		else	 	
			Movie_Show_Img(32,16,Bmp_wordrun);		
		break;
	case 2:
             if(system_flag_table.lang == 1)
			 OLED_ShowString(32,16,"Cycling");	
		else	 	
			Movie_Show_Img(32,16,Bmp_wordB);
		break;
	case 3:
             if(system_flag_table.lang == 1)
			 OLED_ShowString(32,16,"By car");	
		else	 	
		      Movie_Show_Img(32,16,Bmp_wordCar);
		break;
	case 4:
             if(system_flag_table.lang == 1)
			 OLED_ShowString(32,16,"By plane");	
		else	 	
			Movie_Show_Img(32,16,Bmp_feixing);
		break;
	case 5:
             if(system_flag_table.lang == 1)
			 OLED_ShowString(32,16,"User Defined");	
		else	 	
			Movie_Show_Img(32,16,Bmp_wordcustom);
		break;

	}
}

#define MAN_PARPAM 1.02
#define WOMAN_PARPAM 0.98
void menu_guiji_display(void)
{

	extern const unsigned char Bmp_leiji[];
	extern const unsigned char Bmp_sept[];
	extern const unsigned char Bmp_sart[];
	extern const unsigned char Bmp_dian[];
	extern const unsigned char Bmp_daka[];


	extern const unsigned char Bmp_savedguji[];
	extern const unsigned char Bmp_gujiBI[];

	extern const unsigned char Bmp_xinqudian[];
	extern const unsigned char Bmp_xingqudiansaved[];

	
	u8 h,m;
	u32 distance;
	u32 tp = 0	;
	u16 diff_weght = 0, diff_hight = 0;
//	OLED_Fill(0, 16, 127, 63, 0);
	//Movie_Show_Img(39, 32,Bmpguiji);
//	sprintf((char *)dtbuf,"%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes);	//显示UTC日期
//	OLED_ShowString_head(0,0,dtbuf);	
	
//	Movie_Show_Img(39, 0,Bmp_test_status);

	OLED_Fill(0, 0, 127, 63, 0);
	menu_status_dispplay();
	Movie_Show_Img(85, 0,Bmp_gujiBI);
//	show_levev_space();


	//tp_long = gpsx.longitude;
	//tp_long/=1000000;	 
	//tp_lati = gpsx.latitude;
	//tp_lati/=1000000;
		
		 if((enter_perss != 1)||(guji_mode != RECORED_IDLE))
		 	{
			//Movie_Show_Img(2,16,Bmpstop);
			//menu_show_gujIcon
		if(system_flag_table.lang == 1)
			OLED_ShowString(0,32,"TOT");
		else				
			Movie_Show_Img(0,32,Bmp_leiji);
			distance	 = (floor)(guiji_distance);
			
				{
			//	sprintf((char *)dtbuf,"%dkm",distance);		    		//得到速度字符串		
				if(system_flag_table.myunit.H_W == 0)
					{
					if(guiji_distance>1.0)
					 	sprintf((char *)dtbuf,"%.2fkm",guiji_distance);		    		//得到速度字符串	 
                                else
					       sprintf((char *)dtbuf,"%dm",((u32)(guiji_distance*1000)));			
					}
				else if(system_flag_table.myunit.H_W == 1)
					{
					if((guiji_distance/1.609)>1.0)
					 	sprintf((char *)dtbuf,"%.2fmi",guiji_distance/1.609);	
					else
						sprintf((char *)dtbuf,"%dft",((u32)(guiji_distance*1000/0.3048)));		    		//得到速度字符串	 
						//得到速度字符串	 
					}

				else if(system_flag_table.myunit.H_W == 2)
					{
					if((guiji_distance/1.852)>1.0)
					 	sprintf((char *)dtbuf,"%.2fkt",guiji_distance/1.852);		    		//得到速度字符串	 				
					else						 	
						sprintf((char *)dtbuf,"%dft",((u32)(guiji_distance*1000/0.3048)));		    		//得到速度字符串	 
					}
				OLED_ShowString(0,48,dtbuf);
				}

		
			h = tatol_time/3600;
			m =(tatol_time - h*3600)/60; 
			sprintf((char *)dtbuf,"%02dh%02dm",h,m);		    		//得到速度字符串	
			OLED_ShowString(80,48,dtbuf);
			if(guji_icon_index == 0)
				{
				if(system_flag_table.myunit.H_W == 0)
					tp =(u32)( (guiji_distance*100000)/(system_flag_table.personal_da.height*0.33));
				else
					tp =(u32)( (guiji_distance*100000)/(system_flag_table.personal_da.height_ft*0.33));
				sprintf((char *)dtbuf,"%d",tp);		    		//得到速度字符串
				OLED_ShowString(72-((what_is_numbe_10(tp)*8)),32,dtbuf);
				if(system_flag_table.lang == 1)
					OLED_ShowString(80,32,"Steps");
				else					
					Movie_Show_Img(80,32,Bmp_sept);
				}
			else
				{
				sprintf((char *)dtbuf,"%d",Message_head_number);		    		//得到速度字符串
				OLED_ShowString(72-((what_is_numbe_10(Message_head_number)*8)),32,dtbuf);
				if(system_flag_table.lang == 1)
					OLED_ShowString(80,32,"Points");
				else					
					Movie_Show_Img(80,32,Bmp_dian);
				}
			if(guji_icon_index<=2)
				{
				if((guji_mode >= RECORED_START)&&(gpsx.fixmode <2))
					{
					if(system_flag_table.lang == 1)
						OLED_ShowString(16,16,"Locating Sate.");
					else
						Movie_Show_Img(32,16,Bmp_waitsinge);
					}
				else
					{
					//tp = guiji_distance;
					if(system_flag_table.myunit.H_W == 0)
						{
						diff_weght = system_flag_table.personal_da.weight;
						}
					else
						{
						diff_weght = system_flag_table.personal_da.weight_ft;
						}
					switch(guji_icon_index)
						{
						case 0:
							if(system_flag_table.personal_da.flame == 0)
								tp = diff_weght*guiji_distance*MAN_PARPAM*0.9;
							else
								tp = diff_weght*guiji_distance*WOMAN_PARPAM*0.9;
							break;
						case 1:
							if(system_flag_table.personal_da.flame == 0)
								tp = diff_weght*guiji_distance*MAN_PARPAM;
							else
								tp = diff_weght*guiji_distance*WOMAN_PARPAM;

							break;
						case 2:
							if(system_flag_table.personal_da.flame == 0)
								tp = diff_weght*guiji_distance*MAN_PARPAM*0.5;
							else
								tp = diff_weght*guiji_distance*WOMAN_PARPAM*0.5;

							break;

						}
//					printf("\n\r daka %d ",tp); 	
					//printf("\n\r system_flag_table.personal_da.weight %d ",system_flag_table.personal_da.weight); 	
					sprintf((char *)dtbuf,"%d",tp);		    		//得到速度字符串
					//OLED_ShowString(56,16,dtbuf);
					OLED_ShowString(72-((what_is_numbe_10(tp)*8)),16,dtbuf);
					if(system_flag_table.lang == 1)
						OLED_ShowString(80,16,"Kcal");
					else						
						Movie_Show_Img(80,16,Bmp_daka);	
					}
				}
			else
				{
				if((guji_mode >= RECORED_START)&&(gpsx.fixmode <2))
					{
					if(system_flag_table.lang == 1)
						OLED_ShowString(16,16,"Locating Sate.");
					else
						Movie_Show_Img(32,16,Bmp_waitsinge);
					}
				else				
					menu_show_gujiword(guji_icon_index);
				}
			
		 	}
		menu_show_gujIcon(guji_icon_index);	
    	  	if(guji_mode == RECORED_IDLE)
    	  		{
		  if(enter_perss == 1)
		 	{
		 	 if(key_type == KEY_UP)
		 	 	{
		 	 	if(guji_icon_index>0)
			 	 	guji_icon_index --;
				else
					guji_icon_index = 5;
				stm_write_eerpom(MENU_GUJI_MODE_INDEX_ADDRES ,guji_icon_index);
				
		 	 	}
			 else if(key_type == KEY_DOWN)
			 	{
			 	if(guji_icon_index<5)
			 	 	guji_icon_index ++;
				else
					guji_icon_index = 0;
				stm_write_eerpom(MENU_GUJI_MODE_INDEX_ADDRES ,guji_icon_index);
			 	}
			 Movie_Show_Img(16,16,Bmp_updwon);
			menu_show_gujiword(guji_icon_index);
		 	}
		 else if(enter_perss == 2)
		 	{		 	
			guiji_distance= 0 ;
			tatol_time = 0;
			guji_mode = RECORED_START;
				OLED_Fill(0, 16, 127, 32, 0);
	             if(system_flag_table.lang == 1)
				 OLED_ShowString(32,16,"Start");	
			else	 					
				Movie_Show_Img(32,16,Bmp_sart);
		//	Recording_guji();
			recored_run_flag = 1;
			enter_perss = 4;
		 	}
			 menu_show_gujIcon(guji_icon_index);	
    	  		}
		else if(guji_mode == RECORED_START_DOING)
			{
		     if(enter_perss == 2)
		 	 {
				guji_mode = RECORED_STOP;				
				Recording_guji();
				OLED_Fill(0, 16, 127, 32, 0);
		             if(system_flag_table.lang == 1)
					 OLED_ShowString(32,16,"Track Saved");	
				else	 				
					Movie_Show_Img(32,16,Bmp_savedguji);
				enter_perss = 4;
				menu_show_gujIcon(guji_icon_index);	


				guiji_distance = 0;
				tatol_time = 0;
				Message_head_number = 0;
				}
			else if(enter_perss == 1)
				{
				guji_mode = RECORED_T;
				OLED_Fill(0, 16, 127, 32, 0);
				Movie_Show_Img(16,16,Bmp_gujiBI);
				Movie_Show_Img(0,16,Bmp_xinqudian);
		             if(system_flag_table.lang == 1)
					 OLED_ShowString(32,16,"POI Marked");	
				else	 	
					Movie_Show_Img(32,16,Bmp_xingqudiansaved);
				Recording_guji();	
				enter_perss = 4;						
				}
			else if(enter_perss == 0)
				{
				if((gpsx.fixmode ==3)&&(gpsx.latitude >0)&&(gpsx.longitude>0))
					{
					Movie_Show_Img(16,16,Bmp_gujiBI);
					}
				else
					{
					OLED_Fill(32, 16, 127, 32, 0);
					if(system_flag_table.lang == 1)
						OLED_ShowString(16,16,"Locating Sate.");
					else
						Movie_Show_Img(32,16,Bmp_waitsinge);
					}
				menu_show_gujIcon(guji_icon_index);	
				}

      		}
		else if(guji_mode == RECORED_START)
				{
				 if(enter_perss == 2)
		 			 {
						guji_mode = RECORED_IDLE;	
						enter_perss = 0;
				     	 }
				}
	//printf("\n\r menu_guiji_display  guji_mode:%d  Flash_Index %d ",guji_mode,Flash_Index); 
}

int16_t Max_arry(int16_t* prt,u8 index)
{
u8 cnt;
int16_t Max = 0; 
if(index>0)
	Max=prt[95]; 
for(cnt=96-index;cnt<96;cnt++) 
{ 
     if(prt[cnt]>=Max) Max=prt[cnt]; 
}
 return Max;
}

int16_t Min_arry(int16_t* prt,u8 index)
{
u8 cnt;
int16_t min = 0; 

if(index>0)
	min=prt[95]; 
for(cnt=96-index;cnt<96;cnt++) 
{ 
     if(prt[cnt]<=min) min=prt[cnt]; 
}
 return min;
}

s32 Max_arry32(s32* prt,u8 index)
{
u8 cnt;
s32 Max = 0;

if(index>0)
	Max=prt[95]; 
for(cnt=96-index;cnt<96;cnt++) 
{ 
     if(prt[cnt]>=Max) Max=prt[cnt]; 
}
 return Max;
}

s32 Min_arry32(s32* prt,u8 index)
{
u8 cnt;
s32 min = 0; 

if(index>0)
	min=prt[95]; 
for(cnt=96-index;cnt<96;cnt++) 
{ 
     if(prt[cnt]<=min) min=prt[cnt]; 
}
 return min;
}
void analytical_press_baoyu(void)
{
	switch(baoyun_flag_cnt)
		{
		case 15:
			#if 0
			    if((pressure_pass[15]>=(pressure_pass[0]-400))||(pressure_pass[15]>=(pressure_pass[1]-400))\
					||(pressure_pass[15]>=(pressure_pass[2]-400))||(pressure_pass[15]>=(pressure_pass[3]-400)))
					baoyun_flag = 1;
					
			break;
			#endif
		case 31:
		case 47:
		case 63:
		case 79:
		case 95:
			    if((pressure_pass[95]>=(pressure_pass[80]+400))||(pressure_pass[95]>=(pressure_pass[81]+400))\
					||(pressure_pass[95]>=(pressure_pass[82]+400))||(pressure_pass[95]>=(pressure_pass[83]+400)))
					baoyun_flag = 1;

			break;

		}
}

void save_tsp_tp(void)
{
// static u8 div_cnt = 0;
 int i;
// static u16 start_timer_cnt = 10;

//	current_timer = (u32)(tm.hour*3600+tm.min*60+tm.sec);
//	diff_time = current_timer - start_timer_cnt;


#ifdef NO_BMP085
	pressure= 0.0;
	temperature = 0.0;
#endif
	//	start_timer_cnt = 0;
//		temp = temperature;
		printf("temp:%d \r\n",temperature); 	    

//		if(old_div_cnt != div_cnt)
			{
			if(old_div_cnt <95)
				old_div_cnt ++;
			else if(old_div_cnt  == 0xff)
				old_div_cnt  = 0;
		//	else
			baoyun_flag_cnt++;
		
				
		
                   for(i= 0;i<95;i++)
                   	{
				temperature_pass[i] = temperature_pass[i+1]; 	   					   
				// printf("\n\r temperature_pass[%d] :%d ",i,temperature_pass[i] ); 		
                   	}
			temperature_pass[95] = temperature;
			most_temperature = Max_arry(temperature_pass,old_div_cnt);
			lest_temperature = Min_arry(temperature_pass,old_div_cnt);

                   for(i= 0;i<95;i++)
				pressure_pass[i] = pressure_pass[i+1]; 	   	
			pressure_pass[95] = pressure;		
			most_pressure= Max_arry32(pressure_pass,old_div_cnt);
			lest_pressure= Min_arry32(pressure_pass,old_div_cnt);
			    
 //                      switch
//			pressure_stat_baoyun= Max_arry32(pressure_pass,old_div_cnt);

			 printf("\n\r old_div_cnt:%d ",old_div_cnt); 	    
			}
		 if(KeyTab[keyTabIndex].MenuID ==MENU_TRANSFORMATION ||  KeyTab[keyTabIndex].MenuID ==MENU_MILEAGE)
		 	OLED_Fill(32, 17, 125, 54, 0);	
		 SPI_Flash_Write((u8*)&temperature_pass, CHANGE_TMPE_VAULE_SAVE_ADDRES, 192);		 
		 SPI_Flash_Write((u8*)&pressure_pass, CHANGE_PRE_VAULE_SAVE_ADDRES, 384);
//		 SPI_Flash_Write(&old_div_cnt, CHANGE_TMPE_VAULE_INDEX_ADDRES, 1);
		 stm_write_eerpom(CHANGE_TMPE_VAULE_INDEX_ADDRES,old_div_cnt); 


		analytical_press_baoyu();

		if(baoyun_flag_cnt == 95)
			baoyun_flag_cnt = 79;
	
}


void save_tsp_s(void)
{
//static u16  old_spped_timer_cnt =300;
//static  u8 div_cnt = 0;
u8 i;
//u8 tsp_S_buffer[384];

	//if(old_spped_timer_cnt==300)
		{
//		old_spped_timer_cnt = 0;
		if(system_flag_table.run_mode == 1)
			{
			if(old_spped_div_cnt <95)
				old_spped_div_cnt ++;
			if(old_spped_div_cnt  == 0xff)
				old_spped_div_cnt  = 0; 	
			}

		if(old_altiude_div_cnt <95)
			old_altiude_div_cnt ++;
		if(old_altiude_div_cnt  == 0xff)
			old_altiude_div_cnt  = 0; 		
		
		for(i= 0;i<95;i++)
			{
			if(system_flag_table.run_mode == 1)
				speed_pass[i] = speed_pass[i+1]; 	 
			Altitude_pass[i] = Altitude_pass[i+1]; 	 
			}

		if(system_flag_table.run_mode == 1)
			{
			speed_pass[95] =(u16)(average_speed /1000);
			average_speed = 0xffff;
			most_speed= Max_arry(speed_pass,old_spped_div_cnt);
			lest_speed = Min_arry(speed_pass,old_spped_div_cnt);
			}
		
		Altitude_pass[95] = Altitude;
		//Altitude = 0xffffffff;
		most_Altitude= Max_arry32(Altitude_pass,old_altiude_div_cnt);
		lest_Altitude = Min_arry32(Altitude_pass,old_altiude_div_cnt);

	  	 printf("\n\r Altitude %d  speed_pass[95] :%d ",Altitude,speed_pass[95] ); 
	  	 printf("\n\r old_altiude_div_cnt %d  old_spped_div_cnt:%d ",old_altiude_div_cnt,old_spped_div_cnt); 
		if(system_flag_table.run_mode == 1)
	  	 {
		 	SPI_Flash_Write((u8*)&speed_pass, CHANGE_SPEED_VAULE_SAVE_ADDRES, 192);
		 	stm_write_eerpom(CHANGE_SPEED_VAULE_INDEX_ADDRES,old_spped_div_cnt);		
	  	 }
	//	 memcpy(tsp_S_buffer,(u8 *)&most_Altitude,384);
		 SPI_Flash_Write((u8*)&Altitude_pass, CHANGE_ALTITUD_VAULE_SAVE_ADDRES, 384);
//		 SPI_Flash_Write(&old_altiude_div_cnt, CHANGE_ALTITUD_VAULE_INDEX_ADDRES, 1);
		 stm_write_eerpom(CHANGE_ALTITUD_VAULE_INDEX_ADDRES,old_altiude_div_cnt);		 


		}
//      else
	//	old_spped_timer_cnt++;	 
}
	


/****设置菜单*****/
#define MENU_SETTING_MAIN_PAGE 0
#define MENU_SETTING_SEC_PAGE 1
#define MENU_SETTING_THR_PAGE 2
#define MENU_SETTING_FOR_PAGE 3

#define PAGE_RUN_MODEL 0
#define PAGE_GUJI_RECORD 1
#define PAGE_GESHI_DANGWEI 2
#define PAGE_GEREN_ZHILIAO 3
#define PAGE_SYSTEM_SET 4
#define PAGE_QUIT 5




typedef enum
{ 
  SENSOR_MODEL = 0,	
  GPS_MODEL,
}RUN_MODEL;

typedef enum
{
  CUSTOM_RECORED,
  RECORED_MODEL,	
  AUTO_MODEL,
  DELECT_ALL,
  CUTOME_RECORED_RETURN,
}CUTOME_RECORED;


typedef enum
{ 
  UNIT_TIME = 0,	
  UNIT_DATE,
  UNIT_COORD,
  UNIT_DISCANCE,
  UNIT_TEMP,
  UNIT_PRES,
  UNIT_REUTNR,
  
}UNIT_MODEL;

typedef enum
{ 
    PERSONAL_FALEM = 0,
    PERSONAL_HEIGHT ,
    PERSONAL_WEIGHT,	
    PERSONAL_REUTRN,
}PERSONAL_data;

typedef enum
{ 
    SYSTEM_ADR_ADJUST = 0,
    SCREEN_STANBY,	
    TIMER_ZONE,
    LANGUAGE,
    POWERDONW_CONTRL,
    SYSTEM_INFO,
    FACTROY,
    SYSTEM_RETURN,
}SYSTEM_SETS;



typedef enum
{ 
    BY_DISTANCE = 0,
    BY_TIMES,	
    BY_SPEED,
    RECORED_MODEL_OPTIONS_RETURN,
}RECORED_CUSTOM_OPTIONS;



typedef enum
{ 
    AUTO_RECOVRY = 0,
    AUTO_STOP,	
    AUTO_MODEL_OPTIONS_RETURN,
}RECORED_MODEL_OPTIONS;

typedef enum
{ 
    AUTO_ON = 0,
    AUTO_OFF,	
}AUTO_MODEL_OPTIONS;


u8 menu_setting_index = 0xff;
u8 menu_setting_page  = PAGE_RUN_MODEL;
u8 menu_setting_item  = SENSOR_MODEL;
long menu_setting_postion  = 0;
long menu_setting_postion_2  = 0;





extern const u8 BMP_CHOOSE1[];
extern const u8 BMP_CHOOSE2[];
extern const u8 BMP_CHOOSE3[];
extern const u8 BMP_SETTING_RETURN[];


const u8 arry_bydistance[5]={5,10,20,50,100};
const u16 arry_bydistance_2[5]={20,50,100,200,500};
const u8 arry_time[5]={1,5,10,30,60};
//static u32 setting_tp;
extern const u8 bmp_autoreocrd_1[];
void menu_display_settingitem_recode(void)
{
extern const u8 BMP_RECORD_HEAD[];
extern const u8 BMP_BYDICTANCE[];
extern const u8 BMP_BYtime[];
extern const u8 BMP_BYspeed[];
extern const u8 bmp_jilufashi[];
extern const u8 bmp_jilufashi_1[];
extern const u8 bmp_CustomRecored[];
extern const u8 bmp_autoreocrd[];
extern const u8 bmp_delect_record_word[];
extern const u8 bmp_guji_remove_recode[];

u32 setting_tp;
u8 i;

    Movie_Show_Img(1,0,BMP_RECORD_HEAD);	
	if(system_flag_table.lang == 1)
	{
		OLED_ShowString(16,0,"Track Log    "); 
	}
if(menu_setting_index == MENU_SETTING_SEC_PAGE)
	{	
	if(menu_setting_item>(DELECT_ALL+1))
		menu_setting_item = 0;	
  if(menu_setting_item<=DELECT_ALL)
  	{

	    if(system_flag_table.lang == 1)
			{
		   	  OLED_ShowString12(16,16,"User Defined"); 
		   	  OLED_ShowString12(16,28,"Auto Overwrite"); 
		   	  OLED_ShowString12(16,40,"Auto Start"); 
		   	  OLED_ShowString12(16,52,"Delete All Tracks"); 

			}	
		else
			{
			Movie_Show_Img(16,16,bmp_CustomRecored);
			Movie_Show_Img(16,29,bmp_jilufashi);
			Movie_Show_Img(16,40,bmp_autoreocrd);	
			Movie_Show_Img(16,53,bmp_delect_record_word);
			}
		Movie_Show_Img(4,(menu_setting_item*12+16),BMP_CHOOSE1);
  	}
  else
  	{
  		 if(system_flag_table.lang == 1)
			{
		   	  OLED_ShowString12(16,16,"Return"); 
			}	
		else
			{
			Movie_Show_Img(16,16,BMP_SETTING_RETURN);
			}
			Movie_Show_Img(4,16,BMP_CHOOSE1);		

  	}
      }			


	switch(menu_setting_item)
		{
		
		case CUSTOM_RECORED:

	          if(menu_setting_index == MENU_SETTING_THR_PAGE)
	          	{
	  		 if(system_flag_table.lang == 1)
				{
				OLED_ShowString12(16,16,"User Defined"); 
				}	
			else	          	
				Movie_Show_Img(16,16,bmp_CustomRecored);
                    if(menu_setting_postion>RECORED_MODEL_OPTIONS_RETURN)
				menu_setting_postion =0;
					
			if(menu_setting_postion<=BY_SPEED)
				{
		  		 if(system_flag_table.lang == 1)
					{
					OLED_ShowString12(16,28,"By Distance"); 
					OLED_ShowString12(16,40,"By Time"); 
					OLED_ShowString12(16,52,"Speed Mask"); 
					}	
				else					
					{
					Movie_Show_Img(16,28,BMP_BYDICTANCE);
					Movie_Show_Img(16,40,BMP_BYtime);
					Movie_Show_Img(16,52,BMP_BYspeed);
					}
					Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE1);
				}
			else if(menu_setting_postion<=RECORED_MODEL_OPTIONS_RETURN)
				{
					Movie_Show_Img(4,28,BMP_CHOOSE1);
					if(system_flag_table.lang == 1)
						{
						OLED_ShowString12(16,28,"Return"); 
						}
					else
						Movie_Show_Img(16,28,BMP_SETTING_RETURN);

					
				}
					
			Movie_Show_Img(2,16,BMP_CHOOSE3);		
			
	          	}
			  
		     if(menu_setting_index == MENU_SETTING_FOR_PAGE)
		     	{			    
			Movie_Show_Img(2,16,BMP_CHOOSE3);	
			stm_read_eerpom((MENU_SETTING_GUJI_BY_DISTANCE+menu_setting_postion) ,&setting_tp);
			if(setting_tp>1000)
				setting_tp =0 ;
			


	//		 printf("\n\r setting_tp:%d ",setting_tp); 
	//		  printf("\n\r menu_setting_postion_2:%d ",menu_setting_postion_2); 
			switch(menu_setting_postion)
				{
				case BY_DISTANCE:
//					stm_read_eerpom(MENU_SETTING_GUJI_BY_DISTANCE ,&setting_tp);
       			     if(system_flag_table.lang == 1)
						{
						OLED_ShowString12(16,16,"By Distance"); 
					//	OLED_ShowString12(16,28,"5m"); 
					//	OLED_ShowString12(16,40,"10m"); 
					//	OLED_ShowString12(16,52,"20m"); 						
						}   
					 else
						Movie_Show_Img(16,16,BMP_BYDICTANCE);	
					 
                                if(menu_setting_postion_2 >4)
						menu_setting_postion_2 = 0;
									
					if(system_flag_table.myunit.H_W == 0)
						{
						if(menu_setting_postion_2<=2)
							{
							for(i =0;i<3;i++)
								{
							//	system_flag_table.guji_record.by_distance_vaule= arry_bydistance[i];
								sprintf((char *)dtbuf,"%dm",arry_bydistance[i]);		    		//得到速度字符串
								OLED_ShowString12(16,(29+(i*12)),dtbuf);
								}
							}
						else
							{
							for(i =3;i<5;i++)
								{
								//system_flag_table.guji_record.by_distance_vaule= arry_bydistance[i];
								sprintf((char *)dtbuf,"%dm",arry_bydistance[i]);		    		//得到速度字符串
								OLED_ShowString12(16,(29+((i-3)*12)),dtbuf);
								}
							}
						}
					else
						{
						if(menu_setting_postion_2<=2)
							{
							for(i =0;i<3;i++)
								{
								//system_flag_table.guji_record.by_distance_vaule = arry_bydistance_2[i];
								sprintf((char *)dtbuf,"%dft",arry_bydistance_2[i]);		    		//得到速度字符串
								OLED_ShowString12(16,(29+(i*12)),dtbuf);
								}
							}
						else
							{
							for(i =3;i<5;i++)
								{
							
								sprintf((char *)dtbuf,"%dft",arry_bydistance_2[i]);		    		//得到速度字符串
								OLED_ShowString12(16,(29+((i-3)*12)),dtbuf);
								}
							}						
						}

					if(key_type == KEY_ENTER)
						{
						setting_tp = menu_setting_postion_2;
						if(system_flag_table.myunit.H_W >= 1)
							system_flag_table.guji_record.by_distance_vaule_ft= arry_bydistance_2[setting_tp];
						else
							system_flag_table.guji_record.by_distance_vaule = arry_bydistance[setting_tp];

						stm_write_eerpom(MENU_SETTING_GUJI_BY_DISTANCE ,setting_tp);
						stm_write_eerpom(MENU_SETTING_GUJI_BY_MODE ,0);
						system_flag_table.guji_record.by_distance_time_flag= 0;					
						enter_perss = 4;
						}

					break;
				case BY_TIMES:
		                          if(menu_setting_postion_2 >4)
    							menu_setting_postion_2 = 0;
//						stm_read_eerpom(MENU_SETTING_GUJI_BY_TIME ,&setting_tp);
  					       if(system_flag_table.lang == 1)
							{
							OLED_ShowString12(16,16,"By Time"); 					
							}   
						 else
							Movie_Show_Img(16,16,BMP_BYtime);	
						if(menu_setting_postion_2<=2)
							{
							for(i =0;i<3;i++)
								{
							//	system_flag_table.guji_record.by_time_vaule= arry_time[i];
								sprintf((char *)dtbuf,"%ds",arry_time[i]);		    		//得到速度字符串
								OLED_ShowString12(16,(29+(i*12)),dtbuf);
								}
							}
						else
							{
							for(i =3;i<5;i++)
								{
							//	system_flag_table.guji_record.by_time_vaule = arry_time[i];
								sprintf((char *)dtbuf,"%ds",arry_time[i]);		    		//得到速度字符串
								OLED_ShowString12(16,(29+((i-3)*12)),dtbuf);
								}
							}
						
						if(key_type == KEY_ENTER)
							{
							setting_tp = menu_setting_postion_2;
							system_flag_table.guji_record.by_time_vaule= arry_time[setting_tp];
							stm_write_eerpom(MENU_SETTING_GUJI_BY_TIME,setting_tp);
							enter_perss = 4;
							stm_write_eerpom(MENU_SETTING_GUJI_BY_MODE ,1);
							system_flag_table.guji_record.by_distance_time_flag= 1;					

							}							
					break;
				case BY_SPEED:
					     if(system_flag_table.lang == 1)
							{
							OLED_ShowString12(16,16,"Speed Mark"); 					
							}   
						else
							 Movie_Show_Img(16,16,BMP_BYspeed);	
						Movie_Show_Img(0,26,Bmp_updwon);	
						menu_updown_flag = 1;
						//setting_tp = menu_setting_postion_2;
						#if 1
						if(system_flag_table.myunit.H_W == 0)
							{
							#if 0
							if(menu_setting_postion_2>200)
								{
								menu_setting_postion_2 = 0;
								setting_tp = 0;
								}
							
							#endif	
							sprintf((char *)dtbuf,"%dkm",menu_setting_postion_2);		    		//得到速度字符串
							}
						else if(system_flag_table.myunit.H_W == 1)
							{
							#if 0
							if(menu_setting_postion_2>200)
								{
								menu_setting_postion_2 = 0;
								setting_tp = 0;
								}
							#endif
							sprintf((char *)dtbuf,"%dmi",menu_setting_postion_2);		    		//得到速度字符串
							}
						else
							{
							sprintf((char *)dtbuf,"%dkt",menu_setting_postion_2);		    		//得到速度字符串
							}
						#endif
					//	
						OLED_ShowString12(16,29,dtbuf);
						if(key_type == KEY_ENTER)
						{
							 if(system_flag_table.myunit.H_W == 0)
							 	{
								system_flag_table.guji_record.by_speed_vaule = menu_setting_postion_2;
								stm_write_eerpom(MENU_SETTING_GUJI_BY_SPEED,menu_setting_postion_2);
							 	}
							else if(system_flag_table.myunit.H_W == 1)
							 	{
								system_flag_table.guji_record.by_speed_vaule_1 = menu_setting_postion_2;
								stm_write_eerpom(MENU_SETTING_GUJI_BY_SPEED_1,menu_setting_postion_2);

							 	}
							else
							 	{
								system_flag_table.guji_record.by_speed_vaule_2 = menu_setting_postion_2;
								stm_write_eerpom(MENU_SETTING_GUJI_BY_SPEED_2,menu_setting_postion_2);

							 	}

							enter_perss = 4;
						}			
					break;				
				}
			if(menu_setting_postion != BY_SPEED)
				{
                         if(menu_setting_postion_2<3)
					Movie_Show_Img(4,(menu_setting_postion_2*12+28),BMP_CHOOSE2);		
				else	
					Movie_Show_Img(4,((menu_setting_postion_2-3)*12+28),BMP_CHOOSE2);		

			if(setting_tp<3)	
				{
				if(menu_setting_postion_2<3)
					Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
				}
			else	
				{
					if(menu_setting_postion_2>=3)
						Movie_Show_Img(4,((setting_tp-3)*12+28),BMP_CHOOSE1);
				}


				}
		     	}
			

			  
			break;
		case RECORED_MODEL:
			if(menu_setting_index == MENU_SETTING_THR_PAGE)
			 	{

  			     if(system_flag_table.lang == 1)
						{
						OLED_ShowString12(16,16,"Auto Overwrite"); 
						OLED_ShowString12(16,28,"On"); 
						OLED_ShowString12(16,40,"Off"); 
						}   
					 else
					 	{
					 	Movie_Show_Img(16,17,bmp_jilufashi);
					 	Movie_Show_Img(16,29,bmp_jilufashi_1);
					 	}
			 	stm_read_eerpom(MENU_SETTING_GUJI_RECOREDMETH ,&setting_tp);

				Movie_Show_Img(2,16,BMP_CHOOSE3);	
				switch(menu_setting_postion)
					{
					case AUTO_RECOVRY:
						//setting_tp = 0;
						break;
					case AUTO_STOP:
					//	setting_tp = 1;
						break;
					default:
						menu_setting_postion = 0;
						//setting_tp = 0;
						break;
					}
				if(key_type == KEY_ENTER)
						{
							setting_tp = menu_setting_postion;
							system_flag_table.guji_record.recoed_meth= menu_setting_postion;
							stm_write_eerpom(MENU_SETTING_GUJI_RECOREDMETH ,setting_tp);
							enter_perss = 4;
						}
				
				Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);		
				Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
				}
			break;
			
		case AUTO_MODEL:
			if(menu_setting_index == MENU_SETTING_THR_PAGE)
			 	{
			 	Movie_Show_Img(2,16,BMP_CHOOSE3);	

  			     if(system_flag_table.lang == 1)
					{
					OLED_ShowString12(16,16,"Auto Start"); 
					OLED_ShowString12(16,28,"Off"); 
					OLED_ShowString12(16,40,"On"); 
					}   
				 else
				 	{
				 	Movie_Show_Img(16,16,bmp_autoreocrd);
				 	Movie_Show_Img(16,29,bmp_autoreocrd_1);	
				 	}
			 	stm_read_eerpom(MENU_SETTING_RECODED_AUTOFLAG ,&setting_tp);

				
			switch(menu_setting_postion)
				{
				case AUTO_ON:
					//setting_tp = 0;
					break;
				case AUTO_OFF:
				//	setting_tp = 1;
					break;	
				default:
					menu_setting_postion = 0;
					//setting_tp = 0;
					break;					
				}
				if(key_type == KEY_ENTER)
						{
							setting_tp = menu_setting_postion;
							system_flag_table.guji_record.auto_recoed_flag = menu_setting_postion;
							stm_write_eerpom(MENU_SETTING_RECODED_AUTOFLAG ,setting_tp);
							enter_perss = 4;
						}

				
				Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
				Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
				}
			
			break;

		case DELECT_ALL:
			if(menu_setting_index == MENU_SETTING_THR_PAGE)
			 	{	
  			     if(system_flag_table.lang == 1)
					{
					OLED_ShowString12(16,16,"Delete All Tracks"); 
					OLED_ShowString12(16,28,"No"); 
					OLED_ShowString12(16,40,"Yes"); 
					}   
				 else			 	
					Movie_Show_Img(0,0,bmp_guji_remove_recode);
				switch(menu_setting_postion)
				{
				case AUTO_ON:
					//setting_tp = 0;
					break;
				case AUTO_OFF:
				//	setting_tp = 1;
					break;	
				default:
					menu_setting_postion = 0;
					//setting_tp = 0;
					break;					
				}				
				
				if(key_type == KEY_ENTER)
				{
					if(menu_setting_postion  == 1)
					{
					Flash_Index = 0;
					stm_write_eerpom(CUURENT_FLASH_ADDRER ,Flash_Index);	
					}

					enter_perss = 4;
				}				
				Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
				Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);				
				}
			break;
		}		

}



void menu_enter_flash(u16 my_timer_flash)
{
if(KeyTab[keyTabIndex].subnumber == 'S')
	{
	if((my_timer_flash == 0)||(my_timer_flash == 200)||(my_timer_flash == 100))
		{
		OLED_Fill(0, 28, 15, 63, 0);
		if(menu_updown_flag)
			{
			if(menu_updown_flag == 2)
				Movie_Show_Img(4,40,BMP_CHOOSE1);	
			else
				Movie_Show_Img(4,28,BMP_CHOOSE1);	
			}
		else
			{
						
			if(menu_setting_index == MENU_SETTING_THR_PAGE)
				{
				if(menu_setting_postion>2)
					Movie_Show_Img(4,((menu_setting_postion-3)*12+28),BMP_CHOOSE1);
				else
					Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE1);
				}
			else if (menu_setting_index == MENU_SETTING_FOR_PAGE)
				{
				if(menu_setting_postion_2>2)
					Movie_Show_Img(4,((menu_setting_postion_2-3)*12+28),BMP_CHOOSE1);
				else
					Movie_Show_Img(4,(menu_setting_postion_2*12+28),BMP_CHOOSE1);
				}
			else
				Movie_Show_Img(4,(menu_setting_item*12+16),BMP_CHOOSE1);
				}

			OLED_Refresh_Gram(); 	
		}
	else if((my_timer_flash == 50)||(my_timer_flash == 150))
		{
		     if(menu_setting_index == MENU_SETTING_SEC_PAGE)
			     	OLED_Fill(0, 16, 15, 63, 0);
			else 
				OLED_Fill(0, 28, 15, 63, 0);
			OLED_Refresh_Gram(); 	
		}
	if(my_timer_flash == 200)
		{
		menu_setting_index--;
		KeyTab[keyTabIndex].CurrentOperate();	
		enter_perss = 1;
		}
	}
}
void menu_display_setting(void)
{
extern const u8 BMP_SETTING_HEAD[];
extern const u8 BMP_SETTING_HEAD_1[];
extern const u8 BMP_SETTING_HEAD_2[];
extern const u8 BMP_SETTING_RUNMODE[];
extern const u8 bmp_unit_head[];
extern const u8 bmp_timer[];
extern const u8 bmp_data[];
extern const u8 bmp_crood[];
extern const u8 bmp_distance_hwe[];
extern const u8 bmp_temp[];
extern const u8 bmp_pres[];

extern const u8 Bmp_time1[];
extern const u8 Bmp_data1[];
extern const u8 Bmp_coord1[];
//extern const u8 bmp_distance_hwe[];
extern const u8 Bmp_temp1[];
extern const u8 Bmp_temp2[];
extern const u8 Bmp_pres_1[];
extern const u8 Bmp_pres_2[];

extern const unsigned char Bmp_personal_head[];
extern const unsigned char Bmp_weigt[];
extern const unsigned char Bmp_heigt[];
extern const u8 Bmp_xinbie[];
extern const u8 Bmp_manfalem[];
static u32 setting_tp;
float setting_tpf;

u16 year,month,data;

extern const u8 Bmp_system_head[];
extern const u8 Bmp_arr_ajust[];
extern const u8 Bmp_arr_ajust_1[];
extern const u8 Bmp_sreen_stanby[];
extern const u8 Bmp_sreen_stanby_1[];
extern const u8 Bmp_timer_zone[];
extern const u8 Bmp_lan[];
extern const u8 Bmp_lan_1[];
extern const u8 Bmp_yes_or_no[];
extern const u8 Bmp_factroy[];

extern const u8 bmp_delect_record_word[];
extern const u8 bmp_powerdownword[];
extern const u8 bmp_system_word[];

//extern const u8 bmp_system_power_contrl[];
extern const u8 bmp_system_status[];

OLED_Fill(0, 0, 127, 63, 0);


if(key_type == KEY_LONG_ENTER)
	{
	enter_perss = 0;
	menu_setting_index = 0xff;
	key_type = 0xff;
	keyTabIndex = MENU_MAIN;
	return;
	}
	
	switch(menu_setting_index)
		{
		case 0xff:
			menu_setting_page = 0;
			if(key_type == KEY_ENTER)
				{
				menu_setting_index = MENU_SETTING_MAIN_PAGE;
				key_type = 0xff;
				}
			menu_updown_flag = 0; 
			break;
		case MENU_SETTING_MAIN_PAGE:
			menu_setting_item = 0;
			menu_updown_flag = 0; 
			if(key_type == KEY_UP)
				{
				if(menu_setting_page >0)
					menu_setting_page--;
				else
					menu_setting_page = PAGE_QUIT;
					
				}
			else if(key_type == KEY_DOWN)
				{
				if(menu_setting_page <PAGE_QUIT)
					menu_setting_page++;
				else
					menu_setting_page = 0;
				}
			else if(key_type == KEY_ENTER)
				{
				if(menu_setting_page != PAGE_QUIT)
					{
					menu_setting_index = MENU_SETTING_SEC_PAGE;
					if(menu_setting_page == PAGE_RUN_MODEL)
						{
						
						stm_read_eerpom(MENU_SETTING_RUNMODEL_ADDRES ,&setting_tp);
						menu_setting_item= setting_tp;
						}
					}
				else
					{
					menu_setting_page = PAGE_RUN_MODEL;
					menu_setting_index = 0xff;
					enter_perss = 0;
					OLED_Fill(0, 28, 16, 63, 0);
					}
				key_type = 0xff;
				}			
			break;
		case MENU_SETTING_SEC_PAGE:
			menu_updown_flag = 0; 
			if(key_type == KEY_UP)
				{
				if(menu_setting_item >0)
					menu_setting_item--;
				else
					{
				    switch(menu_setting_page)
					{
					case PAGE_RUN_MODEL:
							menu_setting_item = 1;	
						break;					 
					case PAGE_GUJI_RECORD:
							menu_setting_item = 4;	
						break;					
					case PAGE_GESHI_DANGWEI:
							menu_setting_item = 6;	
						break;					
					case PAGE_GEREN_ZHILIAO:
							menu_setting_item = 3;	
						break;					
					case PAGE_SYSTEM_SET:		
							menu_setting_item = 7;	
						break;
				    	}
					}
				}
			else if(key_type == KEY_DOWN)
				menu_setting_item++;
			else if(key_type == KEY_ENTER)
				{
				setting_tp = 0;
				menu_setting_index = MENU_SETTING_THR_PAGE;
			    switch(menu_setting_page)
				{
				case PAGE_RUN_MODEL:
					setting_tp = menu_setting_item;
					stm_write_eerpom(MENU_SETTING_RUNMODEL_ADDRES ,setting_tp);
					if(system_flag_table.run_mode != menu_setting_item)
						{
						 if(system_flag_table.run_mode == 1)
						 	{
						 	GPS_power_ctrl(0);
						 	 USART_Cmd(USART2, DISABLE);
						 	}
						 else
						 	{
						 	GPS_power_ctrl(1);
						 	 USART_Cmd(USART2, ENABLE);
						 	}
						}

					system_flag_table.run_mode = menu_setting_item;
					
					menu_setting_index--;
					enter_perss = 4;								
					break;					 
				case PAGE_GUJI_RECORD:
					if(menu_setting_item ==CUTOME_RECORED_RETURN )
						menu_setting_index = MENU_SETTING_MAIN_PAGE;
					else
						{
						menu_setting_postion = 0;
						if(menu_setting_item ==RECORED_MODEL )	
							{
							stm_read_eerpom(MENU_SETTING_GUJI_RECOREDMETH,&setting_tp);
							menu_setting_postion = setting_tp;
							}
						else if(menu_setting_item ==AUTO_MODEL )
							{
							stm_read_eerpom(MENU_SETTING_RECODED_AUTOFLAG,&setting_tp);
							menu_setting_postion = setting_tp;
							}
						else if(menu_setting_item ==DELECT_ALL)
							menu_setting_postion = 0;
						else
							menu_setting_postion = system_flag_table.guji_record.by_distance_time_flag;

						}
					break;					
				case PAGE_GESHI_DANGWEI:
					if(menu_setting_item ==UNIT_REUTNR )
						menu_setting_index = MENU_SETTING_MAIN_PAGE;
					else
						{
						stm_read_eerpom(MENU_SETTING_UNIT_TIMER+ menu_setting_item,&setting_tp);
						menu_setting_postion = setting_tp;
						}

					break;					
				case PAGE_GEREN_ZHILIAO:
					if(menu_setting_item ==PERSONAL_REUTRN )
						menu_setting_index = MENU_SETTING_MAIN_PAGE;
					else
						{
						if((menu_setting_item == PERSONAL_HEIGHT)&&(system_flag_table.myunit.H_W >=1))
							{
							stm_read_eerpom(MENU_SETTING_HEIGHT_1,&setting_tp);
							}	
						else if((menu_setting_item == PERSONAL_WEIGHT)&&(system_flag_table.myunit.H_W >=1))
							{
							stm_read_eerpom(MENU_SETTING_WEIGHT_1,&setting_tp);
							}	
						else
							stm_read_eerpom(MENU_SETTING_FALME+ menu_setting_item,&setting_tp);
						menu_setting_postion = setting_tp;
						}

					break;					
				case PAGE_SYSTEM_SET:		
					if(menu_setting_item ==SYSTEM_RETURN )
						menu_setting_index = MENU_SETTING_MAIN_PAGE;
					else
						{
						if(menu_setting_item <SYSTEM_INFO)
							{
							stm_read_eerpom(MENU_SETTING_AJUST_FLAG+ menu_setting_item,&setting_tp);
							menu_setting_postion = setting_tp;
							}
						else
							{
							menu_setting_postion = 0;
							setting_tp = 0;
							}
						}

					break;
			    	}

				key_type = 0xff;
				}			
			break;
		case MENU_SETTING_THR_PAGE:
			if(key_type == KEY_UP)
				{
				if((menu_setting_page == PAGE_GEREN_ZHILIAO)&&(menu_setting_item ==  PERSONAL_HEIGHT||menu_setting_item ==  PERSONAL_WEIGHT))				
					{
					menu_setting_postion++;
					}
				else
					{
					if(menu_setting_postion >0)
						menu_setting_postion--;
					if((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  SYSTEM_INFO))
						{
						menu_setting_postion = 0;
						menu_setting_index = MENU_SETTING_SEC_PAGE;
						}
					}
				}
			else if(key_type == KEY_DOWN)
				{
				if((menu_setting_page == PAGE_GEREN_ZHILIAO)&&(menu_setting_item ==  PERSONAL_HEIGHT||menu_setting_item ==  PERSONAL_WEIGHT))				
					{
					if(menu_setting_postion >0)
						menu_setting_postion--;
					}
				else
					{
					menu_setting_postion++;
					if((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  SYSTEM_INFO))
						{
							menu_setting_postion = 0;
							menu_setting_index = MENU_SETTING_SEC_PAGE;
						}
					}
				}
			else if(key_type == KEY_ENTER)
				{	
				if((menu_setting_page == PAGE_GUJI_RECORD)&&(menu_setting_item == CUSTOM_RECORED))
					{
					if(menu_setting_postion == RECORED_MODEL_OPTIONS_RETURN)
						{
						menu_setting_postion = 0;
						menu_setting_index = MENU_SETTING_SEC_PAGE;
						}
					else
						{
						menu_setting_index = MENU_SETTING_FOR_PAGE;
						if((menu_setting_page == PAGE_GUJI_RECORD)&&(menu_setting_item ==  CUSTOM_RECORED)&&(menu_setting_postion == BY_SPEED))		
							{
							if(system_flag_table.myunit.H_W == 0)
								{
								stm_read_eerpom(MENU_SETTING_GUJI_BY_SPEED,&setting_tp);
								menu_setting_postion_2 =setting_tp;
								}
							else if(system_flag_table.myunit.H_W == 1)
								{
								stm_read_eerpom(MENU_SETTING_GUJI_BY_SPEED_1,&setting_tp);
								menu_setting_postion_2 =setting_tp;

								}
							else

								{
								stm_read_eerpom(MENU_SETTING_GUJI_BY_SPEED_2,&setting_tp);
								menu_setting_postion_2 =setting_tp;

								}
							}
						}
					key_type = 0xff;
					}
				
				if((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  SYSTEM_ADR_ADJUST)&&(menu_setting_postion == 1))
					{

						menu_setting_index = MENU_SETTING_FOR_PAGE;
						//stm_read_eerpom(MENU_SETTING_AJUST_ARR ,&setting_tp);
						
						system_flag_table.ajust_Arr_flag  = 0;
						ConvAltitude();
						if(gpsx.fixmode >1)
							{
							if(system_flag_table.myunit.H_W == 0)
								menu_setting_postion_2 = gpsx.altitude/10;						
							else
								menu_setting_postion_2 = gpsx.altitude/10/0.3048;						
							}
						else
							{
							if(system_flag_table.myunit.H_W == 0)
								menu_setting_postion_2 = Altitude/100;						
							else
								menu_setting_postion_2 = Altitude/100/0.3048;						

							}
					   	key_type = 0xff;	
					}
				
				if((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  SYSTEM_INFO))
					{
						menu_setting_postion = 0;
						menu_setting_index = MENU_SETTING_SEC_PAGE;
					}
				}			
			break;

		case MENU_SETTING_FOR_PAGE:

		if((menu_setting_page == PAGE_GUJI_RECORD)&&(menu_setting_item ==  CUSTOM_RECORED)&&menu_setting_postion ==BY_SPEED )
			{
			if(key_type == KEY_DOWN)
				{
				if(menu_setting_postion_2 >=5)
					menu_setting_postion_2-=5;
				else
					menu_setting_postion_2 = 200;
				
				}
			else if(key_type == KEY_UP)
				{
				if(menu_setting_postion_2 <=195)
					menu_setting_postion_2+=5;
				else
					menu_setting_postion_2 = 0;
					
				}
			}
		//||((menu_setting_page == PAGE_GEREN_ZHILIAO)&&(menu_setting_item ==  PERSONAL_HEIGHT||menu_setting_item ==  PERSONAL_WEIGHT))
	         else if((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  SYSTEM_ADR_ADJUST)&&menu_setting_index ==MENU_SETTING_FOR_PAGE )
			
			{
			if(key_type == KEY_DOWN)
				{
				if(((menu_setting_page == PAGE_SYSTEM_SET)&&(menu_setting_item ==  SYSTEM_ADR_ADJUST)&&menu_setting_index ==MENU_SETTING_FOR_PAGE ))
					{
				//	if(menu_setting_postion_2)
						menu_setting_postion_2--;
					}
				else
					{
					if(menu_setting_postion_2 >0)
						menu_setting_postion_2--;
					}
				}
			else if(key_type == KEY_UP)
				menu_setting_postion_2++;			
			}
		else
			{
			if(key_type == KEY_UP)
				{
				if(menu_setting_postion_2 >0)
					menu_setting_postion_2--;
				}
			else if(key_type == KEY_DOWN)
				menu_setting_postion_2++;
			}
			break;
			}


	if(menu_setting_index == 0xff)
		{
		
		Movie_Show_Img(0,0,BMP_SETTING_HEAD);
		if(system_flag_table.lang == 1)
			{
		   	  OLED_ShowString(16,0,"SETTINGS"); 
			}
		#if 0
		if(menu_setting_page<PAGE_SYSTEM_SET)
			Movie_Show_Img(16,16,BMP_SETTING_HEAD_1);
		else	
			Movie_Show_Img(16,16,BMP_SETTING_HEAD_2);;
		#endif
		}
	else if(menu_setting_index == MENU_SETTING_MAIN_PAGE)
		{
		Movie_Show_Img(0,0,BMP_SETTING_HEAD);
		if(system_flag_table.lang == 1)
			{
		   	  OLED_ShowString(16,0,"SETTINGS"); 
			}
		
		if(menu_setting_page<PAGE_SYSTEM_SET)
			{
			Movie_Show_Img(4,(menu_setting_page*12+16),BMP_CHOOSE1);	

			if(system_flag_table.lang == 1)
				{
		   		  OLED_ShowString12(16,16,"Operating Modes"); 
			   	  OLED_ShowString12(16,28,"Track Log"); 
			   	  OLED_ShowString12(16,40,"Format/Units"); 
			   	  OLED_ShowString12(16,52,"Profiles"); 			  
				}
			else
				{
				Movie_Show_Img(16,16,BMP_SETTING_HEAD_1);
				}
			}
		else	
			{
			Movie_Show_Img(4,((menu_setting_page-PAGE_SYSTEM_SET)*12+16),BMP_CHOOSE1);	

			if(system_flag_table.lang == 1)
			{
		   	  OLED_ShowString12(16,16,"Setup"); 
		   	  OLED_ShowString12(16,28,"Exit"); 
			}
			else
				{
				Movie_Show_Img(16,16,BMP_SETTING_HEAD_2);
				}

			}

		}

	else if((menu_setting_index == MENU_SETTING_SEC_PAGE)||(menu_setting_index == MENU_SETTING_THR_PAGE)||(menu_setting_index == MENU_SETTING_FOR_PAGE))
		{
		    switch(menu_setting_page)
			{
			case PAGE_RUN_MODEL:
			stm_read_eerpom(MENU_SETTING_RUNMODEL_ADDRES ,&setting_tp);
			Movie_Show_Img(0,0,BMP_SETTING_RUNMODE);
			if(system_flag_table.lang == 1)
			{
			  OLED_ShowString(16,0,"Operating Mode"); 
		   	  OLED_ShowString12(16,16,"Sensor Mode"); 
		   	  OLED_ShowString12(16,28,"GPS Mode"); 
			}	

				
			if(menu_setting_item >1)				
				menu_setting_item = 0;
			Movie_Show_Img(4,(menu_setting_item*12+16),BMP_CHOOSE2);
			Movie_Show_Img(4,(setting_tp*12+16),BMP_CHOOSE1);
			switch(menu_setting_item)
				{
				case SENSOR_MODEL:				
					setting_tp = 0;
					break;
				case GPS_MODEL:
					rtc_set_flag = 1;
					setting_tp = 1;
					break;
				default:
				
					break;
	
				}
		//	if(key_type == KEY_ENTER)
		//	{
	//		stm_write_eerpom(MENU_SETTING_RUNMODEL_ADDRES ,setting_tp);
	//		menu_setting_index = MENU_SETTING_MAIN_PAGE;	
		//	enter_perss = 4;
//			key_type = 0xff;
		//	}
		break;
	case PAGE_GUJI_RECORD:
			menu_display_settingitem_recode();
		break;
	case PAGE_GESHI_DANGWEI:
		Movie_Show_Img(0,0,bmp_unit_head);
		if(system_flag_table.lang == 1)
			{
			  OLED_ShowString(16,0,"Format/Unit    "); 
			}
		
			if(menu_setting_index == MENU_SETTING_THR_PAGE)
      		      	{
			
		
			Movie_Show_Img(2,16,BMP_CHOOSE3);

				
			switch(menu_setting_item)
				{
				case UNIT_TIME:
				//	setting_tp
					if(system_flag_table.lang == 1)
						{
						  OLED_ShowString12(16,16,"Time"); 
						  OLED_ShowString12(16,28,"12-Hour"); 
						  OLED_ShowString12(16,40,"24-Hour"); 
						}				
					else
						{
						Movie_Show_Img(16,16,bmp_timer);
						Movie_Show_Img(16,28,Bmp_time1);
						}
					if(menu_setting_postion>1)
						menu_setting_postion = 0;
				       system_flag_table.myunit.TIMER = menu_setting_postion;
					   	Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);	
					break;
				case UNIT_DATE:
					if(system_flag_table.lang == 1)
						{
						  OLED_ShowString12(16,16,"Date"); 
						  OLED_ShowString12(16,28,"YY-MM-DD"); 
						  OLED_ShowString12(16,40,"MM-DD-YY"); 
						  OLED_ShowString12(16,52,"DD-MM-YY"); 
						}				
					else
						{
						Movie_Show_Img(16,16,bmp_data);
						Movie_Show_Img(16,28,Bmp_data1);					
						}
					if(menu_setting_postion>2)
						menu_setting_postion = 0;

				       system_flag_table.myunit.DATA= menu_setting_postion;
					Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);					   
					break;
				case UNIT_DISCANCE:

					if(menu_setting_postion>2)
						menu_setting_postion = 0;
					if(system_flag_table.lang == 1)
						{
						  OLED_ShowString12(16,16,"Distance/Weight"); 
						}
					else
						Movie_Show_Img(16,16,bmp_distance_hwe);
					sprintf((char *)dtbuf,"m/km/kg");	
					OLED_ShowString12(16,28,dtbuf);
					sprintf((char *)dtbuf,"ft/mi/pound");	
					OLED_ShowString12(16,40,dtbuf);
					sprintf((char *)dtbuf,"ft/kt/pound");	
					OLED_ShowString12(16,52,dtbuf);

				       system_flag_table.myunit.H_W= menu_setting_postion;
					Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);   
					break;
				case UNIT_COORD:
					if(menu_setting_postion>2)
						menu_setting_postion = 0;
					if(system_flag_table.lang == 1)
						{
						  OLED_ShowString12(16,16,"Position"); 
						  OLED_ShowString12(16,28,"ddd.dddd#"); 
						  OLED_ShowString12(16,40,"ddd.mm.mmm$"); 
						  OLED_ShowString12(16,52,"ddd.mm$ss.s&"); 						  
						}
					else
						{
						Movie_Show_Img(16,16,bmp_crood);
						Movie_Show_Img(16,29,Bmp_coord1);							
						}
				       system_flag_table.myunit.COORD= menu_setting_postion;					
					Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
					break;
				//	setting_tp
				case UNIT_TEMP:
					if(menu_setting_postion>1)
						menu_setting_postion = 0;
					if(system_flag_table.lang == 1)
						{
						  OLED_ShowString12(16,16,"Temperature"); 
						  OLED_ShowString12(16,28,"Celsius"); 
						  OLED_ShowString12(16,40,"Fahrenheit"); 
						}
					else
						{
						Movie_Show_Img(16,16,bmp_temp);
						Movie_Show_Img(16,28,Bmp_temp1);
						Movie_Show_Img(16,40,Bmp_temp2);
						}
				       system_flag_table.myunit.TEMP= menu_setting_postion;
					Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);	
					break;
				case UNIT_PRES:

					if(menu_setting_postion>3)
						menu_setting_postion = 0;
					if(system_flag_table.lang == 1)
						{
						  OLED_ShowString12(16,16,"Pressure"); 
						}
					else
						Movie_Show_Img(16,16,bmp_pres);
						
					if(menu_setting_postion<3)	
						{
						if(system_flag_table.lang == 1)
							{
							  OLED_ShowString12(16,16,"Pressure"); 
							  OLED_ShowString12(16,28,"Hectopascals"); 
							  OLED_ShowString12(16,40,"Millibars"); 
							  OLED_ShowString12(16,52,"mm Hg"); 							  
							}

						else
							{

								Movie_Show_Img(16,28,Bmp_pres_1);
							}
						}
					else
						{
							if(system_flag_table.lang == 1)
								{
								  OLED_ShowString12(16,28,"Inch Hg"); 
								}
							else						
								Movie_Show_Img(16,28,Bmp_pres_2);
						}
						
				       system_flag_table.myunit.PREE= menu_setting_postion;
					   
					if(menu_setting_postion<3)
						Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
					else	
						Movie_Show_Img(4,((menu_setting_postion-3)*12+28),BMP_CHOOSE2);					

	
					break;
				case UNIT_REUTNR:
//				       system_flag_table.unit.DATA= menu_setting_postion;
					break;

				}
			
			if(setting_tp<3)	
				{
				if(menu_setting_postion<3)
					Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
				}
			else	
				{
				if(menu_setting_postion>=3)
					Movie_Show_Img(4,((setting_tp-3)*12+28),BMP_CHOOSE1);
				}
			
		     if(key_type == KEY_ENTER)
			{
			setting_tp = menu_setting_postion;			
			stm_write_eerpom(MENU_SETTING_UNIT_TIMER+ menu_setting_item ,setting_tp);
			enter_perss = 4;
			}
	
				
         	   	}
		else if(menu_setting_index == MENU_SETTING_SEC_PAGE)
			{
			if(menu_setting_item>UNIT_REUTNR)
				{
				menu_setting_item = 0;
				}
			if(menu_setting_item<=UNIT_DISCANCE)
				{
				if(system_flag_table.lang == 1)
					{
					  OLED_ShowString12(16,16,"Time"); 
					  OLED_ShowString12(16,28,"Date"); 
					  OLED_ShowString12(16,40,"Position"); 
					  OLED_ShowString12(16,52,"Distance/Weight"); 					  
					}	
				else
					{
					Movie_Show_Img(16,16,bmp_timer);
					Movie_Show_Img(16,28,bmp_data);
					Movie_Show_Img(16,40,bmp_crood);
					Movie_Show_Img(16,53,bmp_distance_hwe);
					}
					Movie_Show_Img(4,(menu_setting_item*12+16),BMP_CHOOSE1);
				}
			else
				{
					if(system_flag_table.lang == 1)
						{
						  OLED_ShowString12(16,16,"Temperature"); 
						  OLED_ShowString12(16,28,"Pressure"); 
						  OLED_ShowString12(16,40,"Return"); 
						}
					else			
						{
						Movie_Show_Img(16,17,bmp_temp);
						Movie_Show_Img(16,28,bmp_pres);
						Movie_Show_Img(16,40,BMP_SETTING_RETURN);
						}
					Movie_Show_Img(4,((menu_setting_item-UNIT_TEMP)*12+16),BMP_CHOOSE1);
				}

//		     if(key_type == KEY_ENTER)
				{
	//			menu_setting_index = MENU_SETTING_THR_PAGE;
				}					
			}
			
	
		break;
	case PAGE_GEREN_ZHILIAO:
		Movie_Show_Img(0,0,Bmp_personal_head);
 		if(menu_setting_index == MENU_SETTING_SEC_PAGE)
			{

			if(menu_setting_item>3)
				{
				menu_setting_item = 0;
				}
			
				    if(system_flag_table.lang == 1)
						{
						  OLED_ShowString12(16,16,"Gender"); 
						  OLED_ShowString12(16,28,"Height"); 
						  OLED_ShowString12(16,40,"Weight"); 
						  OLED_ShowString12(16,52,"Return"); 
						}
					else			
						{			
						Movie_Show_Img(16,17,Bmp_xinbie);
						Movie_Show_Img(16,29,Bmp_heigt);
						Movie_Show_Img(16,41,Bmp_weigt);
						Movie_Show_Img(16,52,BMP_SETTING_RETURN);
						}
					Movie_Show_Img(4,(menu_setting_item*12+16),BMP_CHOOSE1);
	//	     if(key_type == KEY_ENTER)
	//			{
	//		menu_setting_index = MENU_SETTING_THR_PAGE;
	//			}					
			}
		else if(menu_setting_index == MENU_SETTING_THR_PAGE)
			{
//			if(menu_setting_postion > 0x500)
//				menu_setting_postion = 0;							
			Movie_Show_Img(2,16,BMP_CHOOSE3);

		
			switch(menu_setting_item)
				{
				case PERSONAL_FALEM:
				if(menu_setting_postion>1)
					{
					menu_setting_postion = 0;
					}
				    if(system_flag_table.lang == 1)
						{
						  OLED_ShowString12(16,16,"Gender"); 
						  OLED_ShowString12(16,28,"Male"); 
						  OLED_ShowString12(16,40,"Female"); 
						}
					else			
						{
						Movie_Show_Img(16,17,Bmp_xinbie);
						Movie_Show_Img(16,29,Bmp_manfalem);
						}
					Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
					Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
					break;
				case PERSONAL_HEIGHT:
					menu_updown_flag = 1;
				    if(system_flag_table.lang == 1)
						{
						  OLED_ShowString12(16,16,"Height"); 
					    	}
					else
						{
						Movie_Show_Img(16,17,Bmp_heigt);
						}
						Movie_Show_Img(0,27,Bmp_updwon);
					if(system_flag_table.myunit.H_W == 0)
						{
					if(menu_setting_postion>150)
						{
						menu_setting_postion = 0;
						}

						sprintf((char *)dtbuf,"%d cm",(100+menu_setting_postion));	
					//	OLED_ShowString(16,40,dtbuf);
					//	sprintf((char *)dtbuf,"cm");	
						}
					else
						{
					if(menu_setting_postion>50)
						{
						menu_setting_postion = 0;
						}

						setting_tpf =(30+menu_setting_postion);
						
						sprintf((char *)dtbuf,"%.1f ft",setting_tpf/10);	
				//		OLED_ShowString(16,40,dtbuf);
				//		sprintf((char *)dtbuf,"ft");	
						}
					OLED_ShowString12(16,29,dtbuf);
					break;
				case PERSONAL_WEIGHT:
						menu_updown_flag = 1;
				       if(system_flag_table.lang == 1)
						{
						  OLED_ShowString12(16,16,"Weight"); 
					    	}
					else
						{						
						Movie_Show_Img(16,17,Bmp_weigt);
						}
					Movie_Show_Img(0,27,Bmp_updwon);
					if(system_flag_table.myunit.H_W == 0)
						{
						if(menu_setting_postion>180)
							{
							menu_setting_postion = 0;
							}

						sprintf((char *)dtbuf,"%d kg",(20+menu_setting_postion));
//						OLED_ShowString(16,40,dtbuf);
//						sprintf((char *)dtbuf,"kg");	
						}
					else
						{
						if(menu_setting_postion>460)
							{
							menu_setting_postion = 0;
							}
 	
						sprintf((char *)dtbuf,"%d lbs",(40+menu_setting_postion));
					//	OLED_ShowString(16,40,dtbuf);
					//	sprintf((char *)dtbuf,"lbs");	
						}
					OLED_ShowString12(16,29,dtbuf);
					break;
	
				}

		     if(key_type == KEY_ENTER)
			{
			if(menu_setting_item == PERSONAL_WEIGHT)
				{
				 if(system_flag_table.myunit.H_W == 0)
				 	{
				 	system_flag_table.personal_da.weight = menu_setting_postion+20;
						stm_write_eerpom(MENU_SETTING_FALME+ menu_setting_item ,setting_tp);
				 	}
				 else
				 	{
						system_flag_table.personal_da.weight_ft= ((menu_setting_postion+40)*0.4536);
						stm_write_eerpom(MENU_SETTING_WEIGHT_1 ,menu_setting_postion);
				 	}
				}
			else if(menu_setting_item == PERSONAL_HEIGHT)
				{
				 if(system_flag_table.myunit.H_W == 0)
				 	{
				 	system_flag_table.personal_da.height= menu_setting_postion+100;
						stm_write_eerpom(MENU_SETTING_FALME+ menu_setting_item ,menu_setting_postion);
				 	}
				 else
				 	{
						system_flag_table.personal_da.height_ft=(menu_setting_postion+30)/0.328;
						stm_write_eerpom(MENU_SETTING_HEIGHT_1 ,menu_setting_postion);
				 	}				
				}
			//	setting_tp = menu_setting_postion;			
			
				enter_perss = 4;
			}			
			}
		break;
	case PAGE_SYSTEM_SET:
		Movie_Show_Img(0,0,Bmp_system_head);
		if(system_flag_table.lang == 1)
			{
		   	  OLED_ShowString(16,0,"Setup    "); 
			}
		
 		if(menu_setting_index == MENU_SETTING_SEC_PAGE)
			{
				if(menu_setting_item>7)
					menu_setting_item = 0;
					
				if(menu_setting_item<=3)
					{
				    if(system_flag_table.lang == 1)
						{
						  OLED_ShowString12(16,16,"Altimeter Cal."); 
						  OLED_ShowString12(16,28,"Screen Off"); 
				
						  OLED_ShowString12(16,40,"Time Zone"); 						  
						  OLED_ShowString12(16,52,"Language"); 
					    	}
					else
						{
						Movie_Show_Img(16,16,Bmp_arr_ajust);
						Movie_Show_Img(16,28,Bmp_sreen_stanby);
						Movie_Show_Img(16,41,Bmp_timer_zone);
						Movie_Show_Img(16,52,Bmp_lan);
						}
					Movie_Show_Img(4,(menu_setting_item*12+16),BMP_CHOOSE1);
					}
				else
					{
				    if(system_flag_table.lang == 1)
						{

						  OLED_ShowString12(16,16,"Auto On/Off"); 
						  OLED_ShowString12(16,28,"System Info"); 
						  OLED_ShowString12(16,40,"Factory Reset"); 
						  OLED_ShowString12(16,52,"Return"); 						  
					    	}
					else
						{
						Movie_Show_Img(16,16,bmp_powerdownword);
						Movie_Show_Img(16,29,bmp_system_word);
						Movie_Show_Img(16,40,Bmp_factroy);
						Movie_Show_Img(16,52,BMP_SETTING_RETURN);
						}
/*
extern const u8 bmp_delect_record_word[];
extern const u8 bmp_powerdownword[];
extern const u8 bmp_system_word[];
*/					
					Movie_Show_Img(4,((menu_setting_item-4)*12+16),BMP_CHOOSE1);
					}
					
			
	//	     if(key_type == KEY_ENTER)
	//			{
	//			menu_setting_index = MENU_SETTING_THR_PAGE;
	//			}					
			}	
		else if((menu_setting_index == MENU_SETTING_THR_PAGE)||(menu_setting_index == MENU_SETTING_FOR_PAGE))
			{

			Movie_Show_Img(2,16,BMP_CHOOSE3)	;		
			switch(menu_setting_item)
				{
				case SYSTEM_ADR_ADJUST:
					if(menu_setting_index == MENU_SETTING_THR_PAGE)
						{
						if(menu_setting_postion>1)
							menu_setting_postion = 0;
				    if(system_flag_table.lang == 1)
						{
						  OLED_ShowString12(16,16,"Altimeter Cal."); 
						  OLED_ShowString12(16,28,"Off"); 						  
						  OLED_ShowString12(16,40,"On"); 						  
						}
					else
						{						
						Movie_Show_Img(16,16,Bmp_arr_ajust);	
						Movie_Show_Img(16,29,bmp_autoreocrd_1);
						}
			        		Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
						Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);						

						}
					else
						{
						if(system_flag_table.myunit.H_W == 0)
							{
							if(menu_setting_postion_2>7999)
								menu_setting_postion_2 = 7999;

							if(menu_setting_postion_2<-7999)
								menu_setting_postion_2 = -7999;

							}
						else 

							{
							if(menu_setting_postion_2>26246)
								menu_setting_postion_2 = 26246;
							
							if(menu_setting_postion_2<-26246)
								menu_setting_postion_2 = -26246;
							}

					    if(system_flag_table.lang == 1)
							{
							  OLED_ShowString12(16,16,"Altimeter Cal."); 
							  OLED_ShowString12(16,28,"Current Elevation?"); 						  				  
							}
						else
							{						
							Movie_Show_Img(16,16,Bmp_arr_ajust);					
							Movie_Show_Img(16,29,Bmp_arr_ajust_1)	;	
							}
							Movie_Show_Img(0,39,Bmp_updwon);
							menu_updown_flag = 2;
							if(system_flag_table.myunit.H_W == 0)
								sprintf((char *)dtbuf,"%d m",(menu_setting_postion_2));
							else
								sprintf((char *)dtbuf,"%d ft",(menu_setting_postion_2));
							
							OLED_ShowString12(16,41,dtbuf);
						    if(key_type == KEY_ENTER)
						  	{
						  	Altitude=BMP085_Get_Altitude();
						  	if(system_flag_table.myunit.H_W == 0)
						  		{
								
								if( Altitude/100>menu_setting_postion_2)
									system_flag_table.ajust_arr = Altitude/100-menu_setting_postion_2;
								else
									{
									system_flag_table.ajust_arr = menu_setting_postion_2 - Altitude/100;
										system_flag_table.ajust_arr = -system_flag_table.ajust_arr;
									}
								stm_write_eerpom(MENU_SETTING_AJUST_FLAG ,menu_setting_postion);
								
								stm_write_eerpom(MENU_SETTING_AJUST_ARR ,system_flag_table.ajust_arr);
								printf("\r\n%d-%d-%d\r\n",system_flag_table.ajust_arr,menu_setting_postion_2,Altitude/100);
						  		}
							else
								{
								// menu_setting_postion_2;
								setting_tpf = menu_setting_postion_2*0.3048;	
								if( Altitude/100 >setting_tpf)
									system_flag_table.ajust_arr = Altitude/100-setting_tpf;
								else
									{
									system_flag_table.ajust_arr = setting_tpf - Altitude/100;
									system_flag_table.ajust_arr = -system_flag_table.ajust_arr;
									}
							      stm_write_eerpom(MENU_SETTING_AJUST_FLAG ,menu_setting_postion);	
								stm_write_eerpom(MENU_SETTING_AJUST_ARR ,system_flag_table.ajust_arr);
								printf("\r\n%d-%d-%d\r\n",system_flag_table.ajust_arr,menu_setting_postion_2,Altitude/100);
							}
								
								system_flag_table.ajust_Arr_flag = 1;
							key_type = 0xff;
							menu_setting_index--;
							enter_perss = 4;
						  	}							
						}

					break;
				case SCREEN_STANBY:
					if(menu_setting_postion>2)
						menu_setting_postion = 0;
					    if(system_flag_table.lang == 1)
							{
							  OLED_ShowString12(16,16,"Screen Off"); 
							  OLED_ShowString12(16,28,"15 sec"); 						  				  
							  OLED_ShowString12(16,40,"30 sec"); 						  				  
							  OLED_ShowString12(16,52,"Always On"); 						  				  							  
							}
						else
							{
							Movie_Show_Img(16,16,Bmp_sreen_stanby)	;	
							Movie_Show_Img(16,29,Bmp_sreen_stanby_1)		;	
							}
					  if(key_type == KEY_ENTER)
					  	{
					  	system_flag_table.screen_timer = menu_setting_postion;
					  	}
					break;
				case TIMER_ZONE:
					if(menu_setting_postion>41)
						menu_setting_postion = 0;
					    if(system_flag_table.lang == 1)
							{
							  OLED_ShowString12(16,16,"Time Zone"); 					  				  							  
							}
						else
							{					
							Movie_Show_Img(16,17,Bmp_timer_zone)	;	
							}
					Movie_Show_Img(0,28,Bmp_updwon);
					menu_updown_flag = 1;
					sprintf((char *)dtbuf,"%s",timer_zone_Aarry[menu_setting_postion]);
					OLED_ShowString12(16,29,dtbuf);

					break;
				case LANGUAGE:
					if(menu_setting_postion>1)
						menu_setting_postion = 0;	
					
						Movie_Show_Img(16,16,Bmp_lan)	;	
						Movie_Show_Img(16,29,Bmp_lan_1);					
						
					break;
				case POWERDONW_CONTRL:
					if(menu_setting_postion>1)
						menu_setting_postion = 0;
					    if(system_flag_table.lang == 1)
							{
							  OLED_ShowString12(16,16,"Auto On/Off"); 					  				  							  
							  OLED_ShowString12(16,28,"Off"); 					  				  							  
							  OLED_ShowString12(16,40,"On"); 					  				  							  							  
							}
						else
							{					
						//	Movie_Show_Img(0,0,bmp_system_power_contrl);
							Movie_Show_Img(16,16,bmp_powerdownword);
							Movie_Show_Img(16,29,bmp_autoreocrd_1);	
							}
				      	OLED_Fill(0, 28, 15, 63, 0);
					  if(key_type == KEY_ENTER)
					  	{
					  	system_flag_table.usb_powerdown_flash = menu_setting_postion;
					  	}					
					break;

				case SYSTEM_INFO:	
					menu_setting_postion = 0;	
					    if(system_flag_table.lang == 1)
							{	
							  OLED_ShowString12(16,16,"Sytem Info"); 					  				  							  
							  OLED_ShowString12(16,28,"FW:"); 					  				  							  
							  OLED_ShowString12(16,40,"Power On:"); 								
							  OLED_ShowString12(16,52,"First Use:"); 								
					    		}
						else
							{
							Movie_Show_Img(0,0,bmp_system_status);
						    	}
					OLED_ShowString12(72,28,"V0.8"); 
					sprintf((char *)dtbuf,"%d",system_flag_table.powerdown_timers);	//most				
					OLED_ShowString12(72,40,dtbuf); 
					stm_read_eerpom(MENU_FRIST_POWER_INDEX_ADDRES ,&setting_tp);
					if(setting_tp == 0x12345678)
						{
						stm_read_eerpom(MENU_FRIST_POWERDATE_INDEX_ADDRES ,&setting_tp);
						year = (setting_tp&0x00ff0000)>>16;
						month = (setting_tp&0x0000ff00)>>8;
						data = (setting_tp&0x000000ff);
						sprintf((char *)dtbuf,"%02d-%02d-%02d",year,month,data);	//显示UTC日期
						OLED_ShowString12(72,52,dtbuf);		
						}
					//OLED_ShowString12(80,52,COMPILED_DATE); 
					break;

				case FACTROY:
					if(menu_setting_postion>1)
						menu_setting_postion = 0;	
					    if(system_flag_table.lang == 1)
							{	
							  OLED_ShowString12(16,16,"Factory Reset"); 					  				  							  
							  OLED_ShowString12(16,28,"No"); 					  				  							  
							  OLED_ShowString12(16,40,"Yes"); 								
					    		}
						else
							{					
							Movie_Show_Img(16,16,Bmp_factroy)	;	
							Movie_Show_Img(16,29,Bmp_yes_or_no);					
							}

					break;

				}

	        	if((menu_setting_item!= SYSTEM_INFO)&&(menu_setting_item!= SYSTEM_ADR_ADJUST)&&(menu_setting_item!=TIMER_ZONE))
	        		{
	        		
	        		Movie_Show_Img(4,(menu_setting_postion*12+28),BMP_CHOOSE2);
				Movie_Show_Img(4,(setting_tp*12+28),BMP_CHOOSE1);
	        		}
				
		     if(key_type == KEY_ENTER)
			{
			       if((menu_setting_item != FACTROY)&&(menu_setting_item != SYSTEM_INFO))
			       	{
					setting_tp = menu_setting_postion;
					stm_write_eerpom(MENU_SETTING_AJUST_FLAG+ menu_setting_item ,setting_tp);
					if(menu_setting_item == SYSTEM_ADR_ADJUST)
						{
						system_flag_table.ajust_Arr_flag = 0;
						system_flag_table.ajust_arr  = 0;
						}
			       	}
				   else
				   	{
				   	if((menu_setting_postion == 1)&&(menu_setting_item == FACTROY))
				   		{
						stm_write_eerpom(MENU_SETTING_INDEX_ADDRES,0x20077);
						check_eeprom_init();
						old_spped_div_cnt = 0;
						old_div_cnt = 0;
						old_altiude_div_cnt = 0;
						//SPI_Flash_Write(&old_spped_div_cnt, CHANGE_SPEED_VAULE_INDEX_ADDRES, 1);
						//SPI_Flash_Write(&old_div_cnt, CHANGE_TMPE_VAULE_INDEX_ADDRES, 1);
						//SPI_Flash_Write(&old_altiude_div_cnt, CHANGE_ALTITUD_VAULE_INDEX_ADDRES, 1);		  
						
				   		}
				   	}

				
				if(menu_setting_item==TIMER_ZONE)
					{
					    rtc_set_flag = 1;
					    system_flag_table.time_zone = menu_setting_postion;
					
						time_zone = ((((timer_zone_Aarry[system_flag_table.time_zone][4]-'0')*10)+(timer_zone_Aarry[system_flag_table.time_zone][5]-'0')));
						time_zone =time_zone/60;
						time_zone += ((timer_zone_Aarry[system_flag_table.time_zone][1]-'0')*10)+(timer_zone_Aarry[system_flag_table.time_zone][2]-'0');
						printf("\r\ntime_zone-%f\r\n",time_zone);
						check_time();
					}		

				if(menu_setting_item == LANGUAGE)
					{
					system_flag_table.lang = menu_setting_postion;
					}
					enter_perss = 4;
			}			
			}
		break;
	case PAGE_QUIT:
		enter_perss =  0;
		break;

	}
}
}


/***设置菜单END******/


static u8 need_to_powerdownflag = 0;

void system_power_down_process()
{
	extern const u8 bmp_consure_powerdown[];
	  	
   	Movie_Show_Img(0,16,bmp_consure_powerdown);
     if(system_flag_table.lang == 1)
		{
		OLED_ShowString12(64,19,"Power off "); 
		OLED_ShowString12(64,32,"Return    "); 
		}   
		
	if(key_type == KEY_ENTER)
		{
			KeyTab[keyTabIndex].CurrentOperate();	  
			system_flag_table.powerdown_timers++;
			stm_write_eerpom(MENU_POWERDOWNTIME_INDEX_ADDRES ,system_flag_table.powerdown_timers|0x12000000);
			need_to_powerdownflag = 0;
			

			if(USB_CONNECT == 0)
				Choose_power_mode(MCU_STBY);
			else
				{
				system_power_flag = SYSTEM_POWER_STANBY;
				OLED_Clear();
				di_and_flash_empet(3);
				}

		}
	else if((key_type == KEY_UP)||(key_type == KEY_DOWN))
		{
		      KeyTab[keyTabIndex].CurrentOperate();	  
			need_to_powerdownflag = 0;			 
		}
   	OLED_Refresh_Gram();
}



void di_and_flash_empet(u8 times)
{
u8 i;

if(system_power_flag == SYSTEM_POWER_ON)
	{

	for(i=0;i<times;i++)
 	{
	 if(system_flag_table.batt_Status == 0xff)	
 	 	{
 	 //		Movie_Show_Img(43,0,Bmp_batt_0);
	//		OLED_Refresh_Gram(); 		
 	 	}
	 
			Button_key_perss(1);
			delay_ms(50);
			Button_key_perss(0);		
			delay_ms(50);


	 if(system_flag_table.batt_Status == 0xff)	
 	 	{
	//		OLED_Fill(0, 0, 127, 16, 0);
	//		OLED_Refresh_Gram(); 		
 	 	}

 	}
	}
else if(system_power_flag == SYSTEM_POWER_STANBY)
	{
       OLED_Clear();
	for(i=0;i<times;i++)
 	{
	 if(system_flag_table.batt_Status == 0xff)	
 	 	{
			Movie_Show_Img(43,0,Bmp_batt_0);
			OLED_Refresh_Gram(); 		 
			delay_ms(500);
			OLED_Fill(0, 0, 127, 16, 0);
			OLED_Refresh_Gram(); 	
			delay_ms(500);
 	 	}
		

 	}	
		Button_key_perss(1);
		delay_ms(600);
		Button_key_perss(0);		
	}

}



void menu_keyprocess(u8 key)
{
static u8 menu_info_update;
extern u8 button_100_time;
#if 1
  if(key != 0)
  	{
	Send_to_goto_lowpower();

	baoyun_flag = 0;
  	if(system_power_flag == SYSTEM_POWER_LOW)
  		{
		system_power_flag  =  SYSTEM_POWER_ON;
		if((system_flag_table.run_mode == 0)&&(USB_CONNECT == 0))
			Choose_power_mode(MCU_RUN);
		KeyTab[keyTabIndex].CurrentOperate();	  
		OLED_Refresh_Gram(); 	
		Button_key_perss(1);
		button_100_time = 5;	  	
		return ;
  		}
	else if(system_power_flag == SYSTEM_POWER_LOW_2)
		{
		system_power_flag = SYSTEM_POWER_LOW;

		Button_key_perss(1);
		button_100_time = 5;	  
		if(USB_CONNECT == 1)
			LED0 = 1;
		To_lower_timercnt = 6;
		OLED_Fill(0, 0, 127, 63, 0);
		OLED_Refresh_Gram(); 	
		return;
		}
	else
		{
			if(system_power_flag != SYSTEM_POWER_STANBY)
			{
			if((need_to_powerdownflag != 1)||(key != 3))
				{
				Button_key_perss(1);
				button_100_time = 5;	  
				}
			}	
		}

	

  	}
else
	{
 	if(system_power_flag == SYSTEM_POWER_LOW)
 		{
 		if(menu_info_update == 1)
 			{
 			menu_info_update = 0;
			
			//sprintf((char *)dtbuf,"%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes);	//显示UTC日期
			//	OLED_ShowString_head(0,0,dtbuf);
			menu_status_dispplay();
			if(system_flag_table.run_mode == 0)
				OLED_Refresh_Gram_low(); 	
			else
				OLED_Refresh_Gram(); 
 			}
		else
			menu_info_update ++;
 		 return;
 		}
	else if(system_power_flag == 0)
		{
			menu_status_dispplay();
			OLED_Refresh_Gram(); 
		}
	else if(system_power_flag == 3)
		{
		if(system_flag_table.run_mode == 0)
			{
			menu_status_dispplay();
			OLED_Refresh_Gram(); 
			}
		return;
		}
		

	}


       if(need_to_powerdownflag == 1)
       	{
			{
			To_lower_timercnt = 0;
			if((key == 1)||(key == 2))
				key_type = KEY_UP;
			else if(key == 3)
				key_type = KEY_ENTER;
	      		 system_power_down_process();
			 //need_to_powerdownflag = 0;	 
				return;
			}
       	}

		
#endif
	if(enter_perss == 3 || enter_perss == 4)
		{
		return ;
		}

	  if((system_power_flag == SYSTEM_POWER_STANBY)&&((key !=5)&&(key !=6)))
	  	{
//			  	usb_delay_timer_poweroff = 
			  	return;
	  	}

 switch(key)	
	{
	case 1:// up
		key_type = KEY_UP;
		if((home_flag!=3)&&(KeyTab[keyTabIndex].subnumber != 'S'))
			OLED_Fill(0, 0, 127, 63, 0);
		if(enter_perss >0)
			{
			if(KeyTab[keyTabIndex].subnumber == 'H')
				{
				if(home_flag<=1)
					menu_item_index--;
				}
			else if(KeyTab[keyTabIndex].subnumber == 'C')
				RTC_Get(0);
			else if((KeyTab[keyTabIndex].subnumber != 'G')&&(KeyTab[keyTabIndex].subnumber != 'S'))
				get_prev();				
			}
		else
			{	      
			get_prev();

			printf("\r\n %s",KeyTab[keyTabIndex].MenuName);
		#if 0
			if((KeyTab[keyTabIndex].MenuID >= MENU_RIDE_TABLE)&&(KeyTab[keyTabIndex].MenuID <=MENU_SETTING_COOR))
				menu_id_show_timer = 100;
			else
				menu_id_show_timer = 10;
		#endif	
			}
			KeyTab[keyTabIndex].CurrentOperate();	  
		break;

	case 2://down
		key_type = KEY_DOWN;	
		if((home_flag!=3)&&(KeyTab[keyTabIndex].subnumber != 'S'))
			OLED_Fill(0, 0, 127, 63, 0);
		if(enter_perss>0)
			{
			if(KeyTab[keyTabIndex].subnumber == 'H')
				{
				if(home_flag<=1)
					menu_item_index++;
				}
			else if(KeyTab[keyTabIndex].subnumber == 'C')
				RTC_Get(1);
			else if((KeyTab[keyTabIndex].subnumber != 'G')&&(KeyTab[keyTabIndex].subnumber != 'S'))
				get_next();


			}
		else
		{
		 
		get_next();

#if 0
 		if((KeyTab[keyTabIndex].MenuID >= MENU_RIDE_TABLE)&&(KeyTab[keyTabIndex].MenuID <=MENU_SETTING_COOR))
			menu_id_show_timer = 100;
		else
			menu_id_show_timer = 10;
#endif	      
			printf("\r\n %s",KeyTab[keyTabIndex].MenuName);
		}
	  
		KeyTab[keyTabIndex].CurrentOperate();	
		break;
	case 3://OK
		key_type = KEY_ENTER;	
		if((home_flag!=3)&&(KeyTab[keyTabIndex].subnumber != 'G'))
			OLED_Fill(0, 0, 127, 63, 0);
	//	   OLED_Clear();
		if(KeyTab[keyTabIndex].subnumber ||(menu_change_index == KeyTab[keyTabIndex].MenuID)|| (enter_perss >0))
			{
			if(enter_perss)
				{
				if((KeyTab[keyTabIndex].subnumber == 2)||(KeyTab[keyTabIndex].ParMenuID == MENU_TRANSFORMATION))
					{				
					//keyTabIndex  = ParMenuID_loop;
					menu_change_index = KeyTab[keyTabIndex].MenuID;	
					}
				else if(KeyTab[keyTabIndex].subnumber == 'H')
					{
					home_menu_item_index = home_menu_item_index+menu_item_index;
						menu_item_index = 0; 
			
					if(home_flag == 1)
						home_flag  = 2;
						
					}

					
				}
			else
				{
				if(KeyTab[keyTabIndex].subnumber == 2)
					ParMenuID_loop = KeyTab[keyTabIndex].MenuID;			
				}
			
		//		if(KeyTab[keyTabIndex].subnumber == 'G')
		//			enter_perss =1 ;
		//	       else
			       	
			       if(menu_setting_index == 0xff)
			       	{
			       	if((enter_perss)&&(home_flag==0))
						enter_perss =0;
					else
						enter_perss  = 1;
			       	}
			
			}
	//	 if(menu_setting_index != 0xff)
	//		enter_perss  = 1;
		
		KeyTab[keyTabIndex].CurrentOperate();	  
		printf("\r\nkey 3  enter_perss  :%d",enter_perss);
		break;
	case 4:

			key_type = KEY_LONG_ENTER;	
			enter_perss = 2;
			if((KeyTab[keyTabIndex].subnumber == 'H')||(KeyTab[keyTabIndex].subnumber == 'G')||(KeyTab[keyTabIndex].subnumber == 'S'))
				{
				if((KeyTab[keyTabIndex].subnumber == 'H'))
					{
					if((home_flag == 0))//&&(gpsx.fixmode == 3))
						home_flag =1; 	
					else if(home_flag == 1)
						home_flag  = 2;

					}
				KeyTab[keyTabIndex].CurrentOperate();	
				//enter_perss = 0;

				}
			printf("\r\nkey 4");
			
		break;

	case 5:
		printf("\n\r MCU_STBY ");
		if(need_to_powerdownflag == 0)
			{
	       	if(system_power_flag == SYSTEM_POWER_STANBY)
      	 		{

#if 0      		 		
				if(USB_CONNECT == 1)
					{
					//printf("\r\nUSB_CONNECT:1"); 
			#if 1								
					LED0 = 1;	
					Choose_power_mode(MCU_RUN_USB);
					change_clk_need_int(1);
					USB_Interrupts_Config();					
			#endif			
					system_flag_table.charger_connected = 1;
					
					
					}
				else
#endif					
					{
				//	if(system_flag_table.charger_connected == 1)
					USB_Interrupts_Config_disable();
					system_flag_table.charger_connected = 0;
					LED0 = 0;
					Choose_power_mode(MCU_RUN);	
					change_clk_need_int(0);
	
					
					//	menu_keyprocess(0xff);	
					}
					system_power_flag = SYSTEM_POWER_ON;	
					  old_div_cnt = 0;
					  old_spped_div_cnt = 0;
					  old_altiude_div_cnt = 0;
					stm_write_eerpom(MENU_POWER_STATE_INDEX_ADDRES ,0);						
					OLED_Clear();
					OLED_Display_On();

				
					{
						
						Movie_Show_Img(13, 10,BMP_logo);
						OLED_ShowString(60,30,"0.8"); 
						OLED_Refresh_Gram();	 
						Init_BMP085(); 
						di_and_flash_empet(2);
						//delay_ms(1200);
						delay_ms(1200);
						keyTabIndex = MENU_MAIN;
						KeyTab[keyTabIndex].CurrentOperate();	
						Send_to_goto_lowpower();
					
					}
				keyTabIndex = MENU_MAIN;
					
       		}
			else
			{
			//if(USB_CONNECT == 0)
				{
				OLED_Clear();
				need_to_powerdownflag =1;
				}
			}
			
			return ;
			}
			
		break;

	case 6:
		if(system_power_flag == SYSTEM_POWER_STANBY)
      	 		{
      	 		menu_keyprocess(5);
			stm_write_eerpom(MENU_FRIST_POWER_INDEX_ADDRES ,0x0);
			system_flag_table.powerdown_timers = 0;
			stm_write_eerpom(MENU_POWERDOWNTIME_INDEX_ADDRES ,system_flag_table.powerdown_timers|0x12000000);
			return;	
			}
		else
			SoftReset();
		break;
      case 7:
		keyTabIndex = MENU_SETTING_COOR;
		guji_icon_index = 1;
		enter_perss = 2;
		if(system_flag_table.run_mode == 0 )
			{
				system_flag_table.run_mode  =1;
				stm_write_eerpom(MENU_SETTING_RUNMODEL_ADDRES ,1);
				GPS_power_ctrl(1);
				USART_Cmd(USART2, ENABLE);
			}
			
		KeyTab[keyTabIndex].CurrentOperate();	
		
	  	break;

	}

 //	if((KeyTab[keyTabIndex].subnumber == 'S')&&(menu_setting_index == MENU_SETTING_THR_PAGE)&&((key_type == KEY_UP)||(key_type == KEY_DOWN)))
//		;
//	else
	
		  key_type = 0xff;
	
if(home_flag == 3)
  {
 #if 1
  if(menu_info_update == 20)
  	{

	sprintf((char *)dtbuf,"_");
	if(input_index<9)
		OLED_ShowString(input_jinweidu_post[input_index],32,dtbuf);
	else
		OLED_ShowString(input_jinweidu_post[input_index],48,dtbuf);
//	LED0=!LED0; 	 
	menu_info_update = 0;
  	}
  else if(menu_info_update == 10)
  	{
		sprintf((char *)dtbuf,"%c",input_memery[input_index]);
	if(input_index<9)
		OLED_ShowString(input_jinweidu_post[input_index],32,dtbuf);
	else
		OLED_ShowString(input_jinweidu_post[input_index],48,dtbuf);

		menu_info_update++; 
	//	LED0=!LED0; 	 
  	}
  else  	
  	menu_info_update++; 
  }
#endif

	if((KeyTab[keyTabIndex].subnumber != 'C')&&(enter_perss == 0)&&(KeyTab[keyTabIndex].subnumber != 'S'))
		{
		KeyTab[keyTabIndex].CurrentOperate();	  	
		}
	else if((KeyTab[keyTabIndex].MenuID == MENU_TRANSFORMATION)||(KeyTab[keyTabIndex].ParMenuID == MENU_TRANSFORMATION))
		KeyTab[keyTabIndex].CurrentOperate();	  	
	 OLED_Refresh_Gram(); 	

	
}







