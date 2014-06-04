#include "flash.h" 
#include "spi.h"
#include "delay.h"
#include "stm_eeprom.h"
#include "sys.h"
#include "usart.h"
#include "menutal.h"
#include "hw_config.h"


//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//W25X16 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/13 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
 

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25X16
//����Ϊ2M�ֽ�,����32��Block,512��Sector 

//��ʼ��SPI FLASH��IO��
void SPI_Flash_Init(void)
{
#if 0
	RCC->APB2ENR|=1<<2;       //PORTAʱ��ʹ�� 	    
	//���� 
	GPIOA->CRL&=0XFFF0F0FF; 
	GPIOA->CRL|=0X00030300;//PA2.3.4 ���� 	    
	GPIOA->ODR|=0X5<<2;    //PA2.3.4����
#endif
	SPIx_Init();		   //��ʼ��SPI
}  

//��ȡSPI_FLASH��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	SPI_FLASH_CS=0;                            //ʹ������   
	SPIx_ReadWriteByte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPIx_ReadWriteByte(0Xff);             //��ȡһ���ֽ�  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     
	return byte;   
} 
//дSPI_FLASH״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void SPI_FLASH_Write_SR(u8 sr)   
{   
	SPI_FLASH_CS=0;                            //ʹ������   
	SPIx_ReadWriteByte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
	SPIx_ReadWriteByte(sr);               //д��һ���ֽ�  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
}   
//SPI_FLASHдʹ��	
//��WEL��λ   
void SPI_FLASH_Write_Enable(void)   
{
	SPI_FLASH_CS=0;                            //ʹ������   
    SPIx_ReadWriteByte(W25X_WriteEnable);      //����дʹ��  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
} 
//SPI_FLASHд��ֹ	
//��WEL����  
void SPI_FLASH_Write_Disable(void)   
{  
	SPI_FLASH_CS=0;                            //ʹ������   
    SPIx_ReadWriteByte(W25X_WriteDisable);     //����д��ָֹ��    
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
} 			    
//��ȡоƬID W25X16��ID:0XEF14
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	SPI_FLASH_CS=0;				    
	SPIx_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	 			   
	Temp|=SPIx_ReadWriteByte(0xFF)<<8;  
	Temp|=SPIx_ReadWriteByte(0xFF);	 
	SPI_FLASH_CS=1;				    
	return Temp;
}   		    
//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;    												    
	SPI_FLASH_CS=0;                            //ʹ������   
    SPIx_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����   
    SPIx_ReadWriteByte((u8)((ReadAddr)>>16));  //����24bit��ַ    
    SPIx_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPIx_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPIx_ReadWriteByte(0XFF);   //ѭ������  
    }
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
}  
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    SPI_FLASH_Write_Enable();                  //SET WEL 
	SPI_FLASH_CS=0;                            //ʹ������   
    SPIx_ReadWriteByte(W25X_PageProgram);      //����дҳ����   
    SPIx_ReadWriteByte((u8)((WriteAddr)>>16)); //����24bit��ַ    
    SPIx_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPIx_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPIx_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ 
	SPI_Flash_Wait_Busy();					   //�ȴ�д�����
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)  		   
u8 SPI_FLASH_BUF[4096];
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	
	secpos=WriteAddr/4096;//������ַ 0~511 for w25x16
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   

	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			SPI_Flash_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//д����������  

		}else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};	 	 
}
//��������оƬ
//��Ƭ����ʱ��:
//W25X16:25s 
//W25X32:40s 
//W25X64:40s 
//�ȴ�ʱ�䳬��...
void SPI_Flash_Erase_Chip(void)   
{                                             
    SPI_FLASH_Write_Enable();                  //SET WEL 
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_CS=0;                            //ʹ������   
    SPIx_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
	SPI_Flash_Wait_Busy();   				   //�ȴ�оƬ��������
}   
//����һ������
//Dst_Addr:������ַ 0~511 for w25x16
//����һ��ɽ��������ʱ��:150ms
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{   
	Dst_Addr*=4096;
    SPI_FLASH_Write_Enable();                  //SET WEL 	 
    SPI_Flash_Wait_Busy();   
  	SPI_FLASH_CS=0;                            //ʹ������   
    SPIx_ReadWriteByte(W25X_SectorErase);      //������������ָ�� 
    SPIx_ReadWriteByte((u8)((Dst_Addr)>>16));  //����24bit��ַ    
    SPIx_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPIx_ReadWriteByte((u8)Dst_Addr);  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
    SPI_Flash_Wait_Busy();   				   //�ȴ��������
}  
//�ȴ�����
void SPI_Flash_Wait_Busy(void)   
{   
	while ((SPI_Flash_ReadSR()&0x01)==0x01);   // �ȴ�BUSYλ���
}  
//�������ģʽ
void SPI_Flash_PowerDown(void)   
{ 
  	SPI_FLASH_CS=0;                            //ʹ������   
    SPIx_ReadWriteByte(W25X_PowerDown);        //���͵�������  
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
    delay_us(3);                               //�ȴ�TPD  
}   
//����
void SPI_Flash_WAKEUP(void)   
{  
  	SPI_FLASH_CS=0;                            //ʹ������   
    SPIx_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	SPI_FLASH_CS=1;                            //ȡ��Ƭѡ     	      
    delay_us(3);                               //�ȴ�TRES1


}

#if 1
static u16 a_number;
static u16 t_number;
void buffer_Analysis(u8 *buffer,u16 munber)
{
	u16 index = 0,angle;
	u8 year,moth,day,hour,min,sec,lat_flag,lon_flag,record_type;
	u32 lat,log,message_number_index,attiautl;
	float tp_lat,tp_lon,att,temp,per,speed;

	// printf("\r\n buffer_Analysis");	 
	 
	do
	   {
	   message_number_index = (buffer[2+index]|(buffer[1+index]<<8)|(buffer[0+index]<<16));
	   record_type = buffer[3+index];
	    year = buffer[4+index];
	    moth= buffer[5+index];
	    day = buffer[6+index];		
		
	    hour = buffer[7+index];
	    min=   buffer[8+index];
	    sec =  buffer[9+index];		

	   lat= (buffer[13+index]|(buffer[12+index]<<8)|(buffer[11+index]<<16)|(buffer[10+index]<<24));	    	
	   log = (buffer[17+index]|(buffer[16+index]<<8)|(buffer[15+index]<<16)|(buffer[14+index]<<24));	    	
	   attiautl = (buffer[21+index]|(buffer[20+index]<<8)|(buffer[19+index]<<16)|(buffer[18+index]<<24));

	   speed = (buffer[23+index]|(buffer[22+index]<<8));
	   
	    angle=  (buffer[25+index]|(buffer[24+index]<<8));
		
          temp =  (buffer[27+index]|(buffer[26+index]<<8));		

	   per = (buffer[29+index]|(buffer[28+index]<<8));
	   
  	  lat_flag	   = buffer[30+index];
  	  lon_flag	   = buffer[31+index];
	   index = index+32;
		tp_lat = lat;
		tp_lon = log;
		att = attiautl;
		if(record_type == 'S')
			t_number++;
		else if(record_type == 'A')
			a_number++;
		
	    printf("\r %d  %c  %02d-%02d-%02d",message_number_index,record_type,year,moth,day);
	    printf(" %02d:%02d:%02d  %.6f %c",hour,min,sec,tp_lat/1000000,lat_flag);
          printf(" %.6f %c  %.1f  %.1f  %d  %.1f  %.1f",tp_lon/1000000,lon_flag,att/10,speed,angle,temp/10,per/10);	   
	   }
	while((index < munber));
	
	
}

#endif

int Mycrc16(u8 *buffer,int buffer_len)
{
int cksum = 0,i,stat = 0;
cksum = 0; //��ʼ��Ϊ0
for (stat=buffer_len ; stat>0; stat--) //len����Ҫ����ĳ���
	{
		cksum = cksum^(int)(*buffer++) << 8; //�����û�����ס�
		for (i=8; i!=0; i--)
		{
			if (cksum & 0x8000)
				cksum = cksum << 1 ^ 0x1021;
			else
				cksum = cksum << 1;
		}
	}
return cksum;
}
extern uint32_t USART_Rx_ptr_in ;
extern uint32_t USART_Rx_ptr_out ;

void send_packge_to_usb(u8 comd ,u8 *buffer, u32 len)
{
 	u32 i;    
	int cksum = 0;
       u32 len_tmp;
	len_tmp = len;
	if(USB_CONNECT == 1)
	{

	
	USART_To_USB_Send_Data(0x55);		
	USART_To_USB_Send_Data(comd);
	len_tmp= len_tmp+2;
	USART_To_USB_Send_Data((u8)(len_tmp>>24&0xff));		
	USART_To_USB_Send_Data((u8)(len_tmp>>16&0xff));		
	USART_To_USB_Send_Data((u8)(len_tmp>>8&0xff));		
	USART_To_USB_Send_Data((u8)(len_tmp&0xff));		

      if(comd != 0)
      	{
	for(i=0;i<len_tmp-2;i++)
	  	USART_To_USB_Send_Data(buffer[i]);		
	cksum = Mycrc16(buffer, len);
      	}
	USART_To_USB_Send_Data((u8)((cksum>>8)&0xff));		
	USART_To_USB_Send_Data((u8)(cksum&0xff));	
	}
}

//#define 0x00
u8 gps_printf_status = 0;
u8 send_status = 0;
u8 send_status_flag = 0;
void test_read_flash_and_printf(void)
{
	static u32 WriteAddr = 0;
	static u32 NumByteToRead;
	extern u32 Flash_Index;
	static u32 secpos;
	 u16 secoff;
	static u16 secremain;	   
 //	static u16 i;    
//	int cksum = 0;
	
	u8 temp[4] ={0};
	

	switch(send_status)
		{
		case 0:
				NumByteToRead = Flash_Index; 
				WriteAddr = 0;
				secpos=WriteAddr/4096;//������ַ 0~511 for w25x16
				secoff=WriteAddr%4096;//�������ڵ�ƫ��
				secremain=4096-secoff;//����ʣ��ռ��С   
				if(NumByteToRead<=secremain) 
					secremain=NumByteToRead;//������4096���ֽ�

			//	 printf("\r\n INDEX,TAG,  DATE,  TIME  , LATITUDE N/S, LONGITUDE E/W, HEIGHT,SPEED,ANGLE,TEMP,PER");	   

				//temp[0] = (u8)(NumByteToRead>>24&0xff);
				//temp[1] = (u8)(NumByteToRead>>16&0xff);
				//temp[2] = (u8)(NumByteToRead>>8&0xff);
				//temp[3] = (u8)(NumByteToRead&0xff);
				
				send_packge_to_usb(0x00,temp,NumByteToRead);//��һ�����
				send_status = 1;
		break;
		
		case 1:
			{
			if(USART_Rx_ptr_in != USART_Rx_ptr_out)
				return;
			   SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,secremain);//������������������

			//	   printf("");
			//   buffer_Analysis(SPI_FLASH_BUF,secremain);

			//printf("\r\n test_read_flash_and_printf \r\n");
#if 0
			USART_To_USB_Send_Data(0x55);		
			USART_To_USB_Send_Data(0x01);		
			USART_To_USB_Send_Data((u8)((secremain>>8)&0xff));		
			USART_To_USB_Send_Data((u8)(secremain&0xff));		
			    for(i =0;i<secremain;i++)			
			    	{
				    USART_To_USB_Send_Data(SPI_FLASH_BUF[i]);
						
			    	}
			cksum = Mycrc16(SPI_FLASH_BUF, secremain);
			USART_To_USB_Send_Data((u8)((cksum>>8)&0xff));		
			USART_To_USB_Send_Data((u8)(cksum&0xff));		
#endif		
			printf("\r\n NumByteToRead:%d,secremain:%d \r\n ",NumByteToRead,secremain);
			   if(NumByteToRead==secremain)
			   	{
			   //	  if(USART_Rx_ptr_in != ((USART_Rx_ptr_out+1)%2048))
			//	    if(USART_Rx_ptr_in == USART_Rx_ptr_out);	   
			   	  	{
						send_packge_to_usb(0x02,SPI_FLASH_BUF,secremain);//��һ�����
						send_status = 0;
						send_status_flag = 0;
						printf("\r  ����%d���켣��¼��������%d�飬��Ȥ����%d��",Flash_Index/32,t_number,a_number);
			   	  	}
					 break;//д�������
			   	}
			   else//д��δ����
				{
			//		    if(USART_Rx_ptr_in == USART_Rx_ptr_out);	   
					send_packge_to_usb(0x01,SPI_FLASH_BUF,secremain);//��һ�����
					secpos++;//������ַ��1
					secoff=0;//ƫ��λ��Ϊ0 	 
					WriteAddr+=secremain;//д��ַƫ��	   
				   	NumByteToRead-=secremain;				//�ֽ����ݼ�
					if(NumByteToRead>4096)secremain=4096;	//��һ����������д����
					else secremain=NumByteToRead;			//��һ����������д����
					

				}	
			}
         break;
		}
}





void read_gps(void)
{
	gps_printf_status = 1;
	send_status = 0;
	USART_Rx_ptr_out = USART_Rx_ptr_in;
	send_status_flag = 1;

}

void read_gps_next(void)
{
	send_status_flag = 1;
}

void stop_read_gps(void)
{
	gps_printf_status = 0;
	send_status = 0;
	send_status_flag = 0;

}

//read_last_sector()
void read_last_sector(void)
{
u32 writeaddr = 0;
//	extern u32 Flash_Index;
	SPI_Flash_Read(SPI_FLASH_BUF,0x1FF000,1024);//������������������
//	printf("\r  ����%d���켣��¼��������%d�飬��Ȥ����%d��",Flash_Index/32,t_number,a_number);
	printf("\r\n read change vaule \r\n");
      for(writeaddr = 0;writeaddr<1024;writeaddr++)
      	{
      	if(writeaddr%32 == 0)
			printf("\r\n");
	      printf(" %x",SPI_FLASH_BUF[writeaddr]);
      	}
}







void restore_pass(void)
{
u16 i ;
extern int16_t temperature_pass[];
extern int16_t speed_pass[];
extern s32 pressure_pass[];
extern s32 Altitude_pass[];
extern  u8 old_div_cnt;
extern u8 old_spped_div_cnt ;
extern u8 old_altiude_div_cnt ;


extern s32  most_Altitude ;
extern s32 lest_Altitude ;
extern int16_t most_temperature ;
extern int16_t lest_temperature ;
extern int16_t most_speed ;
extern int16_t lest_speed ;
extern s32 most_pressure ;
extern s32 lest_pressure ;

SPI_Flash_Read(SPI_FLASH_BUF,CHANGE_PRE_VAULE_SAVE_ADDRES,4096);//������������������
//memcpy((u8 *)&pressure_pass,SPI_FLASH_BUF,384);
//memcpy((u8 *)&temperature_pass,&SPI_FLASH_BUF[384],192);
//memcpy((u8 *)&speed_pass,&SPI_FLASH_BUF[768],192);
//SPI_Flash_Read((u8 *)&pressure_pass,CHANGE_PRE_VAULE_SAVE_ADDRES,384);
//SPI_Flash_Read((u8 *)&temperature_pass,CHANGE_TMPE_VAULE_SAVE_ADDRES,192);
//SPI_Flash_Read((u8 *)&speed_pass,CHANGE_SPEED_VAULE_SAVE_ADDRES,192);


//if(SPI_FLASH_BUF[4095] == 0x12)
{

//old_div_cnt = SPI_FLASH_BUF[CHANGE_TMPE_.VAULE_INDEX_ADDRES- 0x1FF000];
//old_spped_div_cnt = SPI_FLASH_BUF[CHANGE_SPEED_VAULE_INDEX_ADDRES - 0x1FF000];
//old_altiude_div_cnt = SPI_FLASH_BUF[CHANGE_ALTITUD_VAULE_INDEX_ADDRES - 0x1FF000];
#if  1
      for(i = 0;i<1344;i++)
      	{
      	if(i%32 == 0)
			printf("\r\n [%d] \r",i);
	      printf(" %x",SPI_FLASH_BUF[i]);
      	}
#endif
printf("\r\n old_div_cnt:%d\r\n",old_div_cnt);
printf("\r\n old_spped_div_cnt:%d\r\n",old_spped_div_cnt);
printf("\r\n old_altiude_div_cnt:%d\r\n",old_altiude_div_cnt);
//printf("\r\n CHANGE_TMPE_VAULE_SAVE_ADDRES:%d\r\n",CHANGE_TMPE_VAULE_SAVE_ADDRES - 0x1FF000);
//printf("\r\n �¶�:\r\n");

if(old_div_cnt<=96)
{
for(i = 0;i<96;i++)
	{
	temperature_pass[i] = ((SPI_FLASH_BUF[384+(i*2)])|(SPI_FLASH_BUF[385+(i*2)]<<8));
//	printf("\r SPI_FLASH_BUF[%d] = %d",384+(i*2),SPI_FLASH_BUF[384+(i*2)]);
	//printf("%d ",temperature_pass[i]);
	
	}
#if 1
//printf("\r\n ��ѹ:\r\n");
for(i = 0;i<96;i++)
	{
	pressure_pass[i] = ((SPI_FLASH_BUF[0+(i*4)])|(SPI_FLASH_BUF[1+(i*4)]<<8)|(SPI_FLASH_BUF[2+(i*4)]<<16)|(SPI_FLASH_BUF[3+(i*4)]<<24));
//	printf("%d ",pressure_pass[i]);
//	printf("s%d ",SPI_FLASH_BUF[0+(i*4)]);		
	}
}

if(old_spped_div_cnt <= 96 )
{
//printf("\r\n �ٶ�:\r\n");
for(i = 0;i<96;i++)
	{
	speed_pass[i] = (SPI_FLASH_BUF[768+(i*2)])|(SPI_FLASH_BUF[769+(i*2)]<<8);
//	printf("%d ",speed_pass[i]);
	}
}
if(old_altiude_div_cnt <= 96)
{
for(i = 0;i<96;i++)
	{
	Altitude_pass[i] = ((SPI_FLASH_BUF[960+(i*4)])|(SPI_FLASH_BUF[961+(i*4)]<<8)|(SPI_FLASH_BUF[962+(i*4)]<<16)|(SPI_FLASH_BUF[963+(i*4)]<<24));
//	printf("%x ",Altitude_pass[i]);
//	if(Altitude_pass[i]&0x80000000)
		{
		//Altitude_pass[i] -=1;
	//	Altitude_pass[i] =~Altitude_pass[i] ;
	//	Altitude_pass[i] |=0x80000000;
		}
	}
//printf("\r\n");
#endif
}


		most_temperature = Max_arry(temperature_pass,old_div_cnt);
		lest_temperature = Min_arry(temperature_pass,old_div_cnt);
		
		most_pressure= Max_arry32(pressure_pass,old_div_cnt);
		lest_pressure= Min_arry32(pressure_pass,old_div_cnt);	
		
		most_Altitude= Max_arry32(Altitude_pass,old_altiude_div_cnt);
		lest_Altitude = Min_arry32(Altitude_pass,old_altiude_div_cnt);
		
		most_speed= Max_arry(speed_pass,old_spped_div_cnt);
		lest_speed = Min_arry(speed_pass,old_spped_div_cnt);
	//	printf(" lest_Altitude:%x\r\n",lest_Altitude);
}
}













