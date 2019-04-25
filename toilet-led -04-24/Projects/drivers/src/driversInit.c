#include "stm32f10x.h"
#include "driversInit.h"

#define ARRAY_SIZE(array, type) (sizeof(array)/sizeof(type))
	
extern int receiveByteFromMCU(enum BOARD_USART_TYPE usart_no,unsigned char recv);
extern int receiveByteFromF8L10D_LORA(enum BOARD_USART_TYPE usart_no,unsigned char recv);
void RCC_Configuration(void) 
{ 
	RCC_ClocksTypeDef RCC_Clocks;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|	RCC_APB2Periph_GPIOB | 
												 RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_GetClocksFreq(&RCC_Clocks);
	
//	SysTick_Config(RCC_Clocks.SYSCLK_Frequency / 100);		
	NVIC_SetPriority (SysTick_IRQn, 1);	
}

void init_SysClk(void)
{
	SystemInit();
	RCC_Configuration();
}

void init_led(void)
{
	struct LedDefine conf[] = {
		{ RED_LED, GPIOA, GPIO_Pin_10, LEDOFF }, 
		{ GREEN_LED, GPIOA, GPIO_Pin_8,  LEDOFF },
		{ BLUE_LED, GPIOA, GPIO_Pin_9,  LEDOFF }
	};
	LED_Configuration( conf, ARRAY_SIZE( conf, struct LedDefine ) );
}

void init_usart(void)
{
	struct USARTDefine conf[] = {		
	{ BOARD_USART_2,USART2, GPIOA, GPIO_Pin_2, GPIOA, GPIO_Pin_3,9600,receiveByteFromMCU},    //  MCU		
	{ BOARD_USART_3,USART3, GPIOB, GPIO_Pin_10, GPIOB, GPIO_Pin_11,115200,receiveByteFromF8L10D_LORA},    //四信模块
	};
	USART_Configuration( conf, ARRAY_SIZE( conf, struct USARTDefine ) );
}

void init_switch(void)
{
	
	struct SwitchDefine conf[] = {
		{ Switch_NO_1,GPIOB, GPIO_Pin_9}, 
		{ Switch_NO_2,GPIOB, GPIO_Pin_8},
		{ Switch_NO_3,GPIOB, GPIO_Pin_7},
		{ Switch_NO_4,GPIOB, GPIO_Pin_6}, 
		{ Switch_NO_5,GPIOB, GPIO_Pin_5},
		{ Switch_NO_6,GPIOB, GPIO_Pin_4},		
		{ Switch_NO_7,GPIOB, GPIO_Pin_3}, 
		{ Switch_NO_8,GPIOA, GPIO_Pin_15},
		
	};
	Switch_Configuration( conf, ARRAY_SIZE( conf, struct SwitchDefine ) );
	
}
void init_di(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;                      //定义GPIO_InitTypeDef结构体
//		EXTI_InitTypeDef EXTI_InitStructure;  
//		NVIC_InitTypeDef NVIC_InitStructure;
	                                     
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);     //开启GPIOB和复用功能时钟                   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;                 //选择GPIO_Pin_15
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;             //选择上拉输入模式
    GPIO_Init(GPIOB, &GPIO_InitStructure); 

//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);//选择EXTI信号源                                                                                        
//    EXTI_InitStructure.EXTI_Line = EXTI_Line15;                 //中断线选择
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;      		//EXTI为中断模式
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  		//下降沿触发
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;                		//使能中断
//    EXTI_Init(&EXTI_InitStructure); 
//	
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);           //配置NVIC优先级分组为1
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;      //中断源
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级：1
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //子优先级：1
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //使能中断通道
//    NVIC_Init(&NVIC_InitStructure);

}
void init_timer(uint16_t arr,uint16_t period,uint16_t pulse_red,uint16_t pulse_green,uint16_t pulse_blue)
{
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		TIM_OCInitTypeDef TIM_OCInitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
		/* PA9设置为功能脚(PWM) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
		TIM_DeInit(TIM1);
		/*TIM1时钟配置*/
		TIM_TimeBaseStructure.TIM_Prescaler = arr - 1;	//预分频(时钟分频)64M/64=1000K
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
		TIM_TimeBaseStructure.TIM_Period = period;       //装载值 1000k/1000=1Khz
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;
		TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
		
		/* Channel 1 Configuration in PWM mode */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //PWM模式2
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //正向通道有效
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;//反向通道无效
		TIM_OCInitStructure.TIM_Pulse = pulse_green;        //占空时间
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性           有效电平
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; //互补端的极性 
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
		TIM_OC1Init(TIM1,&TIM_OCInitStructure); //通道1
		/* Channel 2 Configuration in PWM mode */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //PWM模式2
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //正向通道有效
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;//反向通道无效
		TIM_OCInitStructure.TIM_Pulse = pulse_blue;        //占空时间
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性           有效电平
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; //互补端的极性 
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
		TIM_OC2Init(TIM1,&TIM_OCInitStructure); //通道2
		/* Channel 3 Configuration in PWM mode */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //PWM模式2
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //正向通道有效
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;//反向通道无效
		TIM_OCInitStructure.TIM_Pulse = pulse_red;        //占空时间
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性           有效电平
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; //互补端的极性 
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
		TIM_OC3Init(TIM1,&TIM_OCInitStructure); //通道3

		TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //
		TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //
		TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //

		/* TIM1 Main Output Enable */
		TIM_CtrlPWMOutputs(TIM1,ENABLE);
		/* TIM1 counter enable */
		TIM_Cmd(TIM1,ENABLE);
}

void init_timer2(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    //清除中断标志位
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
 
    TIM_TimeBaseStructure.TIM_Prescaler = 64000-1;	
    TIM_TimeBaseStructure.TIM_Period = 1;//1ms
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
    //开启中断
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    //开启外设
	  TIM_Cmd(TIM2,ENABLE);
	
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}



//lora接收模组
void init_lora(void)
{
		Lora_Init();
}
//lora接收模组采用spi通信
void init_spi(void)
{
	struct SPIDefine conf[]={
		{SPI1, GPIOA, GPIO_Pin_5, GPIOA, GPIO_Pin_6, GPIOA, GPIO_Pin_7},
	};
	SPI_Configration( conf, ARRAY_SIZE( conf, struct SPIDefine ) );
}
//lora发送模组，采用串口通信
void init_F8L10_LORA(void)
{
		F8L10_LORA_Init();
}
