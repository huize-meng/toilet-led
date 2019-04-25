#include "switch.h"
#include <string.h>
#include "usart.h"

#define   READ_1   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)
#define   READ_2   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)
#define   READ_3   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)
#define   READ_4   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)
#define   READ_5   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)
#define   READ_6   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
#define   READ_7   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)
#define   READ_8   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)

struct SwitchDefine _switchs_defines[Switch_MAX_SIZE];

uint8_t Switch_Configuration(const struct SwitchDefine* switchs, uint8_t len)
{
    uint8_t l;
    GPIO_InitTypeDef	GPIO_InitStructure;
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|	RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE );
    memset( (void*)&_switchs_defines[0], 0, sizeof(_switchs_defines) );
	  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);//复用引脚
	
    for (l = 0;l < len; ++l )
    {
        const struct SwitchDefine def = switchs[l];
        _switchs_defines[def.switch_no] = def;

        GPIO_InitStructure.GPIO_Pin 	= def.pin;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IPD;
        GPIO_Init(def.pin_group, &GPIO_InitStructure);
    }
    return 0;
}
//读取拨码开关地址
uint8_t switch_read(void)
{
		uint16_t data;
	  data = (uint16_t)(GPIO_ReadInputData(GPIOB) & 0X03F8) >> 0x02;
    if(READ_8)
		{
				data += 0x01;
		}
		else
		{
				data += 0x00;
		}
	  return (uint8_t)data;
}

//uint8_t switch_read(void)
//{
//	
//			usart_sendchar(USART2,READ_1);
//		  usart_sendchar(USART2,READ_2);
//			usart_sendchar(USART2,READ_3);
//			usart_sendchar(USART2,READ_4);
//			usart_sendchar(USART2,READ_5);
//		  usart_sendchar(USART2,READ_6);
//			usart_sendchar(USART2,READ_7);
//			usart_sendchar(USART2,READ_8);
//	    return 0;
//	
//}



