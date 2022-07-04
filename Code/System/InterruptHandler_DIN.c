#include "stm32f37x.h"
#include "CommFlagDef.h"
#include "Struct.h"
#include "extern_includes.h"


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
  * @brief  This function handles PendSVC exception.
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
void SysTick_Handler(void)
{
  
}


/******************************************************************************/
/*                 STM32L1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l1xx_md.s).                                            */
/******************************************************************************/

//      This function handles DMA Transfer Complete interrupt request.

void DMA1_Channel1_IRQHandler    (void)
{

}



// Timer 2 Handler

void TIM2_IRQHandler(void)
{ 
  
  TIM2->SR =0;   // Should be cleared at least few intruction before return else its not cleared and the intrrrupt comes twice
  ProcessMainInterrupt();
 
}

// Timer 4 Handler

void TIM4_IRQHandler(void)
{ 
  
  TIM4->SR =0;   
  Process1SecInterrupt();
 
}

// Timer 3 Handler

void TIM3_IRQHandler(void) 
{ 
  
  TIM3->SR =0;   
  ProcessFreqCapture();
 
}

uint8_t Dummy;
  

void USART3_IRQHandler(void)
{ 
  if(USART3->ISR & 0X800)
  {
    USART3->ICR|=0X800;
    Timer.End_Frame=1;
    
  }
  else if(USART3->ISR & 1)
  {
    Timer.ParityError =1;
    USART3->ICR |=1;
  }
  else
  {
    if(ReceiveLength>=(MAX_BYTE_TO_RECIEVE-2))
     Dummy = USART3->RDR;
    else 
    {
      RecieveArray[ReceiveLength] = USART3->RDR;
      ReceiveLength++;
    }
  }
}



void DMA1_Channel2_IRQHandler(void)
{
  if(DMA1->ISR & DMA_ISR_TCIF2)
  {
    Timer.transfercomplete=1;
  }
  else if(DMA1->ISR & DMA_ISR_TEIF2)
  {
    Timer.TransmissionFailed = 1;
  }
  DMA1_Channel2->CCR &=~ 0x0A;
  DMA1->IFCR |= 0x10;
  SWITCH_OFF_LED_COMM;
}




