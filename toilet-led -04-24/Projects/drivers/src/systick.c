#include "systick.h"
#include "schd.h"
//uint32_t TimingDelay;


void Systick_Init(void)
{
//        if(SysTick_Config(64000))//����Ϊ 1 ����
//        {
//            while(1);
//        }
    SysTick_Config(64000);//����Ϊ 1 ����
}
/*
 SysTick����ʱ�䣬���ڼ���ʱ������ÿ1ms����һ�Σ�
 */
__IO uint32_t _localTimeTick_ms = 0;
#define SYSTEMTICK_PERIOD_MS  0x01
void SysTick_Handler(void) {
	_localTimeTick_ms += SYSTEMTICK_PERIOD_MS;
	// ���ö�ʱ������
	Schd_Run(_localTimeTick_ms);
}

uint32_t local_ticktime(void) {
	return _localTimeTick_ms;
}

bool timeout(uint32_t last_time, uint32_t ms) {
	return (bool)((_localTimeTick_ms - last_time) > ms);
}

