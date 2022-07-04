/*
******************************************************************************
  * @file    systeminitialization_din.c
  * @author  MRM R&D Team
  * @version V1.0.0
  * @date    07-06-2022
  * @brief   This file contains all the functions definitions 
             for the meter intialization
******************************************************************************
*/


#include "stm32f37x.h"
#include <stdio.h>
#include "stm32f37x_flash.h"
#include "CommFlagDef.h"
#include "extern_includes.h"

void InitI2C(void);
void SystemInit (void);
static void SetSystemTimer(void);
static void Set1SecTimer(void);
static void InitPort(void);
static void SetPwrRegisters(void);
void InitializeHardware(void);
static void FrequencyTimer(void);
static void SetAdc(void);
static void SetSDADC(void);

    
static void InitPort(void)
{ 
  GPIOA->MODER=0X69555555;
  GPIOA->OTYPER=0;
  GPIOA->OSPEEDR=0X3C000000;
  GPIOA->PUPDR=0X24150010;
  GPIOA->AFR[0]=0;
  GPIOA->AFR[1]=0;
  
  GPIOB->MODER=0XF005540F;
  GPIOB->OTYPER=0;
  GPIOB->OSPEEDR=0;
  GPIOB->PUPDR=0X150;
  GPIOB->AFR[0]=0;
  GPIOB->AFR[1]=0;
  
  GPIOC->MODER=0X1A955FF;
  GPIOC->OTYPER=0;
  GPIOC->OSPEEDR=0X540000;
  GPIOC->PUPDR=0;
  GPIOC->AFR[0]=0;
  GPIOC->AFR[1]=0x7720;
  
  GPIOD->MODER=0;
  GPIOD->OTYPER=0;
  GPIOD->OSPEEDR=0;
  GPIOD->PUPDR=0X20;
  GPIOD->AFR[0]=0;
  GPIOD->AFR[1]=0;

  GPIOE->MODER=0xF0000;
  GPIOE->OTYPER=0;
  GPIOE->OSPEEDR=0;
  GPIOE->PUPDR=0x00;
  GPIOE->AFR[0]=0;
  GPIOE->AFR[1]=0x00;
  
  GPIOF->MODER=0xA000;
  GPIOF->OTYPER=0XC0;
  GPIOF->OSPEEDR=0;
  GPIOF->PUPDR=0x5000;
  GPIOF->AFR[0]=0X44000000;
  GPIOF->AFR[1]=0x00;
  
  SWITCH_OFF_LED_COMM;
  GPIOA->BSRR = PORT_BIT_11;

}

static void SetPwrRegisters(void)
{
  
  RCC->AHBENR=0x7E0001;
  RCC->APB2ENR=0x7000201;
  RCC->APB1ENR=0x10440007;
  RCC->CFGR3 |=0x31;
  RCC->CFGR |=0X98000000;
  RCC->APB2RSTR=0x70F5201;  // Reset  Pheripherial Rgegisters
  RCC->APB1RSTR=0x76E6CBFF;
  RCC->APB2RSTR=0;
  RCC->APB1RSTR=0;;
  PWR->CR=0xE00;

}

static void SetDmaRegisters(void)
{
  DMA1_Channel1->CNDTR=0X7; // NO OF DATA TRANSFER
  DMA1_Channel1->CPAR=(uint32_t)&ADC1->DR;
  DMA1_Channel1->CMAR=(uint32_t)&AdcDataInArray;
  DMA1_Channel1->CCR=0X5A1;
  
  //DMA1_Channel1->CPAR=ADC1_BASE+0x4C;
  

}
static void SetAdc(void)
{
  
  ADC1->CR1=0X100;
  //ADC1->CR2=0X1E7109;  //Reset Cal register
  //ADC1->CR2=0X1E7105;  // calibrate
  //while(ADC1->CR2 & 0x04); // wait for calibration to be over
  ADC1->SMPR1=0x36DB6DB6;
  ADC1->SMPR2=0x36DB6DB6;
  ADC1->SQR3=0x16C6B16A;
  ADC1->SQR2=0xa;
  ADC1->SQR1=0x600000;
  ADC1->CR2=0X1E7105;

}


void InitializeHardware(void)
{
  
  SetPwrRegisters();
  InitPort();
  SetAdc();
  SetSystemTimer();
  InitI2C();
  FrequencyTimer();
  Set1SecTimer();
  SetDmaRegisters();
  SetSDADC();
  
}


void InitWatchDog(void)
{
  if(!(IWDG->SR & 1))
  {
    IWDG->KR=0X5555;
    IWDG->PR=0X02;
  }
  if(!(IWDG->SR & 2))
  {
    IWDG->KR=0X5555;
    IWDG->RLR=0XFFF;
  }
  IWDG->KR=0xCCCC; //Start
  DBGMCU->APB1FZ=0x1BF7; // Halt peripherals in debug mode
  DBGMCU->APB2FZ=0x1C;
  DBGMCU->CR=0;  
  
}

void InitI2C(void)
{
   
  I2C2->CR1 = 0x0000;      
  I2C2->CR2 = 0;
  I2C2->OAR1 = 0;   
  I2C2->OAR2 = 0;
  
  I2C2->TIMINGR = 0x10805E89;           // 48Mhhz source, 100Khz operation, analog filter on
   I2C2->CR1 |=0X01;
}


static void SetSystemTimer(void)
{
  
  // TIM2 as a system timmer with 3.2Khz interrupt
  TIM2->CR1=0X4;
  TIM2->CR2=0;
  TIM2->DIER=0X01;
  TIM2->SR=0;
  TIM2->EGR=0;
  TIM2->CCMR1=0;
  TIM2->CCMR2=0;
  TIM2->CCER=0;
  TIM2->PSC=0;
  TIM2->ARR=3750;  // DIVIDE BY 12e6/3750=3200
  TIM2->CR1 |=0X01; // STRT TIMER
  NVIC_EnableIRQ(TIM2_IRQn);  
  
}

static void Set1SecTimer(void)
{
  
  // TIM4 as a system timmer with 1Khz interrupt
  TIM4->CR1=0X04;
  TIM4->CR2=0;
  TIM4->DIER=0X01;
  TIM4->SR=0;
  TIM4->EGR=0;
  TIM4->CCMR1=0;
  TIM4->CCMR2=0;
  TIM4->CCER=0;
  TIM4->PSC=50000;
  TIM4->ARR=240;  // 1Hz
  TIM4->CR1 |=0X01; // STRT TIMER
  NVIC_EnableIRQ(TIM4_IRQn);  
  
}

static void FrequencyTimer(void)
{
  
  // TIM3 as a frequency counter 200Khz
  TIM3->CR1=0X84;
  TIM3->CR2=0;
  TIM3->DIER=0X10; // Capture 4 interrupt enabled
  TIM3->SR=0;
  TIM3->EGR=0;
  TIM3->CCMR2=0x3d3d;  // once every 8 events or 160 milliec at 50 hz
  TIM3->CCMR1=0;
  TIM3->CCER=0x1000;
  TIM3->PSC=59;//DIVIDE BY 12e6/60=200K
  TIM3->ARR=0xffff;  // always up count DIVIDE BY 12e6/60=200K
  TIM3->CR1 |=0X01; // STRT TIMER
  NVIC_EnableIRQ(TIM3_IRQn);  //capture interrupt
  
}

void SetSDADC(void)
{
  SDADC1->CR1=0X4100;
  SDADC1->JCHGR=0X40;
  SDADC1->CONF0R=0X00000;
  
  SDADC2->CR1=0X4000;
  SDADC2->JCHGR=0X100;
  SDADC2->CONF0R=0X00000;
  
  SDADC3->CR1=0X4000;
  SDADC3->JCHGR=0X100;
  SDADC3->CONF0R=0X00000;
  SDADC1->CR2=0X01;
  SDADC2->CR2=0X01;
  SDADC3->CR2=0X01;
  while(SDADC1->ISR & SDADC_ISR_STABIP);
  while(SDADC2->ISR & SDADC_ISR_STABIP);
  while(SDADC3->ISR & SDADC_ISR_STABIP);
//  SDADC1->CR2 |=SDADC_CR2_STARTCALIB;
//  SDADC2->CR2 |=SDADC_CR2_STARTCALIB;
//  SDADC3->CR2 |=SDADC_CR2_STARTCALIB;
//  while(SDADC1->ISR & SDADC_ISR_CALIBIP);
//  while(SDADC2->ISR & SDADC_ISR_CALIBIP);
//  while(SDADC3->ISR & SDADC_ISR_CALIBIP);
}
  
volatile uint16_t TimeOutCommTx;
void InitUart(uint8_t baud,uint8_t parity1,uint8_t Stopbit1)
{
   USART3->RTOR=24;
  if (baud==BaudRate_1200) 
  {
    USART3->BRR=0x2710;
    TimeOutCommTx=(uint16_t)(1.8*12*(float)NO_OF_SAMPLES/1200)+10;
    Check485DirCount=5*NO_OF_SAMPLES;
    
  }
  else if(baud==BaudRate_2400) 
  {
    USART3->BRR=0x1388;
    TimeOutCommTx=(uint16_t)(1.8*12*(float)NO_OF_SAMPLES/2400)+10;
    Check485DirCount=5*NO_OF_SAMPLES;
  }
  else if(baud==BaudRate_4800) 
  {
    USART3->BRR=0x9C4;
    TimeOutCommTx=(uint16_t)(1.8*12*(float)NO_OF_SAMPLES/4800)+8;
    Check485DirCount=3*NO_OF_SAMPLES;
  }
  else if(baud==BaudRate_9600) 
  {
    USART3->BRR=0x4E2;
    TimeOutCommTx=(uint16_t)(1.8*12*(float)NO_OF_SAMPLES/9600)+6;
    Check485DirCount=3*NO_OF_SAMPLES;
  }
  else if(baud==BaudRate_19200) 
  {
    USART3->BRR=0x271;
    TimeOutCommTx=(uint16_t)(1.8*12*(float)NO_OF_SAMPLES/19200)+6;
    Check485DirCount=3*NO_OF_SAMPLES;
  }
  Check485DirCounter=0;
  USART3->CR1 &=~1;
  if (parity1==Parity_Odd) USART3->CR1=0x1600;
  else if(parity1==Parity_Even) USART3->CR1=0x1400;
  else if(parity1==Parity_None) USART3->CR1=0x00;

  if (Stopbit1==Stopbit_one) USART3->CR2=0x00;
  else if(Stopbit1==Stopbit_two) USART3->CR2=0x200;
  RS485Receive;
  SWITCH_OFF_LED_COMM;
  USART3->CR1 |= 0x400002C;            // Transmitter enable, Receiver enable, RXNEIE,Receiver timeout interrupt enable
  USART3->CR2 |=  0x800000;             //Receiver timeout enable 
  USART3->CR3 =  0x80;                 // DMA
  USART3->ICR =  0;
  USART3->CR1 |= 1;
  DMA1_Channel2->CPAR = (uint32_t)&USART3->TDR;
  DMA1_Channel2->CCR = 0x1092;    // Memory inc, read from memory, tcie
  NVIC_EnableIRQ(USART3_IRQn);
  NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  
 
}
  

  

  
  
  
  

  
  


