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
void CalculateFreqYBPhase(int16_t TempIntFreqVol);

 
struct SAMPLE   RCurSample;
struct SAMPLE   YCurSample;
struct SAMPLE   BCurSample;
struct SAMPLE   RVolSample;
struct SAMPLE   YVolSample;
struct SAMPLE   BVolSample;

extern volatile uint16_t TimeOutCommTx;

uint16_t OneSecCounter;

#define FREQ_LIMIT_VOL    (90)

void ProcessMainInterrupt(void)
{

  float IntVolRPhase,IntVolYPhase,IntVolBPhase;
  float IntCurRPhase,IntCurYPhase,IntCurBPhase;
  float IntNeuCurrent;
  float TempGainMult;
  int16_t TempInt;  
  uint8_t i;
#ifdef MODEL_DATA_SAVE    
  if(AdcDataInArray[ADC_POWER_SENSE]<POWER_FAIL_SENSE_VALUE)
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
  TempInt=AdcDataInArray[ADC_VR1];
  TempInt=TempInt+AdcDataInArray[ADC_VR2];
  TempInt=TempInt-0x1000;
  IntVolRPhase  =(float)TempInt-VIOffset.VolRPhase;
  IntVolRPhase *=WorkingCopyGain.VR_GAIN;
  
  
  TempInt=AdcDataInArray[ADC_VY1];
  TempInt=TempInt+AdcDataInArray[ADC_VY2];
  TempInt=TempInt-0x1000;
  IntVolYPhase  =(float)TempInt-VIOffset.VolYPhase;
  IntVolYPhase *=WorkingCopyGain.VY_GAIN;
  
  
  TempInt=AdcDataInArray[ADC_VB1];
  TempInt=TempInt+AdcDataInArray[ADC_VB2];
  TempInt=TempInt-0x1000;
  IntVolBPhase  =(float)TempInt-VIOffset.VolBPhase;
  IntVolBPhase *=WorkingCopyGain.VB_GAIN;
   
  
  TempInt=SDADC_CHANNEL_R;
  IntCurRPhase=TempInt*WorkingCopyGain.IR_GAIN-VIOffset.CurRPhase;
  
  TempInt=SDADC_CHANNEL_Y;
  IntCurYPhase=TempInt*WorkingCopyGain.IY_GAIN-VIOffset.CurYPhase;  
  
  TempInt=SDADC_CHANNEL_B;
  IntCurBPhase=TempInt*WorkingCopyGain.IB_GAIN-VIOffset.CurBPhase;  
  
  ADC1->CR2 |=ADC_CR2_SWSTART;   // Start New conversion 
  SDADC1->CR2 |=SDADC_CR2_JSWSTART;
  
  TempGainMult=(RCurSample.PrevIn_2+IntCurRPhase)*FILT_600_COEFF_X1+\
                RCurSample.PrevIn_1*FILT_600_COEFF_X2+\
                RCurSample.PrevOut_1*FILT_600_COEFF_Y1-\
                RCurSample.PrevOut_2*FILT_600_COEFF_Y2;
  RCurSample.PrevOut_2=RCurSample.PrevOut_1;
  RCurSample.PrevOut_2=TempGainMult;
  RCurSample.PrevIn_2=RCurSample.PrevIn_1;
  RCurSample.PrevIn_1=IntCurRPhase;
  IntCurRPhase=TempGainMult;
  
  TempGainMult=(YCurSample.PrevIn_2+IntCurYPhase)*FILT_600_COEFF_X1+\
                YCurSample.PrevIn_1*FILT_600_COEFF_X2+\
                YCurSample.PrevOut_1*FILT_600_COEFF_Y1-\
                YCurSample.PrevOut_2*FILT_600_COEFF_Y2;
  YCurSample.PrevOut_2=YCurSample.PrevOut_1;
  YCurSample.PrevOut_2=TempGainMult;
  YCurSample.PrevIn_2=YCurSample.PrevIn_1;
  YCurSample.PrevIn_1=IntCurYPhase;
  IntCurYPhase=TempGainMult;
  
  TempGainMult=(BCurSample.PrevIn_2+IntCurBPhase)*FILT_600_COEFF_X1+\
                BCurSample.PrevIn_1*FILT_600_COEFF_X2+\
                BCurSample.PrevOut_1*FILT_600_COEFF_Y1-\
                BCurSample.PrevOut_2*FILT_600_COEFF_Y2;
  BCurSample.PrevOut_2=BCurSample.PrevOut_1;
  BCurSample.PrevOut_2=TempGainMult;
  BCurSample.PrevIn_2=BCurSample.PrevIn_1;
  BCurSample.PrevIn_1=IntCurBPhase;
  IntCurBPhase=TempGainMult;
  
  TempGainMult=(RVolSample.PrevIn_2+IntVolRPhase)*FILT_600_COEFF_X1+\
                RVolSample.PrevIn_1*FILT_600_COEFF_X2+\
                RVolSample.PrevOut_1*FILT_600_COEFF_Y1-\
                RVolSample.PrevOut_2*FILT_600_COEFF_Y2;
  RVolSample.PrevOut_2=RVolSample.PrevOut_1;
  RVolSample.PrevOut_2=TempGainMult;
  RVolSample.PrevIn_2=RVolSample.PrevIn_1;
  RVolSample.PrevIn_1=IntVolRPhase;
  IntVolRPhase=TempGainMult;
  
  TempGainMult=(YVolSample.PrevIn_2+IntVolYPhase)*FILT_600_COEFF_X1+\
                YVolSample.PrevIn_1*FILT_600_COEFF_X2+\
                YVolSample.PrevOut_1*FILT_600_COEFF_Y1-\
                YVolSample.PrevOut_2*FILT_600_COEFF_Y2;
  YVolSample.PrevOut_2=YVolSample.PrevOut_1;
  YVolSample.PrevOut_2=TempGainMult;
  YVolSample.PrevIn_2=YVolSample.PrevIn_1;
  YVolSample.PrevIn_1=IntVolYPhase;
  IntVolYPhase=TempGainMult;
  
  TempGainMult=(BVolSample.PrevIn_2+IntVolBPhase)*FILT_600_COEFF_X1+\
                BVolSample.PrevIn_1*FILT_600_COEFF_X2+\
                BVolSample.PrevOut_1*FILT_600_COEFF_Y1-\
                BVolSample.PrevOut_2*FILT_600_COEFF_Y2;
  BVolSample.PrevOut_2=BVolSample.PrevOut_1;
  BVolSample.PrevOut_2=TempGainMult;
  BVolSample.PrevIn_2=BVolSample.PrevIn_1;
  BVolSample.PrevIn_1=IntVolBPhase;
  IntVolBPhase=TempGainMult;
  
  
  OneSecCounter++;
  if(SampleCounter==(NO_OF_SAMPLES-1))
  {
    IntDataSave.TimerCountValue= IntTimerCount.TimerPresentValue;
    IntTimerCount.TimerPresentValue=IntTimerCount.TimerNewValue;
    TIM2->ARR=IntTimerCount.TimerPresentValue;
  }
  else if(SampleCounter==NO_OF_SAMPLES)
  {
    IntWattPerPulseTick=WattPerPulseTick;
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
    IntDataSave.YRPower3P3W=IntDataSum.YRPower3P3W;
    IntDataSave.YBPower3P3W=IntDataSum.YBPower3P3W;
    
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
      
    for(i=0;i<21;i++)
    {
      *(&IntDataSum.VolRPhase+i)=0;
    }
    
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
    }
    FftSampleData.FFT_Counter=0;
    FftSampleData.FFT_CounterIndex=0;
  }
#ifdef MODEL_EPULSE  
  if(KWhLedOnCounter)KWhLedOnCounter--;
  if(KWhLedOnCounter==1)KWH_LED_OFF;;
  SumWattPerPulseTick +=IntWattPerPulseTick;
  if(SumWattPerPulseTick>=3600000000)
  {
    SumWattPerPulseTick -=3600000000;
    KWH_LED_ON;
    KWhLedOnCounter=25;
  }
#endif    
#ifdef MODEL_RS485      
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
  if(InterruptFlag & INT_R_PHASE_REV)IntNeuCurrent -=IntCurRPhase;
  else IntNeuCurrent +=IntCurRPhase;
  if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_3P4W)
  {
    if(InterruptFlag & INT_Y_PHASE_REV)IntNeuCurrent -=IntCurYPhase;
    else IntNeuCurrent +=IntCurYPhase;
  }
  if(InterruptFlag & INT_B_PHASE_REV)IntNeuCurrent -=IntCurBPhase;
  else IntNeuCurrent +=IntCurBPhase;
  
  IntDataSum.CurNeutral +=IntNeuCurrent*IntNeuCurrent;
  
  
// Phase compensation  y [n] = x[n]+ b x[n -1]

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
  
  IntDataSum.YRPower3P3W +=IntVolYPhase*IntCurRPhase; //in case of 3P3W
  IntDataSum.YBPower3P3W +=IntVolYPhase*IntCurBPhase; //in case of 3P3W

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
    if(((USART3->ISR & 0X40) == 0x40)||(CounterSendComplete==TempInt)||(Timer.TransmissionFailed))
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
  }
 
       
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

   if(!(GPIOB->IDR & 0x010))SwPressed |=0x01;
   if(!(GPIOB->IDR & 0x08))SwPressed |=0x02;
   if(!(GPIOB->IDR & 0x04))SwPressed |=0x04;
   
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
  for(i=0;i<11;i++)*(&IntDataSum.VolRPhase+i)=0;

  for(i=0;i<3;i++)*(&IntDataSum.VolRYPhPh+i)=0;

  for(i=0;i<6;i++)*(&IntDataSum.OffsetVolRPhase+i)=0;

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
    
    
  }
  FftSampleData.FFT_Counter=0;
  FftSampleData.FFT_CounterIndex=0;
}
