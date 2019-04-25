#include "spi.h"

void SPI_Configration( struct SPIDefine* def, uint8_t len )
{
	 uint8_t u;
	 GPIO_InitTypeDef GPIO_InitStructure;
   SPI_InitTypeDef  SPI_InitStructure;
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	for( u = 0; u < len; u++ )
	{
		struct SPIDefine config = def[u];	
			GPIO_InitStructure.GPIO_Pin = config.pin_of_miso ;	  
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(config.pin_of_miso_group, &GPIO_InitStructure);
			
			GPIO_InitStructure.GPIO_Pin = config.pin_of_sck ;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(config.pin_of_sck_group, &GPIO_InitStructure);
			
			GPIO_InitStructure.GPIO_Pin = config.pin_of_mosi ;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(config.pin_of_mosi_group, &GPIO_InitStructure);
		  
		  //GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7); 
		
//			GPIO_InitStructure.GPIO_Pin = config.pin_of_miso ;	  
//			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//		
//			GPIO_Init(config.pin_of_miso_group, &GPIO_InitStructure);
//			
//			GPIO_InitStructure.GPIO_Pin = config.pin_of_sck ;
//			GPIO_Init(config.pin_of_sck_group, &GPIO_InitStructure);
//			
//			GPIO_InitStructure.GPIO_Pin = config.pin_of_mosi ;
//			GPIO_Init(config.pin_of_mosi_group, &GPIO_InitStructure);
	
//		if(config.SPIx == SPI1 )
//		{
//			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);		

//			RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);		
//			RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);	
//		}

		SPI_InitStructure.SPI_Direction				 = SPI_Direction_2Lines_FullDuplex;  
		SPI_InitStructure.SPI_Mode 						 = SPI_Mode_Master;		
		SPI_InitStructure.SPI_DataSize 			   = SPI_DataSize_8b;	
		SPI_InitStructure.SPI_CPOL 						 = SPI_CPOL_Low;		
		SPI_InitStructure.SPI_CPHA 						 = SPI_CPHA_1Edge;	
		SPI_InitStructure.SPI_NSS 						 = SPI_NSS_Soft;		
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		
		SPI_InitStructure.SPI_FirstBit 					= SPI_FirstBit_MSB;	
		SPI_InitStructure.SPI_CRCPolynomial 		= 7;	
		SPI_Init(config.SPIx, &SPI_InitStructure);  
	 
		SPI_Cmd(config.SPIx, ENABLE); 
		SPI_ReadWriteByte(config.SPIx,0xff);
	}
}

void SPIx_SetSpeed(SPI_TypeDef* SPIx,uint8_t SpeedSet)
{
	SPIx->CR1 &= 0XFFC7;	
	switch(SpeedSet)
	{
		case SPI_SPEED_2:
			SPIx->CR1|=0<<3;
			break;
		case SPI_SPEED_8:
			SPIx->CR1|=2<<3;
			break;
		case SPI_SPEED_16:
			SPIx->CR1|=3<<3;
			break;
		default:
			SPIx->CR1|=7<<3;
			break;
	}
	SPIx->CR1|=1<<6; 
} 

uint8_t SPI_ReadWriteByte(SPI_TypeDef* SPIx, uint8_t TxData)
{
    uint8_t retry = 0;
	
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET) 
    {
        retry++;
        if (retry > 200)
					return 0;
    }		
    SPI_I2S_SendData(SPIx, TxData); 
    retry=0;
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET)
    {
        retry++;
        if (retry > 200)
					return 0;
    }
    return SPI_I2S_ReceiveData(SPIx); 
}




