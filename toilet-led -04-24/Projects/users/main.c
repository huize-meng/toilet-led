#include "stm32f10x.h"
#include "driversInit.h"
#include "systick.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "timer.h"
#include "F8L10_LORA.h"
#include "stdlib.h"
#include 	"F8L10_TASK.h"

#define		read_di    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)
#define   def_timer_count     0x01;
extern __IO uint32_t _localTimeTick_ms;
uint8_t mcu_data;
uint8_t di_flag;
uint16_t timer3_period;
uint16_t timer3_count; 
typedef struct
{
	  uint8_t  flag;
	  uint8_t  address;
		uint8_t  led_status;
	  uint8_t  color;
	  uint8_t  wink_status;
	  uint8_t  di_status;
}LED_Status;
LED_Status LED_status;
//�ϴ�֡
typedef struct
{
		uint8_t  start;
    uint8_t  address;
	  uint8_t  command;
	  uint8_t  led_status;
	  uint8_t  color;
	  uint8_t  wink_status;
	  uint8_t  di_status;
	  uint8_t  def;
	  uint8_t  end;
}LED_Frame;
volatile uint8_t toilet_id;
uint8_t lora_recv_flag = 0x01;
uint8_t lora_temp[128];
volatile uint8_t upload_flag;
void Lora_Rec(void);
void initDrivers(void);
void wookloop(void);
void lora_upload_con(LED_Status *STATUS);
void rain_bow(uint16_t intervel,uint16_t change);
void init_iwdg(uint8_t pre_scaler_value, uint32_t reload_value)
{
    /*���üĴ���*/
    IWDG->KR = 0x5555;  //�ر�д������������IWDG_PR��IWDG_RLR�Ĵ�����д�����ݡ�
    IWDG->PR = pre_scaler_value;        //����Ԥ��Ƶ���ӣ�4 == 0100,��ʾ���Ƶ����Ϊ64
    IWDG->RLR = reload_value;           //������װ��ֵ����ʽ��T(ms) == [(4 * 2^pre_Scaler) * reload] / 40;

    /*��ʼ��ι��һ��*/
    IWDG->KR = 0xAAAA;  //��һ��ι����

    /*�������Ź�*/
    IWDG->KR = 0xCCCC;  
}

//ι��API
void feed_iwdg(void)
{
    IWDG->KR = 0xAAAA;//ι��;
}

/*****************************************	
        //ɫ������
				init_timer(64,1000,500,000,000);//��
	      Delay_ms(1000);		
			  init_timer(64,1000,500,250,000);//��
				Delay_ms(1000);		
				init_timer(64,1000,500,500,000);//��
	      Delay_ms(1000);		
			  init_timer(64,1000,0x00,500,000);//��
				Delay_ms(1000);					
				init_timer(64,1000,0x00,0x00,500);//��
	      Delay_ms(1000);		
			  init_timer(64,1000,0x00,500,500);//��
				Delay_ms(1000);					
			  init_timer(64,1000,250,000,500);//��
				Delay_ms(1000);					
*****************************************/
int receiveByteFromMCU(enum BOARD_USART_TYPE usart_no,unsigned char recv)
{
		return recv;
}

void loraSendDataTest(void)	
{
		uint8_t databuff[] = {0x01,0x02,0x03};
		Lora_Send(databuff, 4);//����ָ����������
}

void  lora_recv(void)//����lora����
{
		uint8_t len;
	  uint8_t i;
    len = Lora_GetNumofRecData();	//��ȡ���ݽ��ճ���
	  if(len)
		{
			  memset(lora_temp,0,len);
				Lora_RecDataGet(lora_temp, len);//��ȡָ���������ݵ�����
			  for(i = 0; i < len; i++)
	      {
						usart_sendchar (USART2,lora_temp[i]);
				}
				RF_RECEIVE();
		}
		
}
void led_turnoff(void)
{
		init_timer(64,1000,000,000,000);//close
		Delay_ms(50);
}

//������Ӧ��
void led_control(uint8_t color)
{
		switch(color)//��ɫ��
		{
				case 0x00:
											init_timer(64,1000,000,000,000);//colourless
				break;
				case 0x01:
											init_timer(64,1000,500,000,000);//red
				break;
				case 0x02:
											init_timer(64,1000,000,500,000);//green
				break;
				case 0x03:
											init_timer(64,1000,0x00,0x00,500);//blue
				break;
				case 0x04:
											init_timer(64,1000,500,500,000);//yellow
				break;
				case 0x05:
											init_timer(64,1000,250,000,500);//purple
				break;
				default:
											init_timer(64,1000,000,000,000);//close
				break;
		}	
		Delay_ms(50);
}

//led��˸����
void led_wink_control(uint8_t color,uint8_t interval)
{
		led_control(color);//������
	  if(interval != 0x00)//��˸
		{
				Delay_ms(50);//���ȳ���ʱ��
				led_turnoff();
				Delay_ms(interval *100);
		}
		//����˸
}

//led��˸����ʱ�䣨�������+����ʱ�䣩-- Ĭ����˸���ؿ���(��������ʱ����������)
uint8_t sys_flag = 0;
uint32_t temp;//ÿ1msʵʱ����һ��
void led_wink_con(uint8_t color,uint8_t wink,uint8_t interval,uint8_t period)
{
	  if(period == 0)//һֱ��˸
		{
				led_wink_control(color,interval);
			  //memset(lora_temp,0,128);//��������
		}
		else
		{
			  if(!sys_flag)//��ǰ����ʱ�俪ʼ��־
				{
						temp = local_ticktime();
						sys_flag = 0x01;
				}	
				if(timeout(temp,period *1000))//��˸--��ʱ
				{
						if(wink == 0x02)
						{
								led_control(color);//��ʱ����
							  //usart_sendchar(USART2,color);
						}
						else if (wink ==0x03)
						{
							  led_turnoff();
						}
						sys_flag = 0x00;
						_localTimeTick_ms = 0x00;
						memset(lora_temp,0,128);//��������
						return ;
				}
				else
				{
						led_wink_control(color,interval);//δ��ʱ��˸
					  //usart_sendchar(USART2,0XBB);
				}
		}
}
//led��������
void led_breathe_control(uint8_t color,uint8_t interval)
{
	  uint8_t i;
	  uint8_t speed = 0x01;
		switch (color)
		{
				case 0x00:
											init_timer(64,1000,000,000,000);//colourless
				break;
				case 0x01:	
											for(i = 0;i <= 250;i++)
											{
													init_timer(64,1000,2*i,000,000);//red
												  Delay_ms(speed);
											}
											Delay_ms(interval *100);
											for(i = 0;i <= 250;i++)
											{
													init_timer(64,1000,500 - 2*i,000,000);//red
												  Delay_ms(speed);
											}
				break;
				case 0x02:    
											for(i = 0;i <= 250;i++)
											{
													init_timer(64,1000,000,2*i,000);//green
												  Delay_ms(speed);
											}
											Delay_ms(interval *100 );
					            for(i = 0;i <= 250;i++)
											{
													init_timer(64,1000,000,500 - 2*i,000);//green
												  Delay_ms(speed);
											}											
				break;
				case 0x03:		
											for(i = 0;i <= 250;i++)
											{
													init_timer(64,1000,000,000,2*i);//blue
												  Delay_ms(speed);
											}
											Delay_ms(interval *100);
											for(i = 0;i <= 250;i++)
											{
													init_timer(64,1000,000,000,500 - 2*i);//blue
												  Delay_ms(speed);
											}
				break;
				case 0x04:
											for(i = 0;i <= 250;i++)
											{
													init_timer(64,1000,2*i,2*i,000);//yellow
												  Delay_ms(speed);
											}
											Delay_ms(interval *100 );
											for(i = 0;i <= 250;i++)
											{
													init_timer(64,1000,500 -2*i,500 - 2*i,000);//yellow
												  Delay_ms(speed);
											}					
				break;
				case 0x05:
					            for(i = 0;i <= 250;i++)
											{
													init_timer(64,1000,i,000,2*i);//red
												  Delay_ms(speed);
											}
											Delay_ms(interval *100 );
										  for(i = 0;i <= 250;i++)
											{
													init_timer(64,1000,250 - i,000,500 - 2*i);//purple
												  Delay_ms(speed);
											}					
				break;
				default:
											init_timer(64,1000,000,000,000);//close
				break;
		}
		Delay_ms(interval *100);
}

//led��������ʱ�䣨�������+����ʱ�䣩-- Ĭ�Ϻ������ؿ���(��������ʱ����������)
void led_breathe_con(uint8_t color,uint8_t wink,uint8_t interval,uint8_t period)
{
	  if(period == 0)//һֱ��˸
		{
				led_breathe_control(color,interval);
		}
		else
		{
			  if(!sys_flag)//��ǰ����ʱ�俪ʼ��־
				{
						temp = local_ticktime();
						sys_flag = 0x01;
				}	
				if(timeout(temp,period *1000))//��˸--��ʱ
				{
						if(wink == 0x05)
						{
								led_control(color);//��ʱ����
						}
						else if (wink ==0x06)
						{
							  led_turnoff();
						}
						sys_flag = 0x00;
						_localTimeTick_ms = 0x00;
						memset(lora_temp,0,128);//��������
						return ;
				}
				else
				{
						led_breathe_control(color,interval);//δ��ʱ��˸
				}
		}
}

//led�ʺ�ƿ���
void led_rainbow_control(uint8_t color,uint8_t intervel)
{
			  uint8_t i;		
	      uint8_t speed = 1;
	      for( i = 0;i <= 250;i++)//��ɫ->��ɫ
			  {
						init_timer(64,1000,2*i,0,000);
					  Delay_ms(speed);
				}
				Delay_ms(intervel *100);	
			  for( i = 0;i <= 250;i++)//��ɫ->��ɫ
			  {
						init_timer(64,1000,500,i,000);
					  Delay_ms(speed);
				}
				Delay_ms(intervel *100);	
				
			  for( i = 0;i <= 250;i++)//��ɫ->��ɫ
			  {
						init_timer(64,1000,500,250+i,000);
					  Delay_ms(speed);
				}
				Delay_ms(intervel *100);					
				
			  for( i = 0;i <= 250;i++)//��ɫ->��ɫ
			  {
						init_timer(64,1000,500 - 2*i,500,000);
					  Delay_ms(speed);
				}
				Delay_ms(intervel *100);					

			  for( i = 0;i <= 250;i++)//��ɫ->��ɫ
			  {
						init_timer(64,1000,000,500 - 2*i,2 * i);
					  Delay_ms(speed);
				}
				Delay_ms(intervel *100);					
				
			  for( i = 0;i <= 250;i++)//��ɫ->��ɫ
			  {
						init_timer(64,1000,000,2 * i,500);
					  Delay_ms(speed);
				}
				Delay_ms(intervel *100);	

			  for( i = 0;i <= 250;i++)//��ɫ->��ɫ
			  {
						init_timer(64,1000,i,500 - 2*i,500);
					  Delay_ms(speed);
				}
				Delay_ms(intervel *100);	
				
//			  for( i = 0;i <= 250;i++)//��ɫ->��ɫ
//			  {
//						init_timer(64,1000,250+i,0,500 - 2*i);
//					  Delay_ms(change);
//				}
//				Delay_ms(intervel);	
			  for( i = 0;i <= 250;i++)//��ɫ->��ɫ
			  {
						init_timer(64,1000,250 - i,0,500 - 2*i);
					  Delay_ms(speed);
				}
//				Delay_ms(intervel);	

}

//led�ʺ����ʱ�䣨�������+����ʱ�䣩-- Ĭ�ϲʺ翪�ؿ���(��������ʱ����������)
void led_rainbow_con(uint8_t color,uint8_t wink,uint8_t interval,uint8_t period)
{
	  if(period == 0)//һֱ��˸
		{
				led_rainbow_control(color,interval);
			  //memset(lora_temp,0,128);//��������
		}
		else
		{
			  if(!sys_flag)//��ǰ����ʱ�俪ʼ��־
				{
						temp = local_ticktime();
						sys_flag = 0x01;
				}	
				if(timeout(temp,period *1000))//��˸--��ʱ
				{
						if(wink == 0x08)
						{
								led_control(color);//��ʱ����
							  //usart_sendchar(USART2,color);
						}
						else if (wink ==0x09)
						{
							  led_turnoff();
						}
						sys_flag = 0x00;
						_localTimeTick_ms = 0x00;
						memset(lora_temp,0,128);//��������
						return ;
				}
				else
				{
						led_rainbow_control(color,interval);//δ��ʱ��˸
				}
		}
}
//��˸���ؿ��� -- ���������������ɫ����
void led_wink(uint8_t color,uint8_t wink,uint8_t interval,uint8_t period)
{
		if(wink == 0x00)
		{
				led_control(color);//����˸
		}
		else if(wink == 0x01)
		{
				led_wink_control(color,interval);//������˸��ֱ���յ�ֹͣ�ź�
			  //memset(lora_temp,0,128);//��������
		}
		else if(wink == 0x02 || wink == 0x03)
		{
				led_wink_con(color,wink,interval,period);
	  }		
		/*��Ӻ�����*/
		else if(wink == 0x04)
		{
				led_breathe_control(color,interval);//����������ֱ���յ�ֹͣ�ź�
		}
		else if(wink == 0x05 || wink == 0x06)
		{
				led_breathe_con(color,wink,interval,period);
	  }			
		/*��Ӳʺ��*/
		else if(wink == 0x07)
		{
				led_rainbow_control(color,interval);//�����ʺ磬ֱ���յ�ֹͣ�ź�
		}
		else if(wink == 0x08 || wink == 0x09)
		{
				led_rainbow_con(color,wink,interval,period);
	  }				
}
//�ƿ��Ʋ���
void led_operate(uint8_t light,uint8_t color,uint8_t wink,uint8_t interval,uint8_t period)
{
		if(light == 0x00)
		{
				led_turnoff();//�رյ�
			  memset(lora_temp,0,128);//��������
		}
		else if( light == 0x64)
		{
				led_wink(color,wink,interval,period);
		}
		
}
void  led_setting(uint16_t up_period,uint8_t di_lora_flag)
{
		if(di_lora_flag == 0x64)
		{
				lora_recv_flag = 0x01;
		}
		else
		{
				lora_recv_flag = 0x00;
		}
		//��Ӷ�ʱ�����������ú���
		
		if(up_period != 0x00)//���ò�����Ч
		{
				timer3_count = up_period;
		}
		else
		{
				timer3_count = def_timer_count;
		}
		upload_flag = 0x00;
		memset(lora_temp,0,128);//��������
}
void led_status_recv(LED_Status *STATUS ,uint8_t *lora_data)
{
		    STATUS->flag = 0X01;
	      STATUS->led_status = lora_data[3];
	      STATUS->color = lora_data[4];
	      STATUS->wink_status = lora_data[5];
	      STATUS->di_status = 0x00;
}
//2018-03-15 ģ�鷢�ͺ���
void  lora_upload(void)
{
	  uint8_t i;
	  LED_Frame LED_frame;
	  char *hexTemp = (char *)malloc(sizeof(LED_frame) * 2);
	  LED_frame.start = 0XA5;
	  LED_frame.address =  toilet_id;
		LED_frame.command = 0x0B;
		LED_frame.led_status = LED_status.led_status;
		LED_frame.color  = LED_status.color;
		LED_frame.wink_status = LED_status.wink_status;
		LED_frame.di_status = LED_status.di_status;
		LED_frame.def  = 0x00;
		LED_frame.end  = 0x5A;
	  
	  for( i = 0;i < sizeof(LED_frame);i++ )
	  {
				sprintf(hexTemp + 2*i, "%2x", *(&(LED_frame.start)+i));
		}
		F8L10D_SendHEX((uint8_t *)hexTemp);
		
}

//lora����֡����
void led_frame_verify(uint8_t  *lora_data)
{
		if(lora_data[0] == 0xA5 && lora_data[1] == toilet_id && lora_data[8] == 0x5A)
		{
			  if(lora_data[2] == 0x0E)//�ƿ��Ʋ���
				{
							uint8_t light,color,wink,interval,period;
							light =  lora_data[3];
							color =  lora_data[4];
							wink =  lora_data[5];
							interval =  lora_data[6];
							period =  lora_data[7];
					    lora_recv_flag = 0x00;//��ֹdi����
					    led_status_recv(&LED_status,lora_data);//��ȡ��ǰ�����µ�״̬
							led_operate(light,color,wink,interval,period);
//					    led_operate(lora_data[3],lora_data[4],lora_data[5],lora_data[6],lora_data[7]);
				}	
			 else if(lora_data[2] == 0x0D)//��di���Ƽ���״̬�ϴ�����
			 {
						uint8_t up_period,di_lora_flag;
				    di_lora_flag  =  lora_data[3];
				    up_period = (uint16_t)((lora_data[4] << 8) + lora_data[5]);
				    LED_status.flag = 0x00;
				    LED_status.di_status = di_lora_flag;
				    timer3_period = up_period;
				    led_setting(up_period,di_lora_flag);
			 }
			 else if(lora_data[2] == 0x0C)
			 {
						lora_upload_con(&LED_status);
			 }
				//lora_recv_flag = 0x00;
		}
}
//����di����
void di_control(void)
{
	  if(lora_recv_flag)
		{
				if(read_di)
				{
						init_timer(64,1000,0x00,500,000);//��
					  Delay_ms(100);
				}
				else
				{
						init_timer(64,1000,500,000,000);//��
					  Delay_ms(100);
				}
		}
//		else
//		{
////				init_timer(64,1000,000,000,000);//�ر�
////			  Delay_ms(250);
//		}
}
//loraģ�鶨ʱ�ϴ���ǰ״̬��Ϣ
void lora_upload_con(LED_Status *STATUS)
{
//״̬��Ϣ������״̬��ɫ�ʡ���˸״̬��DI����״̬��
/*�ڴ˴����Lora���ͺ���*/
		if(STATUS->flag)//������Ʋ���״̬
		{
			//��ӷ��ͺ����ṹ��
				lora_upload();/*************************************************************/
		}
		else
		{
			  STATUS->flag = 0x00;
				if(STATUS->di_status == 0x64)
				{
						if(read_di)
						{
								STATUS->color = 0X02;//��
						}
						else
						{
								STATUS->color = 0x01;//��
						}
						STATUS->wink_status = 0X00;
						STATUS->led_status = 0X64;//Ĭ�ϵƿ���
						//��ӷ��ͺ����ṹ�壻
            lora_upload();/***********************************************/						
				}
				else if(STATUS->di_status == 0x00)
				{
						STATUS->led_status = 0x00;//Ϩ�� 
					  STATUS->color = 0X00;//��ɫ
					  STATUS->wink_status = 0x00;//����˸
					//��ӷ��ͺ����ṹ��
					  lora_upload();/******************************************/
				}
		}
}
//��ʱ�ϴ���������
void lora_upload_operate(void)
{
		if(upload_flag)
		{
				lora_upload_con(&LED_status);
			  upload_flag = 0x00;
		}
}

int main()
{   
		initDrivers();
		wookloop();
}

void initDrivers(void) 
{
	  init_SysClk();
		__disable_irq();
	  Systick_Init();
    init_led();	
	  init_usart();
	  init_timer2();//��ȷ��ʱ
	  init_timer3();//���ڶ�ʱ�ϴ�״̬��Ϣ(Ĭ�ϼ��min/�� ,����6s/��)
		init_spi();
	  init_lora();
	  init_F8L10_LORA();
	  init_di();
		//init_iwdg(IWDG_Prescaler_64,625*10);//1s:(64\40K)*625 *10= 10s
	  //init_timer(64,1000,500,500,500);
	  init_switch();//�����������ʼ��
		__enable_irq();
}
void F8L10D_Test(void);
void led_bre_test(void)
{
		uint8_t i;
	  for(i = 0 ;i<5;i++)
	  {
				led_breathe_control(1,1);
		}
}
void wookloop(void)
{
	  Delay_ms(1000);
		toilet_id = switch_read();
	  usart_sendchar(USART2,toilet_id);  //�豸��ַ 
    LED_status.address = toilet_id;  //����豸��ַ
	  timer3_count = def_timer_count;	 //Ĭ���ϱ�����ʱ��
	  timer3_period = def_timer_count;
		
		while(1)
		{
			  //led_bre_test();   
//				sacnTaskRecvBuff();
			    F8L10D_Test();
			  //feed_iwdg();
			  lora_recv();	//ģ�����
			  led_frame_verify(lora_temp);	//�������ݴ���
  			di_control();  //di���� -100ms��ʱ
			  //usart_sendchar(USART2,mcu_data);
       // lora_upload_operate();
		}
}
