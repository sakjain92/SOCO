/*
******************************************************************************
  * @file    metrology.c
  * @author  MRM R&D Team
  * @version V1.0.0
  * @date    07-06-2022
  * @brief   This file contains all the functions definitions 
             for the calculations
******************************************************************************
*/


#include <stdio.h>
#include "CommFlagDef.h"
#include "Struct.h"
#include <Math.h>
#include <arm_math.h> 
#include "arm_const_structs.h"
#include "extern_includes.h"


void CalculateHarmonicComponents(void);
void CalculateVoCur(void);
uint8_t MinMaxCounter;

void Metrology(void)
{
  
  uint16_t tempc;
  float TempFloat,FloatMin,FloatMax;
  
  for(tempc=0;tempc<6;tempc++)
  {
    *((float *)&VIOffset.VolRPhase+tempc) +=(*((float *)&IntDataSave.OffsetVolRPhase+tempc)/NO_OF_SAMPLES);
  }
  if(IntDataSave.RPhasePower<0)InterruptFlag |=INT_R_PHASE_REV;
  else InterruptFlag &=~INT_R_PHASE_REV;
  if(IntDataSave.YPhasePower<0)InterruptFlag |=INT_Y_PHASE_REV;
  else InterruptFlag &=~INT_Y_PHASE_REV;
  if(IntDataSave.BPhasePower<0)InterruptFlag |=INT_B_PHASE_REV;
  else InterruptFlag &=~INT_B_PHASE_REV;
  CalculateVoCur();
  CalculateHarmonicComponents();
  if(CopySetPara[PARA_SYSTEM_CONFIG]==SYSTEM_CONFIG_1P)
  {
    IntDataSave.VolYPhase=0;
    IntDataSave.VolBPhase=0;
    IntDataSave.CurYPhase=0;
    IntDataSave.CurBPhase=0;
    IntDataSave.CurNeutral=0;
    IntDataSave.YPhasePower=0;
    IntDataSave.BPhasePower=0;
    IntDataSave.VolRYPhPh=0;
    IntDataSave.VolYBPhPh=0;
    IntDataSave.VolBRPhPh=0;
  }
  
  TempFloat=((float)CopySetPara[PARA_STARTING_CURRENT])/1000;
  
  if(InstantPara.CurrentR<MIN_TOTAL_CUR_LIMIT)InstantPara.CurrentR=InstantPara.FunRCurr;
  if(InstantPara.CurrentR<TempFloat)
  {
    InstantPara.CurrentR=0;
    InstantPara.FunRCurr=0;
    InstantPara.HarRPhaseCur=0;
    IntDataSave.RPhasePower=0;
  }
  
  if(InstantPara.CurrentY<MIN_TOTAL_CUR_LIMIT)InstantPara.CurrentY=InstantPara.FunYCurr;
  if(InstantPara.CurrentY<TempFloat)
  {
    InstantPara.CurrentY=0;
    InstantPara.FunYCurr=0;
    InstantPara.HarYPhaseCur=0;
    IntDataSave.YPhasePower=0;
  }
  
 
  if(InstantPara.CurrentB<MIN_TOTAL_CUR_LIMIT)InstantPara.CurrentB=InstantPara.FunBCurr;
  if(InstantPara.CurrentB<TempFloat)
  {
    InstantPara.CurrentB=0;
    InstantPara.FunBCurr=0;
    InstantPara.HarBPhaseCur=0;
    IntDataSave.BPhasePower=0;
  }
  
  if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_3P4W)
  {
    if(InstantPara.CurrentN<MIN_TOTAL_CUR_LIMIT)InstantPara.CurrentN=InstantPara.FunNCurr;
    if(InstantPara.CurrentN<MIN_NEU_CUR_LIMIT)InstantPara.CurrentN=0;
  }
  else if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_3P3W)
  {
    if(InstantPara.CurrentY<MIN_TOTAL_CUR_LIMIT)InstantPara.CurrentY=InstantPara.FunNCurr;
    if(InstantPara.CurrentY<TempFloat)InstantPara.CurrentY=0;                                            
  }
   // Vol
  
  if(InstantPara.VolR<MIN_VOL_LIMIT)
  {
    InstantPara.VolR=0;
    IntDataSave.RPhasePower=0;
    InstantPara.HarRPhaseVol=0;
    InstantPara.FunRVol=0;
  }
  
  if(InstantPara.VolRY<MIN_VOL_LIMIT_PH_PH)InstantPara.VolRY=0;
  
  if(InstantPara.VolY<MIN_VOL_LIMIT)
  {
    InstantPara.VolY=0;
    IntDataSave.YPhasePower=0;
    InstantPara.HarYPhaseVol=0;
    InstantPara.FunYVol=0;
  }
  
  if(InstantPara.VolYB<MIN_VOL_LIMIT_PH_PH)InstantPara.VolYB=0;
  
  if(InstantPara.VolB<MIN_VOL_LIMIT)
  {
    InstantPara.VolB=0;
    IntDataSave.BPhasePower=0;
    InstantPara.HarBPhaseVol=0;
    InstantPara.FunBVol=0;
  }
  
  if(InstantPara.VolBR<MIN_VOL_LIMIT_PH_PH)InstantPara.VolBR=0;
  
  // Min/Max
  /*
  InstantPara.CurrentR=1.234;
  InstantPara.CurrentY=22.34;
  InstantPara.CurrentB=323.4;
  InstantPara.VolR=41.23;
  InstantPara.VolY=51.23;
  InstantPara.VolB=612.3;
  
  InstantPara.VolRY=71.23;
  InstantPara.VolYB=81.23;
  InstantPara.VolBR=912.3;
  
  */
  
  if(PowerUpCounter>3)
  {
    if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_3P3W)
    {
      InstantPara.VrSum +=InstantPara.VolRY;
      InstantPara.VySum +=InstantPara.VolYB;
      InstantPara.VbSum +=InstantPara.VolBR;
    }
    else
    {
      InstantPara.VrSum +=InstantPara.VolR;
      InstantPara.VySum +=InstantPara.VolY;
      InstantPara.VbSum +=InstantPara.VolB;
    }
    InstantPara.IrSum +=InstantPara.CurrentR;
    InstantPara.IySum +=InstantPara.CurrentY;
    InstantPara.IbSum +=InstantPara.CurrentB;
    MinMaxCounter++;
    if(MinMaxCounter==30)
    {
      InstantPara.IrSum=InstantPara.IrSum/30;
      InstantPara.IySum=InstantPara.IySum/30;
      InstantPara.IbSum=InstantPara.IbSum/30;
      
      InstantPara.VrSum=InstantPara.VrSum/30;
      InstantPara.VySum=InstantPara.VySum/30;
      InstantPara.VbSum=InstantPara.VbSum/30;
      if(InstantPara.IrMin==0)InstantPara.IrMin=InstantPara.IrSum;
      else if((InstantPara.IrSum<InstantPara.IrMin)&&(InstantPara.IrMin))InstantPara.IrMin=InstantPara.IrSum;
      if (InstantPara.IrSum>InstantPara.IrMax)InstantPara.IrMax=InstantPara.IrSum;
      
      if(InstantPara.IyMin==0)InstantPara.IyMin=InstantPara.IySum;
      else if((InstantPara.IySum<InstantPara.IyMin)&&(InstantPara.IyMin))InstantPara.IyMin=InstantPara.IySum;
      if (InstantPara.IySum>InstantPara.IyMax)InstantPara.IyMax=InstantPara.IySum;
      
      if(InstantPara.IbMin==0)InstantPara.IbMin=InstantPara.IbSum;
      else if((InstantPara.IbSum<InstantPara.IbMin)&&(InstantPara.IbMin))InstantPara.IbMin=InstantPara.IbSum;
      if (InstantPara.IbSum>InstantPara.IbMax)InstantPara.IbMax=InstantPara.IbSum;
      
      if(InstantPara.VrMin==0)InstantPara.VrMin=InstantPara.VrSum;
      else if((InstantPara.VrSum<InstantPara.VrMin)&&(InstantPara.VrMin))InstantPara.VrMin=InstantPara.VrSum;
      if (InstantPara.VrSum>InstantPara.VrMax)InstantPara.VrMax=InstantPara.VrSum;
      
      if(InstantPara.VyMin==0)InstantPara.VyMin=InstantPara.VySum;
      else if((InstantPara.VySum<InstantPara.VyMin)&&(InstantPara.VyMin))InstantPara.VyMin=InstantPara.VySum;
      if (InstantPara.VySum>InstantPara.VyMax)InstantPara.VyMax=InstantPara.VySum;
      
      if(InstantPara.VbMin==0)InstantPara.VbMin=InstantPara.VbSum;
      else if((InstantPara.VbSum<InstantPara.VbMin)&&(InstantPara.VbMin))InstantPara.VbMin=InstantPara.VbSum;
      if (InstantPara.VbSum>InstantPara.VbMax)InstantPara.VbMax=InstantPara.VbSum;
      MinMaxCounter=0;
      InstantPara.IrSum=0;
      InstantPara.IySum=0;
      InstantPara.IbSum=0;
      
      InstantPara.VrSum=0;
      InstantPara.VySum=0;
      InstantPara.VbSum=0;
    }
  }
  
  
  
  
// Total Power  
  InstantPara.TotalPowerR=POWER_COEFF_3P4W*IntDataSave.RPhasePower;
  if(InstantPara.CurrentR==0)InstantPara.TotalPowerR=0; // Starting 
  if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_3P4W)
  {
      InstantPara.TotalPowerY=POWER_COEFF_3P4W*IntDataSave.YPhasePower;
      if(InstantPara.CurrentY==0)InstantPara.TotalPowerY=0; // Starting 
  }
  else if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_3P3W)
  {
    TempFloat=(POWER_COEFF_3P4W*IntDataSave.YRPower3P3W)+(POWER_COEFF_3P4W*IntDataSave.YBPower3P3W);
    InstantPara.TotalPowerY=-TempFloat;
  }
 
  InstantPara.TotalPowerB=POWER_COEFF_3P4W*IntDataSave.BPhasePower;
  if(InstantPara.CurrentB==0)InstantPara.TotalPowerB=0; // Starting 
#if ((defined MODEL_IMPORT_ONLY)||(defined  MODEL_DUAL_ENERGY))
  
  if(CopySetPara[PARA_SYSTEM_CONFIG]!=SYSTEM_CONFIG_3P3W)
  {
    InstantPara.SumTotalPower=0;
    if(InterruptFlag  & INT_R_PHASE_REV)InstantPara.SumTotalPower -=InstantPara.TotalPowerR;
    else InstantPara.SumTotalPower +=InstantPara.TotalPowerR;
    if(InterruptFlag  & INT_Y_PHASE_REV)InstantPara.SumTotalPower -=InstantPara.TotalPowerY;
    else InstantPara.SumTotalPower +=InstantPara.TotalPowerY;
    if(InterruptFlag  & INT_B_PHASE_REV)InstantPara.SumTotalPower -=InstantPara.TotalPowerB;
    else InstantPara.SumTotalPower +=InstantPara.TotalPowerB;
  }
  else 
    InstantPara.SumTotalPower=(InstantPara.TotalPowerR+InstantPara.TotalPowerY+InstantPara.TotalPowerB);
#else
  InstantPara.SumTotalPower=(InstantPara.TotalPowerR+InstantPara.TotalPowerY+InstantPara.TotalPowerB);
#endif

  
  // VA
  
  if( CopySetPara[PARA_KVA_TYPE]==1) // Airthmatic
  {
    InstantPara.AppPowerR=(InstantPara.VolR*InstantPara.CurrentR);
    InstantPara.AppPowerY=(InstantPara.VolY*InstantPara.CurrentY);
    InstantPara.AppPowerB=(InstantPara.VolB*InstantPara.CurrentB);
  }
  else
  {
     TempFloat=InstantPara.FunRVol*InstantPara.FunRVol*InstantPara.FunRCurr*InstantPara.FunRCurr;
     TempFloat +=(InstantPara.FunRVol*InstantPara.FunRVol*InstantPara.HarRPhaseCur*InstantPara.HarRPhaseCur);
     TempFloat +=(InstantPara.HarRPhaseVol*InstantPara.HarRPhaseVol*InstantPara.FunRCurr*InstantPara.FunRCurr);
     TempFloat +=(InstantPara.HarRPhaseVol*InstantPara.HarRPhaseVol*InstantPara.HarRPhaseCur*InstantPara.HarRPhaseCur);
     InstantPara.AppPowerR=sqrt(TempFloat);
     TempFloat=InstantPara.FunBVol*InstantPara.FunBVol*InstantPara.FunBCurr*InstantPara.FunBCurr;
     TempFloat +=(InstantPara.FunBVol*InstantPara.FunBVol*InstantPara.HarBPhaseCur*InstantPara.HarBPhaseCur);
     TempFloat +=(InstantPara.HarBPhaseVol*InstantPara.HarBPhaseVol*InstantPara.FunBCurr*InstantPara.FunBCurr);
     TempFloat +=(InstantPara.HarBPhaseVol*InstantPara.HarBPhaseVol*InstantPara.HarBPhaseCur*InstantPara.HarBPhaseCur);
     InstantPara.AppPowerB=sqrt(TempFloat);
     TempFloat=InstantPara.FunYVol*InstantPara.FunYVol*InstantPara.FunYCurr*InstantPara.FunYCurr;
     TempFloat +=(InstantPara.FunYVol*InstantPara.FunYVol*InstantPara.HarYPhaseCur*InstantPara.HarYPhaseCur);
     TempFloat +=(InstantPara.HarYPhaseVol*InstantPara.HarYPhaseVol*InstantPara.FunYCurr*InstantPara.FunYCurr);
     TempFloat +=(InstantPara.HarYPhaseVol*InstantPara.HarYPhaseVol*InstantPara.HarYPhaseCur*InstantPara.HarYPhaseCur);
     InstantPara.AppPowerY=sqrt(TempFloat);
     
       
     
  }
  
  if(InstantPara.AppPowerR< InstantPara.TotalPowerR)InstantPara.AppPowerR=InstantPara.TotalPowerR;
  if(InstantPara.AppPowerY< InstantPara.TotalPowerY)InstantPara.AppPowerY=InstantPara.TotalPowerY;
  if(InstantPara.AppPowerB< InstantPara.TotalPowerB)InstantPara.AppPowerB=InstantPara.TotalPowerB;
  
  // Power Factor
  if(InstantPara.AppPowerR>0)
  { 
      InstantPara.PowerFactorR=InstantPara.TotalPowerR/InstantPara.AppPowerR;
      if(InstantPara.PowerFactorR<0)InstantPara.PowerFactorR=-1*InstantPara.PowerFactorR;
      if(InstantPara.PowerFactorR>1.0)InstantPara.PowerFactorR=1.0;
      
  }
  else InstantPara.PowerFactorR=1.0;
  
  if(InstantPara.AppPowerY>0)
  {
     
    InstantPara.PowerFactorY=InstantPara.TotalPowerY/InstantPara.AppPowerY;
    if(InstantPara.PowerFactorY<0)InstantPara.PowerFactorY=-1*InstantPara.PowerFactorY;
    if(InstantPara.PowerFactorY>1.0)InstantPara.PowerFactorY=1.0;
  }
  else InstantPara.PowerFactorY=1.0;
  
  if(InstantPara.AppPowerB>0)
  {
      InstantPara.PowerFactorB=InstantPara.TotalPowerB/InstantPara.AppPowerB;
      if(InstantPara.PowerFactorB<0)InstantPara.PowerFactorB=-1*InstantPara.PowerFactorB;
      if(InstantPara.PowerFactorB>1.0)InstantPara.PowerFactorB=1.0;
  }
  else InstantPara.PowerFactorB=1.0;
  InstantPara.TotalAppPower=InstantPara.AppPowerR+InstantPara.AppPowerY+InstantPara.AppPowerB;
  if(InstantPara.TotalAppPower>0)
  {
    InstantPara.TotalPowerFactor=InstantPara.SumTotalPower/InstantPara.TotalAppPower;
    if(InstantPara.TotalPowerFactor<0)InstantPara.TotalPowerFactor=-1*InstantPara.TotalPowerFactor;
    if(InstantPara.TotalPowerFactor>1.0)InstantPara.TotalPowerFactor=1.0;
  }
  else InstantPara.TotalPowerFactor=1.0;
  
  if(InstantPara.SumTotalPower>=0) TempFloat=InstantPara.SumTotalPower*1e7; //for accuracy its multiplied by 1e7
  else TempFloat=-InstantPara.SumTotalPower*1e7;
  TempFloat=(TempFloat*(float)IntDataSave.TimerCountValue/3750.0);
  WattPerPulseTick=(uint32_t)(TempFloat/3200.0);
  InstantPara.TotalAppPower=InstantPara.AppPowerR+InstantPara.AppPowerY+InstantPara.AppPowerB;

   
  TempFloat=TempFloat/(3600*1e7); // all energies are saved in whr
  FloatMin=InstantPara.TotalReactPower*(float)IntDataSave.TimerCountValue/13500000.0f; //(3750*3600)  Reactive
  FloatMax=InstantPara.TotalAppPower*(float)IntDataSave.TimerCountValue/13500000.0f; //Apparent
  
#ifdef    MODEL_DATA_SAVE  
#ifdef MODEL_IMPORT_ONLY
  if(PowerUpCounter>3)
  {
    StorageBuffer.ImportWh +=(CtPtRatio*(double)TempFloat);
    if(InstantPara.TotalReactPower>0)
      StorageBuffer.ImportVarhPos +=(CtPtRatio*(double)FloatMin);
    else 
      StorageBuffer.ImportVarhNeg -=(CtPtRatio*(double)FloatMin); 
    StorageBuffer.ImportVAh +=(CtPtRatio*(double)FloatMax); 
  }
#endif // MODEL_IMPORT_ONLY
  
  tempc=0;
  
    
  if(StorageBuffer.ImportWh>EnergyOverflowLimit)
  {
    StorageBuffer.ImportWh-=EnergyOverflowLimit;
    tempc=1;
  }
    
  if(StorageBuffer.ExportWh>EnergyOverflowLimit)
  {
    StorageBuffer.ExportWh-=EnergyOverflowLimit;
    tempc=1;
  }
  if(StorageBuffer.ExportVarhPos>EnergyOverflowLimit)
  {
    StorageBuffer.ExportVarhPos-=EnergyOverflowLimit;
    tempc=1;
  }
  if(StorageBuffer.ExportVarhNeg>EnergyOverflowLimit)
  {
    StorageBuffer.ExportVarhNeg-=EnergyOverflowLimit;
    tempc=1;
  }
  
  if(StorageBuffer.ImportVarhPos>EnergyOverflowLimit)
  {
    StorageBuffer.ImportVarhPos-=EnergyOverflowLimit;
    tempc=1;
  }
  if(StorageBuffer.ImportVarhNeg>EnergyOverflowLimit)
  {
    StorageBuffer.ImportVarhNeg-=EnergyOverflowLimit;
    tempc=1;
  }
  
  if(StorageBuffer.ImportVAh>EnergyOverflowLimit)
  {
    StorageBuffer.ImportVAh-=EnergyOverflowLimit;
    tempc=1;
  }
  
  if(StorageBuffer.ExportVAh>EnergyOverflowLimit)
  {
    StorageBuffer.ExportVAh-=EnergyOverflowLimit;
    tempc=1;
  }

  if((StorageBuffer.RunHourImport>=36e8)||(StorageBuffer.RunHourExport>=36e8)||\
    (StorageBuffer.LoadHourImport>=36e8)||(StorageBuffer.LoadHourExport>=36e8))tempc=1;

  
  if(tempc)
  {
    
    __disable_interrupt();
    __no_operation();
    RESET_WATCH_DOG;
    PowerDownDataSave();
    SaveOldData();
    __enable_interrupt();
  }
#endif // MODEL_DATA_SAVE
  if(InstantPara.VolR>10.0f)
  {
    //FreqSampleFlag=4;
    FreqSampleFlag=1;
    //SaveFreqMeasDuration=0;
    //CycleCounter=0;
    //RYFreqMeasDuration=0;
  }
  else if(InstantPara.VolY>10.0f)FreqSampleFlag=2;
  else if(InstantPara.VolB>10.0f)FreqSampleFlag=3;
  else FreqSampleFlag=0;
  
   // Unbalance Current and Voltage
  if(InstantPara.CurrentR>InstantPara.CurrentY)
  {
    FloatMin=InstantPara.CurrentY;
    FloatMax=InstantPara.CurrentR;
  }
  else
  {
    FloatMax=InstantPara.CurrentY;
    FloatMin=InstantPara.CurrentR;
  }
  if(InstantPara.CurrentB>FloatMax)FloatMax=InstantPara.CurrentB;
  else if(InstantPara.CurrentB<FloatMin)FloatMin=InstantPara.CurrentB;
  
  TempFloat=(InstantPara.CurrentR+InstantPara.CurrentY+InstantPara.CurrentB)/3;
  InstantPara.AvgCurr=TempFloat;
  if(TempFloat)
  {
     if((FloatMax-TempFloat)>(TempFloat-FloatMin))
      InstantPara.CurUnb=100*(FloatMax-TempFloat)/TempFloat;
    else InstantPara.CurUnb=100*(TempFloat-FloatMin)/TempFloat;  
  }
  else InstantPara.CurUnb=0;
 
  if(InstantPara.VolR>InstantPara.VolY)
  {
    FloatMin=InstantPara.VolY;
    FloatMax=InstantPara.VolR;
  }
  else
  {
    FloatMax=InstantPara.VolY;
    FloatMin=InstantPara.VolR;
  }
  if(InstantPara.VolB>FloatMax)FloatMax=InstantPara.VolB;
  else if(InstantPara.VolB<FloatMin)FloatMin=InstantPara.VolB;

  TempFloat=(InstantPara.VolR+InstantPara.VolY+InstantPara.VolB)/3;
  if(TempFloat)
  {
    if((FloatMax-TempFloat)>(TempFloat-FloatMin))
      InstantPara.VolUnb=100*(FloatMax-TempFloat)/TempFloat;
    else InstantPara.VolUnb=100*(TempFloat-FloatMin)/TempFloat;    
        
  }
  else InstantPara.VolUnb=0;
  InstantPara.VolLLAvg=(InstantPara.VolRY+InstantPara.VolYB+InstantPara.VolBR)/3;
  InstantPara.VolLNAvg=(InstantPara.VolR+InstantPara.VolY+InstantPara.VolB)/3;
  
  
  InstantPara.CurrentTotal=(InstantPara.CurrentR+InstantPara.CurrentY+InstantPara.CurrentB);
  TempFloat=InstantPara.FunRCurr*InstantPara.FunRVol;
  if(TempFloat>1)
  {
     FloatMax=InstantPara.FunPowerR/TempFloat;
     if((FloatMax>1)||(FloatMax<-1))InstantPara.AngleRPhase=0;
     else
     {
      InstantPara.AngleRPhase=57.2957795f*acos(FloatMax);
     }
  }
  else InstantPara.AngleRPhase=0;
  TempFloat=InstantPara.FunYCurr*InstantPara.FunYVol;
  if(TempFloat>1)
  {
     FloatMax=InstantPara.FunPowerY/TempFloat;
     if((FloatMax>1)||(FloatMax<-1))InstantPara.AngleYPhase=0;
     else 
     {
       InstantPara.AngleYPhase=57.2957795f*acos(FloatMax);
     }
  }
  else InstantPara.AngleYPhase=0;
 
  
  TempFloat=InstantPara.FunBCurr*InstantPara.FunBVol;
  if(TempFloat>1)
  {
     FloatMax=InstantPara.FunPowerB/TempFloat;
     if((FloatMax>1)||(FloatMax<-1))InstantPara.AngleBPhase=0;
     else 
     {
        InstantPara.AngleBPhase=57.2957795f*acos(FloatMax);
     }
  }
  else InstantPara.AngleBPhase=0;
  
  
  if(InstantPara.PowerFactorR>0.9985)InstantPara.ReactPowerR=0;
  if(InstantPara.PowerFactorY>0.9985)InstantPara.ReactPowerY=0;
  if(InstantPara.PowerFactorB>0.9985)InstantPara.ReactPowerB=0;
  /*
  if((InstantPara.TotalReactPower<1)&&(InstantPara.TotalPowerFactor!=1))
  {
    InstantPara.TotalPowerFactor=-InstantPara.TotalPowerFactor;
    if(InstantPara.TotalPowerFactor>1.0)InstantPara.TotalPowerFactor=0;
    if(InstantPara.TotalPowerFactor<-1.0)InstantPara.TotalPowerFactor=0;
  }
  
  
  if(InstantPara.ReactPowerR<0)
    
  {
    if(InstantPara.PowerFactorR !=1)InstantPara.PowerFactorR=-InstantPara.PowerFactorR;
    if(InstantPara.PowerFactorR>1.0)InstantPara.PowerFactorR=0;
    if(InstantPara.PowerFactorR<-1.0)InstantPara.PowerFactorR=0;
    if(InstantPara.AngleRPhase!=0)InstantPara.AngleRPhase=360.0-InstantPara.AngleRPhase;
  }
  if(InstantPara.ReactPowerY<0)
  {
    if(InstantPara.PowerFactorY !=1)InstantPara.PowerFactorY=-InstantPara.PowerFactorY;
    if(InstantPara.PowerFactorY>1.0)InstantPara.PowerFactorY=0;
    if(InstantPara.PowerFactorY<-1.0)InstantPara.PowerFactorY=0;
    if(InstantPara.AngleYPhase!=0)InstantPara.AngleYPhase=360.0-InstantPara.AngleYPhase;
  }
  if(InstantPara.ReactPowerB<0)
  {
    if(InstantPara.PowerFactorB !=1)InstantPara.PowerFactorB=-InstantPara.PowerFactorB;
    if(InstantPara.PowerFactorB>1.0)InstantPara.PowerFactorB=0;
    if(InstantPara.PowerFactorB<-1.0)InstantPara.PowerFactorB=0;
    if(InstantPara.AngleBPhase!=0)InstantPara.AngleBPhase=360.0-InstantPara.AngleBPhase;
  }
  */
} 






void CalculateHarmonicComponents(void)
{
  uint32_t i;
  float TempRVol=0,TempYVol=0,TempBVol=0;
  float TempRCur=0,TempYCur=0,TempBCur=0,TempNCur=0;
  float TempRW=0,TempYW=0,TempBW=0;
  float TempRVr=0,TempYVr=0,TempBVr=0,TempNVr=0,TempNW=0;

  for(i=0;i<50;i++)
  {
      TempRVol +=((FftSampleData.FFT_RVolSinSave[i]*FftSampleData.FFT_RVolSinSave[i])+
                 (FftSampleData.FFT_RVolCosSave[i]*FftSampleData.FFT_RVolCosSave[i]));
      TempYVol +=((FftSampleData.FFT_YVolSinSave[i]*FftSampleData.FFT_YVolSinSave[i])+
                 (FftSampleData.FFT_YVolCosSave[i]*FftSampleData.FFT_YVolCosSave[i]));
      TempBVol +=((FftSampleData.FFT_BVolSinSave[i]*FftSampleData.FFT_BVolSinSave[i])+
                 (FftSampleData.FFT_BVolCosSave[i]*FftSampleData.FFT_BVolCosSave[i]));
      
      TempRCur +=((FftSampleData.FFT_RCurSinSave[i]*FftSampleData.FFT_RCurSinSave[i])+
                 (FftSampleData.FFT_RCurCosSave[i]*FftSampleData.FFT_RCurCosSave[i]));
      TempYCur +=((FftSampleData.FFT_YCurSinSave[i]*FftSampleData.FFT_YCurSinSave[i])+
                 (FftSampleData.FFT_YCurCosSave[i]*FftSampleData.FFT_YCurCosSave[i]));
      TempBCur +=((FftSampleData.FFT_BCurSinSave[i]*FftSampleData.FFT_BCurSinSave[i])+
                 (FftSampleData.FFT_BCurCosSave[i]*FftSampleData.FFT_BCurCosSave[i]));
      TempNCur +=((FftSampleData.FFT_NeuCurSinSave[i]*FftSampleData.FFT_NeuCurSinSave[i])+
                 (FftSampleData.FFT_NeuCurCosSave[i]*FftSampleData.FFT_NeuCurCosSave[i]));
      TempRW+=((FftSampleData.FFT_RVolSinSave[i]*FftSampleData.FFT_RCurSinSave[i])+
                 (FftSampleData.FFT_RVolCosSave[i]*FftSampleData.FFT_RCurCosSave[i]));
      
      TempYW+=((FftSampleData.FFT_YVolSinSave[i]*FftSampleData.FFT_YCurSinSave[i])+
                 (FftSampleData.FFT_YVolCosSave[i]*FftSampleData.FFT_YCurCosSave[i]));
      
      
      TempBW+=((FftSampleData.FFT_BVolSinSave[i]*FftSampleData.FFT_BCurSinSave[i])+
                 (FftSampleData.FFT_BVolCosSave[i]*FftSampleData.FFT_BCurCosSave[i]));
      
      TempNW+=((FftSampleData.FFT_YVolSinSave[i]*FftSampleData.FFT_NeuCurSinSave[i])+
                 (FftSampleData.FFT_YVolCosSave[i]*FftSampleData.FFT_NeuCurCosSave[i]));
      
      TempRVr+=((FftSampleData.FFT_RVolSinSave[i]*FftSampleData.FFT_RCurCosSave[i])-
                 (FftSampleData.FFT_RVolCosSave[i]*FftSampleData.FFT_RCurSinSave[i]));
      TempYVr+=((FftSampleData.FFT_YVolSinSave[i]*FftSampleData.FFT_YCurCosSave[i])-
                 (FftSampleData.FFT_YVolCosSave[i]*FftSampleData.FFT_YCurSinSave[i]));
      TempBVr+=((FftSampleData.FFT_BVolSinSave[i]*FftSampleData.FFT_BCurCosSave[i])-
                 (FftSampleData.FFT_BVolCosSave[i]*FftSampleData.FFT_BCurSinSave[i]));
      TempNVr+=((FftSampleData.FFT_YVolSinSave[i]*FftSampleData.FFT_NeuCurCosSave[i])- // for 3p3w neutral current mult by Y vol
                 (FftSampleData.FFT_YVolCosSave[i]*FftSampleData.FFT_NeuCurSinSave[i]));
  }
  InstantPara.FunRVol=FUND_VOL_COEFF*sqrt(TempRVol/50);
  InstantPara.FunYVol=FUND_VOL_COEFF*sqrt(TempYVol/50);
  InstantPara.FunBVol=FUND_VOL_COEFF*sqrt(TempBVol/50);
  InstantPara.FunRCurr=FUND_CURRENT_COEFF*sqrt(TempRCur/50);
  InstantPara.FunYCurr=FUND_CURRENT_COEFF*sqrt(TempYCur/50);
  InstantPara.FunBCurr=FUND_CURRENT_COEFF*sqrt(TempBCur/50);
  InstantPara.FunNCurr=FUND_CURRENT_COEFF*sqrt(TempNCur/50);
  InstantPara.FunPowerR=FUND_POWER_COEFF*TempRW/50;
  if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_3P3W)
    InstantPara.FunPowerY=-FUND_POWER_COEFF*TempNW/50;
  else InstantPara.FunPowerY=FUND_POWER_COEFF*TempYW/50;
  InstantPara.FunPowerB=FUND_POWER_COEFF*TempBW/50;
  InstantPara.ReactPowerR=-FUND_POWER_COEFF*TempRVr/50;
  InstantPara.ReactPowerB=-FUND_POWER_COEFF*TempBVr/50;
 
  if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_3P3W)
    InstantPara.FunYCurr=InstantPara.FunNCurr;
  
  
  InstantPara.SumFunPower=InstantPara.FunPowerR+InstantPara.FunPowerY+InstantPara.FunPowerB;
                                                
  if((InstantPara.CurrentR==0)||((InstantPara.ReactPowerR<0.01)&&(InstantPara.ReactPowerR>-0.01)))InstantPara.ReactPowerR=0; 
  
  if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_3P4W) 
  {
    InstantPara.ReactPowerY=-FUND_POWER_COEFF*TempYVr/50;
    if((InstantPara.CurrentY==0)||((InstantPara.ReactPowerY<0.01)&&(InstantPara.ReactPowerY>-0.01)))InstantPara.ReactPowerY=0; 
  }
  else if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_3P3W)
  {
    InstantPara.ReactPowerY=FUND_POWER_COEFF*TempNVr/50;
    if((InstantPara.CurrentR==0)&&(InstantPara.CurrentB==0))InstantPara.ReactPowerY=0; 
    if((InstantPara.ReactPowerY<0.01)&&(InstantPara.ReactPowerY>-0.01))InstantPara.ReactPowerY=0; 
  }
  else InstantPara.ReactPowerY=0;                                              
  if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_1P)InstantPara.ReactPowerB=0;
  if((InstantPara.CurrentB==0)||((InstantPara.ReactPowerB<0.01)&&(InstantPara.ReactPowerB>-0.01)))InstantPara.ReactPowerB=0; 
    
  
  if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_3P4W) 
  {
    if(InterruptFlag & INT_R_PHASE_REV)InstantPara.ReactPowerR=-InstantPara.ReactPowerR;
    if(InterruptFlag & INT_Y_PHASE_REV)InstantPara.ReactPowerY=-InstantPara.ReactPowerY;
    if(InterruptFlag & INT_B_PHASE_REV)InstantPara.ReactPowerB=-InstantPara.ReactPowerB;
  }
  InstantPara.TotalReactPower=InstantPara.ReactPowerR+InstantPara.ReactPowerY+InstantPara.ReactPowerB;  
  

  if(InstantPara.VolR>InstantPara.FunRVol)
  {
    InstantPara.HarRPhaseVol=sqrt(InstantPara.VolR*InstantPara.VolR-InstantPara.FunRVol*InstantPara.FunRVol);
  }
  else InstantPara.HarRPhaseVol=0;
  if(InstantPara.VolY>InstantPara.FunYVol)
    InstantPara.HarYPhaseVol=sqrt(InstantPara.VolY*InstantPara.VolY-InstantPara.FunYVol*InstantPara.FunYVol);
  else InstantPara.HarYPhaseVol=0;
  if(InstantPara.VolB>InstantPara.FunBVol)
    InstantPara.HarBPhaseVol=sqrt(InstantPara.VolB*InstantPara.VolB-InstantPara.FunBVol*InstantPara.FunBVol);
  else InstantPara.HarBPhaseVol=0;
  
  if(InstantPara.CurrentR>InstantPara.FunRCurr)
    InstantPara.HarRPhaseCur=sqrt(InstantPara.CurrentR*InstantPara.CurrentR-InstantPara.FunRCurr*InstantPara.FunRCurr);
  else InstantPara.HarRPhaseCur=0;
  if(InstantPara.CurrentY>InstantPara.FunYCurr)
    InstantPara.HarYPhaseCur=sqrt(InstantPara.CurrentY*InstantPara.CurrentY-InstantPara.FunYCurr*InstantPara.FunYCurr);
  else InstantPara.HarYPhaseCur=0;
  if(InstantPara.CurrentB>InstantPara.FunBCurr)
    InstantPara.HarBPhaseCur=sqrt(InstantPara.CurrentB*InstantPara.CurrentB-InstantPara.FunBCurr*InstantPara.FunBCurr);
  else InstantPara.HarBPhaseCur=0;
  
  if(InstantPara.VolR>5)
    InstantPara.ThdRPhaseVol=100*InstantPara.HarRPhaseVol/InstantPara.FunRVol;
  else InstantPara.ThdRPhaseVol=0;
  if(InstantPara.VolY>5)
  InstantPara.ThdYPhaseVol=100*InstantPara.HarYPhaseVol/InstantPara.FunYVol;
  else InstantPara.ThdYPhaseVol=0;
  if(InstantPara.VolB>5)
  InstantPara.ThdBPhaseVol=100*InstantPara.HarBPhaseVol/InstantPara.FunBVol;
  else InstantPara.ThdBPhaseVol=0;
  
  if(InstantPara.CurrentR>0.02)
    InstantPara.ThdRPhaseCur=100*InstantPara.HarRPhaseCur/InstantPara.FunRCurr;
  else InstantPara.ThdRPhaseCur=0;
  if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_3P4W)
  {
    if(InstantPara.CurrentY>0.02)
      InstantPara.ThdYPhaseCur=100*InstantPara.HarYPhaseCur/InstantPara.FunYCurr;
    else InstantPara.ThdYPhaseCur=0;
  }
  else InstantPara.ThdYPhaseCur=0;
  if(InstantPara.CurrentB>0.02)
    InstantPara.ThdBPhaseCur=100*InstantPara.HarBPhaseCur/InstantPara.FunBCurr;
  else InstantPara.ThdBPhaseCur=0;
  
  
  
}

void CalculateVoCur(void)
{

  InstantPara.CurrentR=CURRENT_COEFF*sqrt(IntDataSave.CurRPhase); 
  InstantPara.CurrentY=CURRENT_COEFF*sqrt(IntDataSave.CurYPhase); 
  InstantPara.CurrentB=CURRENT_COEFF*sqrt(IntDataSave.CurBPhase); 
  InstantPara.VolR=VOLTAGE_COEFF*sqrt(IntDataSave.VolRPhase); 
  InstantPara.VolRY=PH_VOLTAGE_COEFF*sqrt(IntDataSave.VolRYPhPh); 
  InstantPara.VolY=VOLTAGE_COEFF*sqrt(IntDataSave.VolYPhase); 
  InstantPara.VolYB=PH_VOLTAGE_COEFF*sqrt(IntDataSave.VolYBPhPh); 
  InstantPara.VolB=VOLTAGE_COEFF*sqrt(IntDataSave.VolBPhase); 
  InstantPara.VolBR=PH_VOLTAGE_COEFF*sqrt(IntDataSave.VolBRPhPh); 
  
  if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_3P4W)
  {
    InstantPara.CurrentN=NEU_CURRENT_COEFF*sqrt(IntDataSave.CurNeutral); 
  }
  else if(CopySetPara[PARA_SYSTEM_CONFIG]  ==SYSTEM_CONFIG_3P3W)
  {
    InstantPara.CurrentY=NEU_CURRENT_COEFF*sqrt(IntDataSave.CurNeutral);                           
    InstantPara.CurrentN=0;
  }
}


