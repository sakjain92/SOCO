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

/* 
static void InitPort(void)
{ 

  // PA0: MOSI: Output, Push Pull
  // PA1: SCK: Output, Push Pull
  // PA2: OE: Output, Push Pull, Pull Up
  // PA3: RCK: Output, Push Pull
  // PA4: -
  // PA5: -
  // PA6: -
  // PA7: -
  // PA8: LED1: Output, Push Pull, Pull Up
  // PA9: LED2: Output, Push Pull, Pull Up
  // PA10: LED3: Output, Push Pull, Pull Up
  // PA11: LED4: Output, Push Pull (Should be pull up)
  // PA12: LED5: Output, Push Pull (Should be pull up)
  // PA13: SWDIO: Alternate, Push Pull, High Speed, Pull Up, Alternate Function 0 (JTAG)
  // PA14: SWCLK: Alternate, Push Pull, High Speed, Pull Down, Alternate Function 0 (JTAG)
  // PA15: RS485_EN: Output, Push Pull
  //
  GPIOA->MODER=0X69555555;
  GPIOA->OTYPER=0;
  GPIOA->OSPEEDR=0X3C000000;
  GPIOA->PUPDR=0X24150010;
  GPIOA->AFR[0]=0;
  GPIOA->AFR[1]=0;
  
  // There is a mistake in the drawing. PB14 & PB15 is marked as PE14, PE15 in pictutr
  // PB0: IRP: Analog (SDADC1 Channel 6)
  // PB1: IRM: Analog
  // PB2: SW1: Input, Pulll Up
  // PB3: SW2: Input, Pull Up
  // PB4: SW3: Input, Pull Up
  // PB5-13 -
  // PB14: IBP, Analog, (SDADC3 Channel 8)
  // PB15: IBM, Analog
  //
  GPIOB->MODER=0XF005540F;
  GPIOB->OTYPER=0;
  GPIOB->OSPEEDR=0;
  GPIOB->PUPDR=0X150;
  GPIOB->AFR[0]=0;
  GPIOB->AFR[1]=0;
  
  // PC0: VR: Analog (Channel 10)
  // PC1: VY: Analog (Channel 11)
  // PC2: VB: Analog (Channel 12)
  // PC3: 5V Detector: Analog (Channel 13)
  // PC4: CON1: Output, Push Pull
  // PC5: CON2: Output, Push Pull
  // PC6: CON3: Output, Push Pull
  // PC7: CON4: Output, Push Pull
  // PC8: CON5: Output, Push Pull
  // PC9: FREQ: Alternate, Medimum Speed, Alternate Function 2 (TIM3_CH4)
  // PC10: RS485 TX: Alternate, Medium Speed, Alternate Function 7 (UART3_TX)
  // PC11: RS485 RX: Alternate, Medium Spee, Alternate Function 7 (UART3_RX)
  // PC12: LED6: Output, Push Pull (Should be pull up)
  // PC13: -
  // 
  GPIOC->MODER=0X1A955FF;
  GPIOC->OTYPER=0;
  GPIOC->OSPEEDR=0X540000;
  GPIOC->PUPDR=0;
  GPIOC->AFR[0]=0;
  GPIOC->AFR[1]=0x7720;
  
  // PD0: -
  // PD1: -
  // PD2: PROTECT: Input, Pull Down
  // PD3-PD15: -
  //
  GPIOD->MODER=0;
  GPIOD->OTYPER=0;
  GPIOD->OSPEEDR=0;
  GPIOD->PUPDR=0X20;
  GPIOD->AFR[0]=0;
  GPIOD->AFR[1]=0;

  // PE0-PE7: -
  // PE8: IYP: Analog (SDADC2 Channel 8)
  // PE9: IYM: Analog
  // PE10-PE15: -
  //
  GPIOE->MODER=0xF0000;
  GPIOE->OTYPER=0;
  GPIOE->OSPEEDR=0;
  GPIOE->PUPDR=0x00;
  GPIOE->AFR[0]=0;
  GPIOE->AFR[1]=0x00;
  
  // PF0-PF5: -
  // PF6: SCL: Alternate, Open Drain, Pull Up, Alternate Function 4 (I2C2_SCL)
  // PF7: SDA: Alternate, Open Drain, Pull Up, Alternate Function 4 (I2C2_SDA)
  // PF8-PF15: -
  //
  GPIOF->MODER=0xA000;
  GPIOF->OTYPER=0XC0;
  GPIOF->OSPEEDR=0;
  GPIOF->PUPDR=0x5000;
  GPIOF->AFR[0]=0X44000000;
  GPIOF->AFR[1]=0x00;
  
  SWITCH_OFF_LED_COMM;
  GPIOA->BSRR = PORT_BIT_11;
}
*/
static void InitPort(void)
{ 
  // PA0: MOSI: Output, Push Pull
  // PA1: SCK: Output, Push Pull
  // PA2: OE: Output, Push Pull, Pull Up
  // PA3: RCLK: Output, Push Pull
  // PA4: IYP_P: Analog (Channel 4)
  // PA5: IBP_M: Analog (Channel 5)
  // PA6: IRP_M: Analog (Channel 6)
  // PA7: IYP_M: Analog (Channel 7)
  // PA8: SW2: Input, Pull up
  // PA9: SCL: Alternate, Open Drain, Pull Up, Alternate Function: 4 (I2C2_SCL)
  // P10: SDA: Alternate, Open Drain, Pull Up, Alternate Function: 4 (I2C2_SDA)
  // PA11: - (Unused USB1)
  // PA12: - (Unused USB2)
  // PA13: SWDIO: Alternate, Push Pull, High Speed, Pull Up, Alternate Function 0 (JTAG)
  // PA14: SWCLK: Alternate, Push Pull, High Speed, Pull Down, Alternate Function 0 (JTAG)
  // PA15: DIS1: Output, Push Pull
  GPIOA->MODER=0x6828FF55;
  GPIOA->OTYPER=0x600;
  GPIOA->OSPEEDR=0X3C000000;
  GPIOA->PUPDR=0X24150010;
  GPIOA->AFR[0]=0;
  GPIOA->AFR[1]=0x440;

  // PB0: IBP_P: Analog (Channel 8)
  // PB1: IRP_P: Analog (Channel 9)
  // PB2: -
  // PB3: RS4885_TX: Alternate, Medium Speed, Alternate Function 7 (UART2_TX)
  // PB4: RS485_RX: Alternate, Medimum Speed, Alternate Function 7 (UART2_RX)
  // PB5: RS485_EN: Output, Push Pull
  // PB6: DIS3: Output, Push Pull
  // PB7: DIS4: Output, Push Pull
  // PB8: LED8: Output, Push Pull, Pull up
  // PB9: LED7: Output, Push Pull, Pull Up
  // PB10-PB15:-
  GPIOB->MODER=0x5568F;
  GPIOB->OTYPER=0x140;
  GPIOB->OSPEEDR=0;
  GPIOB->PUPDR=0x50000;
  GPIOB->AFR[0]=0x77000;
  GPIOB->AFR[1]=0;

  // PC0: MVRP: Analog (Channel 10)
  // PC1: MVYP: Analog (Channel 11)
  // PC2: MVBP: Analog (Channel 12)
  // PC3: PVRP: Analog (Channel 13)
  // PC4: PVYP: Analog (Channel 14)
  // PC5: PVBP: Analog (Channel 15)
  // PC6-PC8:-
  // PC9: PROTECT: Input, Pull Down
  // PC10: - (Unused Calibrator TX)
  // PC11: - (Unused Calibrator RX)
  // PC12: DIS2: Output, Push Pull
  // PC13-PC15:-
  GPIOC->MODER=0X1000FFF;
  GPIOC->OTYPER=0;
  GPIOC->OSPEEDR=0;
  GPIOC->PUPDR=0x80000;
  GPIOC->AFR[0]=0;
  GPIOC->AFR[1]=0;

  // PD0: LED1: Output, Push Pull, Pull up
  // PD1; LED2: Output, Push Pull, Pull up
  // PD2: LED3: Output, Push Pull, Pull up
  // PD3: LED4: Output, Push Pull, Pull up
  // PD4: LED5: Output, Push Pull, Pull up
  // PD5: LED6: Output, Push Pull, Pull up
  // PD6-PD7: -
  // PD8: RL1: Output, Push Pull
  // PD9: RL2: Output, Push Pull
  // PD10: RL5: Output, Push Pull
  // PD11: RL6: Output, Push Pull
  // PD12-PD15:-
  //
  GPIOD->MODER=0x550555;
  GPIOD->OTYPER=0;
  GPIOD->OSPEEDR=0;
  GPIOD->PUPDR=0X555;
  GPIOD->AFR[0]=0;
  GPIOD->AFR[1]=0;

  // PE0: PFC1: Input, Pull Up
  // PE1: PFC2: Input, Pull Up
  // PE2: PFC3: Input, Pull Up
  // PE3: PFC4: Input, Pull Up
  // PE4: PFC5: Input, Pull Up
  // PE5: PFC6: Input, Pull Up
  // PE6: PFC7: Input, Pull Up
  // PE7:-
  // PE8: RL3: Output, Push Pull, Pull up
  // PE9: RL4: Output, Push Pull, Pull up
  // PE10-PE15: -
  GPIOE->MODER=0x50000;
  GPIOE->OTYPER=0;
  GPIOE->OSPEEDR=0;
  GPIOE->PUPDR=0x50000;
  GPIOE->AFR[0]=0;
  GPIOE->AFR[1]=0;
 
  // PF0-PF3: -
  // PF4: SW1: Input, Pull up
  // PF5: -
  // PF6: SW3: Input, Pull up
  // PF7-PF8:-
  // PF9: DIS5: Output, Push Pull
  // PF10: PFC8: Input, Pull up
  // PF11-PF15: -
  //
  GPIOF->MODER=0x40000;
  GPIOF->OTYPER=0;
  GPIOF->OSPEEDR=0;
  GPIOF->PUPDR=0x40000;
  GPIOF->AFR[0]=0;
  GPIOF->AFR[1]=0;

  SWITCH_OFF_LED_COMM;
  SWITCH_OFF_LED2_R;
  SWITCH_OFF_LED2_G;
  SWITCH_OFF_LED2_B;
  SWITCH_OFF_LED_GRID_DISPLAY;
  SWITCH_OFF_LED_SOLAR_DISPLAY;
  SWITCH_OFF_LED_GRID_HEALTHY;
  SWITCH_OFF_LED_SOLAR_HEALTHY;

  TURN_RELAY1_OFF;
  TURN_RELAY2_OFF;
  TURN_RELAY3_OFF;
  TURN_RELAY4_OFF;
  TURN_RELAY5_OFF;
}

static void SetPwrRegisters(void)
{
  
  // Enable: DMA, PortA-PortF
  RCC->AHBENR=0x7E0001;
  // Sys CFG Enable, ADC, SDADC1, SDADC2, SDADC3
  RCC->APB2ENR=0x7000201;
  // Enable: Tim2, Tim3, Tim4, Usart2, Usart3, I2c2, PWR
  RCC->APB1ENR=0x10460007;
  // Select Sys clock as USART1 clock, I2C, I2C2 clock, PCLK as USART2, PCLK as USART3 clock
  RCC->CFGR3 |=0x31;
  // SDADC clock as Sys clock / 4
  // ADC: PCLK/2 (12Mhz/2)
  RCC->CFGR |=0X98000000;
  // Reset: Sys, ADC, SPI1, USART1, TIM15, TIM16, TIM17, TIM19, SDADC1, SDADC2, SDADC3
  RCC->APB2RSTR=0x70F5201;  // Reset  Pheripherial Rgegisters
  // Reset: TIM2, 3, 4, 5, 6, 7, 12, 13, 14, 18, Window Watchdog, SP12, SPI3, USART2, USART3, I2C, I2C2, USB, CAN, DAC2, PWR, DAC1, CECR
  RCC->APB1RSTR=0x76E6CBFF;
  RCC->APB2RSTR=0;
  RCC->APB1RSTR=0;;
  PWR->CR=0xE00;

}

static void SetDmaRegisters(void)
{
  DMA1_Channel1->CNDTR=0XC; // NO OF DATA TRANSFER
  DMA1_Channel1->CPAR=(uint32_t)&ADC1->DR;
  DMA1_Channel1->CMAR=(uint32_t)&AdcDataInArray;
  // DMA Enabled, Circular Mode, Memmory Increment, 16 bits pheripheral size, 16 bits memory size
  DMA1_Channel1->CCR=0X5A1;
  
  //DMA1_Channel1->CPAR=ADC1_BASE+0x4C;
  

}
static void SetAdc(void)
{
  // SCAN mode
  ADC1->CR1=0X100;
  //ADC1->CR2=0X1E7109;  //Reset Cal register
  //ADC1->CR2=0X1E7105;  // calibrate
  //while(ADC1->CR2 & 0x04); // wait for calibration to be over
 
  // 71.5 cycles for 10-18 channel
  ADC1->SMPR1=0x36DB6DB6;
  // 71.5 cycles for 0-9 channel
  ADC1->SMPR2=0x36DB6DB6;

  // Mains
  //
  // 1th Sequence: 6th channel (IRP_M)
  // 2th sequence: 10th channel (MVRP)
  // 3th sequence: 7th channel (IYP_M)
  // 4th channel: 11th channel (MVYP)
  // 5th sequence: 5th channel (IYB_M)
  // 6th sequence: 12th squence (MVBP)
  //
  // Solar
  // 7th Sequence: 9th channel (IRP_P)
  // 8th sequence: 13th channel (PVRP)
  // 9th sequence: 4th channel (IYP_P)
  // 10th channel: 14th channel (PVYP)
  // 11th sequence: 8th channel (IYB_P)
  // 12th sequence: 15th squence (PVBP)
  //  
  ADC1->SQR3=0x18559D46;
  ADC1->SQR2=0x1E8711A9;
  // 12 total sequences
  ADC1->SQR1=0xB00000;

  // ADC On, Enable Calibration, DMA enabled, JSWSRTART Trigger, SWSTART Trigger, Enable exrternal trigger
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
    // Prescaler 16
    IWDG->PR=0X02;
  }
  if(!(IWDG->SR & 2))
  {
    // Reload value maximum
    IWDG->KR=0X5555;
    IWDG->RLR=0XFFF;
  }
  
  IWDG->KR=0xCCCC; //Start

  // Halt: TIM2,3,46,7,12,13, 14.18.IWDG
  DBGMCU->APB1FZ=0x1BF7; // Halt peripherals in debug mode
  // Halt: TIM15,16,17,19
  DBGMCU->APB2FZ=0x1C;
  DBGMCU->CR=0;  
  
}

void InitI2C(void)
{
   
  I2C2->CR1 = 0x0000;      
  I2C2->CR2 = 0;
  I2C2->OAR1 = 0;   
  I2C2->OAR2 = 0;
  
  // SCL Low Period: 138 cycles, 
  // SCL High Period: 95 cycles
  // SDA Hold time: 0
  // SCL Setup time: 129 cycles
  // Prescaler: 2 (I2C Clock)
  I2C2->TIMINGR = 0x10805E89;           // 48Mhhz source, 100Khz operation, analog filter on
   I2C2->CR1 |=0X01;
}


static void SetSystemTimer(void)
{
  
  // TIM2 as a system timmer with 3.2Khz interrupt
  //
  // Counter underflow/overflow generates interrupt
  TIM2->CR1=0X4;
  TIM2->CR2=0;
  // Update interrupt enabled
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
  // Timer clock prescaller
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
  // Internal reference (1.2V)
  SDADC1->CR1=0X100;
  // Start SDADC1
  SDADC1->CR2=0X01;
  // Wait for SDADC stabilization
  while(SDADC1->ISR & SDADC_ISR_STABIP);
}
  
volatile uint16_t TimeOutCommTx;
void InitUart(uint8_t baud,uint8_t parity1,uint8_t Stopbit1)
{
   USART2->RTOR=24;
  if (baud==BaudRate_1200) 
  {
    USART2->BRR=0x2710;
    TimeOutCommTx=(uint16_t)(1.8*12*(float)NO_OF_SAMPLES/1200)+10;
    Check485DirCount=5*NO_OF_SAMPLES;
    
  }
  else if(baud==BaudRate_2400) 
  {
    USART2->BRR=0x1388;
    TimeOutCommTx=(uint16_t)(1.8*12*(float)NO_OF_SAMPLES/2400)+10;
    Check485DirCount=5*NO_OF_SAMPLES;
  }
  else if(baud==BaudRate_4800) 
  {
    USART2->BRR=0x9C4;
    TimeOutCommTx=(uint16_t)(1.8*12*(float)NO_OF_SAMPLES/4800)+8;
    Check485DirCount=3*NO_OF_SAMPLES;
  }
  else if(baud==BaudRate_9600) 
  {
    USART2->BRR=0x4E2;
    TimeOutCommTx=(uint16_t)(1.8*12*(float)NO_OF_SAMPLES/9600)+6;
    Check485DirCount=3*NO_OF_SAMPLES;
  }
  else if(baud==BaudRate_19200) 
  {
    USART2->BRR=0x271;
    TimeOutCommTx=(uint16_t)(1.8*12*(float)NO_OF_SAMPLES/19200)+6;
    Check485DirCount=3*NO_OF_SAMPLES;
  }
  Check485DirCounter=0;
  USART2->CR1 &=~1;
  if (parity1==Parity_Odd) USART2->CR1=0x1600;
  else if(parity1==Parity_Even) USART2->CR1=0x1400;
  else if(parity1==Parity_None) USART2->CR1=0x00;

  if (Stopbit1==Stopbit_one) USART2->CR2=0x00;
  else if(Stopbit1==Stopbit_two) USART2->CR2=0x200;
  RS485Receive;
  SWITCH_OFF_LED_COMM;
  USART2->CR1 |= 0x400002C;            // Transmitter enable, Receiver enable, RXNEIE,Receiver timeout interrupt enable
  USART2->CR2 |=  0x800000;             //Receiver timeout enable 
  USART2->CR3 =  0x80;                 // DMA
  USART2->ICR =  0;
  USART2->CR1 |= 1;
  DMA1_Channel7->CPAR = (uint32_t)&USART2->TDR;
  DMA1_Channel7->CCR = 0x1092;    // Memory inc, read from memory, tcie
  NVIC_EnableIRQ(USART2_IRQn);
  NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}
  

  

  
  
  
  

  
  


