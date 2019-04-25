#ifndef  _SYSTICK_H
#define  _SYSTICK_H
#include "stm32f10x.h"

typedef enum {FALSE = 0, TRUE = !FALSE} bool;

void Systick_Init(void);
uint32_t local_ticktime(void) ;
void TimingDelay_Decrement(void);
bool timeout(uint32_t last_time, uint32_t ms);
#endif
