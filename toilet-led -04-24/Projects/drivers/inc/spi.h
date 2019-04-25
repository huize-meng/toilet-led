#ifndef __SPI_H
#define __SPI_H
#include <stm32f10x.h>
		
#define MAX_SPI_NUM  3		
// SPI总线速度设置 
#define SPI_SPEED_2   0
#define SPI_SPEED_8   1
#define SPI_SPEED_16  2
#define SPI_SPEED_256 3

struct SPIDefine {
	// SPI1,SPI2,SPi3
	SPI_TypeDef* SPIx;
	// PIN_GROUP（GPIOA ...）
	GPIO_TypeDef* pin_of_sck_group;
	// PIN（GPIO_Pin_1 ...）
	uint16_t pin_of_sck;
	GPIO_TypeDef* pin_of_miso_group;
	uint16_t pin_of_miso;
	GPIO_TypeDef* pin_of_mosi_group;
	uint16_t pin_of_mosi;
};	

extern void SPI_Configration( struct SPIDefine* def , uint8_t len );

extern void SPIx_SetSpeed(SPI_TypeDef* SPIx,uint8_t SpeedSet);			//设置SPI速度   

extern uint8_t SPI_ReadWriteByte(SPI_TypeDef* SPIx,uint8_t TxData);	//SPI总线读写一个字节
 		 
#endif

