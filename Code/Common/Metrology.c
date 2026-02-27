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
  
  VIOffset.VolRPhase += IntDataSave.OffsetVolRPhase/NO_OF_SAMPLES;
  VIOffset.VolYPhase += IntDataSave.OffsetVolYPhase/NO_OF_SAMPLES;
  VIOffset.VolBPhase += IntDataSave.OffsetVolBPhase/NO_OF_SAMPLES;
  VIOffset.CurRPhase += IntDataSave.OffsetCurRPhase/NO_OF_SAMPLES;
  VIOffset.CurYPhase += IntDataSave.OffsetCurYPhase/NO_OF_SAMPLES;
  VIOffset.CurBPhase += IntDataSave.OffsetCurBPhase/NO_OF_SAMPLES;
  VIOffset.VolRSolarPhase += IntDataSave.OffsetVolRSolarPhase/NO_OF_SAMPLES;
  VIOffset.VolYSolarPhase += IntDataSave.OffsetVolYSolarPhase/NO_OF_SAMPLES;
  VIOffset.VolBSolarPhase += IntDataSave.OffsetVolBSolarPhase/NO_OF_SAMPLES;
  VIOffset.CurRSolarPhase += IntDataSave.OffsetCurRSolarPhase/NO_OF_SAMPLES;
  VIOffset.CurYSolarPhase += IntDataSave.OffsetCurYSolarPhase/NO_OF_SAMPLES;
  VIOffset.CurBSolarPhase += IntDataSave.OffsetCurBSolarPhase/NO_OF_SAMPLES;
  VIOffset.Fan1Current += IntDataSave.OffsetFan1Current/NO_OF_SAMPLES;
  VIOffset.Fan2Current += IntDataSave.OffsetFan2Current/NO_OF_SAMPLES;

  if(IntDataSave.RPhasePower<0)InterruptFlag |=INT_R_PHASE_REV;
  else InterruptFlag &=~INT_R_PHASE_REV;
  if(IntDataSave.YPhasePower<0)InterruptFlag |=INT_Y_PHASE_REV;
  else InterruptFlag &=~INT_Y_PHASE_REV;
  if(IntDataSave.BPhasePower<0)InterruptFlag |=INT_B_PHASE_REV;
  else InterruptFlag &=~INT_B_PHASE_REV;

  if(IntDataSave.RSolarPhasePower<0)InterruptFlag |=INT_R_SOLAR_PHASE_REV;
  else InterruptFlag &=~INT_R_SOLAR_PHASE_REV;
  if(IntDataSave.YSolarPhasePower<0)InterruptFlag |=INT_Y_SOLAR_PHASE_REV;
  else InterruptFlag &=~INT_Y_SOLAR_PHASE_REV;
  if(IntDataSave.BSolarPhasePower<0)InterruptFlag |=INT_B_SOLAR_PHASE_REV;
  else InterruptFlag &=~INT_B_SOLAR_PHASE_REV;

  CalculateVoCur();
  CalculateHarmonicComponents();
  
  TempFloat=MIN_CURRENT_LIMIT;
  
  // UNDONE: If current is below certain threshold,
  // we should use fundamental current and fundamental powers and 
  // from there calculate power factor.
  // Since fundamental power relies on fundamental voltage, should we switch
  // to fundamental voltage also
  // UNDONE: Also, we need to measure frequency of solar & grid and use that for
  // controlling sampling frequency which is needed for fundamental
  // This is important else we are assuming 50Hz frequency constant
  //
  if(InstantPara.CurrentR<MIN_TOTAL_CUR_LIMIT)InstantPara.CurrentR=InstantPara.FunRCurr;
  if(InstantPara.CurrentR<TempFloat)
  {
    InstantPara.CurrentR=0;
    InstantPara.FunRCurr=0;
    InstantPara.HarRPhaseCur=0;
    IntDataSave.RPhasePower=0;
    InstantPara.ReactPowerR=0;
  }
  
  if(InstantPara.CurrentY<MIN_TOTAL_CUR_LIMIT)InstantPara.CurrentY=InstantPara.FunYCurr;
  if(InstantPara.CurrentY<TempFloat)
  {
    InstantPara.CurrentY=0;
    InstantPara.FunYCurr=0;
    InstantPara.HarYPhaseCur=0;
    IntDataSave.YPhasePower=0;
    InstantPara.ReactPowerY=0;
  }
  
 
  if(InstantPara.CurrentB<MIN_TOTAL_CUR_LIMIT)InstantPara.CurrentB=InstantPara.FunBCurr;
  if(InstantPara.CurrentB<TempFloat)
  {
    InstantPara.CurrentB=0;
    InstantPara.FunBCurr=0;
    InstantPara.HarBPhaseCur=0;
    IntDataSave.BPhasePower=0;
    InstantPara.ReactPowerB=0;
  }
  
  if(InstantPara.CurrentN<MIN_TOTAL_CUR_LIMIT)InstantPara.CurrentN=InstantPara.FunNCurr;
  if(InstantPara.CurrentN<MIN_NEU_CUR_LIMIT)InstantPara.CurrentN=0;
  // Vol
  if(InstantPara.VolR<MIN_VOL_LIMIT)
  {
    InstantPara.VolR=0;
    IntDataSave.RPhasePower=0;
    InstantPara.HarRPhaseVol=0;
    InstantPara.FunRVol=0;
    InstantPara.ReactPowerR=0;
  }
  
  if(InstantPara.VolRY<MIN_VOL_LIMIT_PH_PH)InstantPara.VolRY=0;
  
  if(InstantPara.VolY<MIN_VOL_LIMIT)
  {
    InstantPara.VolY=0;
    IntDataSave.YPhasePower=0;
    InstantPara.HarYPhaseVol=0;
    InstantPara.FunYVol=0;
    InstantPara.ReactPowerY=0;
  }
  
  if(InstantPara.VolYB<MIN_VOL_LIMIT_PH_PH)InstantPara.VolYB=0;
  
  if(InstantPara.VolB<MIN_VOL_LIMIT)
  {
    InstantPara.VolB=0;
    IntDataSave.BPhasePower=0;
    InstantPara.HarBPhaseVol=0;
    InstantPara.FunBVol=0;
    InstantPara.ReactPowerB=0;
  }
  
  if(InstantPara.VolBR<MIN_VOL_LIMIT_PH_PH)InstantPara.VolBR=0;

  // Solar
  //
  if(InstantPara.CurrentRSolar<MIN_TOTAL_CUR_LIMIT)InstantPara.CurrentRSolar=InstantPara.FunRSolarCurr;
  if(InstantPara.CurrentRSolar<TempFloat)
  {
    InstantPara.CurrentRSolar=0;
    InstantPara.FunRSolarCurr=0;
    IntDataSave.RSolarPhasePower=0;
    InstantPara.ReactPowerRSolar=0;
  }
  
  if(InstantPara.CurrentYSolar<MIN_TOTAL_CUR_LIMIT)InstantPara.CurrentYSolar=InstantPara.FunYSolarCurr;
  if(InstantPara.CurrentYSolar<TempFloat)
  {
    InstantPara.CurrentYSolar=0;
    InstantPara.FunYSolarCurr=0;
    IntDataSave.YSolarPhasePower=0;
    InstantPara.ReactPowerYSolar=0;
  }
  
 
  if(InstantPara.CurrentBSolar<MIN_TOTAL_CUR_LIMIT)InstantPara.CurrentBSolar=InstantPara.FunBSolarCurr;
  if(InstantPara.CurrentBSolar<TempFloat)
  {
    InstantPara.CurrentBSolar=0;
    InstantPara.FunBSolarCurr=0;
    IntDataSave.BSolarPhasePower=0;
    InstantPara.ReactPowerBSolar=0;
  }
  
  if(InstantPara.CurrentNSolar<MIN_TOTAL_CUR_LIMIT)InstantPara.CurrentNSolar=InstantPara.FunNSolarCurr;
  if(InstantPara.CurrentNSolar<MIN_NEU_CUR_LIMIT)InstantPara.CurrentNSolar=0;
  // Vol
  if(InstantPara.VolRSolar<MIN_VOL_LIMIT)
  {
    InstantPara.VolRSolar=0;
    IntDataSave.RSolarPhasePower=0;
    InstantPara.ReactPowerRSolar=0;
  }
  
  if(InstantPara.VolRYSolar<MIN_VOL_LIMIT_PH_PH)InstantPara.VolRYSolar=0;
  
  if(InstantPara.VolYSolar<MIN_VOL_LIMIT)
  {
    InstantPara.VolYSolar=0;
    IntDataSave.YSolarPhasePower=0;
    InstantPara.ReactPowerYSolar=0;
  }
  
  if(InstantPara.VolYBSolar<MIN_VOL_LIMIT_PH_PH)InstantPara.VolYBSolar=0;
  
  if(InstantPara.VolBSolar<MIN_VOL_LIMIT)
  {
    InstantPara.VolBSolar=0;
    IntDataSave.BSolarPhasePower=0;
    InstantPara.ReactPowerBSolar=0;
  }
  
  if(InstantPara.VolBRSolar<MIN_VOL_LIMIT_PH_PH)InstantPara.VolBRSolar=0;
 
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
    InstantPara.VrSum +=InstantPara.VolR;
    InstantPara.VySum +=InstantPara.VolY;
    InstantPara.VbSum +=InstantPara.VolB;
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
  if(InstantPara.CurrentR<MIN_TOTAL_CUR_LIMIT)InstantPara.TotalPowerR=InstantPara.FunPowerR;
  if(InstantPara.CurrentR==0)InstantPara.TotalPowerR=0; // Starting

  InstantPara.TotalPowerY=POWER_COEFF_3P4W*IntDataSave.YPhasePower;
  if(InstantPara.CurrentY<MIN_TOTAL_CUR_LIMIT)InstantPara.TotalPowerY=InstantPara.FunPowerY;
  if(InstantPara.CurrentY==0)InstantPara.TotalPowerY=0; // Starting 
 
  InstantPara.TotalPowerB=POWER_COEFF_3P4W*IntDataSave.BPhasePower;
  if(InstantPara.CurrentB<MIN_TOTAL_CUR_LIMIT)InstantPara.TotalPowerB=InstantPara.FunPowerB;
  if(InstantPara.CurrentB==0)InstantPara.TotalPowerB=0; // Starting

  InstantPara.SumTotalPower=(InstantPara.TotalPowerR+InstantPara.TotalPowerY+InstantPara.TotalPowerB);

  InstantPara.TotalReactPower=InstantPara.ReactPowerR+InstantPara.ReactPowerY+InstantPara.ReactPowerB; 
  
  // VA
  InstantPara.AppPowerR=(InstantPara.VolR*InstantPara.CurrentR);
  InstantPara.AppPowerY=(InstantPara.VolY*InstantPara.CurrentY);
  InstantPara.AppPowerB=(InstantPara.VolB*InstantPara.CurrentB);
  
  if(InstantPara.AppPowerR< fabs(InstantPara.TotalPowerR))InstantPara.AppPowerR=fabs(InstantPara.TotalPowerR);
  if(InstantPara.AppPowerY< fabs(InstantPara.TotalPowerY))InstantPara.AppPowerY=fabs(InstantPara.TotalPowerY);
  if(InstantPara.AppPowerB< fabs(InstantPara.TotalPowerB))InstantPara.AppPowerB=fabs(InstantPara.TotalPowerB);
  
  // Power Factor
  if(InstantPara.AppPowerR>0)
  { 
      InstantPara.PowerFactorR=InstantPara.TotalPowerR/InstantPara.AppPowerR;
      if(InstantPara.PowerFactorR>1.0)InstantPara.PowerFactorR=1.0;
      
  }
  else InstantPara.PowerFactorR=1.0;
  
  if(InstantPara.AppPowerY>0)
  {
     
    InstantPara.PowerFactorY=InstantPara.TotalPowerY/InstantPara.AppPowerY;
    if(InstantPara.PowerFactorY>1.0)InstantPara.PowerFactorY=1.0;
  }
  else InstantPara.PowerFactorY=1.0;
  
  if(InstantPara.AppPowerB>0)
  {
      InstantPara.PowerFactorB=InstantPara.TotalPowerB/InstantPara.AppPowerB;
      if(InstantPara.PowerFactorB>1.0)InstantPara.PowerFactorB=1.0;
  }
  else InstantPara.PowerFactorB=1.0;
  InstantPara.TotalAppPower=InstantPara.AppPowerR+InstantPara.AppPowerY+InstantPara.AppPowerB;
  if(InstantPara.TotalAppPower>0)
  {
    InstantPara.TotalPowerFactor=InstantPara.SumTotalPower/InstantPara.TotalAppPower;
    if(InstantPara.TotalPowerFactor>1.0)InstantPara.TotalPowerFactor=1.0;
  }
  else InstantPara.TotalPowerFactor=1.0;
  
  if(InstantPara.SumTotalPower>=0) TempFloat=InstantPara.SumTotalPower*1e7; //for accuracy its multiplied by 1e7
  else TempFloat=-InstantPara.SumTotalPower*1e7;
  TempFloat=(TempFloat*(float)IntDataSave.TimerCountValue/3750.0);
  InstantPara.TotalAppPower=InstantPara.AppPowerR+InstantPara.AppPowerY+InstantPara.AppPowerB;

   
  TempFloat=TempFloat/(3600*1e7); // all energies are saved in whr
  FloatMin=InstantPara.TotalReactPower*(float)IntDataSave.TimerCountValue/13500000.0f; //(3750*3600)  Reactive
  FloatMax=InstantPara.TotalAppPower*(float)IntDataSave.TimerCountValue/13500000.0f; //Apparent

#ifdef    MODEL_DATA_SAVE  
  if(PowerUpCounter>3)
  {
    if(InstantPara.SumTotalPower>0)
    {
      StorageBuffer.ImportWh +=(CtPtRatio*(double)TempFloat);
      if(InstantPara.TotalReactPower>=0)
        StorageBuffer.ImportVarhPos +=(CtPtRatio*(double)FloatMin); 
      else 
        StorageBuffer.ImportVarhNeg -=(CtPtRatio*(double)FloatMin); 
      StorageBuffer.ImportVAh +=(CtPtRatio*(double)FloatMax);
    }
    else
    {
      StorageBuffer.ExportWh +=(CtPtRatio*(double)TempFloat);
      if(InstantPara.TotalReactPower>=0)
        StorageBuffer.ExportVarhPos +=(CtPtRatio*(double)FloatMin); 
      else 
        StorageBuffer.ExportVarhNeg -=(CtPtRatio*(double)FloatMin); 
      StorageBuffer.ExportVAh +=(CtPtRatio*(double)FloatMax);
    }
  }
#endif // MODEL_DATA_SAVE

  // Solar
  InstantPara.TotalPowerRSolar=POWER_COEFF_3P4W*IntDataSave.RSolarPhasePower;
  if(InstantPara.CurrentRSolar<MIN_TOTAL_CUR_LIMIT)InstantPara.TotalPowerRSolar=InstantPara.FunPowerSolarR;
  if(InstantPara.CurrentRSolar==0)InstantPara.TotalPowerRSolar=0; // Starting 
  InstantPara.TotalPowerYSolar=POWER_COEFF_3P4W*IntDataSave.YSolarPhasePower;
  if(InstantPara.CurrentYSolar<MIN_TOTAL_CUR_LIMIT)InstantPara.TotalPowerYSolar=InstantPara.FunPowerSolarY;
  if(InstantPara.CurrentYSolar==0)InstantPara.TotalPowerYSolar=0; // Starting 
  InstantPara.TotalPowerBSolar=POWER_COEFF_3P4W*IntDataSave.BSolarPhasePower;
  if(InstantPara.CurrentBSolar<MIN_TOTAL_CUR_LIMIT)InstantPara.TotalPowerBSolar=InstantPara.FunPowerSolarB;
  if(InstantPara.CurrentBSolar==0)InstantPara.TotalPowerBSolar=0; // Starting 
  InstantPara.SumTotalPowerSolar=(InstantPara.TotalPowerRSolar+InstantPara.TotalPowerYSolar+InstantPara.TotalPowerBSolar);

  InstantPara.TotalReactPowerSolar=InstantPara.ReactPowerRSolar+InstantPara.ReactPowerYSolar+InstantPara.ReactPowerBSolar;
  // VA
  InstantPara.AppPowerRSolar=(InstantPara.VolRSolar*InstantPara.CurrentRSolar);
  InstantPara.AppPowerYSolar=(InstantPara.VolYSolar*InstantPara.CurrentYSolar);
  InstantPara.AppPowerBSolar=(InstantPara.VolBSolar*InstantPara.CurrentBSolar);
  
  if(InstantPara.AppPowerRSolar< fabs(InstantPara.TotalPowerRSolar))InstantPara.AppPowerRSolar=fabs(InstantPara.TotalPowerRSolar);
  if(InstantPara.AppPowerYSolar< fabs(InstantPara.TotalPowerYSolar))InstantPara.AppPowerYSolar=fabs(InstantPara.TotalPowerYSolar);
  if(InstantPara.AppPowerBSolar< fabs(InstantPara.TotalPowerBSolar))InstantPara.AppPowerBSolar=fabs(InstantPara.TotalPowerBSolar);
  
  // Power Factor
  if(InstantPara.AppPowerRSolar>0)
  { 
      InstantPara.PowerFactorRSolar=InstantPara.TotalPowerRSolar/InstantPara.AppPowerRSolar;
      if(InstantPara.PowerFactorRSolar>1.0)InstantPara.PowerFactorRSolar=1.0;
      
  }
  else InstantPara.PowerFactorRSolar=1.0;
  
  if(InstantPara.AppPowerYSolar>0)
  {
     
    InstantPara.PowerFactorYSolar=InstantPara.TotalPowerYSolar/InstantPara.AppPowerYSolar;
    if(InstantPara.PowerFactorYSolar>1.0)InstantPara.PowerFactorYSolar=1.0;
  }
  else InstantPara.PowerFactorYSolar=1.0;
  
  if(InstantPara.AppPowerBSolar>0)
  {
      InstantPara.PowerFactorBSolar=InstantPara.TotalPowerBSolar/InstantPara.AppPowerBSolar;
      if(InstantPara.PowerFactorBSolar>1.0)InstantPara.PowerFactorBSolar=1.0;
  }
  else InstantPara.PowerFactorBSolar=1.0;
  InstantPara.TotalAppPowerSolar=InstantPara.AppPowerRSolar+InstantPara.AppPowerYSolar+InstantPara.AppPowerBSolar;
  if(InstantPara.TotalAppPowerSolar>0)
  {
    InstantPara.TotalPowerFactorSolar=InstantPara.SumTotalPowerSolar/InstantPara.TotalAppPowerSolar;
    if(InstantPara.TotalPowerFactorSolar>1.0)InstantPara.TotalPowerFactorSolar=1.0;
  }
  else InstantPara.TotalPowerFactorSolar=1.0;
  
  if(InstantPara.SumTotalPowerSolar>=0) TempFloat=InstantPara.SumTotalPowerSolar*1e7; //for accuracy its multiplied by 1e7
  else TempFloat=-InstantPara.SumTotalPowerSolar*1e7;
  TempFloat=(TempFloat*(float)IntDataSave.TimerCountValue/3750.0);
  InstantPara.TotalAppPowerSolar=InstantPara.AppPowerRSolar+InstantPara.AppPowerYSolar+InstantPara.AppPowerBSolar;

  TempFloat=TempFloat/(3600*1e7); // all energies are saved in whr
  FloatMin=InstantPara.TotalReactPowerSolar*(float)IntDataSave.TimerCountValue/13500000.0f; //(3750*3600)  Reactive
  FloatMax=InstantPara.TotalAppPowerSolar*(float)IntDataSave.TimerCountValue/13500000.0f; //Apparent
  

#ifdef    MODEL_DATA_SAVE
  if(PowerUpCounter>3)
  {
    if(InstantPara.SumTotalPowerSolar>0)
    {
      StorageBuffer.SolarImportWh +=(CtPtRatio*(double)TempFloat);
      if(InstantPara.TotalReactPowerSolar>=0)
        StorageBuffer.SolarImportVarhPos +=(CtPtRatio*(double)FloatMin); 
      else 
        StorageBuffer.SolarImportVarhNeg -=(CtPtRatio*(double)FloatMin); 
      StorageBuffer.SolarImportVAh +=(CtPtRatio*(double)FloatMax);
    }
    else
    {
      StorageBuffer.SolarExportWh +=(CtPtRatio*(double)TempFloat);
      if(InstantPara.TotalReactPowerSolar>=0)
        StorageBuffer.SolarExportVarhPos +=(CtPtRatio*(double)FloatMin); 
      else 
        StorageBuffer.SolarExportVarhNeg -=(CtPtRatio*(double)FloatMin); 
      StorageBuffer.SolarExportVAh +=(CtPtRatio*(double)FloatMax);
    }
  }
#endif // MODEL_DATA_SAVE

#ifdef    MODEL_DATA_SAVE  
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

  // Solar
  if(StorageBuffer.SolarImportWh>EnergyOverflowLimit)
  {
    StorageBuffer.SolarImportWh-=EnergyOverflowLimit;
    tempc=1;
  }
    
  if(StorageBuffer.SolarExportWh>EnergyOverflowLimit)
  {
    StorageBuffer.SolarExportWh-=EnergyOverflowLimit;
    tempc=1;
  }
  if(StorageBuffer.SolarExportVarhPos>EnergyOverflowLimit)
  {
    StorageBuffer.SolarExportVarhPos-=EnergyOverflowLimit;
    tempc=1;
  }
  if(StorageBuffer.SolarExportVarhNeg>EnergyOverflowLimit)
  {
    StorageBuffer.SolarExportVarhNeg-=EnergyOverflowLimit;
    tempc=1;
  }
  
  if(StorageBuffer.SolarImportVarhPos>EnergyOverflowLimit)
  {
    StorageBuffer.SolarImportVarhPos-=EnergyOverflowLimit;
    tempc=1;
  }
  if(StorageBuffer.SolarImportVarhNeg>EnergyOverflowLimit)
  {
    StorageBuffer.SolarImportVarhNeg-=EnergyOverflowLimit;
    tempc=1;
  }
  
  if(StorageBuffer.SolarImportVAh>EnergyOverflowLimit)
  {
    StorageBuffer.SolarImportVAh-=EnergyOverflowLimit;
    tempc=1;
  }
  
  if(StorageBuffer.SolarExportVAh>EnergyOverflowLimit)
  {
    StorageBuffer.SolarExportVAh-=EnergyOverflowLimit;
    tempc=1;
  }

  if((StorageBuffer.SolarRunHourImport>=36e8)||(StorageBuffer.SolarRunHourExport>=36e8)||\
    (StorageBuffer.SolarLoadHourImport>=36e8)||(StorageBuffer.SolarLoadHourExport>=36e8))tempc=1;
  
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
  
  // UNDONE: Is this neccessary?
  //
  if(InstantPara.PowerFactorR>0.9985)InstantPara.ReactPowerR=0;
  if(InstantPara.PowerFactorY>0.9985)InstantPara.ReactPowerY=0;
  if(InstantPara.PowerFactorB>0.9985)InstantPara.ReactPowerB=0;

  if(InstantPara.PowerFactorRSolar>0.9985)InstantPara.ReactPowerRSolar=0;
  if(InstantPara.PowerFactorYSolar>0.9985)InstantPara.ReactPowerYSolar=0;
  if(InstantPara.PowerFactorBSolar>0.9985)InstantPara.ReactPowerBSolar=0;

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
  float TempRSolarCur=0,TempYSolarCur=0,TempBSolarCur=0,TempNSolarCur=0;
  float TempRSolarVr=0,TempYSolarVr=0,TempBSolarVr=0;
  float TempRSolarW=0,TempYSolarW=0,TempBSolarW=0;

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

      TempRSolarCur +=((FftSampleData.FFT_RSolarCurSinSave[i]*FftSampleData.FFT_RSolarCurSinSave[i])+
                 (FftSampleData.FFT_RSolarCurCosSave[i]*FftSampleData.FFT_RSolarCurCosSave[i]));
      TempYSolarCur +=((FftSampleData.FFT_YSolarCurSinSave[i]*FftSampleData.FFT_YSolarCurSinSave[i])+
                 (FftSampleData.FFT_YSolarCurCosSave[i]*FftSampleData.FFT_YSolarCurCosSave[i]));
      TempBSolarCur +=((FftSampleData.FFT_BSolarCurSinSave[i]*FftSampleData.FFT_BSolarCurSinSave[i])+
                 (FftSampleData.FFT_BSolarCurCosSave[i]*FftSampleData.FFT_BSolarCurCosSave[i]));
      TempNSolarCur +=((FftSampleData.FFT_NeuSolarCurSinSave[i]*FftSampleData.FFT_NeuSolarCurSinSave[i])+
                 (FftSampleData.FFT_NeuSolarCurCosSave[i]*FftSampleData.FFT_NeuSolarCurCosSave[i]));

      TempRSolarW+=((FftSampleData.FFT_RSolarVolSinSave[i]*FftSampleData.FFT_RSolarCurSinSave[i])+
                 (FftSampleData.FFT_RSolarVolCosSave[i]*FftSampleData.FFT_RSolarCurCosSave[i]));
      
      TempYSolarW+=((FftSampleData.FFT_YSolarVolSinSave[i]*FftSampleData.FFT_YSolarCurSinSave[i])+
                 (FftSampleData.FFT_YSolarVolCosSave[i]*FftSampleData.FFT_YSolarCurCosSave[i]));
      
      TempBSolarW+=((FftSampleData.FFT_BSolarVolSinSave[i]*FftSampleData.FFT_BSolarCurSinSave[i])+
                 (FftSampleData.FFT_BSolarVolCosSave[i]*FftSampleData.FFT_BSolarCurCosSave[i]));

      TempRSolarVr+=((FftSampleData.FFT_RSolarVolSinSave[i]*FftSampleData.FFT_RSolarCurCosSave[i])-
                 (FftSampleData.FFT_RSolarVolCosSave[i]*FftSampleData.FFT_RSolarCurSinSave[i]));
      TempYSolarVr+=((FftSampleData.FFT_YSolarVolSinSave[i]*FftSampleData.FFT_YSolarCurCosSave[i])-
                 (FftSampleData.FFT_YSolarVolCosSave[i]*FftSampleData.FFT_YSolarCurSinSave[i]));
      TempBSolarVr+=((FftSampleData.FFT_BSolarVolSinSave[i]*FftSampleData.FFT_BSolarCurCosSave[i])-
                 (FftSampleData.FFT_BSolarVolCosSave[i]*FftSampleData.FFT_BSolarCurSinSave[i]));

  }
  InstantPara.FunRVol=FUND_VOL_COEFF*sqrt(TempRVol/50);
  InstantPara.FunYVol=FUND_VOL_COEFF*sqrt(TempYVol/50);
  InstantPara.FunBVol=FUND_VOL_COEFF*sqrt(TempBVol/50);
  InstantPara.FunRCurr=FUND_CURRENT_COEFF*sqrt(TempRCur/50);
  InstantPara.FunYCurr=FUND_CURRENT_COEFF*sqrt(TempYCur/50);
  InstantPara.FunBCurr=FUND_CURRENT_COEFF*sqrt(TempBCur/50);
  InstantPara.FunNCurr=FUND_CURRENT_COEFF*sqrt(TempNCur/50);
  InstantPara.FunPowerR=FUND_POWER_COEFF*TempRW/50;
  InstantPara.FunPowerY=FUND_POWER_COEFF*TempYW/50;
  InstantPara.FunPowerB=FUND_POWER_COEFF*TempBW/50;
  InstantPara.ReactPowerR=-FUND_POWER_COEFF*TempRVr/50;
  InstantPara.ReactPowerY=-FUND_POWER_COEFF*TempYVr/50;
  InstantPara.ReactPowerB=-FUND_POWER_COEFF*TempBVr/50;

  InstantPara.SumFunPower=InstantPara.FunPowerR+InstantPara.FunPowerY+InstantPara.FunPowerB;
                                                
  if((InstantPara.CurrentR==0)||((InstantPara.ReactPowerR<0.01)&&(InstantPara.ReactPowerR>-0.01)))InstantPara.ReactPowerR=0; 
  if((InstantPara.CurrentY==0)||((InstantPara.ReactPowerY<0.01)&&(InstantPara.ReactPowerY>-0.01)))InstantPara.ReactPowerY=0; 
  if((InstantPara.CurrentB==0)||((InstantPara.ReactPowerB<0.01)&&(InstantPara.ReactPowerB>-0.01)))InstantPara.ReactPowerB=0; 
    
  InstantPara.TotalReactPower=InstantPara.ReactPowerR+InstantPara.ReactPowerY+InstantPara.ReactPowerB;  

  InstantPara.FunRSolarCurr=FUND_CURRENT_COEFF*sqrt(TempRSolarCur/50);
  InstantPara.FunYSolarCurr=FUND_CURRENT_COEFF*sqrt(TempYSolarCur/50);
  InstantPara.FunBSolarCurr=FUND_CURRENT_COEFF*sqrt(TempBSolarCur/50);
  InstantPara.FunNSolarCurr=FUND_CURRENT_COEFF*sqrt(TempNSolarCur/50);
  InstantPara.FunPowerSolarR=FUND_POWER_COEFF*TempRSolarW/50;
  InstantPara.FunPowerSolarY=FUND_POWER_COEFF*TempYSolarW/50;
  InstantPara.FunPowerSolarB=FUND_POWER_COEFF*TempBSolarW/50;
  InstantPara.ReactPowerRSolar=-FUND_POWER_COEFF*TempRSolarVr/50;
  InstantPara.ReactPowerYSolar=-FUND_POWER_COEFF*TempYSolarVr/50;
  InstantPara.ReactPowerBSolar=-FUND_POWER_COEFF*TempBSolarVr/50;

  // UNDONE: Check this limit for Reactive power. Should we increase it?
  if((InstantPara.CurrentRSolar==0)||((InstantPara.ReactPowerRSolar<0.01)&&(InstantPara.ReactPowerRSolar>-0.01)))InstantPara.ReactPowerRSolar=0; 
  if((InstantPara.CurrentYSolar==0)||((InstantPara.ReactPowerYSolar<0.01)&&(InstantPara.ReactPowerYSolar>-0.01)))InstantPara.ReactPowerYSolar=0; 
  if((InstantPara.CurrentBSolar==0)||((InstantPara.ReactPowerBSolar<0.01)&&(InstantPara.ReactPowerBSolar>-0.01)))InstantPara.ReactPowerBSolar=0; 
    
  InstantPara.TotalReactPowerSolar=InstantPara.ReactPowerRSolar+InstantPara.ReactPowerYSolar+InstantPara.ReactPowerBSolar;  



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
  if(InstantPara.CurrentY>0.02)
    InstantPara.ThdYPhaseCur=100*InstantPara.HarYPhaseCur/InstantPara.FunYCurr;
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
  InstantPara.CurrentN=NEU_CURRENT_COEFF*sqrt(IntDataSave.CurNeutral);

  InstantPara.CurrentRSolar=CURRENT_COEFF*sqrt(IntDataSave.CurRSolarPhase); 
  InstantPara.CurrentYSolar=CURRENT_COEFF*sqrt(IntDataSave.CurYSolarPhase); 
  InstantPara.CurrentBSolar=CURRENT_COEFF*sqrt(IntDataSave.CurBSolarPhase); 
  InstantPara.VolRSolar=VOLTAGE_COEFF*sqrt(IntDataSave.VolRSolarPhase); 
  InstantPara.VolRYSolar=PH_VOLTAGE_COEFF*sqrt(IntDataSave.VolRYSolarPhPh); 
  InstantPara.VolYSolar=VOLTAGE_COEFF*sqrt(IntDataSave.VolYSolarPhase); 
  InstantPara.VolYBSolar=PH_VOLTAGE_COEFF*sqrt(IntDataSave.VolYBSolarPhPh); 
  InstantPara.VolBSolar=VOLTAGE_COEFF*sqrt(IntDataSave.VolBSolarPhase); 
  InstantPara.VolBRSolar=PH_VOLTAGE_COEFF*sqrt(IntDataSave.VolBRSolarPhPh);
  InstantPara.CurrentNSolar=NEU_CURRENT_COEFF*sqrt(IntDataSave.CurNeutralSolar);

  InstantPara.Fan1Current=FAN_CURRENT_COEFF*sqrt(IntDataSave.Fan1Current);
  InstantPara.Fan2Current=FAN_CURRENT_COEFF*sqrt(IntDataSave.Fan2Current);
}
