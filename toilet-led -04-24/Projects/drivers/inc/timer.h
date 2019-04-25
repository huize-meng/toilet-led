#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x.h"
#include "SX1276.h"

void Delay_ms(volatile uint32_t count);//ÑÓ³Ùº¯Êı£¬ÉèÖÃÎª MS
//uint32_t local_ticktime(void);
void init_timer3(void);
#endif
