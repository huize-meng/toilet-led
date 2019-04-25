/**
  ******************************************************************************
  * @file    PWR/PVD/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "systick.h"
#include "sx1276.h"
//#include "stm32_eval.h"
extern uint8_t di_flag;
extern uint16_t timer3_period;
extern volatile uint16_t timer3_count;
extern volatile uint8_t upload_flag;
extern volatile uint32_t  timer_count;
/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup PWR_PVD
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
//	TimingDelay_Decrement();
//}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles the PVD Output interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)                       //EXTI_Line:9...5
{
  if(EXTI_GetITStatus(EXTI_Line13)!= RESET)  
  {  
			SX1278_Interupt();
			EXTI_ClearITPendingBit(EXTI_Line13);
  }   
//  else if(EXTI_GetITStatus(EXTI_Line15)!= RESET)  
//  {  
//    EXTI_ClearITPendingBit(EXTI_Line15);
//		di_flag = 0x01;
//  }   
}

void PVD_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line16) != RESET)
  {
    /* Toggle LED1 */
    /* Clear the Key Button EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line16);
  }
}

//--------------------stm32f10x_it.c------------------------
//中断函数中自己编写
void TIM2_IRQHandler(void)
{
    //判断TIM2更新中断是否发生
    if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
    {
        //必须清楚标志位
				TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
			  timer_count --;
		}
}		

//中断函数中自己编写
void TIM3_IRQHandler(void)
{
    //判断TIM2更新中断是否发生
    if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)
    {
        //必须清楚标志位
				TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
			  if(timer3_count)
				{
						timer3_count --;
				}
				else
				{
					  timer3_count = timer3_period;
					  upload_flag = 0x01;
				}			
		}
}		
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
