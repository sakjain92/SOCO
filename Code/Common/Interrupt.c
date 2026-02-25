/*
******************************************************************************
  * @file    interrupt.c
  * @author  MRM R&D Team
  * @version V1.0.0
  * @date    07-06-2022
  * @brief   This file contains all the functions definitions 
             for the interrupt
******************************************************************************
*/

#include "CommFlagDef.h"
#include "Struct.h"
#include <Math.h>
#include "stm32f37x_flash.h"
#include "CosSinTable.h"
#include "extern_includes.h"

void ReadSwitches(void);
void ReadInputs(void);
void CalculateFreqYBPhase(int16_t TempIntFreqVol);

 
struct SAMPLE   RCurSample;
struct SAMPLE   YCurSample;
struct SAMPLE   BCurSample;
struct SAMPLE   RVolSample;
struct SAMPLE   YVolSample;
struct SAMPLE   BVolSample;

struct SAMPLE   RSolarCurSample;
struct SAMPLE   YSolarCurSample;
struct SAMPLE   BSolarCurSample;
struct SAMPLE   RSolarVolSample;
struct SAMPLE   YSolarVolSample;
struct SAMPLE   BSolarVolSample;


extern volatile uint16_t TimeOutCommTx;

uint16_t OneSecCounter;

#define FREQ_LIMIT_VOL    (90)

// UNDONE: The harmonics code assumes that the interrupt timer is setup such
// that it is called f * 3200 Hz where f is the freqency of input.
// But now, we have two different sources. So ideally, we should either
// be using two timers, one for each source (And decide when to start 
// ADC measurement) or reject Harmonics based measurement altogether
// (Can't ignore as reactive power is based on harmonics)
// This is also another reason for error in current measurement most likely
//
void ProcessMainInterrupt(void)
{

  float IntVolRPhase,IntVolYPhase,IntVolBPhase;
  float IntVolRSolarPhase,IntVolYSolarPhase,IntVolBSolarPhase;
  float IntCurRPhase,IntCurYPhase,IntCurBPhase;
  float IntCurRSolarPhase,IntCurYSolarPhase,IntCurBSolarPhase;
  float IntNeuCurrent, IntNeuSolarCurrent;
  float TempGainMult;
  int16_t TempInt;  
  uint8_t i;
#ifdef MODEL_DATA_SAVE   
  // UNDONE: Fix this
  // If we detect power supply is dropping, then save energy data immediately
  //
  if(/*AdcDataInArray[ADC_POWER_SENSE]<POWER_FAIL_SENSE_VALUE*/ 0)
  {

    if(!(InterruptFlag  & INT_DATA_SAVING_EEPROM))
    {
      if(PowerFailCounter<5)PowerFailCounter++;
      if((PowerFailCounter>=5)&&(PowerUpCounter>=DATA_SAVE_DEBAR_TIME))
      {
        DISP_SR_OFF;
        PowerDownDataSave();
        NVIC_SystemReset();
        return;
      }
    }
    else
    {
      PowerFailCounter=0;
    }
    
  }
  else  

  {
    if(PowerFailCounter)PowerFailCounter--;
    if(PowerFailCounter==0)InterruptFlag |= INT_POWER_OK;
  }  
#endif  
  TempInt=AdcDataInArray[ADC_VR];
  TempInt=TempInt-0x1000;
  IntVolRPhase  =(float)TempInt-VIOffset.VolRPhase;
  IntVolRPhase *=WorkingCopyGain.VR_GAIN;
  
  TempInt=AdcDataInArray[ADC_VY];
  TempInt=TempInt-0x1000;
  IntVolYPhase  =(float)TempInt-VIOffset.VolYPhase;
  IntVolYPhase *=WorkingCopyGain.VY_GAIN;
  
  TempInt=AdcDataInArray[ADC_VB];
  TempInt=TempInt-0x1000;
  IntVolBPhase  =(float)TempInt-VIOffset.VolBPhase;
  IntVolBPhase *=WorkingCopyGain.VB_GAIN;
   
  TempInt=AdcDataInArray[ADC_IR];
  TempInt=TempInt-0x1000;
  IntCurRPhase  =(float)TempInt-VIOffset.CurRPhase;
  IntCurRPhase *=WorkingCopyGain.IR_GAIN;

  TempInt=AdcDataInArray[ADC_IY];
  TempInt=TempInt-0x1000;
  IntCurYPhase  =(float)TempInt-VIOffset.CurYPhase;
  IntCurYPhase *=WorkingCopyGain.IY_GAIN;

  TempInt=AdcDataInArray[ADC_IB];
  TempInt=TempInt-0x1000;
  IntCurBPhase  =(float)TempInt-VIOffset.CurBPhase;
  IntCurBPhase *=WorkingCopyGain.IB_GAIN;

  TempInt=AdcDataInArray[ADC_VR_SOLAR];
  TempInt=TempInt-0x1000;
  IntVolRSolarPhase  =(float)TempInt-VIOffset.VolRSolarPhase;
  IntVolRSolarPhase *=WorkingCopyGain.VR_SOLAR_GAIN;

  TempInt=AdcDataInArray[ADC_VY_SOLAR];
  TempInt=TempInt-0x1000;
  IntVolYSolarPhase  =(float)TempInt-VIOffset.VolYSolarPhase;
  IntVolYSolarPhase *=WorkingCopyGain.VY_SOLAR_GAIN;

  TempInt=AdcDataInArray[ADC_VB_SOLAR];
  TempInt=TempInt-0x1000;
  IntVolBSolarPhase  =(float)TempInt-VIOffset.VolBSolarPhase;
  IntVolBSolarPhase *=WorkingCopyGain.VB_SOLAR_GAIN;

  TempInt=AdcDataInArray[ADC_IR_SOLAR];
  TempInt=TempInt-0x1000;
  IntCurRSolarPhase  =(float)TempInt-VIOffset.CurRSolarPhase;
  IntCurRSolarPhase *=WorkingCopyGain.IR_SOLAR_GAIN;

  TempInt=AdcDataInArray[ADC_IY_SOLAR];
  TempInt=TempInt-0x1000;
  IntCurYSolarPhase  =(float)TempInt-VIOffset.CurYSolarPhase;
  IntCurYSolarPhase *=WorkingCopyGain.IY_SOLAR_GAIN;

  TempInt=AdcDataInArray[ADC_IB_SOLAR];
  TempInt=TempInt-0x1000;
  IntCurBSolarPhase  =(float)TempInt-VIOffset.CurBSolarPhase;
  IntCurBSolarPhase *=WorkingCopyGain.IB_SOLAR_GAIN;

  // UNDONE: Do the current & power calculation for solar

  ADC1->CR2 |=ADC_CR2_SWSTART;   // Start New conversion 
 
  // UNDONE: There is a bug in this second order butterworth filter with cutoff
  // at 600Hz
  //
  TempGainMult=(RCurSample.PrevIn_2+IntCurRPhase)*FILT_600_COEFF_X1+\
                RCurSample.PrevIn_1*FILT_600_COEFF_X2+\
                RCurSample.PrevOut_1*FILT_600_COEFF_Y1+\
                RCurSample.PrevOut_2*FILT_600_COEFF_Y2;
  RCurSample.PrevOut_2=RCurSample.PrevOut_1;
  RCurSample.PrevOut_1=TempGainMult;
  RCurSample.PrevIn_2=RCurSample.PrevIn_1;
  RCurSample.PrevIn_1=IntCurRPhase;
  IntCurRPhase=TempGainMult;
  
  TempGainMult=(YCurSample.PrevIn_2+IntCurYPhase)*FILT_600_COEFF_X1+\
                YCurSample.PrevIn_1*FILT_600_COEFF_X2+\
                YCurSample.PrevOut_1*FILT_600_COEFF_Y1+\
                YCurSample.PrevOut_2*FILT_600_COEFF_Y2;
  YCurSample.PrevOut_2=YCurSample.PrevOut_1;
  YCurSample.PrevOut_1=TempGainMult;
  YCurSample.PrevIn_2=YCurSample.PrevIn_1;
  YCurSample.PrevIn_1=IntCurYPhase;
  IntCurYPhase=TempGainMult;
  
  TempGainMult=(BCurSample.PrevIn_2+IntCurBPhase)*FILT_600_COEFF_X1+\
                BCurSample.PrevIn_1*FILT_600_COEFF_X2+\
                BCurSample.PrevOut_1*FILT_600_COEFF_Y1+\
                BCurSample.PrevOut_2*FILT_600_COEFF_Y2;
  BCurSample.PrevOut_2=BCurSample.PrevOut_1;
  BCurSample.PrevOut_1=TempGainMult;
  BCurSample.PrevIn_2=BCurSample.PrevIn_1;
  BCurSample.PrevIn_1=IntCurBPhase;
  IntCurBPhase=TempGainMult;
  
  TempGainMult=(RVolSample.PrevIn_2+IntVolRPhase)*FILT_600_COEFF_X1+\
                RVolSample.PrevIn_1*FILT_600_COEFF_X2+\
                RVolSample.PrevOut_1*FILT_600_COEFF_Y1+\
                RVolSample.PrevOut_2*FILT_600_COEFF_Y2;
  RVolSample.PrevOut_2=RVolSample.PrevOut_1;
  RVolSample.PrevOut_1=TempGainMult;
  RVolSample.PrevIn_2=RVolSample.PrevIn_1;
  RVolSample.PrevIn_1=IntVolRPhase;
  IntVolRPhase=TempGainMult;
  
  TempGainMult=(YVolSample.PrevIn_2+IntVolYPhase)*FILT_600_COEFF_X1+\
                YVolSample.PrevIn_1*FILT_600_COEFF_X2+\
                YVolSample.PrevOut_1*FILT_600_COEFF_Y1+\
                YVolSample.PrevOut_2*FILT_600_COEFF_Y2;
  YVolSample.PrevOut_2=YVolSample.PrevOut_1;
  YVolSample.PrevOut_1=TempGainMult;
  YVolSample.PrevIn_2=YVolSample.PrevIn_1;
  YVolSample.PrevIn_1=IntVolYPhase;
  IntVolYPhase=TempGainMult;
  
  TempGainMult=(BVolSample.PrevIn_2+IntVolBPhase)*FILT_600_COEFF_X1+\
                BVolSample.PrevIn_1*FILT_600_COEFF_X2+\
                BVolSample.PrevOut_1*FILT_600_COEFF_Y1+\
                BVolSample.PrevOut_2*FILT_600_COEFF_Y2;
  BVolSample.PrevOut_2=BVolSample.PrevOut_1;
  BVolSample.PrevOut_1=TempGainMult;
  BVolSample.PrevIn_2=BVolSample.PrevIn_1;
  BVolSample.PrevIn_1=IntVolBPhase;
  IntVolBPhase=TempGainMult;
  
  TempGainMult=(RSolarCurSample.PrevIn_2+IntCurRSolarPhase)*FILT_600_COEFF_X1+\
                RSolarCurSample.PrevIn_1*FILT_600_COEFF_X2+\
                RSolarCurSample.PrevOut_1*FILT_600_COEFF_Y1+\
                RSolarCurSample.PrevOut_2*FILT_600_COEFF_Y2;
  RSolarCurSample.PrevOut_2=RSolarCurSample.PrevOut_1;
  RSolarCurSample.PrevOut_1=TempGainMult;
  RSolarCurSample.PrevIn_2=RSolarCurSample.PrevIn_1;
  RSolarCurSample.PrevIn_1=IntCurRSolarPhase;
  IntCurRSolarPhase=TempGainMult;
  
  TempGainMult=(YSolarCurSample.PrevIn_2+IntCurYSolarPhase)*FILT_600_COEFF_X1+\
                YSolarCurSample.PrevIn_1*FILT_600_COEFF_X2+\
                YSolarCurSample.PrevOut_1*FILT_600_COEFF_Y1+\
                YSolarCurSample.PrevOut_2*FILT_600_COEFF_Y2;
  YSolarCurSample.PrevOut_2=YSolarCurSample.PrevOut_1;
  YSolarCurSample.PrevOut_1=TempGainMult;
  YSolarCurSample.PrevIn_2=YSolarCurSample.PrevIn_1;
  YSolarCurSample.PrevIn_1=IntCurYSolarPhase;
  IntCurYSolarPhase=TempGainMult;
  
  TempGainMult=(BSolarCurSample.PrevIn_2+IntCurBSolarPhase)*FILT_600_COEFF_X1+\
                BSolarCurSample.PrevIn_1*FILT_600_COEFF_X2+\
                BSolarCurSample.PrevOut_1*FILT_600_COEFF_Y1+\
                BSolarCurSample.PrevOut_2*FILT_600_COEFF_Y2;
  BSolarCurSample.PrevOut_2=BSolarCurSample.PrevOut_1;
  BSolarCurSample.PrevOut_1=TempGainMult;
  BSolarCurSample.PrevIn_2=BSolarCurSample.PrevIn_1;
  BSolarCurSample.PrevIn_1=IntCurBSolarPhase;
  IntCurBSolarPhase=TempGainMult;

  TempGainMult=(RSolarVolSample.PrevIn_2+IntVolRSolarPhase)*FILT_600_COEFF_X1+\
                RSolarVolSample.PrevIn_1*FILT_600_COEFF_X2+\
                RSolarVolSample.PrevOut_1*FILT_600_COEFF_Y1+\
                RSolarVolSample.PrevOut_2*FILT_600_COEFF_Y2;
  RSolarVolSample.PrevOut_2=RSolarVolSample.PrevOut_1;
  RSolarVolSample.PrevOut_1=TempGainMult;
  RSolarVolSample.PrevIn_2=RSolarVolSample.PrevIn_1;
  RSolarVolSample.PrevIn_1=IntVolRSolarPhase;
  IntVolRSolarPhase=TempGainMult;

  TempGainMult=(YSolarVolSample.PrevIn_2+IntVolYSolarPhase)*FILT_600_COEFF_X1+\
                YSolarVolSample.PrevIn_1*FILT_600_COEFF_X2+\
                YSolarVolSample.PrevOut_1*FILT_600_COEFF_Y1+\
                YSolarVolSample.PrevOut_2*FILT_600_COEFF_Y2;
  YSolarVolSample.PrevOut_2=YSolarVolSample.PrevOut_1;
  YSolarVolSample.PrevOut_1=TempGainMult;
  YSolarVolSample.PrevIn_2=YSolarVolSample.PrevIn_1;
  YSolarVolSample.PrevIn_1=IntVolYSolarPhase;
  IntVolYSolarPhase=TempGainMult;

  TempGainMult=(BSolarVolSample.PrevIn_2+IntVolBSolarPhase)*FILT_600_COEFF_X1+\
                BSolarVolSample.PrevIn_1*FILT_600_COEFF_X2+\
                BSolarVolSample.PrevOut_1*FILT_600_COEFF_Y1+\
                BSolarVolSample.PrevOut_2*FILT_600_COEFF_Y2;
  BSolarVolSample.PrevOut_2=BSolarVolSample.PrevOut_1;
  BSolarVolSample.PrevOut_1=TempGainMult;
  BSolarVolSample.PrevIn_2=BSolarVolSample.PrevIn_1;
  BSolarVolSample.PrevIn_1=IntVolBSolarPhase;
  IntVolBSolarPhase=TempGainMult;

  OneSecCounter++;
  if(SampleCounter==(NO_OF_SAMPLES-1))
  {
    IntDataSave.TimerCountValue= IntTimerCount.TimerPresentValue;
    IntTimerCount.TimerPresentValue=IntTimerCount.TimerNewValue;
    TIM2->ARR=IntTimerCount.TimerPresentValue;
  }
  else if(SampleCounter==NO_OF_SAMPLES)
  {
    SampleCounter=0;
    InterruptFlag |=INT_CYCLE_OVER;
    InterruptFlag |=(INT_Y_NEW_FREQ_MEASURE+INT_B_NEW_FREQ_MEASURE);
    IntDataSave.VolRPhase=IntDataSum.VolRPhase;
    IntDataSave.VolYPhase=IntDataSum.VolYPhase;
    IntDataSave.VolBPhase=IntDataSum.VolBPhase;
    IntDataSave.CurRPhase=IntDataSum.CurRPhase;
    IntDataSave.CurYPhase=IntDataSum.CurYPhase;
    IntDataSave.CurBPhase=IntDataSum.CurBPhase;
    
    IntDataSave.RPhasePower=IntDataSum.RPhasePower;
    IntDataSave.YPhasePower=IntDataSum.YPhasePower;
    IntDataSave.BPhasePower=IntDataSum.BPhasePower;
    
    IntDataSave.CurNeutral=IntDataSum.CurNeutral;
    IntDataSave.VolRYPhPh=IntDataSum.VolRYPhPh;
    IntDataSave.VolYBPhPh=IntDataSum.VolYBPhPh;
    IntDataSave.VolBRPhPh=IntDataSum.VolBRPhPh;
    IntDataSave.OffsetVolRPhase=IntDataSum.OffsetVolRPhase;
    IntDataSave.OffsetVolYPhase=IntDataSum.OffsetVolYPhase;
    IntDataSave.OffsetVolBPhase=IntDataSum.OffsetVolBPhase;
    IntDataSave.OffsetCurRPhase=IntDataSum.OffsetCurRPhase;
    IntDataSave.OffsetCurYPhase=IntDataSum.OffsetCurYPhase;
    IntDataSave.OffsetCurBPhase=IntDataSum.OffsetCurBPhase;
     
    IntDataSave.VolRSolarPhase=IntDataSum.VolRSolarPhase;
    IntDataSave.VolYSolarPhase=IntDataSum.VolYSolarPhase;
    IntDataSave.VolBSolarPhase=IntDataSum.VolBSolarPhase;
    IntDataSave.CurRSolarPhase=IntDataSum.CurRSolarPhase;
    IntDataSave.CurYSolarPhase=IntDataSum.CurYSolarPhase;
    IntDataSave.CurBSolarPhase=IntDataSum.CurBSolarPhase;

    IntDataSave.RSolarPhasePower=IntDataSum.RSolarPhasePower;
    IntDataSave.YSolarPhasePower=IntDataSum.YSolarPhasePower;
    IntDataSave.BSolarPhasePower=IntDataSum.BSolarPhasePower;
    
    IntDataSave.CurNeutralSolar=IntDataSum.CurNeutralSolar;
    IntDataSave.VolRYSolarPhPh=IntDataSum.VolRYSolarPhPh;
    IntDataSave.VolYBSolarPhPh=IntDataSum.VolYBSolarPhPh;
    IntDataSave.VolBRSolarPhPh=IntDataSum.VolBRSolarPhPh;
    IntDataSave.OffsetVolRSolarPhase=IntDataSum.OffsetVolRSolarPhase;
    IntDataSave.OffsetVolYSolarPhase=IntDataSum.OffsetVolYSolarPhase;
    IntDataSave.OffsetVolBSolarPhase=IntDataSum.OffsetVolBSolarPhase;
    IntDataSave.OffsetCurRSolarPhase=IntDataSum.OffsetCurRSolarPhase;
    IntDataSave.OffsetCurYSolarPhase=IntDataSum.OffsetCurYSolarPhase;
    IntDataSave.OffsetCurBSolarPhase=IntDataSum.OffsetCurBSolarPhase;

    memset(&IntDataSum, 0, sizeof(IntDataSum));
 
    for(i=0;i<50;i++)
    {    
      FftSampleData.FFT_RVolSinSave[i]=FftSampleData.FFT_RVolSinSum[i];
      FftSampleData.FFT_RVolCosSave[i]=FftSampleData.FFT_RVolCosSum[i];
      FftSampleData.FFT_YVolSinSave[i]=FftSampleData.FFT_YVolSinSum[i];
      FftSampleData.FFT_YVolCosSave[i]=FftSampleData.FFT_YVolCosSum[i];
      FftSampleData.FFT_BVolSinSave[i]=FftSampleData.FFT_BVolSinSum[i];
      FftSampleData.FFT_BVolCosSave[i]=FftSampleData.FFT_BVolCosSum[i];
      
      FftSampleData.FFT_RCurSinSave[i]=FftSampleData.FFT_RCurSinSum[i];
      FftSampleData.FFT_RCurCosSave[i]=FftSampleData.FFT_RCurCosSum[i];
      FftSampleData.FFT_YCurSinSave[i]=FftSampleData.FFT_YCurSinSum[i];
      FftSampleData.FFT_YCurCosSave[i]=FftSampleData.FFT_YCurCosSum[i];
      FftSampleData.FFT_BCurSinSave[i]=FftSampleData.FFT_BCurSinSum[i];
      FftSampleData.FFT_BCurCosSave[i]=FftSampleData.FFT_BCurCosSum[i];
      
      FftSampleData.FFT_NeuCurSinSave[i]=FftSampleData.FFT_NeuCurSinSum[i];
      FftSampleData.FFT_NeuCurCosSave[i]=FftSampleData.FFT_NeuCurCosSum[i];
      
      FftSampleData.FFT_RVolSinSum[i]=0;
      FftSampleData.FFT_RVolCosSum[i]=0;
      FftSampleData.FFT_YVolSinSum[i]=0;
      FftSampleData.FFT_YVolCosSum[i]=0;
      FftSampleData.FFT_BVolSinSum[i]=0;
      FftSampleData.FFT_BVolCosSum[i]=0;
      FftSampleData.FFT_RCurSinSum[i]=0;
      FftSampleData.FFT_RCurCosSum[i]=0;
      FftSampleData.FFT_YCurSinSum[i]=0;
      FftSampleData.FFT_YCurCosSum[i]=0;
      FftSampleData.FFT_BCurSinSum[i]=0;
      FftSampleData.FFT_BCurCosSum[i]=0;
      FftSampleData.FFT_NeuCurSinSum[i]=0;
      FftSampleData.FFT_NeuCurCosSum[i]=0;

      FftSampleData.FFT_RSolarVolSinSave[i]=FftSampleData.FFT_RSolarVolSinSum[i];
      FftSampleData.FFT_RSolarVolCosSave[i]=FftSampleData.FFT_RSolarVolCosSum[i];
      FftSampleData.FFT_YSolarVolSinSave[i]=FftSampleData.FFT_YSolarVolSinSum[i];
      FftSampleData.FFT_YSolarVolCosSave[i]=FftSampleData.FFT_YSolarVolCosSum[i];
      FftSampleData.FFT_BSolarVolSinSave[i]=FftSampleData.FFT_BSolarVolSinSum[i];
      FftSampleData.FFT_BSolarVolCosSave[i]=FftSampleData.FFT_BSolarVolCosSum[i];
      
      FftSampleData.FFT_RSolarCurSinSave[i]=FftSampleData.FFT_RSolarCurSinSum[i];
      FftSampleData.FFT_RSolarCurCosSave[i]=FftSampleData.FFT_RSolarCurCosSum[i];
      FftSampleData.FFT_YSolarCurSinSave[i]=FftSampleData.FFT_YSolarCurSinSum[i];
      FftSampleData.FFT_YSolarCurCosSave[i]=FftSampleData.FFT_YSolarCurCosSum[i];
      FftSampleData.FFT_BSolarCurSinSave[i]=FftSampleData.FFT_BSolarCurSinSum[i];
      FftSampleData.FFT_BSolarCurCosSave[i]=FftSampleData.FFT_BSolarCurCosSum[i];
      
      FftSampleData.FFT_NeuSolarCurSinSave[i]=FftSampleData.FFT_NeuSolarCurSinSum[i];
      FftSampleData.FFT_NeuSolarCurCosSave[i]=FftSampleData.FFT_NeuSolarCurCosSum[i];
      
      FftSampleData.FFT_RSolarVolSinSum[i]=0;
      FftSampleData.FFT_RSolarVolCosSum[i]=0;
      FftSampleData.FFT_YSolarVolSinSum[i]=0;
      FftSampleData.FFT_YSolarVolCosSum[i]=0;
      FftSampleData.FFT_BSolarVolSinSum[i]=0;
      FftSampleData.FFT_BSolarVolCosSum[i]=0;
      FftSampleData.FFT_RSolarCurSinSum[i]=0;
      FftSampleData.FFT_RSolarCurCosSum[i]=0;
      FftSampleData.FFT_YSolarCurSinSum[i]=0;
      FftSampleData.FFT_YSolarCurCosSum[i]=0;
      FftSampleData.FFT_BSolarCurSinSum[i]=0;
      FftSampleData.FFT_BSolarCurCosSum[i]=0;
      FftSampleData.FFT_NeuSolarCurSinSum[i]=0;
      FftSampleData.FFT_NeuSolarCurCosSum[i]=0;

    }
    FftSampleData.FFT_Counter=0;
    FftSampleData.FFT_CounterIndex=0;
  }

#ifdef MODEL_RS485 
  // This is an old code which states that if RS485 is stuck transmitting
  // for more than 5 seconds, let's restart the USART module
  // This was added due to a bug in RS485 code which has been resolved so
  // this code isn't strictly required anymore
  //
  if(RS485TransmitOn)
  {
    if(Check485DirCounter==Check485DirCount)
    {
      Check485DirCounter=0;
      CounterSendComplete=0;
      InitUart(CopySetPara[PARA_BAUD_RATE],CopySetPara[PARA_PARITY],CopySetPara[PARA_STOP_BIT]);
    }
    else if(Check485DirCounter<Check485DirCount)Check485DirCounter++;
  }
  else Check485DirCounter=0;
#endif  
  if(FreqSampleFlag>1)
  {
    if(FreqSampleFlag==2)CalculateFreqYBPhase((uint16_t)IntVolYPhase);
    else if(FreqSampleFlag==3)CalculateFreqYBPhase((uint16_t)IntVolBPhase);
    else if(FreqSampleFlag==4)CalculateFreqYBPhase((uint16_t)IntVolRPhase);
  }
  IntDataSum.OffsetVolRPhase +=IntVolRPhase;
  IntDataSum.OffsetVolYPhase +=IntVolYPhase;
  IntDataSum.OffsetVolBPhase +=IntVolBPhase;
  
  IntDataSum.OffsetCurRPhase +=IntCurRPhase;
  IntDataSum.OffsetCurYPhase +=IntCurYPhase;
  IntDataSum.OffsetCurBPhase +=IntCurBPhase;
  
  IntDataSum.CurRPhase += IntCurRPhase*IntCurRPhase;
  IntDataSum.CurYPhase += IntCurYPhase*IntCurYPhase;
  IntDataSum.CurBPhase += IntCurBPhase*IntCurBPhase;
  
  IntDataSum.VolRPhase += IntVolRPhase*IntVolRPhase;
  IntDataSum.VolYPhase += IntVolYPhase*IntVolYPhase;
  IntDataSum.VolBPhase += IntVolBPhase*IntVolBPhase;
  
  TempGainMult=(IntVolRPhase-IntVolYPhase);
  IntDataSum.VolRYPhPh += TempGainMult*TempGainMult;
  TempGainMult=(IntVolBPhase-IntVolRPhase);
  IntDataSum.VolBRPhPh += TempGainMult*TempGainMult;
  TempGainMult=(IntVolBPhase-IntVolYPhase);
  IntDataSum.VolYBPhPh += TempGainMult*TempGainMult;
  
  // Neutral Current
  IntNeuCurrent=0;
  IntNeuCurrent +=IntCurRPhase;
  IntNeuCurrent +=IntCurYPhase;
  IntNeuCurrent +=IntCurBPhase;
  
  IntDataSum.CurNeutral +=IntNeuCurrent*IntNeuCurrent;
  
// Phase compensation  y [n] = x[n]+ b x[n -1]
  // This is probably phase compensation for the phase shift introduced
  // by internal CTs
  //
  TempGainMult=  (WorkingCopyGain.PR_BETA*IntRPrevSample);
  IntRPrevSample=IntVolRPhase;
  IntVolRPhase +=TempGainMult;
  IntVolRPhase=(WorkingCopyGain.PR_ALFA*IntVolRPhase);
  
  TempGainMult=  (WorkingCopyGain.PY_BETA*IntYPrevSample);
  IntYPrevSample=IntVolYPhase;
  IntVolYPhase +=TempGainMult;
  IntVolYPhase=(WorkingCopyGain.PY_ALFA*IntVolYPhase);
  
  TempGainMult=  (WorkingCopyGain.PB_BETA*IntBPrevSample);
  IntBPrevSample=IntVolBPhase;
  IntVolBPhase +=TempGainMult;
  IntVolBPhase=(WorkingCopyGain.PB_ALFA*IntVolBPhase);
 
  IntDataSum.RPhasePower +=IntVolRPhase*IntCurRPhase;
  IntDataSum.YPhasePower +=IntVolYPhase*IntCurYPhase;
  IntDataSum.BPhasePower +=IntVolBPhase*IntCurBPhase;
  
  // Solar 
  IntDataSum.OffsetVolRSolarPhase +=IntVolRSolarPhase;
  IntDataSum.OffsetVolYSolarPhase +=IntVolYSolarPhase;
  IntDataSum.OffsetVolBSolarPhase +=IntVolBSolarPhase;
  
  IntDataSum.OffsetCurRSolarPhase +=IntCurRSolarPhase;
  IntDataSum.OffsetCurYSolarPhase +=IntCurYSolarPhase;
  IntDataSum.OffsetCurBSolarPhase +=IntCurBSolarPhase;
  
  IntDataSum.CurRSolarPhase += IntCurRSolarPhase*IntCurRSolarPhase;
  IntDataSum.CurYSolarPhase += IntCurYSolarPhase*IntCurYSolarPhase;
  IntDataSum.CurBSolarPhase += IntCurBSolarPhase*IntCurBSolarPhase;

  IntDataSum.VolRSolarPhase += IntVolRSolarPhase*IntVolRSolarPhase;
  IntDataSum.VolYSolarPhase += IntVolYSolarPhase*IntVolYSolarPhase;
  IntDataSum.VolBSolarPhase += IntVolBSolarPhase*IntVolBSolarPhase;

  TempGainMult=(IntVolRSolarPhase-IntVolYSolarPhase);
  IntDataSum.VolRYSolarPhPh += TempGainMult*TempGainMult;
  TempGainMult=(IntVolBSolarPhase-IntVolRSolarPhase);
  IntDataSum.VolBRSolarPhPh += TempGainMult*TempGainMult;
  TempGainMult=(IntVolBSolarPhase-IntVolYSolarPhase);
  IntDataSum.VolYBSolarPhPh += TempGainMult*TempGainMult;

  IntNeuSolarCurrent=0;
  IntNeuSolarCurrent +=IntCurRSolarPhase;
  IntNeuSolarCurrent +=IntCurYSolarPhase;
  IntNeuSolarCurrent +=IntCurBSolarPhase;
  
  IntDataSum.CurNeutralSolar +=IntNeuSolarCurrent*IntNeuSolarCurrent;

  TempGainMult=  (WorkingCopyGain.PR_SOLAR_BETA*IntRSolarPrevSample);
  IntRSolarPrevSample=IntVolRSolarPhase;
  IntVolRSolarPhase +=TempGainMult;
  IntVolRSolarPhase=(WorkingCopyGain.PR_SOLAR_ALFA*IntVolRSolarPhase);
  
  TempGainMult=  (WorkingCopyGain.PY_SOLAR_BETA*IntYSolarPrevSample);
  IntYSolarPrevSample=IntVolYSolarPhase;
  IntVolYSolarPhase +=TempGainMult;
  IntVolYSolarPhase=(WorkingCopyGain.PY_SOLAR_ALFA*IntVolYSolarPhase);
  
  TempGainMult=  (WorkingCopyGain.PB_SOLAR_BETA*IntBSolarPrevSample);
  IntBSolarPrevSample=IntVolBSolarPhase;
  IntVolBSolarPhase +=TempGainMult;
  IntVolBSolarPhase=(WorkingCopyGain.PB_SOLAR_ALFA*IntVolBSolarPhase);

  IntDataSum.RSolarPhasePower +=IntVolRSolarPhase*IntCurRSolarPhase;
  IntDataSum.YSolarPhasePower +=IntVolYSolarPhase*IntCurYSolarPhase;
  IntDataSum.BSolarPhasePower +=IntVolBSolarPhase*IntCurBSolarPhase;
  
  // Summation for FFT Purpose
  
  
  FftSampleData.FFT_RVolSinSum[FftSampleData.FFT_CounterIndex]+=IntVolRPhase*SinTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_RVolCosSum[FftSampleData.FFT_CounterIndex]+=IntVolRPhase*CosTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_YVolSinSum[FftSampleData.FFT_CounterIndex]+=IntVolYPhase*SinTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_YVolCosSum[FftSampleData.FFT_CounterIndex]+=IntVolYPhase*CosTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_BVolSinSum[FftSampleData.FFT_CounterIndex]+=IntVolBPhase*SinTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_BVolCosSum[FftSampleData.FFT_CounterIndex]+=IntVolBPhase*CosTable[FftSampleData.FFT_Counter];
  
  FftSampleData.FFT_RCurSinSum[FftSampleData.FFT_CounterIndex]+=IntCurRPhase*SinTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_RCurCosSum[FftSampleData.FFT_CounterIndex]+=IntCurRPhase*CosTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_YCurSinSum[FftSampleData.FFT_CounterIndex]+=IntCurYPhase*SinTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_YCurCosSum[FftSampleData.FFT_CounterIndex]+=IntCurYPhase*CosTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_BCurSinSum[FftSampleData.FFT_CounterIndex]+=IntCurBPhase*SinTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_BCurCosSum[FftSampleData.FFT_CounterIndex]+=IntCurBPhase*CosTable[FftSampleData.FFT_Counter];
  
  FftSampleData.FFT_NeuCurSinSum[FftSampleData.FFT_CounterIndex]+=IntNeuCurrent*SinTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_NeuCurCosSum[FftSampleData.FFT_CounterIndex]+=IntNeuCurrent*CosTable[FftSampleData.FFT_Counter];

  FftSampleData.FFT_RSolarVolSinSum[FftSampleData.FFT_CounterIndex]+=IntVolRSolarPhase*SinTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_RSolarVolCosSum[FftSampleData.FFT_CounterIndex]+=IntVolRSolarPhase*CosTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_YSolarVolSinSum[FftSampleData.FFT_CounterIndex]+=IntVolYSolarPhase*SinTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_YSolarVolCosSum[FftSampleData.FFT_CounterIndex]+=IntVolYSolarPhase*CosTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_BSolarVolSinSum[FftSampleData.FFT_CounterIndex]+=IntVolBSolarPhase*SinTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_BSolarVolCosSum[FftSampleData.FFT_CounterIndex]+=IntVolBSolarPhase*CosTable[FftSampleData.FFT_Counter];
  
  FftSampleData.FFT_RSolarCurSinSum[FftSampleData.FFT_CounterIndex]+=IntCurRSolarPhase*SinTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_RSolarCurCosSum[FftSampleData.FFT_CounterIndex]+=IntCurRSolarPhase*CosTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_YSolarCurSinSum[FftSampleData.FFT_CounterIndex]+=IntCurYSolarPhase*SinTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_YSolarCurCosSum[FftSampleData.FFT_CounterIndex]+=IntCurYSolarPhase*CosTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_BSolarCurSinSum[FftSampleData.FFT_CounterIndex]+=IntCurBSolarPhase*SinTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_BSolarCurCosSum[FftSampleData.FFT_CounterIndex]+=IntCurBSolarPhase*CosTable[FftSampleData.FFT_Counter];
  
  FftSampleData.FFT_NeuSolarCurSinSum[FftSampleData.FFT_CounterIndex]+=IntNeuSolarCurrent*SinTable[FftSampleData.FFT_Counter];
  FftSampleData.FFT_NeuSolarCurCosSum[FftSampleData.FFT_CounterIndex]+=IntNeuSolarCurrent*CosTable[FftSampleData.FFT_Counter];

  
  FftSampleData.FFT_Counter++;
  if(FftSampleData.FFT_Counter==64)
  {
    FftSampleData.FFT_Counter=0;
    FftSampleData.FFT_CounterIndex++;
  }
  
  SampleCounter++;

  if(SampleCounter & 0x02)InterruptDisplayRefresh();


  if((Timer.transfercomplete)||(Timer.TransmissionFailed))
  {
    TempInt=TimeOutCommTx;
    if(((USART2->ISR & 0X40) == 0x40)||(CounterSendComplete==TempInt)||(Timer.TransmissionFailed))
    {
      RS485Receive;
      SWITCH_OFF_LED_COMM;
     
      Timer.transfercomplete=0;
      Timer.TransmissionFailed=0;
      CounterSendComplete=0;
    }
    else CounterSendComplete++;
  }
  Tc20ms++;
  if(Tc20ms>=T_20MS)
  {
    Tc20ms=0;
    if(pwrDlyFlag==0)++StartDelayCount;
    else StartDelayCount = 0;
    ReadSwitches();
    ReadInputs();
  }
}

static bool IsRPhaseGridHealthyContactorOn()
{
    return INPUT_R_PHASE_GRID_HEALTHY_CONTACTOR_ON;
}
static bool IsYPhaseGridHealthyContactorOn()
{
    return INPUT_Y_PHASE_GRID_HEALTHY_CONTACTOR_ON;
}
static bool IsBPhaseGridHealthyContactorOn()
{
    return INPUT_B_PHASE_GRID_HEALTHY_CONTACTOR_ON;
}
static bool IsLoadOnSolarContactorOn()
{
    return INPUT_LOAD_ON_SOLAR_CONTACTOR_ON;
}
static bool IsLoadOnGridContactorOn()
{
    return INPUT_LOAD_ON_GRID_CONTACTOR_ON;
}
static bool IsSPDFailed()
{
    return !INPUT_SPD_HEALTHY;
}
static bool IsDGOff()
{
    return !INPUT_DG_RUNNING;
}
static bool IsDC48Available()
{
    return INPUT_48V_AVAILABLE;
}

// This is called to read digital inputs (debouncing)
// This function is called once every 20 msec
//
void ReadInputs()
{
    struct DigInputState
    {
        bool* val;
        uint8_t timer;
        bool (*read)(void);
    };

    // DEVNOTE: Keep this debouncing ticks to less than 1 sec in total
    //
#define MAX_DEBOUNCING_TICK 5
    static struct DigInputState dinStateArray[] =
    {
        {
            .val = &g_DigInputs.MainsRPhaseContactorOn,
            .timer = MAX_DEBOUNCING_TICK,
            .read = IsRPhaseGridHealthyContactorOn,
        },
        {
            .val = &g_DigInputs.MainsYPhaseContactorOn,
            .timer = MAX_DEBOUNCING_TICK,
            .read = IsYPhaseGridHealthyContactorOn,
        },
        {
            .val = &g_DigInputs.MainsBPhaseContactorOn,
            .timer = MAX_DEBOUNCING_TICK,
            .read = IsBPhaseGridHealthyContactorOn,
        },
        {
            .val = &g_DigInputs.LoadOnSolarContactorOn,
            .timer = MAX_DEBOUNCING_TICK,
            .read = IsLoadOnSolarContactorOn,
        },
        {
            .val = &g_DigInputs.LoadOnGridContactorOn,
            .timer = MAX_DEBOUNCING_TICK,
            .read = IsLoadOnGridContactorOn,
        },
        {
            .val = &g_DigInputs.SPDFailed,
            .timer = MAX_DEBOUNCING_TICK,
            .read = IsSPDFailed,
        },
        {
            .val = &g_DigInputs.DGOff,
            .timer = MAX_DEBOUNCING_TICK,
            .read = IsDGOff,
        },
        {
            .val = &g_DigInputs.DC48Available,
            .timer = MAX_DEBOUNCING_TICK,
            .read = IsDC48Available,
        },
    };

    for (uint8_t i = 0; i < ARRAY_SIZE(dinStateArray); i++)
    {
        struct DigInputState* dinState = &dinStateArray[i];

        if (dinState->read() != *dinState->val)
        {
            if (dinState->timer)
            {
                dinState->timer--;
            }
            if (!dinState->timer)
            {
                *dinState->val = !*dinState->val;
                dinState->timer = MAX_DEBOUNCING_TICK;
            }
        }
        else
        {
            dinState->timer = MAX_DEBOUNCING_TICK;
        }
    }
#undef MAX_DEBOUNCING_TICK
}

uint16_t freqcounter;
void CalculateFreqYBPhase(int16_t TempIntFreqVol)
{
    FilOut_1=TempIntFreqVol;
 
    if(RYFreqMeasDuration>20e6)
    {

      CycleCounter=0;
      RYFreqMeasDuration=0;
      SaveFreqMeasDuration=0;
      FreqFlag  &=~FF_HIGH_DETECTED;
    }
    
    if(FreqFlag & FF_HIGH_DETECTED)
    {
      RYFreqMeasDuration +=IntTimerCount.TimerPresentValue;
      freqcounter++;
      if((FilOut_1>=FREQ_LIMIT_VOL)&&(PrevSampleVol<FREQ_LIMIT_VOL))
      {
        CycleCounter++;
        if(CycleCounter==50)
        {
          FreqFlag |=FF_MEAS_OVER;
          CycleCounter=0;
          freqcounter=0;
          SaveFreqMeasDuration=RYFreqMeasDuration;
          RYFreqMeasDuration=0;
        }
      }
    }
    else
    {
      RYFreqMeasDuration=0;
      CycleCounter=0;
      freqcounter=0;
      if((FilOut_1>FREQ_LIMIT_VOL)&&(PrevSampleVol<FREQ_LIMIT_VOL))
        FreqFlag |=FF_HIGH_DETECTED;
    }
    PrevSampleVol=FilOut_1;    
  
}

  void ReadSwitches(void)
  {
   SwPressed=0; 

   if(INPUT_KEY_INC)SwPressed |=KEY_INC;
   if(INPUT_KEY_NEXT)SwPressed |=KEY_NEXT;
   if(INPUT_KEY_DEC)SwPressed |=KEY_DEC;
   
   if((SwPressed==0)||(SwPressed!=SwPrev))
   {
      SwTimer=0;
      SwFlag=0;
      SwValue=0;
      SwPrev=SwPressed;
      SwTimeOverflow=0;
      SwTimeOverflow2=0;
      SwTimeOverflow3=0;
    }
    else
    {
      
      if(SwTimer==0)
      {
       SwTimer++; 
       if(SwTimeOverflow3>=SW_OVF_LIMIT3)SwValue=SW_STEP_VALUE3;
       else if(SwTimeOverflow2>=SW_OVF_LIMIT2)SwValue=SW_STEP_VALUE2;
       else if(SwTimeOverflow>=SW_OVF_LIMIT)SwValue=SW_STEP_VALUE;
       else SwValue=1;
       SwFlag |=SW_NEW_VALUE_FLAG;
      }
      else
      {
        SwTimer++;
        if(SwTimer>=SW_1SEC)
        {
          SwTimer=0;
          if(SwTimeOverflow<=SW_OVF_LIMIT)SwTimeOverflow++;
          if(SwTimeOverflow2<=SW_OVF_LIMIT2)SwTimeOverflow2++;
          if(SwTimeOverflow3<=SW_OVF_LIMIT3)SwTimeOverflow3++;
        }
      }
            
    }

  }


void ClearInterruptVariables(void)
{
  uint32_t i;
  SampleCounter=0;
  memset(&IntDataSum, 0, sizeof(IntDataSum));

  for(i=0;i<50;i++)
  {
      FftSampleData.FFT_RVolSinSum[i]=0;
      FftSampleData.FFT_RVolCosSum[i]=0;
      FftSampleData.FFT_YVolSinSum[i]=0;
      FftSampleData.FFT_YVolCosSum[i]=0;
      FftSampleData.FFT_BVolSinSum[i]=0;
      FftSampleData.FFT_BVolCosSum[i]=0;
      FftSampleData.FFT_RCurSinSum[i]=0;
      FftSampleData.FFT_RCurCosSum[i]=0;
      FftSampleData.FFT_YCurSinSum[i]=0;
      FftSampleData.FFT_YCurCosSum[i]=0;
      FftSampleData.FFT_BCurSinSum[i]=0;
      FftSampleData.FFT_BCurCosSum[i]=0;

      FftSampleData.FFT_RSolarVolSinSum[i]=0;
      FftSampleData.FFT_RSolarVolCosSum[i]=0;
      FftSampleData.FFT_YSolarVolSinSum[i]=0;
      FftSampleData.FFT_YSolarVolCosSum[i]=0;
      FftSampleData.FFT_BSolarVolSinSum[i]=0;
      FftSampleData.FFT_BSolarVolCosSum[i]=0;
      FftSampleData.FFT_RSolarCurSinSum[i]=0;
      FftSampleData.FFT_RSolarCurCosSum[i]=0;
      FftSampleData.FFT_YSolarCurSinSum[i]=0;
      FftSampleData.FFT_YSolarCurCosSum[i]=0;
      FftSampleData.FFT_BSolarCurSinSum[i]=0;
      FftSampleData.FFT_BSolarCurCosSum[i]=0;
  }
  FftSampleData.FFT_Counter=0;
  FftSampleData.FFT_CounterIndex=0;
}
