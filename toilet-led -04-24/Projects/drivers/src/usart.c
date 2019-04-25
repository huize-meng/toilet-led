#include "usart.h"
#include "stdio.h"

USART_RECV_CALLBACK _usart_recv_callbacks[6] = { 0 };

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

void _usart_nvic_config(uint32_t usart_irqn, uint32_t priority,uint32_t sub) {
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = usart_irqn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = priority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = sub;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
		
uint32_t USART_Configuration(const struct USARTDefine* configs, uint16_t len) {
	  uint8_t u = 0;
	  GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;	
for( u = 0; u < len; ++u) {
		struct USARTDefine config = configs[u];	
		if(config.usartdef == USART2){
       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟
       RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟//|RCC_APB2Periph_AFIO
       RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//使能GPIOA时钟
				_usart_recv_callbacks[BOARD_USART_2] = config.recvcallback;
		}
		else if(config.usartdef == USART3)
		{
       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOB时钟
       RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
				_usart_recv_callbacks[BOARD_USART_3] = config.recvcallback;			
		}
		GPIO_InitStructure.GPIO_Pin = config.pin_of_txd;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(config.pin_of_txd_group, &GPIO_InitStructure);
		
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin = config.pin_of_rxd;
		GPIO_Init(config.pin_of_rxd_group, &GPIO_InitStructure);
		
		USART_InitStructure.USART_BaudRate = config.bandrate;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl =
		USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
		USART_Init(config.usartdef, &USART_InitStructure);
		
		if( config.recvcallback != 0 ) { 
			USART_ITConfig(config.usartdef,USART_IT_RXNE,ENABLE);
		  if(config.usartdef == USART2 ) {				
				_usart_nvic_config( USART2_IRQn, 1,1);
			} 
			else if(config.usartdef == USART3){
				_usart_nvic_config( USART3_IRQn, 1,2);
			}
		}
		USART_Cmd(config.usartdef, ENABLE);		
	}
	  return 0;
}

void _usart_comm_irqhandler(USART_TypeDef* usarttypedef,
		enum BOARD_USART_TYPE usart_type) {
	unsigned char recv_char;
	if (USART_GetFlagStatus(usarttypedef, USART_FLAG_RXNE) != RESET) {
		USART_ClearFlag(usarttypedef,USART_IT_RXNE); //一定要清除接收中断  
		recv_char = USART_ReceiveData(usarttypedef);
		if (_usart_recv_callbacks[usart_type] != NULL)
			_usart_recv_callbacks[usart_type](usart_type, recv_char);
	}
	
}
void USART2_IRQHandler(void) {
	_usart_comm_irqhandler(USART2, BOARD_USART_2);
}
void USART3_IRQHandler(void) {
	_usart_comm_irqhandler(USART3, BOARD_USART_3);
}

void usart_sendchar(USART_TypeDef* usart_typedef, uint8_t sChar) {
	USART_SendData(usart_typedef, (uint8_t) sChar);
	while (USART_GetFlagStatus(usart_typedef, USART_FLAG_TXE) == RESET) {
	}
}

void usart_sendstring(USART_TypeDef* usart_typedef, uint8_t* string, uint8_t length) {
	uint8_t* p = string;
	uint8_t len;
	if(length==0){
		while( *p++ != '\0' ){
			USART_SendData(usart_typedef, (uint8_t)(*p));
			while (USART_GetFlagStatus(usart_typedef, USART_FLAG_TXE) == RESET) {
			}
		}
	}
	else
	{
		for(len=0;len<length;len++){
			USART_SendData(usart_typedef, (uint8_t)(*p));
			p++;
			while (USART_GetFlagStatus(usart_typedef, USART_FLAG_TXE) == RESET) {
			}
		}
	}
}

PUTCHAR_PROTOTYPE {
	USART_SendData(USART2, (uint8_t) ch);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	return ch;
}

