#include "timer.h"

void init_timer3(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
    //清除中断标志位
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
 
    TIM_TimeBaseStructure.TIM_Prescaler = 64000-1;	
    TIM_TimeBaseStructure.TIM_Period = 6000;//60000;//n * 60000ms = n *1min
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
    //开启中断
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    //开启外设
	  TIM_Cmd(TIM3,ENABLE);
	
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

volatile uint32_t timer_count;

void Delay_ms(volatile uint32_t count)
{
		timer_count = count;
	  while(timer_count);
}

