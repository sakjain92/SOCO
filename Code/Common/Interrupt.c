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

// Per-channel post-IIR sample history, used by BOTH the line-line / neutral
// FIRs and the per-phase PF FIR. These two FIRs used to keep separate copies
// of the same sequence (these *_Hist arrays plus the Int*PrevSample globals),
// each maintained by an O(depth) shift every ISR. They are provably the same
// sequence - the working IntVol*/IntCur* locals are written once by the IIR
// and not touched again until the PF FIR - so a single circular buffer per
// channel now serves both, eliminating ~100 shift-stores per ISR and 48
// globals.
//
// Implementation: one rolling write index (g_HistHead) shared by every
// buffer, since all are pushed exactly once per ISR in lockstep. Buffers are
// HIST_DEPTH (power of two) so the wrap is a mask, not a divide. The newest
// sample sits at [g_HistHead]; "k samples ago" is HIST_AGO(buf,k). Push order
// is push-first: each buffer is written near the top of the sample, so
// HIST_AGO(buf,0) is the current sample - matching the old slot[0]=current /
// IntVol*Phase(live) convention of both FIRs.
//
#define HIST_DEPTH 8                                   /* power of two */
#define HIST_MASK  (HIST_DEPTH - 1)
#define HIST_AGO(buf, k)  ((buf)[(g_HistHead - (k)) & HIST_MASK])

static uint8_t g_HistHead;        // advanced once per ISR, before any push

static float VR_Hist[HIST_DEPTH];
static float VY_Hist[HIST_DEPTH];
static float VB_Hist[HIST_DEPTH];

static float VR_Solar_Hist[HIST_DEPTH];
static float VY_Solar_Hist[HIST_DEPTH];
static float VB_Solar_Hist[HIST_DEPTH];

static float IR_Hist[HIST_DEPTH];
static float IY_Hist[HIST_DEPTH];
static float IB_Hist[HIST_DEPTH];

static float IR_Solar_Hist[HIST_DEPTH];
static float IY_Solar_Hist[HIST_DEPTH];
static float IB_Solar_Hist[HIST_DEPTH];

extern volatile uint16_t TimeOutCommTx;

uint16_t OneSecCounter;

// FFT accumulator ping-pong (replaces the old per-window Sum->Save copy that
// ran inside ProcessMainInterrupt). The ISR accumulates into bank[g_FftAccBank];
// at each window boundary it publishes that bank to the main loop and switches
// to the other bank. The bank is self-seeded as it refills (each bin stores
// its first sample with '=', the rest with '+='), so no bank zeroing is needed.
//
volatile uint8_t g_FftAccBank   = 0;   // bank the ISR is filling
volatile uint8_t g_FftSnapBank  = 0;   // bank exposed to the main loop
volatile uint8_t g_FftSnapReady = 0;   // 1 = snapshot waiting for the main loop

// Self-seeding overwrites only the bins a window actually fills, so it is
// correct ONLY if every window fills all 50 bins; a short window would leave
// stale bins (the old per-bank memset scrubbed those). Lock the invariant:
// NO_OF_SAMPLES / 64-samples-per-bin must equal the 50-element bin arrays.
COMPILE_ASSERT(NO_OF_SAMPLES == 50 * 64);

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
  // DC offset removal is applied AFTER the gain multiply, not before.
  // The OffsetX trickle accumulator in Metrology.c is a first-order
  // recursive update; we want its loop gain = 1 (pole at 0, one-step
  // convergence). If VIOffset were subtracted BEFORE the gain multiply,
  // the post-gain sum that feeds the update would carry an extra
  // factor of G, putting the pole at (1-G). For G > 1 (typical
  // post-cal CT) that pole goes negative and the tracker rings
  // sign-alternating each integration cycle, visible as a slow
  // oscillation in the displayed RMS. Subtracting after gain makes
  // VIOffset converge to G * delta_true instead of delta_true, which
  // is equivalent at steady state: the downstream IntXPhase is G * s
  // either way.
  //
  TempInt=SdAdcDataInArray[SDADC_VR];
  TempInt=TempInt-0x1000;
  IntVolRPhase  =(float)TempInt;
  IntVolRPhase *=WorkingCopyGain.VR_GAIN;
  IntVolRPhase -=VIOffset.VolRPhase;

  TempInt=SdAdcDataInArray[SDADC_VY];
  TempInt=TempInt-0x1000;
  IntVolYPhase  =(float)TempInt;
  IntVolYPhase *=WorkingCopyGain.VY_GAIN;
  IntVolYPhase -=VIOffset.VolYPhase;

  TempInt=SdAdcDataInArray[SDADC_VB];
  TempInt=TempInt-0x1000;
  IntVolBPhase  =(float)TempInt;
  IntVolBPhase *=WorkingCopyGain.VB_GAIN;
  IntVolBPhase -=VIOffset.VolBPhase;

  TempInt=SdAdcDataInArray[SDADC_IR];
  TempInt=TempInt-0x1000;
  IntCurRPhase  =(float)TempInt;
  IntCurRPhase *=WorkingCopyGain.IR_GAIN;
  IntCurRPhase -=VIOffset.CurRPhase;

  TempInt=SdAdcDataInArray[SDADC_IY];
  TempInt=TempInt-0x1000;
  IntCurYPhase  =(float)TempInt;
  IntCurYPhase *=WorkingCopyGain.IY_GAIN;
  IntCurYPhase -=VIOffset.CurYPhase;

  TempInt=SdAdcDataInArray[SDADC_IB];
  TempInt=TempInt-0x1000;
  IntCurBPhase  =(float)TempInt;
  IntCurBPhase *=WorkingCopyGain.IB_GAIN;
  IntCurBPhase -=VIOffset.CurBPhase;

  TempInt=SdAdcDataInArray[SDADC_VR_SOLAR];
  TempInt=TempInt-0x1000;
  IntVolRSolarPhase  =(float)TempInt;
  IntVolRSolarPhase *=WorkingCopyGain.VR_SOLAR_GAIN;
  IntVolRSolarPhase -=VIOffset.VolRSolarPhase;

  TempInt=SdAdcDataInArray[SDADC_VY_SOLAR];
  TempInt=TempInt-0x1000;
  IntVolYSolarPhase  =(float)TempInt;
  IntVolYSolarPhase *=WorkingCopyGain.VY_SOLAR_GAIN;
  IntVolYSolarPhase -=VIOffset.VolYSolarPhase;

  TempInt=SdAdcDataInArray[SDADC_VB_SOLAR];
  TempInt=TempInt-0x1000;
  IntVolBSolarPhase  =(float)TempInt;
  IntVolBSolarPhase *=WorkingCopyGain.VB_SOLAR_GAIN;
  IntVolBSolarPhase -=VIOffset.VolBSolarPhase;

  TempInt=SdAdcDataInArray[SDADC_IR_SOLAR];
  TempInt=TempInt-0x1000;
  IntCurRSolarPhase  =(float)TempInt;
  IntCurRSolarPhase *=WorkingCopyGain.IR_SOLAR_GAIN;
  IntCurRSolarPhase -=VIOffset.CurRSolarPhase;

  TempInt=SdAdcDataInArray[SDADC_IY_SOLAR];
  TempInt=TempInt-0x1000;
  IntCurYSolarPhase  =(float)TempInt;
  IntCurYSolarPhase *=WorkingCopyGain.IY_SOLAR_GAIN;
  IntCurYSolarPhase -=VIOffset.CurYSolarPhase;

  TempInt=SdAdcDataInArray[SDADC_IB_SOLAR];
  TempInt=TempInt-0x1000;
  IntCurBSolarPhase  =(float)TempInt;
  IntCurBSolarPhase *=WorkingCopyGain.IB_SOLAR_GAIN;
  IntCurBSolarPhase -=VIOffset.CurBSolarPhase;

  TempInt=AdcDataInArray[ADC_FAN_1];
  TempInt=TempInt-0x1000;
  IntFan1Current  =(float)TempInt;
  IntFan1Current *=WorkingCopyGain.FAN1_GAIN;
  IntFan1Current -=VIOffset.Fan1Current;

  TempInt=AdcDataInArray[ADC_FAN_2];
  TempInt=TempInt-0x1000;
  IntFan2Current  =(float)TempInt;
  IntFan2Current *=WorkingCopyGain.FAN2_GAIN;
  IntFan2Current -=VIOffset.Fan2Current;

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
 
    // FFT bank hand-off (ping-pong). Previously this spot copied all 28
    // Sum[] arrays into Save[] and zeroed Sum[] (50 iterations, ~10k cycles)
    // which overran the shortened sample period at high input frequency.
    // Instead, just publish the bank we filled and switch to the other one,
    // which the ISR self-seeds as it refills it (no pre-zeroing needed).
    // Guard: if the main loop has not yet consumed the previous snapshot
    // (g_FftSnapReady still set), keep filling the current bank rather than
    // clobber a bank the consumer is still reading. That merges two windows
    // into one snapshot - a rare, benign glitch, never a torn read.
    //
    if(!g_FftSnapReady)
    {
      g_FftSnapBank  = g_FftAccBank;   // expose the completed window
      g_FftAccBank  ^= 1;              // fill the other bank (self-seeded on refill)
      g_FftSnapReady = 1;              // tell the main loop a snapshot is ready
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
  // Advance the shared circular-history write index once for this ISR, then
  // push every per-channel history (push-first: HIST_AGO(buf,0) == current).
  // All 12 buffers below are written against this same g_HistHead.
  //
  g_HistHead = (g_HistHead + 1) & HIST_MASK;
  VR_Hist[g_HistHead] = IntVolRPhase;
  VY_Hist[g_HistHead] = IntVolYPhase;
  VB_Hist[g_HistHead] = IntVolBPhase;

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
          Cur = HIST_AGO(VR_Hist, d); Prev = HIST_AGO(VR_Hist, d + 1);
          A = WorkingCopyGain.VLL_RY_ALFA * (Cur + WorkingCopyGain.VLL_RY_BETA * Prev);
          B = HIST_AGO(VY_Hist, 0);
      }
      else
      {
          int8_t d = -WorkingCopyGain.VLL_RY_INT_DELAY - 1;
          Cur = HIST_AGO(VY_Hist, d); Prev = HIST_AGO(VY_Hist, d + 1);
          A = HIST_AGO(VR_Hist, 0);
          B = WorkingCopyGain.VLL_RY_ALFA * (Cur + WorkingCopyGain.VLL_RY_BETA * Prev);
      }
      TempGainMult = A - B;
      IntDataSum.VolRYPhPh += TempGainMult * TempGainMult;

      // V_BR = V_B - V_R. INT_DELAY >= 0 -> delay B, < 0 -> delay R.
      //
      if (WorkingCopyGain.VLL_BR_INT_DELAY >= 0)
      {
          int8_t d = WorkingCopyGain.VLL_BR_INT_DELAY;
          Cur = HIST_AGO(VB_Hist, d); Prev = HIST_AGO(VB_Hist, d + 1);
          A = WorkingCopyGain.VLL_BR_ALFA * (Cur + WorkingCopyGain.VLL_BR_BETA * Prev);
          B = HIST_AGO(VR_Hist, 0);
      }
      else
      {
          int8_t d = -WorkingCopyGain.VLL_BR_INT_DELAY - 1;
          Cur = HIST_AGO(VR_Hist, d); Prev = HIST_AGO(VR_Hist, d + 1);
          A = HIST_AGO(VB_Hist, 0);
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
          Cur = HIST_AGO(VY_Hist, d); Prev = HIST_AGO(VY_Hist, d + 1);
          A = WorkingCopyGain.VLL_YB_ALFA * (Cur + WorkingCopyGain.VLL_YB_BETA * Prev);
          B = HIST_AGO(VB_Hist, 0);
      }
      else
      {
          int8_t d = -WorkingCopyGain.VLL_YB_INT_DELAY - 1;
          Cur = HIST_AGO(VB_Hist, d); Prev = HIST_AGO(VB_Hist, d + 1);
          A = HIST_AGO(VY_Hist, 0);
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
  IR_Hist[g_HistHead] = IntCurRPhase;
  IY_Hist[g_HistHead] = IntCurYPhase;
  IB_Hist[g_HistHead] = IntCurBPhase;

  {
      float Cur, Prev;
      float Ir_Aligned, Iy_Aligned, Ib_Aligned;
      int8_t d;

      d = WorkingCopyGain.I_N_R_INT_DELAY;
      Cur = HIST_AGO(IR_Hist, d); Prev = HIST_AGO(IR_Hist, d + 1);
      Ir_Aligned = WorkingCopyGain.I_N_R_ALFA * (Cur + WorkingCopyGain.I_N_R_BETA * Prev);

      d = WorkingCopyGain.I_N_Y_INT_DELAY;
      Cur = HIST_AGO(IY_Hist, d); Prev = HIST_AGO(IY_Hist, d + 1);
      Iy_Aligned = WorkingCopyGain.I_N_Y_ALFA * (Cur + WorkingCopyGain.I_N_Y_BETA * Prev);

      d = WorkingCopyGain.I_N_B_INT_DELAY;
      Cur = HIST_AGO(IB_Hist, d); Prev = HIST_AGO(IB_Hist, d + 1);
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
  // History is the unified per-channel circular buffer (VR_Hist / IR_Hist
  // ...), already pushed at the top of this ISR with the post-IIR sample.
  // HIST_AGO(buf,0) is therefore the current pre-PF sample (== the old live
  // IntVol*Phase) and HIST_AGO(buf,k>=1) are the old Int*Prev[k]Sample taps -
  // no separate shift register and no end-of-block shift needed.
  //
  float VCur,VPrev,ICur,IPrev;

  if(WorkingCopyGain.PR_INT_DELAY>=0)
  {
    int8_t d=WorkingCopyGain.PR_INT_DELAY;
    VCur=HIST_AGO(VR_Hist,d); VPrev=HIST_AGO(VR_Hist,d+1);
    IntVolRPhase=WorkingCopyGain.PR_ALFA*(VCur+WorkingCopyGain.PR_BETA*VPrev);
  }
  else
  {
    int8_t d=-WorkingCopyGain.PR_INT_DELAY-1;
    ICur=HIST_AGO(IR_Hist,d); IPrev=HIST_AGO(IR_Hist,d+1);
    IntCurRPhase=WorkingCopyGain.PR_ALFA*(ICur+WorkingCopyGain.PR_BETA*IPrev);
  }

  if(WorkingCopyGain.PY_INT_DELAY>=0)
  {
    int8_t d=WorkingCopyGain.PY_INT_DELAY;
    VCur=HIST_AGO(VY_Hist,d); VPrev=HIST_AGO(VY_Hist,d+1);
    IntVolYPhase=WorkingCopyGain.PY_ALFA*(VCur+WorkingCopyGain.PY_BETA*VPrev);
  }
  else
  {
    int8_t d=-WorkingCopyGain.PY_INT_DELAY-1;
    ICur=HIST_AGO(IY_Hist,d); IPrev=HIST_AGO(IY_Hist,d+1);
    IntCurYPhase=WorkingCopyGain.PY_ALFA*(ICur+WorkingCopyGain.PY_BETA*IPrev);
  }

  if(WorkingCopyGain.PB_INT_DELAY>=0)
  {
    int8_t d=WorkingCopyGain.PB_INT_DELAY;
    VCur=HIST_AGO(VB_Hist,d); VPrev=HIST_AGO(VB_Hist,d+1);
    IntVolBPhase=WorkingCopyGain.PB_ALFA*(VCur+WorkingCopyGain.PB_BETA*VPrev);
  }
  else
  {
    int8_t d=-WorkingCopyGain.PB_INT_DELAY-1;
    ICur=HIST_AGO(IB_Hist,d); IPrev=HIST_AGO(IB_Hist,d+1);
    IntCurBPhase=WorkingCopyGain.PB_ALFA*(ICur+WorkingCopyGain.PB_BETA*IPrev);
  }

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
  VR_Solar_Hist[g_HistHead] = IntVolRSolarPhase;
  VY_Solar_Hist[g_HistHead] = IntVolYSolarPhase;
  VB_Solar_Hist[g_HistHead] = IntVolBSolarPhase;

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
          Cur = HIST_AGO(VR_Solar_Hist, d); Prev = HIST_AGO(VR_Solar_Hist, d + 1);
          A = WorkingCopyGain.VLL_RY_SOLAR_ALFA * (Cur + WorkingCopyGain.VLL_RY_SOLAR_BETA * Prev);
          B = HIST_AGO(VY_Solar_Hist, 0);
      }
      else
      {
          int8_t d = -WorkingCopyGain.VLL_RY_SOLAR_INT_DELAY - 1;
          Cur = HIST_AGO(VY_Solar_Hist, d); Prev = HIST_AGO(VY_Solar_Hist, d + 1);
          A = HIST_AGO(VR_Solar_Hist, 0);
          B = WorkingCopyGain.VLL_RY_SOLAR_ALFA * (Cur + WorkingCopyGain.VLL_RY_SOLAR_BETA * Prev);
      }
      TempGainMult = A - B;
      IntDataSum.VolRYSolarPhPh += TempGainMult * TempGainMult;

      // V_BR_SOLAR = V_B - V_R. INT_DELAY >= 0 -> delay B, < 0 -> delay R.
      //
      if (WorkingCopyGain.VLL_BR_SOLAR_INT_DELAY >= 0)
      {
          int8_t d = WorkingCopyGain.VLL_BR_SOLAR_INT_DELAY;
          Cur = HIST_AGO(VB_Solar_Hist, d); Prev = HIST_AGO(VB_Solar_Hist, d + 1);
          A = WorkingCopyGain.VLL_BR_SOLAR_ALFA * (Cur + WorkingCopyGain.VLL_BR_SOLAR_BETA * Prev);
          B = HIST_AGO(VR_Solar_Hist, 0);
      }
      else
      {
          int8_t d = -WorkingCopyGain.VLL_BR_SOLAR_INT_DELAY - 1;
          Cur = HIST_AGO(VR_Solar_Hist, d); Prev = HIST_AGO(VR_Solar_Hist, d + 1);
          A = HIST_AGO(VB_Solar_Hist, 0);
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
          Cur = HIST_AGO(VY_Solar_Hist, d); Prev = HIST_AGO(VY_Solar_Hist, d + 1);
          A = WorkingCopyGain.VLL_YB_SOLAR_ALFA * (Cur + WorkingCopyGain.VLL_YB_SOLAR_BETA * Prev);
          B = HIST_AGO(VB_Solar_Hist, 0);
      }
      else
      {
          int8_t d = -WorkingCopyGain.VLL_YB_SOLAR_INT_DELAY - 1;
          Cur = HIST_AGO(VB_Solar_Hist, d); Prev = HIST_AGO(VB_Solar_Hist, d + 1);
          A = HIST_AGO(VY_Solar_Hist, 0);
          B = WorkingCopyGain.VLL_YB_SOLAR_ALFA * (Cur + WorkingCopyGain.VLL_YB_SOLAR_BETA * Prev);
      }
      TempGainMult = A - B;
      IntDataSum.VolYBSolarPhPh += TempGainMult * TempGainMult;
  }

  // Solar neutral current with per-channel phase-alignment FIR. Same
  // structure as the grid I_N block above.
  //
  IR_Solar_Hist[g_HistHead] = IntCurRSolarPhase;
  IY_Solar_Hist[g_HistHead] = IntCurYSolarPhase;
  IB_Solar_Hist[g_HistHead] = IntCurBSolarPhase;

  {
      float Cur, Prev;
      float Ir_Aligned, Iy_Aligned, Ib_Aligned;
      int8_t d;

      d = WorkingCopyGain.I_N_R_SOLAR_INT_DELAY;
      Cur = HIST_AGO(IR_Solar_Hist, d); Prev = HIST_AGO(IR_Solar_Hist, d + 1);
      Ir_Aligned = WorkingCopyGain.I_N_R_SOLAR_ALFA * (Cur + WorkingCopyGain.I_N_R_SOLAR_BETA * Prev);

      d = WorkingCopyGain.I_N_Y_SOLAR_INT_DELAY;
      Cur = HIST_AGO(IY_Solar_Hist, d); Prev = HIST_AGO(IY_Solar_Hist, d + 1);
      Iy_Aligned = WorkingCopyGain.I_N_Y_SOLAR_ALFA * (Cur + WorkingCopyGain.I_N_Y_SOLAR_BETA * Prev);

      d = WorkingCopyGain.I_N_B_SOLAR_INT_DELAY;
      Cur = HIST_AGO(IB_Solar_Hist, d); Prev = HIST_AGO(IB_Solar_Hist, d + 1);
      Ib_Aligned = WorkingCopyGain.I_N_B_SOLAR_ALFA * (Cur + WorkingCopyGain.I_N_B_SOLAR_BETA * Prev);

      IntNeuSolarCurrent = Ir_Aligned + Iy_Aligned + Ib_Aligned;
  }

  IntDataSum.CurNeutralSolar +=IntNeuSolarCurrent*IntNeuSolarCurrent;

  // Solar PF FIR: same unified-circular-buffer scheme as the grid PF FIR.
  //
  if(WorkingCopyGain.PR_SOLAR_INT_DELAY>=0)
  {
    int8_t d=WorkingCopyGain.PR_SOLAR_INT_DELAY;
    VCur=HIST_AGO(VR_Solar_Hist,d); VPrev=HIST_AGO(VR_Solar_Hist,d+1);
    IntVolRSolarPhase=WorkingCopyGain.PR_SOLAR_ALFA*(VCur+WorkingCopyGain.PR_SOLAR_BETA*VPrev);
  }
  else
  {
    int8_t d=-WorkingCopyGain.PR_SOLAR_INT_DELAY-1;
    ICur=HIST_AGO(IR_Solar_Hist,d); IPrev=HIST_AGO(IR_Solar_Hist,d+1);
    IntCurRSolarPhase=WorkingCopyGain.PR_SOLAR_ALFA*(ICur+WorkingCopyGain.PR_SOLAR_BETA*IPrev);
  }

  if(WorkingCopyGain.PY_SOLAR_INT_DELAY>=0)
  {
    int8_t d=WorkingCopyGain.PY_SOLAR_INT_DELAY;
    VCur=HIST_AGO(VY_Solar_Hist,d); VPrev=HIST_AGO(VY_Solar_Hist,d+1);
    IntVolYSolarPhase=WorkingCopyGain.PY_SOLAR_ALFA*(VCur+WorkingCopyGain.PY_SOLAR_BETA*VPrev);
  }
  else
  {
    int8_t d=-WorkingCopyGain.PY_SOLAR_INT_DELAY-1;
    ICur=HIST_AGO(IY_Solar_Hist,d); IPrev=HIST_AGO(IY_Solar_Hist,d+1);
    IntCurYSolarPhase=WorkingCopyGain.PY_SOLAR_ALFA*(ICur+WorkingCopyGain.PY_SOLAR_BETA*IPrev);
  }

  if(WorkingCopyGain.PB_SOLAR_INT_DELAY>=0)
  {
    int8_t d=WorkingCopyGain.PB_SOLAR_INT_DELAY;
    VCur=HIST_AGO(VB_Solar_Hist,d); VPrev=HIST_AGO(VB_Solar_Hist,d+1);
    IntVolBSolarPhase=WorkingCopyGain.PB_SOLAR_ALFA*(VCur+WorkingCopyGain.PB_SOLAR_BETA*VPrev);
  }
  else
  {
    int8_t d=-WorkingCopyGain.PB_SOLAR_INT_DELAY-1;
    ICur=HIST_AGO(IB_Solar_Hist,d); IPrev=HIST_AGO(IB_Solar_Hist,d+1);
    IntCurBSolarPhase=WorkingCopyGain.PB_SOLAR_ALFA*(ICur+WorkingCopyGain.PB_SOLAR_BETA*IPrev);
  }

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
  // Accumulate into the active ping-pong bank (see g_FftAccBank). Self-seeding:
  // the first sample of each bin (fc==0) is stored with '=' (overwriting the
  // bank's stale data from its previous window); the other 63 use '+='.
  //
  {
  struct FFT_BANK *pAcc = &FftSampleData.bank[g_FftAccBank];
  uint16_t fc  = FftSampleData.FFT_Counter;
  uint16_t fci = FftSampleData.FFT_CounterIndex;

  if (fc == 0)
  {
    pAcc->RVolSin[fci]=IntVolRPhase*SinTable[fc];
    pAcc->RVolCos[fci]=IntVolRPhase*CosTable[fc];
    pAcc->YVolSin[fci]=IntVolYPhase*SinTable[fc];
    pAcc->YVolCos[fci]=IntVolYPhase*CosTable[fc];
    pAcc->BVolSin[fci]=IntVolBPhase*SinTable[fc];
    pAcc->BVolCos[fci]=IntVolBPhase*CosTable[fc];

    pAcc->RCurSin[fci]=IntCurRPhase*SinTable[fc];
    pAcc->RCurCos[fci]=IntCurRPhase*CosTable[fc];
    pAcc->YCurSin[fci]=IntCurYPhase*SinTable[fc];
    pAcc->YCurCos[fci]=IntCurYPhase*CosTable[fc];
    pAcc->BCurSin[fci]=IntCurBPhase*SinTable[fc];
    pAcc->BCurCos[fci]=IntCurBPhase*CosTable[fc];

    pAcc->NeuCurSin[fci]=IntNeuCurrent*SinTable[fc];
    pAcc->NeuCurCos[fci]=IntNeuCurrent*CosTable[fc];

    pAcc->RSolarVolSin[fci]=IntVolRSolarPhase*SinTable[fc];
    pAcc->RSolarVolCos[fci]=IntVolRSolarPhase*CosTable[fc];
    pAcc->YSolarVolSin[fci]=IntVolYSolarPhase*SinTable[fc];
    pAcc->YSolarVolCos[fci]=IntVolYSolarPhase*CosTable[fc];
    pAcc->BSolarVolSin[fci]=IntVolBSolarPhase*SinTable[fc];
    pAcc->BSolarVolCos[fci]=IntVolBSolarPhase*CosTable[fc];

    pAcc->RSolarCurSin[fci]=IntCurRSolarPhase*SinTable[fc];
    pAcc->RSolarCurCos[fci]=IntCurRSolarPhase*CosTable[fc];
    pAcc->YSolarCurSin[fci]=IntCurYSolarPhase*SinTable[fc];
    pAcc->YSolarCurCos[fci]=IntCurYSolarPhase*CosTable[fc];
    pAcc->BSolarCurSin[fci]=IntCurBSolarPhase*SinTable[fc];
    pAcc->BSolarCurCos[fci]=IntCurBSolarPhase*CosTable[fc];

    pAcc->NeuSolarCurSin[fci]=IntNeuSolarCurrent*SinTable[fc];
    pAcc->NeuSolarCurCos[fci]=IntNeuSolarCurrent*CosTable[fc];
  }
  else
  {
    pAcc->RVolSin[fci]+=IntVolRPhase*SinTable[fc];
    pAcc->RVolCos[fci]+=IntVolRPhase*CosTable[fc];
    pAcc->YVolSin[fci]+=IntVolYPhase*SinTable[fc];
    pAcc->YVolCos[fci]+=IntVolYPhase*CosTable[fc];
    pAcc->BVolSin[fci]+=IntVolBPhase*SinTable[fc];
    pAcc->BVolCos[fci]+=IntVolBPhase*CosTable[fc];

    pAcc->RCurSin[fci]+=IntCurRPhase*SinTable[fc];
    pAcc->RCurCos[fci]+=IntCurRPhase*CosTable[fc];
    pAcc->YCurSin[fci]+=IntCurYPhase*SinTable[fc];
    pAcc->YCurCos[fci]+=IntCurYPhase*CosTable[fc];
    pAcc->BCurSin[fci]+=IntCurBPhase*SinTable[fc];
    pAcc->BCurCos[fci]+=IntCurBPhase*CosTable[fc];

    pAcc->NeuCurSin[fci]+=IntNeuCurrent*SinTable[fc];
    pAcc->NeuCurCos[fci]+=IntNeuCurrent*CosTable[fc];

    pAcc->RSolarVolSin[fci]+=IntVolRSolarPhase*SinTable[fc];
    pAcc->RSolarVolCos[fci]+=IntVolRSolarPhase*CosTable[fc];
    pAcc->YSolarVolSin[fci]+=IntVolYSolarPhase*SinTable[fc];
    pAcc->YSolarVolCos[fci]+=IntVolYSolarPhase*CosTable[fc];
    pAcc->BSolarVolSin[fci]+=IntVolBSolarPhase*SinTable[fc];
    pAcc->BSolarVolCos[fci]+=IntVolBSolarPhase*CosTable[fc];

    pAcc->RSolarCurSin[fci]+=IntCurRSolarPhase*SinTable[fc];
    pAcc->RSolarCurCos[fci]+=IntCurRSolarPhase*CosTable[fc];
    pAcc->YSolarCurSin[fci]+=IntCurYSolarPhase*SinTable[fc];
    pAcc->YSolarCurCos[fci]+=IntCurYSolarPhase*CosTable[fc];
    pAcc->BSolarCurSin[fci]+=IntCurBSolarPhase*SinTable[fc];
    pAcc->BSolarCurCos[fci]+=IntCurBSolarPhase*CosTable[fc];

    pAcc->NeuSolarCurSin[fci]+=IntNeuSolarCurrent*SinTable[fc];
    pAcc->NeuSolarCurCos[fci]+=IntNeuSolarCurrent*CosTable[fc];
  }
  }

  
  FftSampleData.FFT_Counter++;
  if(FftSampleData.FFT_Counter==64)
  {
    FftSampleData.FFT_Counter=0;
    FftSampleData.FFT_CounterIndex++;
  }
  
  SampleCounter++;

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
  SampleCounter=0;
  memset(&IntDataSum, 0, sizeof(IntDataSum));

  // Zero both FFT ping-pong banks and reset the hand-off state. (The old
  // loop here zeroed only the Sum arrays and even skipped the neutral ones;
  // a single memset over both banks is cheaper and complete.)
  //
  memset(FftSampleData.bank, 0, sizeof(FftSampleData.bank));
  g_FftAccBank   = 0;
  g_FftSnapBank  = 0;
  g_FftSnapReady = 0;
  FftSampleData.FFT_Counter=0;
  FftSampleData.FFT_CounterIndex=0;
}
