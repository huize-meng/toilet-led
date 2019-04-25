#include "F8L10_LORA.h"
#include "timer.h"

#define F8L10_LORA_STATUS_PIN 			  GPIO_Pin_0
#define F8L10_LORA_STATUS_PORT		    GPIOB
#define F8L10_LORA_SLEEP_PIN 			    GPIO_Pin_1
#define F8L10_LORA_SLEEP_PORT 		    GPIOB
#define F8L10_LORA_RST_PIN 			      GPIO_Pin_2
#define F8L10_LORA_RST_PORT 		    	GPIOB
//�����߿��ƣ� �ߵ�ƽ���ѣ� �͵�ƽ���ߣ�
#define  F8L10_LORA_SLEEP_L			   GPIO_ResetBits(F8L10_LORA_SLEEP_PORT, F8L10_LORA_SLEEP_PIN)	     
#define  F8L10_LORA_SLEEP_H			   GPIO_SetBits  (F8L10_LORA_SLEEP_PORT, F8L10_LORA_SLEEP_PIN)	  
// (��λ����)  �͵�ƽ��Ч ������200ms
#define  F8L10_LORA_RST_L		    	 GPIO_ResetBits(F8L10_LORA_RST_PORT, F8L10_LORA_RST_PIN)	     
#define  F8L10_LORA_RST_H			   	 GPIO_SetBits  (F8L10_LORA_RST_PORT, F8L10_LORA_RST_PIN)	
//STATUS ��״ָ̬ʾ�� �ߵ�ƽ���ѣ� �͵�ƽ���ߣ�
#define  F8L10_LORA_STATUS_READ	   GPIO_ReadInputDataBit(F8L10_LORA_STATUS_PORT, F8L10_LORA_STATUS_PIN)	     

//lora����ģ�����ݵ�Ԫ
void F8L10_LORA_GPIO_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
		
		GPIO_InitStructure.GPIO_Pin   =  F8L10_LORA_STATUS_PIN ; //status����������
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init( F8L10_LORA_STATUS_PORT, &GPIO_InitStructure );
	
		GPIO_InitStructure.GPIO_Pin   =  F8L10_LORA_SLEEP_PIN ; //sleep �������
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init( F8L10_LORA_SLEEP_PORT, &GPIO_InitStructure );
		F8L10_LORA_SLEEP_H;//Ĭ��Ϊ�ߵ�ƽ������ģʽ
	
		GPIO_InitStructure.GPIO_Pin   =  F8L10_LORA_RST_PIN ; //rst �������
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init( F8L10_LORA_RST_PORT, &GPIO_InitStructure );
    F8L10_LORA_RST_H;//Ĭ��Ϊ�ߵ�ƽ������λ
	
	
}
void F8L10_LORA_RST(void)
{
		Delay1s(10);//��ʼ�����̲�����
		F8L10_LORA_RST_L;
	  Delay1s(200);
	  F8L10_LORA_RST_H;
	  Delay1s(200);

}

void F8L10_LORA_Init(void)
{
		F8L10_LORA_GPIO_Init();
	  F8L10_LORA_RST();
}
