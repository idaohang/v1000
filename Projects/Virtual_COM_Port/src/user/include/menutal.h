#ifndef __MENUTAL_H
#define __MENUTAL_H	 

void menu_keyprocess(u8 key);
void menu_real_speed_display(void);
void menu_main_display(void);
void menu_temp_change_display(void);	
void menu_Pressure_display(void);
void menu_speed_change_display(void);
void menu_3D_display(void);
void menu_home_display(void);
void save_tsp_s(void);
void save_tsp_tp(void);
void menu_chaoxi_display(void);
void menu_guiji_display(void);
void save_the_interest_place(u32 lat1, char latitude1_flag,u32 lng1, char longitude1_flag,u8 index);
void Recording_guji(void);
void  show_levev_space(void);
void Send_to_goto_lowpower(void);
void write_flash(void);
void restore_pass(void);
void guji_Distance(void);	
void menu_enter_flash(u16 my_timer_flash);
void check_eeprom_init(void);
void menu_3d_seq(void);
void menu_status_dispplay(void);
extern int16_t Max_arry(int16_t* prt,u8 index);
extern int16_t Min_arry(int16_t* prt,u8 index);
extern s32 Max_arry32(s32* prt,u8 index);
extern s32 Min_arry32(s32* prt,u8 index);
extern void di_and_flash_empet(u8 times);

#define SYSTEM_POWER_STANBY 0
#define SYSTEM_POWER_ON 1
#define SYSTEM_POWER_LOW  2
#define SYSTEM_POWER_LOW_2  3
#define SYSTEM_POWER_ON_USB 4
typedef struct 
{	//公历日月年周
	u16 w_year;
	u8  w_month;
	u8  w_date;
	u8  week;		 
	u8  sec;		 
}tm;					 


extern tm my_timer;
void check_time(void);


#define RECORED_IDLE 0
#define RECORED_START 1
#define RECORED_START_DOING 2
#define RECORED_T  3
#define RECORED_STOP 4


#endif

