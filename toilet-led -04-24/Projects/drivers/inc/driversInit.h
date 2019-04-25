#ifndef __DRIVERS_INIT_H
#define __DRIVERS_INIT_H
#include "stm32f10x.h"
#include "systick.h"
#include "led.h"
#include "usart.h"
#include "switch.h"
#include "spi.h"
#include "sx1276.h"
#include "F8L10_LORA.h"

void init_usart(void);
void init_SysClk(void);
void init_led(void);
void init_switch(void);
void init_di(void);
void init_timer(uint16_t arr,uint16_t period,uint16_t pulse_red,uint16_t pulse_blue,uint16_t pulse_green);
void init_timer2(void);
void init_spi(void);
void init_lora(void);
void init_F8L10_LORA(void);
#endif
