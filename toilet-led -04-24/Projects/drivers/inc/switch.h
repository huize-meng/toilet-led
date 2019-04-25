#ifndef __SWITCH_H__
#define __SWITCH_H__
#include <stdio.h>
#include "stm32f10x.h"

/*
 @brief LED编号
 */
enum Switch_NO {
	Switch_NO_1	= 0,
	Switch_NO_2 = 1,
	Switch_NO_3 = 2,
	Switch_NO_4 = 3,
	Switch_NO_5	= 4,
	Switch_NO_6 = 5,
	Switch_NO_7 = 6,
	Switch_NO_8 = 7,
};

/*
 @brief LED的最大数量
 */
#define Switch_MAX_SIZE (10)

/*
 @brief LED定义
 */
struct SwitchDefine {
	enum Switch_NO switch_no;
	// PIN_GROUP（GPIOA ...）
	GPIO_TypeDef* pin_group;
	// PIN（GPIO_Pin_1 ...）
	uint16_t pin;
};

/*
 @brief 初始化所有LED
 @param leds LED设置参数
 @param len LED的数量
 */
uint8_t switch_read(void);
uint8_t Switch_Configuration(const struct SwitchDefine* switchs, uint8_t len);
#endif

