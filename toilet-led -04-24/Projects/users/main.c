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
//上传帧
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
    /*配置寄存器*/
    IWDG->KR = 0x5555;  //关闭写保护，可以向IWDG_PR和IWDG_RLR寄存器中写入数据。
    IWDG->PR = pre_scaler_value;        //配置预分频因子，4 == 0100,表示与分频因子为64
    IWDG->RLR = reload_value;           //配置重装载值，公式：T(ms) == [(4 * 2^pre_Scaler) * reload] / 40;

    /*初始化喂狗一次*/
    IWDG->KR = 0xAAAA;  //第一次喂狗。

    /*启动看门狗*/
    IWDG->KR = 0xCCCC;  
}

//喂狗API
void feed_iwdg(void)
{
    IWDG->KR = 0xAAAA;//喂狗;
}

/*****************************************	
        //色彩配置
				init_timer(64,1000,500,000,000);//红
	      Delay_ms(1000);		
			  init_timer(64,1000,500,250,000);//橙
				Delay_ms(1000);		
				init_timer(64,1000,500,500,000);//黄
	      Delay_ms(1000);		
			  init_timer(64,1000,0x00,500,000);//绿
				Delay_ms(1000);					
				init_timer(64,1000,0x00,0x00,500);//蓝
	      Delay_ms(1000);		
			  init_timer(64,1000,0x00,500,500);//靛
				Delay_ms(1000);					
			  init_timer(64,1000,250,000,500);//紫
				Delay_ms(1000);					
*****************************************/
int receiveByteFromMCU(enum BOARD_USART_TYPE usart_no,unsigned char recv)
{
		return recv;
}

void loraSendDataTest(void)	
{
		uint8_t databuff[] = {0x01,0x02,0x03};
		Lora_Send(databuff, 4);//发送指定长度数据
}

void  lora_recv(void)//开启lora接收
{
		uint8_t len;
	  uint8_t i;
    len = Lora_GetNumofRecData();	//获取数据接收长度
	  if(len)
		{
			  memset(lora_temp,0,len);
				Lora_RecDataGet(lora_temp, len);//读取指定长度数据到数组
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

//开启相应灯
void led_control(uint8_t color)
{
		switch(color)//灯色彩
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

//led闪烁控制
void led_wink_control(uint8_t color,uint8_t interval)
{
		led_control(color);//开启灯
	  if(interval != 0x00)//闪烁
		{
				Delay_ms(50);//亮度持续时间
				led_turnoff();
				Delay_ms(interval *100);
		}
		//不闪烁
}

//led闪烁持续时间（包含间隔+持续时间）-- 默认闪烁开关开启(包含持续时间结束后操作)
uint8_t sys_flag = 0;
uint32_t temp;//每1ms实时递增一次
void led_wink_con(uint8_t color,uint8_t wink,uint8_t interval,uint8_t period)
{
	  if(period == 0)//一直闪烁
		{
				led_wink_control(color,interval);
			  //memset(lora_temp,0,128);//结束控制
		}
		else
		{
			  if(!sys_flag)//当前持续时间开始标志
				{
						temp = local_ticktime();
						sys_flag = 0x01;
				}	
				if(timeout(temp,period *1000))//闪烁--超时
				{
						if(wink == 0x02)
						{
								led_control(color);//超时常亮
							  //usart_sendchar(USART2,color);
						}
						else if (wink ==0x03)
						{
							  led_turnoff();
						}
						sys_flag = 0x00;
						_localTimeTick_ms = 0x00;
						memset(lora_temp,0,128);//结束控制
						return ;
				}
				else
				{
						led_wink_control(color,interval);//未超时闪烁
					  //usart_sendchar(USART2,0XBB);
				}
		}
}
//led呼吸控制
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

//led呼吸持续时间（包含间隔+持续时间）-- 默认呼吸开关开启(包含持续时间结束后操作)
void led_breathe_con(uint8_t color,uint8_t wink,uint8_t interval,uint8_t period)
{
	  if(period == 0)//一直闪烁
		{
				led_breathe_control(color,interval);
		}
		else
		{
			  if(!sys_flag)//当前持续时间开始标志
				{
						temp = local_ticktime();
						sys_flag = 0x01;
				}	
				if(timeout(temp,period *1000))//闪烁--超时
				{
						if(wink == 0x05)
						{
								led_control(color);//超时常亮
						}
						else if (wink ==0x06)
						{
							  led_turnoff();
						}
						sys_flag = 0x00;
						_localTimeTick_ms = 0x00;
						memset(lora_temp,0,128);//结束控制
						return ;
				}
				else
				{
						led_breathe_control(color,interval);//未超时闪烁
				}
		}
}

//led彩虹灯控制
void led_rainbow_control(uint8_t color,uint8_t intervel)
{
			  uint8_t i;		
	      uint8_t speed = 1;
	      for( i = 0;i <= 250;i++)//无色->红色
			  {
						init_timer(64,1000,2*i,0,000);
					  Delay_ms(speed);
				}
				Delay_ms(intervel *100);	
			  for( i = 0;i <= 250;i++)//红色->橙色
			  {
						init_timer(64,1000,500,i,000);
					  Delay_ms(speed);
				}
				Delay_ms(intervel *100);	
				
			  for( i = 0;i <= 250;i++)//橙色->黄色
			  {
						init_timer(64,1000,500,250+i,000);
					  Delay_ms(speed);
				}
				Delay_ms(intervel *100);					
				
			  for( i = 0;i <= 250;i++)//黄色->绿色
			  {
						init_timer(64,1000,500 - 2*i,500,000);
					  Delay_ms(speed);
				}
				Delay_ms(intervel *100);					

			  for( i = 0;i <= 250;i++)//绿色->蓝色
			  {
						init_timer(64,1000,000,500 - 2*i,2 * i);
					  Delay_ms(speed);
				}
				Delay_ms(intervel *100);					
				
			  for( i = 0;i <= 250;i++)//蓝色->靛色
			  {
						init_timer(64,1000,000,2 * i,500);
					  Delay_ms(speed);
				}
				Delay_ms(intervel *100);	

			  for( i = 0;i <= 250;i++)//靛色->紫色
			  {
						init_timer(64,1000,i,500 - 2*i,500);
					  Delay_ms(speed);
				}
				Delay_ms(intervel *100);	
				
//			  for( i = 0;i <= 250;i++)//紫色->红色
//			  {
//						init_timer(64,1000,250+i,0,500 - 2*i);
//					  Delay_ms(change);
//				}
//				Delay_ms(intervel);	
			  for( i = 0;i <= 250;i++)//紫色->无色
			  {
						init_timer(64,1000,250 - i,0,500 - 2*i);
					  Delay_ms(speed);
				}
//				Delay_ms(intervel);	

}

//led彩虹持续时间（包含间隔+持续时间）-- 默认彩虹开关开启(包含持续时间结束后操作)
void led_rainbow_con(uint8_t color,uint8_t wink,uint8_t interval,uint8_t period)
{
	  if(period == 0)//一直闪烁
		{
				led_rainbow_control(color,interval);
			  //memset(lora_temp,0,128);//结束控制
		}
		else
		{
			  if(!sys_flag)//当前持续时间开始标志
				{
						temp = local_ticktime();
						sys_flag = 0x01;
				}	
				if(timeout(temp,period *1000))//闪烁--超时
				{
						if(wink == 0x08)
						{
								led_control(color);//超时常亮
							  //usart_sendchar(USART2,color);
						}
						else if (wink ==0x09)
						{
							  led_turnoff();
						}
						sys_flag = 0x00;
						_localTimeTick_ms = 0x00;
						memset(lora_temp,0,128);//结束控制
						return ;
				}
				else
				{
						led_rainbow_control(color,interval);//未超时闪烁
				}
		}
}
//闪烁开关控制 -- 包含四种情况及颜色配置
void led_wink(uint8_t color,uint8_t wink,uint8_t interval,uint8_t period)
{
		if(wink == 0x00)
		{
				led_control(color);//不闪烁
		}
		else if(wink == 0x01)
		{
				led_wink_control(color,interval);//持续闪烁，直到收到停止信号
			  //memset(lora_temp,0,128);//结束控制
		}
		else if(wink == 0x02 || wink == 0x03)
		{
				led_wink_con(color,wink,interval,period);
	  }		
		/*添加呼吸灯*/
		else if(wink == 0x04)
		{
				led_breathe_control(color,interval);//持续呼吸，直到收到停止信号
		}
		else if(wink == 0x05 || wink == 0x06)
		{
				led_breathe_con(color,wink,interval,period);
	  }			
		/*添加彩虹灯*/
		else if(wink == 0x07)
		{
				led_rainbow_control(color,interval);//持续彩虹，直到收到停止信号
		}
		else if(wink == 0x08 || wink == 0x09)
		{
				led_rainbow_con(color,wink,interval,period);
	  }				
}
//灯控制操作
void led_operate(uint8_t light,uint8_t color,uint8_t wink,uint8_t interval,uint8_t period)
{
		if(light == 0x00)
		{
				led_turnoff();//关闭灯
			  memset(lora_temp,0,128);//结束控制
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
		//添加定时器周期性配置函数
		
		if(up_period != 0x00)//配置参数有效
		{
				timer3_count = up_period;
		}
		else
		{
				timer3_count = def_timer_count;
		}
		upload_flag = 0x00;
		memset(lora_temp,0,128);//结束控制
}
void led_status_recv(LED_Status *STATUS ,uint8_t *lora_data)
{
		    STATUS->flag = 0X01;
	      STATUS->led_status = lora_data[3];
	      STATUS->color = lora_data[4];
	      STATUS->wink_status = lora_data[5];
	      STATUS->di_status = 0x00;
}
//2018-03-15 模块发送函数
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

//lora接收帧解析
void led_frame_verify(uint8_t  *lora_data)
{
		if(lora_data[0] == 0xA5 && lora_data[1] == toilet_id && lora_data[8] == 0x5A)
		{
			  if(lora_data[2] == 0x0E)//灯控制操作
				{
							uint8_t light,color,wink,interval,period;
							light =  lora_data[3];
							color =  lora_data[4];
							wink =  lora_data[5];
							interval =  lora_data[6];
							period =  lora_data[7];
					    lora_recv_flag = 0x00;//禁止di控制
					    led_status_recv(&LED_status,lora_data);//获取当前命令下灯状态
							led_operate(light,color,wink,interval,period);
//					    led_operate(lora_data[3],lora_data[4],lora_data[5],lora_data[6],lora_data[7]);
				}	
			 else if(lora_data[2] == 0x0D)//灯di控制及灯状态上传周期
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
//本地di控制
void di_control(void)
{
	  if(lora_recv_flag)
		{
				if(read_di)
				{
						init_timer(64,1000,0x00,500,000);//绿
					  Delay_ms(100);
				}
				else
				{
						init_timer(64,1000,500,000,000);//红
					  Delay_ms(100);
				}
		}
//		else
//		{
////				init_timer(64,1000,000,000,000);//关闭
////			  Delay_ms(250);
//		}
}
//lora模块定时上传当前状态信息
void lora_upload_con(LED_Status *STATUS)
{
//状态信息包含灯状态、色彩、闪烁状态、DI开启状态；
/*在此处添加Lora发送函数*/
		if(STATUS->flag)//进入控制参数状态
		{
			//添加发送函数结构体
				lora_upload();/*************************************************************/
		}
		else
		{
			  STATUS->flag = 0x00;
				if(STATUS->di_status == 0x64)
				{
						if(read_di)
						{
								STATUS->color = 0X02;//绿
						}
						else
						{
								STATUS->color = 0x01;//红
						}
						STATUS->wink_status = 0X00;
						STATUS->led_status = 0X64;//默认灯开启
						//添加发送函数结构体；
            lora_upload();/***********************************************/						
				}
				else if(STATUS->di_status == 0x00)
				{
						STATUS->led_status = 0x00;//熄灭 
					  STATUS->color = 0X00;//无色
					  STATUS->wink_status = 0x00;//不闪烁
					//添加发送函数结构体
					  lora_upload();/******************************************/
				}
		}
}
//定时上传处理函数；
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
	  init_timer2();//精确延时
	  init_timer3();//用于定时上传状态信息(默认间隔min/次 ,测试6s/次)
		init_spi();
	  init_lora();
	  init_F8L10_LORA();
	  init_di();
		//init_iwdg(IWDG_Prescaler_64,625*10);//1s:(64\40K)*625 *10= 10s
	  //init_timer(64,1000,500,500,500);
	  init_switch();//必须放在最后初始化
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
	  usart_sendchar(USART2,toilet_id);  //设备地址 
    LED_status.address = toilet_id;  //添加设备地址
	  timer3_count = def_timer_count;	 //默认上报数据时间
	  timer3_period = def_timer_count;
		
		while(1)
		{
			  //led_bre_test();   
//				sacnTaskRecvBuff();
			    F8L10D_Test();
			  //feed_iwdg();
			  lora_recv();	//模组接收
			  led_frame_verify(lora_temp);	//接收数据处理
  			di_control();  //di控制 -100ms延时
			  //usart_sendchar(USART2,mcu_data);
       // lora_upload_operate();
		}
}
