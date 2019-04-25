#ifndef __USART_H__
#define __USART_H__

#include "stm32F10x_usart.h"
#include <stm32F10x.h>

#define NULL 0
/*
 * @brief USART���߱��
 */
enum BOARD_USART_TYPE {
	BOARD_USART_1 = 1, BOARD_USART_2 = 2, BOARD_USART_3 = 3,
	BOARD_UART_4 = 4, BOARD_UART_5 = 5,
};

typedef int (*USART_RECV_CALLBACK)(enum BOARD_USART_TYPE usart_no,
		unsigned char recv);

struct USARTDefine {
	enum BOARD_USART_TYPE uart_board_no;
	USART_TypeDef* usartdef;		
	GPIO_TypeDef* pin_of_txd_group;
	uint16_t pin_of_txd;
	GPIO_TypeDef* pin_of_rxd_group;
	uint16_t pin_of_rxd;
	uint32_t bandrate;
	USART_RECV_CALLBACK recvcallback;
};

/*
 @brief ʹ��UART��ʼ���ӿ�
 @param config ��������
 @param len ��ʼ��������
 @return ���ý��(0=SUCCESS, 1=FAILED)
 @desc ��Ҫ������NVIC (@ref BOARD_NVIC_Init) ��RCC
 */
extern uint32_t USART_Configuration(const struct USARTDefine* configs, uint16_t len);

extern void usart_sendchar(USART_TypeDef* usart_typedef, uint8_t sChar);

extern void usart_sendstring(USART_TypeDef* usart_typedef, uint8_t* string, uint8_t length);


#endif
