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
void ReadInputs();
void CalculateFreqYBPhase(int16_t TempIntFreqVol);
void CalculateFreqPerPhase(struct FreqMeasState *s, int16_t TempIntFreqVol);
 
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
struct SAMPLE   Fan1CurrentSample;
struct SAMPLE   Fan2CurrentSample;

// Grid V_R / V_Y / V_B history (post-IIR samples) used only for the
// line-line voltage accumulator's per-pair phase-shift FIR. Slot [0] =
// current ISR sample, slots [1..4] = older samples. Updated every ISR;
// per-phase RMS, V*I power, FFT and PR_/PY_/PB_ all continue to use the
// raw IntVolRPhase / IntVolYPhase / IntVolBPhase locals.
//
static float VR_Hist[5];
static float VY_Hist[5];
static float VB_Hist[5];

// Same for solar V_R / V_Y / V_B (post-IIR) feeding the solar V_LL FIR.
//
static float VR_Solar_Hist[5];
static float VY_Solar_Hist[5];
static float VB_Solar_Hist[5];

// Grid I_R / I_Y / I_B history (post-IIR, post-gain, pre per-phase PF
// FIR) used only by the I_N accumulator's per-channel phase-alignment
// FIR. Slot [0] = current ISR sample, slots [1..4] = older samples.
// Per-phase RMS, V*I power, FFT and PR_/PY_/PB_ all continue to use the
// raw IntCurRPhase / IntCurYPhase / IntCurBPhase locals.
//
static float IR_Hist[5];
static float IY_Hist[5];
static float IB_Hist[5];

// Same for solar I_R / I_Y / I_B feeding the solar I_N FIR.
//
static float IR_Solar_Hist[5];
static float IY_Solar_Hist[5];
static float IB_Solar_Hist[5];

extern volatile uint16_t TimeOutCommTx;

uint16_t OneSecCounter;

// Instantaneous sample for checking if AC/DC Aux Power supply is available
// or not
//
static bool isDCPowerAvailableSample;
static bool isACPowerAvailableSample;

// UNDONE: The harmonics code assumes that the interrupt timer is setup such
// that it is called f * 3200 Hz where f is the freqency of input.
// But now, we have two different sources. So ideally, we should either
// be using two timers, one for each source (And decide when to start 
// ADC measurement) or reject Harmonics based measurement altogether
// (Can't ignore as reactive power is based on harmonics)
// This is also another reason for error in current measurement most likely
//
// DEVNOTE: SDADC3 is working maximum upto 3.3ksps
//
void ProcessMainInterrupt(void)
{

  float IntVolRPhase,IntVolYPhase,IntVolBPhase;
  float IntVolRSolarPhase,IntVolYSolarPhase,IntVolBSolarPhase;
  float IntCurRPhase,IntCurYPhase,IntCurBPhase;
  float IntCurRSolarPhase,IntCurYSolarPhase,IntCurBSolarPhase;
  float IntNeuCurrent, IntNeuSolarCurrent;
  float IntFan1Current, IntFan2Current;
  float IntAmbientTemperature;
  float IntVRefInt;
  float TempGainMult;
  int16_t TempInt;  
  uint8_t i;

  isDCPowerAvailableSample = AdcDataInArray[ADC_DC_PWR]>POWER_FAIL_SENSE_VALUE;
  isACPowerAvailableSample = AdcDataInArray[ADC_AC_PWR]>POWER_FAIL_SENSE_VALUE;

#ifdef MODEL_DATA_SAVE   
  // If we detect power supply is dropping, then save energy data immediately
  // UNDONE: This is not working
  // Make sure it works and also make sure that it isn't being triggered
  // multiple times
  //
  if(!isDCPowerAvailableSample && !isACPowerAvailableSample)
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
  TempInt=SdAdcDataInArray[SDADC_VR];
  TempInt=TempInt-0x1000;
  IntVolRPhase  =(float)TempInt-VIOffset.VolRPhase;
  IntVolRPhase *=WorkingCopyGain.VR_GAIN;
  
  TempInt=SdAdcDataInArray[SDADC_VY];
  TempInt=TempInt-0x1000;
  IntVolYPhase  =(float)TempInt-VIOffset.VolYPhase;
  IntVolYPhase *=WorkingCopyGain.VY_GAIN;
  
  TempInt=SdAdcDataInArray[SDADC_VB];
  TempInt=TempInt-0x1000;
  IntVolBPhase  =(float)TempInt-VIOffset.VolBPhase;
  IntVolBPhase *=WorkingCopyGain.VB_GAIN;
   
  TempInt=SdAdcDataInArray[SDADC_IR];
  TempInt=TempInt-0x1000;
  IntCurRPhase  =(float)TempInt-VIOffset.CurRPhase;
  IntCurRPhase *=WorkingCopyGain.IR_GAIN;

  TempInt=SdAdcDataInArray[SDADC_IY];
  TempInt=TempInt-0x1000;
  IntCurYPhase  =(float)TempInt-VIOffset.CurYPhase;
  IntCurYPhase *=WorkingCopyGain.IY_GAIN;

  TempInt=SdAdcDataInArray[SDADC_IB];
  TempInt=TempInt-0x1000;
  IntCurBPhase  =(float)TempInt-VIOffset.CurBPhase;
  IntCurBPhase *=WorkingCopyGain.IB_GAIN;

  TempInt=SdAdcDataInArray[SDADC_VR_SOLAR];
  TempInt=TempInt-0x1000;
  IntVolRSolarPhase  =(float)TempInt-VIOffset.VolRSolarPhase;
  IntVolRSolarPhase *=WorkingCopyGain.VR_SOLAR_GAIN;

  TempInt=SdAdcDataInArray[SDADC_VY_SOLAR];
  TempInt=TempInt-0x1000;
  IntVolYSolarPhase  =(float)TempInt-VIOffset.VolYSolarPhase;
  IntVolYSolarPhase *=WorkingCopyGain.VY_SOLAR_GAIN;

  TempInt=SdAdcDataInArray[SDADC_VB_SOLAR];
  TempInt=TempInt-0x1000;
  IntVolBSolarPhase  =(float)TempInt-VIOffset.VolBSolarPhase;
  IntVolBSolarPhase *=WorkingCopyGain.VB_SOLAR_GAIN;

  TempInt=SdAdcDataInArray[SDADC_IR_SOLAR];
  TempInt=TempInt-0x1000;
  IntCurRSolarPhase  =(float)TempInt-VIOffset.CurRSolarPhase;
  IntCurRSolarPhase *=WorkingCopyGain.IR_SOLAR_GAIN;

  TempInt=SdAdcDataInArray[SDADC_IY_SOLAR];
  TempInt=TempInt-0x1000;
  IntCurYSolarPhase  =(float)TempInt-VIOffset.CurYSolarPhase;
  IntCurYSolarPhase *=WorkingCopyGain.IY_SOLAR_GAIN;

  TempInt=SdAdcDataInArray[SDADC_IB_SOLAR];
  TempInt=TempInt-0x1000;
  IntCurBSolarPhase  =(float)TempInt-VIOffset.CurBSolarPhase;
  IntCurBSolarPhase *=WorkingCopyGain.IB_SOLAR_GAIN;

  TempInt=AdcDataInArray[ADC_FAN_1];
  TempInt=TempInt-0x1000;
  IntFan1Current  =(float)TempInt-VIOffset.Fan1Current;
  IntFan1Current *=WorkingCopyGain.FAN1_GAIN;

  TempInt=AdcDataInArray[ADC_FAN_2];
  TempInt=TempInt-0x1000;
  IntFan2Current  =(float)TempInt-VIOffset.Fan2Current;
  IntFan2Current *=WorkingCopyGain.FAN2_GAIN;

  IntAmbientTemperature=AdcDataInArray[ADC_A_TEMP];
  IntVRefInt=AdcDataInArray[ADC_VREFINT];

  // UNDONE: Do the current & power calculation for solar

  ADC1->CR2 |=ADC_CR2_SWSTART;   // Start New conversion 
  SDADC1->CR2 |=SDADC_CR2_JSWSTART;
 
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

  TempGainMult=(Fan1CurrentSample.PrevIn_2+IntFan1Current)*FILT_600_COEFF_X1+\
                Fan1CurrentSample.PrevIn_1*FILT_600_COEFF_X2+\
                Fan1CurrentSample.PrevOut_1*FILT_600_COEFF_Y1+\
                Fan1CurrentSample.PrevOut_2*FILT_600_COEFF_Y2;
  Fan1CurrentSample.PrevOut_2=Fan1CurrentSample.PrevOut_1;
  Fan1CurrentSample.PrevOut_1=TempGainMult;
  Fan1CurrentSample.PrevIn_2=Fan1CurrentSample.PrevIn_1;
  Fan1CurrentSample.PrevIn_1=IntFan1Current;
  IntFan1Current=TempGainMult;

  TempGainMult=(Fan2CurrentSample.PrevIn_2+IntFan2Current)*FILT_600_COEFF_X1+\
                Fan2CurrentSample.PrevIn_1*FILT_600_COEFF_X2+\
                Fan2CurrentSample.PrevOut_1*FILT_600_COEFF_Y1+\
                Fan2CurrentSample.PrevOut_2*FILT_600_COEFF_Y2;
  Fan2CurrentSample.PrevOut_2=Fan2CurrentSample.PrevOut_1;
  Fan2CurrentSample.PrevOut_1=TempGainMult;
  Fan2CurrentSample.PrevIn_2=Fan2CurrentSample.PrevIn_1;
  Fan2CurrentSample.PrevIn_1=IntFan2Current;
  IntFan2Current=TempGainMult;

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

    IntDataSave.Fan1Current=IntDataSum.Fan1Current;
    IntDataSave.Fan2Current=IntDataSum.Fan2Current;
    IntDataSave.OffsetFan1Current=IntDataSum.OffsetFan1Current;
    IntDataSave.OffsetFan2Current=IntDataSum.OffsetFan2Current;

    IntDataSave.AmbientTemperature = IntDataSum.AmbientTemperature;
    IntDataSave.VRefInt            = IntDataSum.VRefInt;

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
  //
  // Per-phase frequency zero-crossing on all 6 phases
  //
  CalculateFreqPerPhase(&g_FreqState.RPhase, (uint16_t)IntVolRPhase);
  CalculateFreqPerPhase(&g_FreqState.YPhase, (uint16_t)IntVolYPhase);
  CalculateFreqPerPhase(&g_FreqState.BPhase, (uint16_t)IntVolBPhase);
  CalculateFreqPerPhase(&g_FreqState.RSolarPhase, (uint16_t)IntVolRSolarPhase);
  CalculateFreqPerPhase(&g_FreqState.YSolarPhase, (uint16_t)IntVolYSolarPhase);
  CalculateFreqPerPhase(&g_FreqState.BSolarPhase, (uint16_t)IntVolBSolarPhase);

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

  // Push raw post-IIR voltage samples to per-channel history. The
  // line-line accumulators below use FIR-corrected samples for V_LL only;
  // every other consumer of IntVolRPhase / IntVolYPhase / IntVolBPhase
  // (per-phase RMS just above, V*I power, FFT, PR_/PY_/PB_) keeps using
  // the raw locals.
  //
  VR_Hist[4] = VR_Hist[3]; VR_Hist[3] = VR_Hist[2];
  VR_Hist[2] = VR_Hist[1]; VR_Hist[1] = VR_Hist[0];
  VR_Hist[0] = IntVolRPhase;
  VY_Hist[4] = VY_Hist[3]; VY_Hist[3] = VY_Hist[2];
  VY_Hist[2] = VY_Hist[1]; VY_Hist[1] = VY_Hist[0];
  VY_Hist[0] = IntVolYPhase;
  VB_Hist[4] = VB_Hist[3]; VB_Hist[3] = VB_Hist[2];
  VB_Hist[2] = VB_Hist[1]; VB_Hist[1] = VB_Hist[0];
  VB_Hist[0] = IntVolBPhase;

  // Line-line voltage accumulators with per-pair phase-shift FIR. Same
  // 2-tap fractional-delay structure as PR_/PY_/PB_:
  //   y[n] = ALFA * (x[n - int_d] + BETA * x[n - int_d - 1])
  // INT_DELAY sign selects which channel of the pair gets the delay (>=0
  // -> first letter; <0 -> second letter). The unaffected channel uses its
  // current sample directly.
  //
  {
      float Cur, Prev;
      float A, B;

      // V_RY = V_R - V_Y. INT_DELAY >= 0 -> delay R, < 0 -> delay Y.
      //
      if (WorkingCopyGain.VLL_RY_INT_DELAY >= 0)
      {
          int8_t d = WorkingCopyGain.VLL_RY_INT_DELAY;
          if      (d == 0) { Cur = VR_Hist[0]; Prev = VR_Hist[1]; }
          else if (d == 1) { Cur = VR_Hist[1]; Prev = VR_Hist[2]; }
          else if (d == 2) { Cur = VR_Hist[2]; Prev = VR_Hist[3]; }
          else             { Cur = VR_Hist[3]; Prev = VR_Hist[4]; }
          A = WorkingCopyGain.VLL_RY_ALFA * (Cur + WorkingCopyGain.VLL_RY_BETA * Prev);
          B = VY_Hist[0];
      }
      else
      {
          int8_t d = -WorkingCopyGain.VLL_RY_INT_DELAY - 1;
          if      (d == 0) { Cur = VY_Hist[0]; Prev = VY_Hist[1]; }
          else if (d == 1) { Cur = VY_Hist[1]; Prev = VY_Hist[2]; }
          else if (d == 2) { Cur = VY_Hist[2]; Prev = VY_Hist[3]; }
          else             { Cur = VY_Hist[3]; Prev = VY_Hist[4]; }
          A = VR_Hist[0];
          B = WorkingCopyGain.VLL_RY_ALFA * (Cur + WorkingCopyGain.VLL_RY_BETA * Prev);
      }
      TempGainMult = A - B;
      IntDataSum.VolRYPhPh += TempGainMult * TempGainMult;

      // V_BR = V_B - V_R. INT_DELAY >= 0 -> delay B, < 0 -> delay R.
      //
      if (WorkingCopyGain.VLL_BR_INT_DELAY >= 0)
      {
          int8_t d = WorkingCopyGain.VLL_BR_INT_DELAY;
          if      (d == 0) { Cur = VB_Hist[0]; Prev = VB_Hist[1]; }
          else if (d == 1) { Cur = VB_Hist[1]; Prev = VB_Hist[2]; }
          else if (d == 2) { Cur = VB_Hist[2]; Prev = VB_Hist[3]; }
          else             { Cur = VB_Hist[3]; Prev = VB_Hist[4]; }
          A = WorkingCopyGain.VLL_BR_ALFA * (Cur + WorkingCopyGain.VLL_BR_BETA * Prev);
          B = VR_Hist[0];
      }
      else
      {
          int8_t d = -WorkingCopyGain.VLL_BR_INT_DELAY - 1;
          if      (d == 0) { Cur = VR_Hist[0]; Prev = VR_Hist[1]; }
          else if (d == 1) { Cur = VR_Hist[1]; Prev = VR_Hist[2]; }
          else if (d == 2) { Cur = VR_Hist[2]; Prev = VR_Hist[3]; }
          else             { Cur = VR_Hist[3]; Prev = VR_Hist[4]; }
          A = VB_Hist[0];
          B = WorkingCopyGain.VLL_BR_ALFA * (Cur + WorkingCopyGain.VLL_BR_BETA * Prev);
      }
      TempGainMult = A - B;
      IntDataSum.VolBRPhPh += TempGainMult * TempGainMult;

      // V_YB stored as |V_B - V_Y|^2 in this codebase (RMS is symmetric so
      // the sign of the subtraction is irrelevant for the magnitude). The
      // FIR sign convention is on the (Y, B) pair: INT_DELAY >= 0 -> delay
      // Y, < 0 -> delay B.
      //
      if (WorkingCopyGain.VLL_YB_INT_DELAY >= 0)
      {
          int8_t d = WorkingCopyGain.VLL_YB_INT_DELAY;
          if      (d == 0) { Cur = VY_Hist[0]; Prev = VY_Hist[1]; }
          else if (d == 1) { Cur = VY_Hist[1]; Prev = VY_Hist[2]; }
          else if (d == 2) { Cur = VY_Hist[2]; Prev = VY_Hist[3]; }
          else             { Cur = VY_Hist[3]; Prev = VY_Hist[4]; }
          A = WorkingCopyGain.VLL_YB_ALFA * (Cur + WorkingCopyGain.VLL_YB_BETA * Prev);
          B = VB_Hist[0];
      }
      else
      {
          int8_t d = -WorkingCopyGain.VLL_YB_INT_DELAY - 1;
          if      (d == 0) { Cur = VB_Hist[0]; Prev = VB_Hist[1]; }
          else if (d == 1) { Cur = VB_Hist[1]; Prev = VB_Hist[2]; }
          else if (d == 2) { Cur = VB_Hist[2]; Prev = VB_Hist[3]; }
          else             { Cur = VB_Hist[3]; Prev = VB_Hist[4]; }
          A = VY_Hist[0];
          B = WorkingCopyGain.VLL_YB_ALFA * (Cur + WorkingCopyGain.VLL_YB_BETA * Prev);
      }
      TempGainMult = A - B;
      IntDataSum.VolYBPhPh += TempGainMult * TempGainMult;
  }
  
  // Neutral current with per-channel phase-alignment FIR.
  //
  // Each I channel has its own RC-filter / SDADC-slot delay, so even at
  // perfectly balanced load I_R + I_Y + I_B doesn't sum to zero. We delay
  // each channel by a calibrated amount so the three currents end up with
  // the same total channel delay (and remain exactly 120 deg apart) before
  // summing. FIR coefficients are derived once at boot from V_LL +
  // per-phase PF cal data; see DeriveNeutralFir() in Comm.c. INT_DELAY is
  // always >= 0 (FIR is delay-only) so no sign branching.
  //
  // Position: post-IIR, post per-channel I_GAIN, BEFORE the per-phase PF
  // FIR (which would otherwise overwrite the I locals). Histories are
  // pushed every cycle so the FIR taps are always coherent.
  //
  IR_Hist[4] = IR_Hist[3]; IR_Hist[3] = IR_Hist[2];
  IR_Hist[2] = IR_Hist[1]; IR_Hist[1] = IR_Hist[0];
  IR_Hist[0] = IntCurRPhase;
  IY_Hist[4] = IY_Hist[3]; IY_Hist[3] = IY_Hist[2];
  IY_Hist[2] = IY_Hist[1]; IY_Hist[1] = IY_Hist[0];
  IY_Hist[0] = IntCurYPhase;
  IB_Hist[4] = IB_Hist[3]; IB_Hist[3] = IB_Hist[2];
  IB_Hist[2] = IB_Hist[1]; IB_Hist[1] = IB_Hist[0];
  IB_Hist[0] = IntCurBPhase;

  {
      float Cur, Prev;
      float Ir_Aligned, Iy_Aligned, Ib_Aligned;
      int8_t d;

      d = WorkingCopyGain.I_N_R_INT_DELAY;
      if      (d == 0) { Cur = IR_Hist[0]; Prev = IR_Hist[1]; }
      else if (d == 1) { Cur = IR_Hist[1]; Prev = IR_Hist[2]; }
      else if (d == 2) { Cur = IR_Hist[2]; Prev = IR_Hist[3]; }
      else             { Cur = IR_Hist[3]; Prev = IR_Hist[4]; }
      Ir_Aligned = WorkingCopyGain.I_N_R_ALFA * (Cur + WorkingCopyGain.I_N_R_BETA * Prev);

      d = WorkingCopyGain.I_N_Y_INT_DELAY;
      if      (d == 0) { Cur = IY_Hist[0]; Prev = IY_Hist[1]; }
      else if (d == 1) { Cur = IY_Hist[1]; Prev = IY_Hist[2]; }
      else if (d == 2) { Cur = IY_Hist[2]; Prev = IY_Hist[3]; }
      else             { Cur = IY_Hist[3]; Prev = IY_Hist[4]; }
      Iy_Aligned = WorkingCopyGain.I_N_Y_ALFA * (Cur + WorkingCopyGain.I_N_Y_BETA * Prev);

      d = WorkingCopyGain.I_N_B_INT_DELAY;
      if      (d == 0) { Cur = IB_Hist[0]; Prev = IB_Hist[1]; }
      else if (d == 1) { Cur = IB_Hist[1]; Prev = IB_Hist[2]; }
      else if (d == 2) { Cur = IB_Hist[2]; Prev = IB_Hist[3]; }
      else             { Cur = IB_Hist[3]; Prev = IB_Hist[4]; }
      Ib_Aligned = WorkingCopyGain.I_N_B_ALFA * (Cur + WorkingCopyGain.I_N_B_BETA * Prev);

      IntNeuCurrent = Ir_Aligned + Iy_Aligned + Ib_Aligned;
  }

  IntDataSum.CurNeutral +=IntNeuCurrent*IntNeuCurrent;
  
// Phase compensation: y[n] = A*(x[n-int_D] + B*x[n-int_D-1])
  // The integer part of the delay (int_D) selects which stored samples to use.
  // The fractional part is handled by the FIR coefficients A (ALFA) and B (BETA).
  // History is maintained as a 4-deep shift register per channel for both
  // voltage and current. Sign of PX_INT_DELAY selects which signal the
  // correction is applied to:
  //   >= 0: delay on voltage, int_D = PX_INT_DELAY         (0..3 samples).
  //    < 0: delay on current, int_D = -PX_INT_DELAY - 1    (0..3 samples).
  // Only one channel per phase is filtered per sample; the other passes
  // through unchanged. Both history registers always shift so either branch
  // can be selected at any time without waking-up artefacts.
  //
  float VCur,VPrev,ICur,IPrev;
  float IntVolRPhaseOrig,IntVolYPhaseOrig,IntVolBPhaseOrig;
  float IntCurRPhaseOrig,IntCurYPhaseOrig,IntCurBPhaseOrig;

  IntVolRPhaseOrig=IntVolRPhase;
  IntCurRPhaseOrig=IntCurRPhase;
  if(WorkingCopyGain.PR_INT_DELAY>=0)
  {
    int8_t d=WorkingCopyGain.PR_INT_DELAY;
    if(d==0)      {VCur=IntVolRPhase;    VPrev=IntRPrevSample;}
    else if(d==1) {VCur=IntRPrevSample;  VPrev=IntRPrev2Sample;}
    else if(d==2) {VCur=IntRPrev2Sample; VPrev=IntRPrev3Sample;}
    else          {VCur=IntRPrev3Sample; VPrev=IntRPrev4Sample;}
    IntVolRPhase=WorkingCopyGain.PR_ALFA*(VCur+WorkingCopyGain.PR_BETA*VPrev);
  }
  else
  {
    int8_t d=-WorkingCopyGain.PR_INT_DELAY-1;
    if(d==0)      {ICur=IntCurRPhase;       IPrev=IntCurRPrevSample;}
    else if(d==1) {ICur=IntCurRPrevSample;  IPrev=IntCurRPrev2Sample;}
    else if(d==2) {ICur=IntCurRPrev2Sample; IPrev=IntCurRPrev3Sample;}
    else          {ICur=IntCurRPrev3Sample; IPrev=IntCurRPrev4Sample;}
    IntCurRPhase=WorkingCopyGain.PR_ALFA*(ICur+WorkingCopyGain.PR_BETA*IPrev);
  }
  IntRPrev4Sample=IntRPrev3Sample;
  IntRPrev3Sample=IntRPrev2Sample;
  IntRPrev2Sample=IntRPrevSample;
  IntRPrevSample=IntVolRPhaseOrig;
  IntCurRPrev4Sample=IntCurRPrev3Sample;
  IntCurRPrev3Sample=IntCurRPrev2Sample;
  IntCurRPrev2Sample=IntCurRPrevSample;
  IntCurRPrevSample=IntCurRPhaseOrig;

  IntVolYPhaseOrig=IntVolYPhase;
  IntCurYPhaseOrig=IntCurYPhase;
  if(WorkingCopyGain.PY_INT_DELAY>=0)
  {
    int8_t d=WorkingCopyGain.PY_INT_DELAY;
    if(d==0)      {VCur=IntVolYPhase;    VPrev=IntYPrevSample;}
    else if(d==1) {VCur=IntYPrevSample;  VPrev=IntYPrev2Sample;}
    else if(d==2) {VCur=IntYPrev2Sample; VPrev=IntYPrev3Sample;}
    else          {VCur=IntYPrev3Sample; VPrev=IntYPrev4Sample;}
    IntVolYPhase=WorkingCopyGain.PY_ALFA*(VCur+WorkingCopyGain.PY_BETA*VPrev);
  }
  else
  {
    int8_t d=-WorkingCopyGain.PY_INT_DELAY-1;
    if(d==0)      {ICur=IntCurYPhase;       IPrev=IntCurYPrevSample;}
    else if(d==1) {ICur=IntCurYPrevSample;  IPrev=IntCurYPrev2Sample;}
    else if(d==2) {ICur=IntCurYPrev2Sample; IPrev=IntCurYPrev3Sample;}
    else          {ICur=IntCurYPrev3Sample; IPrev=IntCurYPrev4Sample;}
    IntCurYPhase=WorkingCopyGain.PY_ALFA*(ICur+WorkingCopyGain.PY_BETA*IPrev);
  }
  IntYPrev4Sample=IntYPrev3Sample;
  IntYPrev3Sample=IntYPrev2Sample;
  IntYPrev2Sample=IntYPrevSample;
  IntYPrevSample=IntVolYPhaseOrig;
  IntCurYPrev4Sample=IntCurYPrev3Sample;
  IntCurYPrev3Sample=IntCurYPrev2Sample;
  IntCurYPrev2Sample=IntCurYPrevSample;
  IntCurYPrevSample=IntCurYPhaseOrig;

  IntVolBPhaseOrig=IntVolBPhase;
  IntCurBPhaseOrig=IntCurBPhase;
  if(WorkingCopyGain.PB_INT_DELAY>=0)
  {
    int8_t d=WorkingCopyGain.PB_INT_DELAY;
    if(d==0)      {VCur=IntVolBPhase;    VPrev=IntBPrevSample;}
    else if(d==1) {VCur=IntBPrevSample;  VPrev=IntBPrev2Sample;}
    else if(d==2) {VCur=IntBPrev2Sample; VPrev=IntBPrev3Sample;}
    else          {VCur=IntBPrev3Sample; VPrev=IntBPrev4Sample;}
    IntVolBPhase=WorkingCopyGain.PB_ALFA*(VCur+WorkingCopyGain.PB_BETA*VPrev);
  }
  else
  {
    int8_t d=-WorkingCopyGain.PB_INT_DELAY-1;
    if(d==0)      {ICur=IntCurBPhase;       IPrev=IntCurBPrevSample;}
    else if(d==1) {ICur=IntCurBPrevSample;  IPrev=IntCurBPrev2Sample;}
    else if(d==2) {ICur=IntCurBPrev2Sample; IPrev=IntCurBPrev3Sample;}
    else          {ICur=IntCurBPrev3Sample; IPrev=IntCurBPrev4Sample;}
    IntCurBPhase=WorkingCopyGain.PB_ALFA*(ICur+WorkingCopyGain.PB_BETA*IPrev);
  }
  IntBPrev4Sample=IntBPrev3Sample;
  IntBPrev3Sample=IntBPrev2Sample;
  IntBPrev2Sample=IntBPrevSample;
  IntBPrevSample=IntVolBPhaseOrig;
  IntCurBPrev4Sample=IntCurBPrev3Sample;
  IntCurBPrev3Sample=IntCurBPrev2Sample;
  IntCurBPrev2Sample=IntCurBPrevSample;
  IntCurBPrevSample=IntCurBPhaseOrig;

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

  // Push raw post-IIR solar voltage samples to per-channel history. Same
  // structure as the grid VR_Hist / VY_Hist / VB_Hist above; consumed only
  // by the solar V_LL accumulator's per-pair phase-shift FIR below.
  //
  VR_Solar_Hist[4] = VR_Solar_Hist[3]; VR_Solar_Hist[3] = VR_Solar_Hist[2];
  VR_Solar_Hist[2] = VR_Solar_Hist[1]; VR_Solar_Hist[1] = VR_Solar_Hist[0];
  VR_Solar_Hist[0] = IntVolRSolarPhase;
  VY_Solar_Hist[4] = VY_Solar_Hist[3]; VY_Solar_Hist[3] = VY_Solar_Hist[2];
  VY_Solar_Hist[2] = VY_Solar_Hist[1]; VY_Solar_Hist[1] = VY_Solar_Hist[0];
  VY_Solar_Hist[0] = IntVolYSolarPhase;
  VB_Solar_Hist[4] = VB_Solar_Hist[3]; VB_Solar_Hist[3] = VB_Solar_Hist[2];
  VB_Solar_Hist[2] = VB_Solar_Hist[1]; VB_Solar_Hist[1] = VB_Solar_Hist[0];
  VB_Solar_Hist[0] = IntVolBSolarPhase;

  // Solar line-line voltage accumulators with per-pair phase-shift FIR.
  // Same 2-tap fractional-delay structure as the grid V_LL block above.
  //
  {
      float Cur, Prev;
      float A, B;

      // V_RY_SOLAR = V_R - V_Y. INT_DELAY >= 0 -> delay R, < 0 -> delay Y.
      //
      if (WorkingCopyGain.VLL_RY_SOLAR_INT_DELAY >= 0)
      {
          int8_t d = WorkingCopyGain.VLL_RY_SOLAR_INT_DELAY;
          if      (d == 0) { Cur = VR_Solar_Hist[0]; Prev = VR_Solar_Hist[1]; }
          else if (d == 1) { Cur = VR_Solar_Hist[1]; Prev = VR_Solar_Hist[2]; }
          else if (d == 2) { Cur = VR_Solar_Hist[2]; Prev = VR_Solar_Hist[3]; }
          else             { Cur = VR_Solar_Hist[3]; Prev = VR_Solar_Hist[4]; }
          A = WorkingCopyGain.VLL_RY_SOLAR_ALFA * (Cur + WorkingCopyGain.VLL_RY_SOLAR_BETA * Prev);
          B = VY_Solar_Hist[0];
      }
      else
      {
          int8_t d = -WorkingCopyGain.VLL_RY_SOLAR_INT_DELAY - 1;
          if      (d == 0) { Cur = VY_Solar_Hist[0]; Prev = VY_Solar_Hist[1]; }
          else if (d == 1) { Cur = VY_Solar_Hist[1]; Prev = VY_Solar_Hist[2]; }
          else if (d == 2) { Cur = VY_Solar_Hist[2]; Prev = VY_Solar_Hist[3]; }
          else             { Cur = VY_Solar_Hist[3]; Prev = VY_Solar_Hist[4]; }
          A = VR_Solar_Hist[0];
          B = WorkingCopyGain.VLL_RY_SOLAR_ALFA * (Cur + WorkingCopyGain.VLL_RY_SOLAR_BETA * Prev);
      }
      TempGainMult = A - B;
      IntDataSum.VolRYSolarPhPh += TempGainMult * TempGainMult;

      // V_BR_SOLAR = V_B - V_R. INT_DELAY >= 0 -> delay B, < 0 -> delay R.
      //
      if (WorkingCopyGain.VLL_BR_SOLAR_INT_DELAY >= 0)
      {
          int8_t d = WorkingCopyGain.VLL_BR_SOLAR_INT_DELAY;
          if      (d == 0) { Cur = VB_Solar_Hist[0]; Prev = VB_Solar_Hist[1]; }
          else if (d == 1) { Cur = VB_Solar_Hist[1]; Prev = VB_Solar_Hist[2]; }
          else if (d == 2) { Cur = VB_Solar_Hist[2]; Prev = VB_Solar_Hist[3]; }
          else             { Cur = VB_Solar_Hist[3]; Prev = VB_Solar_Hist[4]; }
          A = WorkingCopyGain.VLL_BR_SOLAR_ALFA * (Cur + WorkingCopyGain.VLL_BR_SOLAR_BETA * Prev);
          B = VR_Solar_Hist[0];
      }
      else
      {
          int8_t d = -WorkingCopyGain.VLL_BR_SOLAR_INT_DELAY - 1;
          if      (d == 0) { Cur = VR_Solar_Hist[0]; Prev = VR_Solar_Hist[1]; }
          else if (d == 1) { Cur = VR_Solar_Hist[1]; Prev = VR_Solar_Hist[2]; }
          else if (d == 2) { Cur = VR_Solar_Hist[2]; Prev = VR_Solar_Hist[3]; }
          else             { Cur = VR_Solar_Hist[3]; Prev = VR_Solar_Hist[4]; }
          A = VB_Solar_Hist[0];
          B = WorkingCopyGain.VLL_BR_SOLAR_ALFA * (Cur + WorkingCopyGain.VLL_BR_SOLAR_BETA * Prev);
      }
      TempGainMult = A - B;
      IntDataSum.VolBRSolarPhPh += TempGainMult * TempGainMult;

      // V_YB_SOLAR stored as |V_B - V_Y|^2 (RMS is symmetric). FIR sign
      // convention is on the (Y, B) pair: INT_DELAY >= 0 -> delay Y, < 0
      // -> delay B.
      //
      if (WorkingCopyGain.VLL_YB_SOLAR_INT_DELAY >= 0)
      {
          int8_t d = WorkingCopyGain.VLL_YB_SOLAR_INT_DELAY;
          if      (d == 0) { Cur = VY_Solar_Hist[0]; Prev = VY_Solar_Hist[1]; }
          else if (d == 1) { Cur = VY_Solar_Hist[1]; Prev = VY_Solar_Hist[2]; }
          else if (d == 2) { Cur = VY_Solar_Hist[2]; Prev = VY_Solar_Hist[3]; }
          else             { Cur = VY_Solar_Hist[3]; Prev = VY_Solar_Hist[4]; }
          A = WorkingCopyGain.VLL_YB_SOLAR_ALFA * (Cur + WorkingCopyGain.VLL_YB_SOLAR_BETA * Prev);
          B = VB_Solar_Hist[0];
      }
      else
      {
          int8_t d = -WorkingCopyGain.VLL_YB_SOLAR_INT_DELAY - 1;
          if      (d == 0) { Cur = VB_Solar_Hist[0]; Prev = VB_Solar_Hist[1]; }
          else if (d == 1) { Cur = VB_Solar_Hist[1]; Prev = VB_Solar_Hist[2]; }
          else if (d == 2) { Cur = VB_Solar_Hist[2]; Prev = VB_Solar_Hist[3]; }
          else             { Cur = VB_Solar_Hist[3]; Prev = VB_Solar_Hist[4]; }
          A = VY_Solar_Hist[0];
          B = WorkingCopyGain.VLL_YB_SOLAR_ALFA * (Cur + WorkingCopyGain.VLL_YB_SOLAR_BETA * Prev);
      }
      TempGainMult = A - B;
      IntDataSum.VolYBSolarPhPh += TempGainMult * TempGainMult;
  }

  // Solar neutral current with per-channel phase-alignment FIR. Same
  // structure as the grid I_N block above.
  //
  IR_Solar_Hist[4] = IR_Solar_Hist[3]; IR_Solar_Hist[3] = IR_Solar_Hist[2];
  IR_Solar_Hist[2] = IR_Solar_Hist[1]; IR_Solar_Hist[1] = IR_Solar_Hist[0];
  IR_Solar_Hist[0] = IntCurRSolarPhase;
  IY_Solar_Hist[4] = IY_Solar_Hist[3]; IY_Solar_Hist[3] = IY_Solar_Hist[2];
  IY_Solar_Hist[2] = IY_Solar_Hist[1]; IY_Solar_Hist[1] = IY_Solar_Hist[0];
  IY_Solar_Hist[0] = IntCurYSolarPhase;
  IB_Solar_Hist[4] = IB_Solar_Hist[3]; IB_Solar_Hist[3] = IB_Solar_Hist[2];
  IB_Solar_Hist[2] = IB_Solar_Hist[1]; IB_Solar_Hist[1] = IB_Solar_Hist[0];
  IB_Solar_Hist[0] = IntCurBSolarPhase;

  {
      float Cur, Prev;
      float Ir_Aligned, Iy_Aligned, Ib_Aligned;
      int8_t d;

      d = WorkingCopyGain.I_N_R_SOLAR_INT_DELAY;
      if      (d == 0) { Cur = IR_Solar_Hist[0]; Prev = IR_Solar_Hist[1]; }
      else if (d == 1) { Cur = IR_Solar_Hist[1]; Prev = IR_Solar_Hist[2]; }
      else if (d == 2) { Cur = IR_Solar_Hist[2]; Prev = IR_Solar_Hist[3]; }
      else             { Cur = IR_Solar_Hist[3]; Prev = IR_Solar_Hist[4]; }
      Ir_Aligned = WorkingCopyGain.I_N_R_SOLAR_ALFA * (Cur + WorkingCopyGain.I_N_R_SOLAR_BETA * Prev);

      d = WorkingCopyGain.I_N_Y_SOLAR_INT_DELAY;
      if      (d == 0) { Cur = IY_Solar_Hist[0]; Prev = IY_Solar_Hist[1]; }
      else if (d == 1) { Cur = IY_Solar_Hist[1]; Prev = IY_Solar_Hist[2]; }
      else if (d == 2) { Cur = IY_Solar_Hist[2]; Prev = IY_Solar_Hist[3]; }
      else             { Cur = IY_Solar_Hist[3]; Prev = IY_Solar_Hist[4]; }
      Iy_Aligned = WorkingCopyGain.I_N_Y_SOLAR_ALFA * (Cur + WorkingCopyGain.I_N_Y_SOLAR_BETA * Prev);

      d = WorkingCopyGain.I_N_B_SOLAR_INT_DELAY;
      if      (d == 0) { Cur = IB_Solar_Hist[0]; Prev = IB_Solar_Hist[1]; }
      else if (d == 1) { Cur = IB_Solar_Hist[1]; Prev = IB_Solar_Hist[2]; }
      else if (d == 2) { Cur = IB_Solar_Hist[2]; Prev = IB_Solar_Hist[3]; }
      else             { Cur = IB_Solar_Hist[3]; Prev = IB_Solar_Hist[4]; }
      Ib_Aligned = WorkingCopyGain.I_N_B_SOLAR_ALFA * (Cur + WorkingCopyGain.I_N_B_SOLAR_BETA * Prev);

      IntNeuSolarCurrent = Ir_Aligned + Iy_Aligned + Ib_Aligned;
  }

  IntDataSum.CurNeutralSolar +=IntNeuSolarCurrent*IntNeuSolarCurrent;

  float IntVolRSolarPhaseOrig,IntVolYSolarPhaseOrig,IntVolBSolarPhaseOrig;
  float IntCurRSolarPhaseOrig,IntCurYSolarPhaseOrig,IntCurBSolarPhaseOrig;

  IntVolRSolarPhaseOrig=IntVolRSolarPhase;
  IntCurRSolarPhaseOrig=IntCurRSolarPhase;
  if(WorkingCopyGain.PR_SOLAR_INT_DELAY>=0)
  {
    int8_t d=WorkingCopyGain.PR_SOLAR_INT_DELAY;
    if(d==0)      {VCur=IntVolRSolarPhase;    VPrev=IntRSolarPrevSample;}
    else if(d==1) {VCur=IntRSolarPrevSample;  VPrev=IntRSolarPrev2Sample;}
    else if(d==2) {VCur=IntRSolarPrev2Sample; VPrev=IntRSolarPrev3Sample;}
    else          {VCur=IntRSolarPrev3Sample; VPrev=IntRSolarPrev4Sample;}
    IntVolRSolarPhase=WorkingCopyGain.PR_SOLAR_ALFA*(VCur+WorkingCopyGain.PR_SOLAR_BETA*VPrev);
  }
  else
  {
    int8_t d=-WorkingCopyGain.PR_SOLAR_INT_DELAY-1;
    if(d==0)      {ICur=IntCurRSolarPhase;       IPrev=IntCurRSolarPrevSample;}
    else if(d==1) {ICur=IntCurRSolarPrevSample;  IPrev=IntCurRSolarPrev2Sample;}
    else if(d==2) {ICur=IntCurRSolarPrev2Sample; IPrev=IntCurRSolarPrev3Sample;}
    else          {ICur=IntCurRSolarPrev3Sample; IPrev=IntCurRSolarPrev4Sample;}
    IntCurRSolarPhase=WorkingCopyGain.PR_SOLAR_ALFA*(ICur+WorkingCopyGain.PR_SOLAR_BETA*IPrev);
  }
  IntRSolarPrev4Sample=IntRSolarPrev3Sample;
  IntRSolarPrev3Sample=IntRSolarPrev2Sample;
  IntRSolarPrev2Sample=IntRSolarPrevSample;
  IntRSolarPrevSample=IntVolRSolarPhaseOrig;
  IntCurRSolarPrev4Sample=IntCurRSolarPrev3Sample;
  IntCurRSolarPrev3Sample=IntCurRSolarPrev2Sample;
  IntCurRSolarPrev2Sample=IntCurRSolarPrevSample;
  IntCurRSolarPrevSample=IntCurRSolarPhaseOrig;

  IntVolYSolarPhaseOrig=IntVolYSolarPhase;
  IntCurYSolarPhaseOrig=IntCurYSolarPhase;
  if(WorkingCopyGain.PY_SOLAR_INT_DELAY>=0)
  {
    int8_t d=WorkingCopyGain.PY_SOLAR_INT_DELAY;
    if(d==0)      {VCur=IntVolYSolarPhase;    VPrev=IntYSolarPrevSample;}
    else if(d==1) {VCur=IntYSolarPrevSample;  VPrev=IntYSolarPrev2Sample;}
    else if(d==2) {VCur=IntYSolarPrev2Sample; VPrev=IntYSolarPrev3Sample;}
    else          {VCur=IntYSolarPrev3Sample; VPrev=IntYSolarPrev4Sample;}
    IntVolYSolarPhase=WorkingCopyGain.PY_SOLAR_ALFA*(VCur+WorkingCopyGain.PY_SOLAR_BETA*VPrev);
  }
  else
  {
    int8_t d=-WorkingCopyGain.PY_SOLAR_INT_DELAY-1;
    if(d==0)      {ICur=IntCurYSolarPhase;       IPrev=IntCurYSolarPrevSample;}
    else if(d==1) {ICur=IntCurYSolarPrevSample;  IPrev=IntCurYSolarPrev2Sample;}
    else if(d==2) {ICur=IntCurYSolarPrev2Sample; IPrev=IntCurYSolarPrev3Sample;}
    else          {ICur=IntCurYSolarPrev3Sample; IPrev=IntCurYSolarPrev4Sample;}
    IntCurYSolarPhase=WorkingCopyGain.PY_SOLAR_ALFA*(ICur+WorkingCopyGain.PY_SOLAR_BETA*IPrev);
  }
  IntYSolarPrev4Sample=IntYSolarPrev3Sample;
  IntYSolarPrev3Sample=IntYSolarPrev2Sample;
  IntYSolarPrev2Sample=IntYSolarPrevSample;
  IntYSolarPrevSample=IntVolYSolarPhaseOrig;
  IntCurYSolarPrev4Sample=IntCurYSolarPrev3Sample;
  IntCurYSolarPrev3Sample=IntCurYSolarPrev2Sample;
  IntCurYSolarPrev2Sample=IntCurYSolarPrevSample;
  IntCurYSolarPrevSample=IntCurYSolarPhaseOrig;

  IntVolBSolarPhaseOrig=IntVolBSolarPhase;
  IntCurBSolarPhaseOrig=IntCurBSolarPhase;
  if(WorkingCopyGain.PB_SOLAR_INT_DELAY>=0)
  {
    int8_t d=WorkingCopyGain.PB_SOLAR_INT_DELAY;
    if(d==0)      {VCur=IntVolBSolarPhase;    VPrev=IntBSolarPrevSample;}
    else if(d==1) {VCur=IntBSolarPrevSample;  VPrev=IntBSolarPrev2Sample;}
    else if(d==2) {VCur=IntBSolarPrev2Sample; VPrev=IntBSolarPrev3Sample;}
    else          {VCur=IntBSolarPrev3Sample; VPrev=IntBSolarPrev4Sample;}
    IntVolBSolarPhase=WorkingCopyGain.PB_SOLAR_ALFA*(VCur+WorkingCopyGain.PB_SOLAR_BETA*VPrev);
  }
  else
  {
    int8_t d=-WorkingCopyGain.PB_SOLAR_INT_DELAY-1;
    if(d==0)      {ICur=IntCurBSolarPhase;       IPrev=IntCurBSolarPrevSample;}
    else if(d==1) {ICur=IntCurBSolarPrevSample;  IPrev=IntCurBSolarPrev2Sample;}
    else if(d==2) {ICur=IntCurBSolarPrev2Sample; IPrev=IntCurBSolarPrev3Sample;}
    else          {ICur=IntCurBSolarPrev3Sample; IPrev=IntCurBSolarPrev4Sample;}
    IntCurBSolarPhase=WorkingCopyGain.PB_SOLAR_ALFA*(ICur+WorkingCopyGain.PB_SOLAR_BETA*IPrev);
  }
  IntBSolarPrev4Sample=IntBSolarPrev3Sample;
  IntBSolarPrev3Sample=IntBSolarPrev2Sample;
  IntBSolarPrev2Sample=IntBSolarPrevSample;
  IntBSolarPrevSample=IntVolBSolarPhaseOrig;
  IntCurBSolarPrev4Sample=IntCurBSolarPrev3Sample;
  IntCurBSolarPrev3Sample=IntCurBSolarPrev2Sample;
  IntCurBSolarPrev2Sample=IntCurBSolarPrevSample;
  IntCurBSolarPrevSample=IntCurBSolarPhaseOrig;

  IntDataSum.RSolarPhasePower +=IntVolRSolarPhase*IntCurRSolarPhase;
  IntDataSum.YSolarPhasePower +=IntVolYSolarPhase*IntCurYSolarPhase;
  IntDataSum.BSolarPhasePower +=IntVolBSolarPhase*IntCurBSolarPhase;
 
  IntDataSum.Fan1Current += IntFan1Current*IntFan1Current;
  IntDataSum.Fan2Current += IntFan2Current*IntFan2Current;
  IntDataSum.OffsetFan1Current += IntFan1Current;
  IntDataSum.OffsetFan2Current += IntFan2Current;

  IntDataSum.AmbientTemperature += IntAmbientTemperature;
  IntDataSum.VRefInt            += IntVRefInt;

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
      if (!g_testingStatus.TestingModeEnabled)
      {
          g_LedStatus.Comm = false;
      }
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

// This is called to read digital inputs (debouncing)
// This function is called once every 20 msec
//
void ReadInputs()
{
    static bool* ptr[NUMBER_OF_INPUTS + NUMBER_OF_POWER_SUPPLIES] =
    {
        &g_DigInputs.Inputs[0],
        &g_DigInputs.Inputs[1],
        &g_DigInputs.Inputs[2],
        &g_DigInputs.Inputs[3],
        &g_DigInputs.Inputs[4],
        &g_DigInputs.Inputs[5],
        &g_DigInputs.Inputs[6],
        &g_DigInputs.Inputs[7],
        &g_powerSupplyStatus.Status[0],
        &g_powerSupplyStatus.Status[1],
    };
    bool currentValues[NUMBER_OF_INPUTS + NUMBER_OF_POWER_SUPPLIES] = 
    {
        IS_DIG_INPUT_1_HIGH(),
        IS_DIG_INPUT_2_HIGH(),
        IS_DIG_INPUT_3_HIGH(),
        IS_DIG_INPUT_4_HIGH(),
        IS_DIG_INPUT_5_HIGH(),
        IS_DIG_INPUT_6_HIGH(),
        IS_DIG_INPUT_7_HIGH(),
        IS_DIG_INPUT_8_HIGH(),
        isDCPowerAvailableSample,
        isACPowerAvailableSample,
    };
    // DEVNOTE: Keep this debouncing ticks to less than 1 sec in total
    //
#define MAX_DEBOUNCING_TICK 5
    static uint8_t timers[NUMBER_OF_INPUTS + NUMBER_OF_POWER_SUPPLIES] = 
    {
        MAX_DEBOUNCING_TICK,
        MAX_DEBOUNCING_TICK,
        MAX_DEBOUNCING_TICK,
        MAX_DEBOUNCING_TICK,
        MAX_DEBOUNCING_TICK,
        MAX_DEBOUNCING_TICK,
        MAX_DEBOUNCING_TICK,
        MAX_DEBOUNCING_TICK,
        MAX_DEBOUNCING_TICK,
        MAX_DEBOUNCING_TICK,
    };

    for (uint8_t i = 0; i < NUMBER_OF_INPUTS + NUMBER_OF_POWER_SUPPLIES; i++)
    {
        if (currentValues[i] != *ptr[i])
        {
            if (timers[i])
            {
                timers[i]--;
            }
            if (!timers[i])
            {
                *ptr[i] = !*ptr[i];
                timers[i] = MAX_DEBOUNCING_TICK;
            }
        }
        else
        {
            timers[i] = MAX_DEBOUNCING_TICK;
        }
    }
#undef MAX_DEBOUNCING_TICK
}

uint16_t freqcounter;
void CalculateFreqYBPhase(int16_t TempIntFreqVol)
{
    FilOut_1=TempIntFreqVol;
 
    if(RYFreqMeasDuration>20000000u)
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
      if((FilOut_1>=FREQ_LIMIT_VOL_ADC)&&(PrevSampleVol<FREQ_LIMIT_VOL_ADC))
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
      if((FilOut_1>FREQ_LIMIT_VOL_ADC)&&(PrevSampleVol<FREQ_LIMIT_VOL_ADC))
        FreqFlag |=FF_HIGH_DETECTED;
    }
    PrevSampleVol=FilOut_1;

}

//
// Generic per-phase frequency measurement via zero-crossing detection
// Each phase has its own FreqMeasState so frequencies are independent
// UNDONE: Verify FREQ_LIMIT_VOL * 20e6 and complete frequency logic
//
void CalculateFreqPerPhase(struct FreqMeasState *s, int16_t TempIntFreqVol)
{
    if(s->MeasDuration>20000000u)
    {
      s->CycleCounter=0;
      s->MeasDuration=0;
      s->SaveMeasDuration=0;
      s->Flag &=~FF_HIGH_DETECTED;
    }

    if(s->Flag & FF_HIGH_DETECTED)
    {
      s->MeasDuration +=IntTimerCount.TimerPresentValue;
      if((TempIntFreqVol>=FREQ_LIMIT_VOL_ADC)&&(s->PrevSampleVol<FREQ_LIMIT_VOL_ADC))
      {
        s->CycleCounter++;
        if(s->CycleCounter==50)
        {
          s->Flag |=FF_MEAS_OVER;
          s->CycleCounter=0;
          s->SaveMeasDuration=s->MeasDuration;
          s->MeasDuration=0;
        }
      }
    }
    else
    {
      s->MeasDuration=0;
      s->CycleCounter=0;
      if((TempIntFreqVol>FREQ_LIMIT_VOL_ADC)&&(s->PrevSampleVol<FREQ_LIMIT_VOL_ADC))
        s->Flag |=FF_HIGH_DETECTED;
    }
    s->PrevSampleVol=TempIntFreqVol;
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
