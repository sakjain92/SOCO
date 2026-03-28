/*
******************************************************************************
  * @file    comm.c
  * @author  MRM R&D Team
  * @version V1.0.0
  * @date    07-04-2022
  * @brief   This file contains all the functions definitions 
             for the meter communication
******************************************************************************
*/

#include "Comm.h"

// Table containing start address on modbus table & number of entries in the
// modbus table
// Note: The address should be monotonically increasing with any missing
// entries
//
struct ModbusTableSection_t
{
    uint16_t address;
    uint16_t numEntries;
};

struct ModbusTableSection_t ModbusTableSections[] =
{
    {
        1000, InstPara_NewAll
    },
    {
        1200, InstPara_GridFreq
    },
    {
        2000, InstPara_THD
    },
    {
        2100, InstPara_ENERGYIMPORT
    },
    {
        2200, InstPara_ENERGYEXPORT
    },
    {
        2300, InstPara_ENERGYOLD
    },
    {
        2400, InstPara_ENERGYTrip
    },
    {
        2500, InstPara_TotalRMS
    },
    {
        2600, InstPara_RphaseRMS
    },
    {
        2700, InstPara_YphaseRMS
    },
    {
        2800, InstPara_BphaseRMS
    },
    {
        3000, InstPara_Demand
    },
    {
        5000, MAX_PARAM_LIMIT
    },
    {
        6000, InstPara_LoadOnGridDisableSec,
    },
    {
        11000, InstPara_NewSolarAll
    },
    {
        11200, InstPara_SolarFreq
    },
    {
        12100, InstPara_Solar_ENERGYIMPORT
    },
    {
        12200, InstPara_Solar_ENERGYEXPORT
    },
    {
        50000, InstPara_InternalTesting
    }
};

/*
Inf: Production release 
Inp: None
Ret: None
*/
void ProtectionReset(void)
{
#ifdef MODEL_RELEASED   
  Delay1Msec12Mhz(100);
  if(PROTECTION_BIT_LOW)NVIC_SystemReset();
#endif
} 


/*
Inf: Save Flash Data 
Inp: None
Ret: None
*/
void SaveFlashData(void)
{
  uint16_t i;
  for (i=0;i<sizeof(CalibrationCoeff);i++)*((uint8_t *)&CalBuffer+i)=*((uint8_t *)&CalibrationCoeff+i);
}

/*
Inf: Write Flash Data 
Inp: None
Ret: None
*/
void WriteFlashData(void)
{
  
  uint16_t i,k;
  FLASH_Unlock();
  FLASH_ErasePage((uint32_t )&CalibrationCoeff);
  k=sizeof(CalibrationCoeff);
  k=k/4;
  for (i=0;i<k;i++)
  {
    FLASH_ProgramWord((uint32_t )&CalibrationCoeff+i*4,*((uint32_t *)&CalBuffer +i));
  }
  FLASH_Lock();
    
}

/*
Inf: Calibration Process 
Inp: None
Ret: None
*/
void DirectCalibration(void)
{
 float TempFloat;

 if(FlagDirectCalibration==CALIBRATE_L_PF)
 {
   // UNDONE: Should the guard rails be on average or instant parameters
   //
   if(
      (InstantPara.CurrentR >I_L_LOWER_LIMIT) && (InstantPara.CurrentR<I_L_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_L_LOWER_LIMIT)&& (InstantPara.CurrentY<I_L_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_L_LOWER_LIMIT) && (InstantPara.CurrentB<I_L_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_L_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_L_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_L_LOWER_LIMIT)&&

      (InstantPara.CurrentRSolar >I_L_LOWER_LIMIT) && (InstantPara.CurrentRSolar<I_L_UPPER_LIMIT)&&
      (InstantPara.CurrentYSolar >I_L_LOWER_LIMIT)&& (InstantPara.CurrentYSolar<I_L_UPPER_LIMIT)&&
      (InstantPara.CurrentBSolar >I_L_LOWER_LIMIT) && (InstantPara.CurrentBSolar<I_L_UPPER_LIMIT)&&
      (InstantPara.TotalPowerRSolar<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerRSolar>PF_POWER_L_LOWER_LIMIT)&&
      (InstantPara.TotalPowerYSolar<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerYSolar>PF_POWER_L_LOWER_LIMIT)&&
      (InstantPara.TotalPowerBSolar<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerBSolar>PF_POWER_L_LOWER_LIMIT)
    )
   {

      ProtectionReset();              
      __disable_interrupt();
      __no_operation();
      RESET_WATCH_DOG;
      SaveFlashData();  

      TempFloat=(CalPowR-CAL_PF_POWER_SETTING_LOW)/CAL_PF_POWER_SETTING_LOW;
      CalBuffer.IR_LOW_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowY-CAL_PF_POWER_SETTING_LOW)/CAL_PF_POWER_SETTING_LOW;
      CalBuffer.IY_LOW_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowB-CAL_PF_POWER_SETTING_LOW)/CAL_PF_POWER_SETTING_LOW;
      CalBuffer.IB_LOW_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowRSolar-CAL_PF_POWER_SETTING_LOW)/CAL_PF_POWER_SETTING_LOW;
      CalBuffer.IR_SOLAR_LOW_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowYSolar-CAL_PF_POWER_SETTING_LOW)/CAL_PF_POWER_SETTING_LOW;
      CalBuffer.IY_SOLAR_LOW_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowBSolar-CAL_PF_POWER_SETTING_LOW)/CAL_PF_POWER_SETTING_LOW;
      CalBuffer.IB_SOLAR_LOW_PH_ERROR=TempFloat-0.001;

      if (CalBuffer.IR_LOW_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IR_LOW_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IY_LOW_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IY_LOW_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IB_LOW_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IB_LOW_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IR_SOLAR_LOW_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IR_SOLAR_LOW_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IY_SOLAR_LOW_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IY_SOLAR_LOW_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IB_SOLAR_LOW_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IB_SOLAR_LOW_PH_ERROR<PH_ERROR_D_MAX)
      {
          DisplayImproperSettings();
      }
      
   }
   else 
   {
       DisplayImproperSettings();
   } 
 } 
  
 if(FlagDirectCalibration==CALIBRATE_L_VI)
 {
   if(
      (InstantPara.CurrentR >I_L_LOWER_LIMIT) && (InstantPara.CurrentR<I_L_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_L_LOWER_LIMIT)&& (InstantPara.CurrentY<I_L_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_L_LOWER_LIMIT) && (InstantPara.CurrentB<I_L_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_L_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_L_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_L_LOWER_LIMIT)&&

      (InstantPara.CurrentRSolar >I_L_LOWER_LIMIT) && (InstantPara.CurrentRSolar<I_L_UPPER_LIMIT)&&
      (InstantPara.CurrentYSolar >I_L_LOWER_LIMIT)&& (InstantPara.CurrentYSolar<I_L_UPPER_LIMIT)&&
      (InstantPara.CurrentBSolar >I_L_LOWER_LIMIT) && (InstantPara.CurrentBSolar<I_L_UPPER_LIMIT)&&
      (InstantPara.TotalPowerRSolar<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerRSolar>PF_POWER_L_LOWER_LIMIT)&&
      (InstantPara.TotalPowerYSolar<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerYSolar>PF_POWER_L_LOWER_LIMIT)&&
      (InstantPara.TotalPowerBSolar<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerBSolar>PF_POWER_L_LOWER_LIMIT)
    )
   {

      ProtectionReset();              
      __disable_interrupt();
      __no_operation();
      RESET_WATCH_DOG;
      SaveFlashData();  
            
      CalBuffer.IR_LOW_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_LOW)/CalCurR);
      CalBuffer.IY_LOW_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_LOW)/CalCurY);
      CalBuffer.IB_LOW_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_LOW)/CalCurB);

      CalBuffer.IR_SOLAR_LOW_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_LOW)/CalCurRSolar);
      CalBuffer.IY_SOLAR_LOW_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_LOW)/CalCurYSolar);
      CalBuffer.IB_SOLAR_LOW_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_LOW)/CalCurBSolar);
      
   }
   else
   {
       DisplayImproperSettings();
   } 
 } 
  
 else if(FlagDirectCalibration==CALIBRATE_M_PF)
 {
   if(
      (InstantPara.CurrentR >I_M_LOWER_LIMIT) && (InstantPara.CurrentR<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_M_LOWER_LIMIT)&& (InstantPara.CurrentY<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_M_LOWER_LIMIT) && (InstantPara.CurrentB<I_M_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_M_LOWER_LIMIT)&&

      (InstantPara.CurrentRSolar >I_M_LOWER_LIMIT) && (InstantPara.CurrentRSolar<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentYSolar >I_M_LOWER_LIMIT)&& (InstantPara.CurrentYSolar<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentBSolar >I_M_LOWER_LIMIT) && (InstantPara.CurrentBSolar<I_M_UPPER_LIMIT)&&
      (InstantPara.TotalPowerRSolar<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerRSolar>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerYSolar<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerYSolar>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerBSolar<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerBSolar>PF_POWER_M_LOWER_LIMIT)
    )  
   {

      ProtectionReset();              
      __disable_interrupt();
      __no_operation();
      RESET_WATCH_DOG;
      SaveFlashData();  

      TempFloat=(CalPowR-CAL_PF_POWER_SETTING_MID)/CAL_PF_POWER_SETTING_MID;
      CalBuffer.IR_MID_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowY-CAL_PF_POWER_SETTING_MID)/CAL_PF_POWER_SETTING_MID;
      CalBuffer.IY_MID_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowB-CAL_PF_POWER_SETTING_MID)/CAL_PF_POWER_SETTING_MID;
      CalBuffer.IB_MID_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowRSolar-CAL_PF_POWER_SETTING_MID)/CAL_PF_POWER_SETTING_MID;
      CalBuffer.IR_SOLAR_MID_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowYSolar-CAL_PF_POWER_SETTING_MID)/CAL_PF_POWER_SETTING_MID;
      CalBuffer.IY_SOLAR_MID_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowBSolar-CAL_PF_POWER_SETTING_MID)/CAL_PF_POWER_SETTING_MID;
      CalBuffer.IB_SOLAR_MID_PH_ERROR=TempFloat-0.001;

      if (CalBuffer.IR_MID_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IR_MID_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IY_MID_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IY_MID_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IB_MID_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IB_MID_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IR_SOLAR_MID_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IR_SOLAR_MID_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IY_SOLAR_MID_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IY_SOLAR_MID_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IB_SOLAR_MID_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IB_SOLAR_MID_PH_ERROR<PH_ERROR_D_MAX)
      {
          DisplayImproperSettings();
      }
   }
   else
   {
       DisplayImproperSettings();
   } 
 } 
  
 else if(FlagDirectCalibration==CALIBRATE_M_VI)
 {
   if(
      (InstantPara.CurrentR >I_M_LOWER_LIMIT) && (InstantPara.CurrentR<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_M_LOWER_LIMIT)&& (InstantPara.CurrentY<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_M_LOWER_LIMIT) && (InstantPara.CurrentB<I_M_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_M_LOWER_LIMIT)&&

      (InstantPara.CurrentRSolar >I_M_LOWER_LIMIT) && (InstantPara.CurrentRSolar<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentYSolar >I_M_LOWER_LIMIT)&& (InstantPara.CurrentYSolar<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentBSolar >I_M_LOWER_LIMIT) && (InstantPara.CurrentBSolar<I_M_UPPER_LIMIT)&&
      (InstantPara.TotalPowerRSolar<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerRSolar>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerYSolar<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerYSolar>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerBSolar<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerBSolar>PF_POWER_M_LOWER_LIMIT)
    )  
   {

      ProtectionReset();              
      __disable_interrupt();
      __no_operation();
      RESET_WATCH_DOG;
      SaveFlashData();  
            
      CalBuffer.IR_MID_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_MID)/CalCurR);
      CalBuffer.IY_MID_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_MID)/CalCurY);
      CalBuffer.IB_MID_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_MID)/CalCurB);

      CalBuffer.IR_SOLAR_MID_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_MID)/CalCurRSolar);
      CalBuffer.IY_SOLAR_MID_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_MID)/CalCurYSolar);
      CalBuffer.IB_SOLAR_MID_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_MID)/CalCurBSolar);
   }
   else
   {
       DisplayImproperSettings();
   } 
 } 
   
 else if(FlagDirectCalibration==CALIBRATE_H_PF)
 {
   if(
     (InstantPara.VolR >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolR<VOLTAGE_HIGHER_LIMIT) &&  
      (InstantPara.VolY >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolY<VOLTAGE_HIGHER_LIMIT)&& 
      (InstantPara.VolB >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolB<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.CurrentR >I_H_LOWER_LIMIT) && (InstantPara.CurrentR<I_H_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_H_LOWER_LIMIT)&& (InstantPara.CurrentY<I_H_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_H_LOWER_LIMIT) && (InstantPara.CurrentB<I_H_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_H_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_H_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_H_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_H_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_H_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_H_LOWER_LIMIT)&&

      (InstantPara.VolRSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolRSolar<VOLTAGE_HIGHER_LIMIT) &&  
      (InstantPara.VolYSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolYSolar<VOLTAGE_HIGHER_LIMIT)&& 
      (InstantPara.VolBSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolBSolar<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.CurrentRSolar >I_H_LOWER_LIMIT) && (InstantPara.CurrentRSolar<I_H_UPPER_LIMIT)&&
      (InstantPara.CurrentYSolar >I_H_LOWER_LIMIT)&& (InstantPara.CurrentYSolar<I_H_UPPER_LIMIT)&&
      (InstantPara.CurrentBSolar >I_H_LOWER_LIMIT) && (InstantPara.CurrentBSolar<I_H_UPPER_LIMIT)&&
      (InstantPara.TotalPowerRSolar<PF_POWER_H_UPPER_LIMIT)&&(InstantPara.TotalPowerRSolar>PF_POWER_H_LOWER_LIMIT)&&
      (InstantPara.TotalPowerYSolar<PF_POWER_H_UPPER_LIMIT)&&(InstantPara.TotalPowerYSolar>PF_POWER_H_LOWER_LIMIT)&&
      (InstantPara.TotalPowerBSolar<PF_POWER_H_UPPER_LIMIT)&&(InstantPara.TotalPowerBSolar>PF_POWER_H_LOWER_LIMIT)
    )  
   {

      ProtectionReset();              
      __disable_interrupt();
      __no_operation();
      RESET_WATCH_DOG;
      SaveFlashData();  
      
      TempFloat=(CalPowR-CAL_PF_POWER_SETTING_HIGH)/CAL_PF_POWER_SETTING_HIGH;
      CalBuffer.IR_HIGH_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowY-CAL_PF_POWER_SETTING_HIGH)/CAL_PF_POWER_SETTING_HIGH;
      CalBuffer.IY_HIGH_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowB-CAL_PF_POWER_SETTING_HIGH)/CAL_PF_POWER_SETTING_HIGH;
      CalBuffer.IB_HIGH_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowRSolar-CAL_PF_POWER_SETTING_HIGH)/CAL_PF_POWER_SETTING_HIGH;
      CalBuffer.IR_SOLAR_HIGH_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowYSolar-CAL_PF_POWER_SETTING_HIGH)/CAL_PF_POWER_SETTING_HIGH;
      CalBuffer.IY_SOLAR_HIGH_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowBSolar-CAL_PF_POWER_SETTING_HIGH)/CAL_PF_POWER_SETTING_HIGH;
      CalBuffer.IB_SOLAR_HIGH_PH_ERROR=TempFloat-0.001;

      if (CalBuffer.IR_HIGH_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IR_HIGH_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IY_HIGH_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IY_HIGH_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IB_HIGH_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IB_HIGH_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IR_SOLAR_HIGH_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IR_SOLAR_HIGH_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IY_SOLAR_HIGH_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IY_SOLAR_HIGH_PH_ERROR<PH_ERROR_D_MAX ||
          CalBuffer.IB_SOLAR_HIGH_PH_ERROR>PH_ERROR_D_ZERO ||
          CalBuffer.IB_SOLAR_HIGH_PH_ERROR<PH_ERROR_D_MAX)
      {
          DisplayImproperSettings();
      }
   }
   else
   {
       DisplayImproperSettings();
   } 
 }
 
  else if(FlagDirectCalibration==CALIBRATE_H_VI)
  {
    if(
      (InstantPara.VolR >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolR<VOLTAGE_HIGHER_LIMIT) &&  
      (InstantPara.VolY >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolY<VOLTAGE_HIGHER_LIMIT)&& 
      (InstantPara.VolB >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolB<VOLTAGE_HIGHER_LIMIT) && 
      (InstantPara.CurrentR >I_H_LOWER_LIMIT) && (InstantPara.CurrentR<I_H_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_H_LOWER_LIMIT)&& (InstantPara.CurrentY<I_H_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_H_LOWER_LIMIT) && (InstantPara.CurrentB<I_H_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_H_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_H_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_H_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_H_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_H_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_H_LOWER_LIMIT) &&

      (InstantPara.VolRSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolRSolar<VOLTAGE_HIGHER_LIMIT) &&  
      (InstantPara.VolYSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolYSolar<VOLTAGE_HIGHER_LIMIT)&& 
      (InstantPara.VolBSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolBSolar<VOLTAGE_HIGHER_LIMIT) && 
      (InstantPara.CurrentRSolar >I_H_LOWER_LIMIT) && (InstantPara.CurrentRSolar<I_H_UPPER_LIMIT)&&
      (InstantPara.CurrentYSolar >I_H_LOWER_LIMIT)&& (InstantPara.CurrentYSolar<I_H_UPPER_LIMIT)&&
      (InstantPara.CurrentBSolar >I_H_LOWER_LIMIT) && (InstantPara.CurrentBSolar<I_H_UPPER_LIMIT)&&
      (InstantPara.TotalPowerRSolar<PF_POWER_H_UPPER_LIMIT)&&(InstantPara.TotalPowerRSolar>PF_POWER_H_LOWER_LIMIT)&&
      (InstantPara.TotalPowerYSolar<PF_POWER_H_UPPER_LIMIT)&&(InstantPara.TotalPowerYSolar>PF_POWER_H_LOWER_LIMIT)&&
      (InstantPara.TotalPowerBSolar<PF_POWER_H_UPPER_LIMIT)&&(InstantPara.TotalPowerBSolar>PF_POWER_H_LOWER_LIMIT)&&

      (InstantPara.Fan1Current<FAN_CUR_HIGHER_LIMIT)&&(InstantPara.Fan1Current>FAN_CUR_LOWER_LIMIT)&&
      (InstantPara.Fan2Current<FAN_CUR_HIGHER_LIMIT)&&(InstantPara.Fan2Current>FAN_CUR_LOWER_LIMIT)
    )  

   {

      ProtectionReset();              
      __disable_interrupt();
      __no_operation();
      RESET_WATCH_DOG;
      SaveFlashData();  
      CalBuffer.VR_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH)/CalVolR);
      CalBuffer.VY_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH)/CalVolY);
      CalBuffer.VB_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH)/CalVolB);
      CalBuffer.IR_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH)/CalCurR);
      CalBuffer.IY_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH)/CalCurY);
      CalBuffer.IB_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH)/CalCurB);

      CalBuffer.VR_SOLAR_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH)/CalVolRSolar);
      CalBuffer.VY_SOLAR_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH)/CalVolYSolar);
      CalBuffer.VB_SOLAR_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH)/CalVolBSolar);
      CalBuffer.IR_SOLAR_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH)/CalCurRSolar);
      CalBuffer.IY_SOLAR_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH)/CalCurYSolar);
      CalBuffer.IB_SOLAR_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH)/CalCurBSolar);

      CalBuffer.FAN1_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_FAN_CUR_SETTING)/CalFan1Current);
      CalBuffer.FAN2_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_FAN_CUR_SETTING)/CalFan2Current);
   }
   else
   {
       DisplayImproperSettings();
   } 
  }
  

  RESET_WATCH_DOG;
  WriteFlashData();
  RESET_WATCH_DOG;
  ClearInterruptVariables();
  __enable_interrupt();
}

/*
Inf: Set Calibrated Values 
Inp: None
Ret: None
*/
void AccumulateDataForCalibration(void)
{
   CalVolR+=InstantPara.VolR;
   CalVolY+=InstantPara.VolY;
   CalVolB+=InstantPara.VolB;
   CalCurR+=InstantPara.CurrentR;
   CalCurY+=InstantPara.CurrentY;
   CalCurB+=InstantPara.CurrentB;
   CalPowR+=InstantPara.TotalPowerR;
   CalPowY+=InstantPara.TotalPowerY;
   CalPowB+=InstantPara.TotalPowerB;

   CalVolRSolar+=InstantPara.VolRSolar;
   CalVolYSolar+=InstantPara.VolYSolar;
   CalVolBSolar+=InstantPara.VolBSolar;
   CalCurRSolar+=InstantPara.CurrentRSolar;
   CalCurYSolar+=InstantPara.CurrentYSolar;
   CalCurBSolar+=InstantPara.CurrentBSolar;
   CalPowRSolar+=InstantPara.TotalPowerRSolar;
   CalPowYSolar+=InstantPara.TotalPowerYSolar;
   CalPowBSolar+=InstantPara.TotalPowerBSolar;

   CalFan1Current+=InstantPara.Fan1Current;
   CalFan2Current+=InstantPara.Fan2Current;
 }

/*
Inf: Clear Calibrated values 
Inp: None
Ret: None
*/
void ClearCalAccumulatedData(void)
{
   CalVolR=0;
   CalVolY=0;
   CalVolB=0;
   CalCurR=0;
   CalCurY=0;
   CalCurB=0;
   CalPowR=0;
   CalPowY=0;
   CalPowB=0;

   CalVolRSolar=0;
   CalVolYSolar=0;
   CalVolBSolar=0;
   CalCurRSolar=0;
   CalCurYSolar=0;
   CalCurBSolar=0;
   CalPowRSolar=0;
   CalPowYSolar=0;
   CalPowBSolar=0;

   CalFan1Current=0;
   CalFan2Current=0;
}

/*
Inf: Calibrate Power Factor 
Inp: Error, gain pointer, value pointer
Ret: None
*/  
float W,PhasenRadian,D,B,A;
void CalPF(float Error, float * CalGainBufferPointer, float * CalBetaBufferPointer, uint8_t * CalIntDelayPointer)
{
    // Phase correction: converts PH_ERROR to IIR filter coefficients A, B and
    // an integer sample delay.
    //
    // D = total fractional-sample delay needed.  For D >= 1, the integer part
    // is stored in *CalIntDelayPointer and only the fractional part (< 1) is
    // fed into the IIR formula, keeping the discriminant positive for all
    // valid inputs.
    //
    // Valid input range: PH_ERROR_D_MAX <= Error <= PH_ERROR_D_ZERO
    // (enforced by bounds check in DirectCalibration before values are stored).
    //
    W=(3.14159265f)/32;  // 2*freq*PI/sampling freq
    PhasenRadian=acos(0.5f*(1+Error))-(3.14159265f/3);
    D=PhasenRadian/W;

    *CalIntDelayPointer=(uint8_t)D;
    float fracD = D - (float)(*CalIntDelayPointer);

    B=-((1-2*fracD)*cos(W)-sqrt(((1-2*fracD)*(1-2*fracD)*cos(W)*cos(W))+(4*fracD*(1-fracD))))/(2*(1-fracD));
    A=1/(sqrt(((cos(W)+B)*(cos(W)+B))+sin(W)*sin(W)));
    *CalGainBufferPointer=A;
    *CalBetaBufferPointer=B;
}
   
/*
Inf: Modbus communication API
Inp: None
Ret: None
*/

#ifdef MODEL_RS485

// Sends data over modbus
// @param buffer: Buffer containing data to be sent over modbus (should be in big endian)
// @param length; Length of data in bytes to be sent
// @param func: The current function code to be sent over
//
static void ModbusSendData(uint8_t* buffer, uint8_t length, uint8_t func)
{
  memcpy(&Mod_TransmitFrame.Data_Array[1], buffer, length);
  Mod_TransmitFrame.Data_Array[0] = length;
  SendData_UART((uint8_t)CopySetPara[PARA_DEVICE_ID], func, (length+1));
}

#ifdef MODBUS_MAP_PROCOM
void ModBusCommunication(void)
{
   float MultiplicationFactor;
   Temp_Start_Add=0;
   
   if((Timer.End_Frame)&&(!Timer.ParityError))
   {
     if ((ReceiveLength < 4) || (ReceiveLength > MAX_BYTE_TO_RECIEVE))
     {
       // Drop invalid frames to avoid buffer overruns and spurious faults
       Timer.End_Frame = 0;
       Timer.ParityError = 0;
       ReceiveLength = 0;
       return;
     }

     Timer.End_Frame=0;
     Add_Received = RecieveArray[0];
     Fun_Received = RecieveArray[1];
     
     for(int DataReceived =0; DataReceived<(ReceiveLength-2); DataReceived++)
     {
       Data_Received[DataReceived] = RecieveArray[DataReceived];
     }
     
     CRC_Received_High = RecieveArray[ReceiveLength-1];
     CRC_Received_Low = RecieveArray[ReceiveLength-2]; 
     CRC_Received = (CRC_Received_Low) + (CRC_Received_High<<8);
     
     uint8_t Temp_Length = ReceiveLength-2;
     CRC_Calculated = ModBusCRCCalculation(Data_Received, Temp_Length);
          
     if((Add_Received == (uint8_t)CopySetPara[PARA_DEVICE_ID]) && (CRC_Received==CRC_Calculated))
     {
       switch(Fun_Received)
       {                 
        case 0x03:
        { 
            Start_Add_High = RecieveArray[2];
            Start_Add_Low = RecieveArray[3];
            Start_Add = (Start_Add_Low) + (Start_Add_High<<8);
            NoOfBytes_High = RecieveArray[4];
            NoOfBytes_Low = RecieveArray[5];
            NoOfBytes = (NoOfBytes_Low) + (NoOfBytes_High<<8);

            if((NoOfBytes%2) || (NoOfBytes == 0))
            {
                Fun_Received |= 0x80;
                Mod_TransmitFrame.Data_Array[0] = 0x03;
                SendData_UART((uint8_t)CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
                Fun_Received &=~ 0x80;
                break;
            }
            NoOfBytes=NoOfBytes/2;
            Timer.DoubleData = 0;
            
            bool found = false;
            uint16_t ArrayIndex = 0;
            for (uint8_t i = 0; i < ARRAY_SIZE(ModbusTableSections); i++)
            {
                if ((Start_Add >= ModbusTableSections[i].address) &&
                    (Start_Add < ModbusTableSections[i].address + ModbusTableSections[i].numEntries * 2) &&
                    (!(Start_Add%2)))
                {
                    Start_Add -= ModbusTableSections[i].address; 
                    AvailableByte = ModbusTableSections[i].numEntries - Start_Add/2;
                    ArrayIndex += Start_Add/2;

                    found = true;
                    break;
                }
                ArrayIndex += ModbusTableSections[i].numEntries;
            }
            
            if (!found)    
            {
              Fun_Received |= 0x80;
              Mod_TransmitFrame.Data_Array[0] = 0x02;
              SendData_UART((uint8_t)CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
              Fun_Received &=~ 0x80;
              break;
            }

            ////// Exception Response for Illegal Value //////
            if(NoOfBytes > AvailableByte)
            {
              Fun_Received |= 0x80;
              Mod_TransmitFrame.Data_Array[0] = 0x03;
              SendData_UART((uint8_t)CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
              Fun_Received &=~ 0x80;
              break;
            }

             if(Timer.DoubleData==1)
             {
               for(uint8_t d=0; d<NoOfBytes; d++)
                {
                  DataLengthRegister = BlockAll[ArrayIndex+d].DataType;
                  if( BlockAll[ArrayIndex+d].Multiply==CT_RATIO)MultiplicationFactor=CtRatio;
                  else if (BlockAll[ArrayIndex+d].Multiply==PT_RATIO)MultiplicationFactor=PtRatio;
                  else if (BlockAll[ArrayIndex+d].Multiply==CT_PT_RATIO)MultiplicationFactor=CtPtRatio;
                  else MultiplicationFactor=1;
                  uint64_t TempDataTransfer; 
                  if(DataLengthRegister == DATA_TYPE_16)
                  {
                    TempDataTransfer =  *(uint16_t *)(*(uint32_t *)&BlockAll[ArrayIndex+d]);
                    TempDataTransfer = TempDataTransfer * (uint16_t)MultiplicationFactor;
                  }
                  else if(DataLengthRegister==DATA_TYPE_32)
                  {
                    TempDataTransfer =  *(uint32_t *)(*(uint32_t *)&BlockAll[ArrayIndex+d]);
                    TempDataTransfer = TempDataTransfer * (uint32_t)MultiplicationFactor;
                  }
                  else if(DataLengthRegister==DATA_TYPE_Float)
                  {
                    float DataFloatTemp;
                    double Data64Temporary;
                    DataFloatTemp =  *(float *)(*(uint32_t *)&BlockAll[ArrayIndex+d]);
                    DataFloatTemp = DataFloatTemp * (float)MultiplicationFactor;
                    Data64Temporary = DataFloatTemp;
                    TempDataTransfer = *(uint64_t *)&Data64Temporary;
                  }
                  else if(DataLengthRegister==DATA_TYPE_Double)
                  {
                    double Data64Temporary;
                    Data64Temporary =  *(double *)(*(uint32_t *)&BlockAll[ArrayIndex+d]);
                    Data64Temporary = Data64Temporary * (float)MultiplicationFactor;
                    TempDataTransfer = *(uint64_t *)&Data64Temporary;
                  }
                  
                  if(BlockAll[ArrayIndex+d].lookup)
                  {
                    if(CopySetPara[PARA_ENDIAN]==0)
                    {
                      Mod_TransmitFrame.Data_Array[DataArrLoc+8] = *((uint8_t *)&TempDataTransfer);
                      Mod_TransmitFrame.Data_Array[DataArrLoc+7] = *((uint8_t *)&TempDataTransfer+1);
                      Mod_TransmitFrame.Data_Array[DataArrLoc+6] = *((uint8_t *)&TempDataTransfer+2);
                      Mod_TransmitFrame.Data_Array[DataArrLoc+5] = *((uint8_t *)&TempDataTransfer+3);
                      Mod_TransmitFrame.Data_Array[DataArrLoc+4] = *((uint8_t *)&TempDataTransfer+4);
                      Mod_TransmitFrame.Data_Array[DataArrLoc+3] = *((uint8_t *)&TempDataTransfer+5);
                      Mod_TransmitFrame.Data_Array[DataArrLoc+2] = *((uint8_t *)&TempDataTransfer+6);
                      Mod_TransmitFrame.Data_Array[DataArrLoc+1] = *((uint8_t *)&TempDataTransfer+7);
                    }
                    else
                     {
                       Mod_TransmitFrame.Data_Array[DataArrLoc+2] = *((uint8_t *)&TempDataTransfer);
                       Mod_TransmitFrame.Data_Array[DataArrLoc+1] = *((uint8_t *)&TempDataTransfer+1);
                       Mod_TransmitFrame.Data_Array[DataArrLoc+4] = *((uint8_t *)&TempDataTransfer+2);
                       Mod_TransmitFrame.Data_Array[DataArrLoc+3] = *((uint8_t *)&TempDataTransfer+3);
                       Mod_TransmitFrame.Data_Array[DataArrLoc+6] = *((uint8_t *)&TempDataTransfer+4);
                       Mod_TransmitFrame.Data_Array[DataArrLoc+5] = *((uint8_t *)&TempDataTransfer+5);
                       Mod_TransmitFrame.Data_Array[DataArrLoc+8] = *((uint8_t *)&TempDataTransfer+6);
                       Mod_TransmitFrame.Data_Array[DataArrLoc+7] = *((uint8_t *)&TempDataTransfer+7);
                     }
                  }
                  else
                  {
                    Mod_TransmitFrame.Data_Array[DataArrLoc+1]=0;
                    Mod_TransmitFrame.Data_Array[DataArrLoc+2]=0;
                    Mod_TransmitFrame.Data_Array[DataArrLoc+3]=0;
                    Mod_TransmitFrame.Data_Array[DataArrLoc+4]=0;
                    Mod_TransmitFrame.Data_Array[DataArrLoc+5]=0;
                    Mod_TransmitFrame.Data_Array[DataArrLoc+6]=0;
                    Mod_TransmitFrame.Data_Array[DataArrLoc+7]=0;
                    Mod_TransmitFrame.Data_Array[DataArrLoc+8]=0;
                  }
                  ByteCount +=8;
                  DataArrLoc += 8;
                }
               }
               else
               {
                  for(uint8_t d=0; d<NoOfBytes; d++)
                  {
                    DataLengthRegister = BlockAll[ArrayIndex+d].DataType;
                    if( BlockAll[ArrayIndex+d].Multiply==CT_RATIO)MultiplicationFactor=CtRatio;
                    else if (BlockAll[ArrayIndex+d].Multiply==PT_RATIO)MultiplicationFactor=PtRatio;
                    else if (BlockAll[ArrayIndex+d].Multiply==CT_PT_RATIO)MultiplicationFactor=CtPtRatio;
                    else MultiplicationFactor=1;
                    uint32_t TempDataTransfer; 
                    if(DataLengthRegister == DATA_TYPE_16)
                    {
                      TempDataTransfer =  *(uint16_t *)(*(uint32_t *)&BlockAll[ArrayIndex+d]);
                      TempDataTransfer = TempDataTransfer * (uint16_t)MultiplicationFactor;
                    }
                    else if(DataLengthRegister==DATA_TYPE_32)
                    {
                      TempDataTransfer =  *(uint32_t *)(*(uint32_t *)&BlockAll[ArrayIndex+d]);
                      TempDataTransfer = TempDataTransfer * (uint32_t)MultiplicationFactor;
                    }
                    else if(DataLengthRegister==DATA_TYPE_Float)
                    {
                      float DataFloatTemp;
                      DataFloatTemp =  *(float *)(*(uint32_t *)&BlockAll[ArrayIndex+d]);
                      DataFloatTemp = DataFloatTemp * (float)MultiplicationFactor;
                      TempDataTransfer = *(uint32_t *)&DataFloatTemp;
                    }
                    else if(DataLengthRegister==DATA_TYPE_Double)
                    {
                      double Data64Temporary;
                      float DataFloatTemp;
                      Data64Temporary =  *(double *)(*(uint32_t *)&BlockAll[ArrayIndex+d]);
                      DataFloatTemp = (float)Data64Temporary;
                      DataFloatTemp = DataFloatTemp * (float)MultiplicationFactor;
                      TempDataTransfer = *(uint32_t *)&DataFloatTemp;
                    }
                    
                    if(BlockAll[ArrayIndex+d].lookup)
                    {
                      if(CopySetPara[PARA_ENDIAN]==0)
                      {
                        Mod_TransmitFrame.Data_Array[DataArrLoc+4] = *((uint8_t *)&TempDataTransfer);
                        Mod_TransmitFrame.Data_Array[DataArrLoc+3] = *((uint8_t *)&TempDataTransfer+1);
                        Mod_TransmitFrame.Data_Array[DataArrLoc+2] = *((uint8_t *)&TempDataTransfer+2);
                        Mod_TransmitFrame.Data_Array[DataArrLoc+1] = *((uint8_t *)&TempDataTransfer+3);
                      }
                      else
                      {
                        Mod_TransmitFrame.Data_Array[DataArrLoc+2] = *((uint8_t *)&TempDataTransfer);
                        Mod_TransmitFrame.Data_Array[DataArrLoc+1] = *((uint8_t *)&TempDataTransfer+1);
                        Mod_TransmitFrame.Data_Array[DataArrLoc+4] = *((uint8_t *)&TempDataTransfer+2);
                        Mod_TransmitFrame.Data_Array[DataArrLoc+3] = *((uint8_t *)&TempDataTransfer+3);
                          
                       }
                    }
                    else
                    {
                      Mod_TransmitFrame.Data_Array[DataArrLoc+1]=0;
                      Mod_TransmitFrame.Data_Array[DataArrLoc+2]=0;
                      Mod_TransmitFrame.Data_Array[DataArrLoc+3]=0;
                      Mod_TransmitFrame.Data_Array[DataArrLoc+4]=0;
                    }
                    ByteCount +=4;
                    DataArrLoc += 4;
                  }
               }     
            Mod_TransmitFrame.Data_Array[0] = ByteCount;

            SendData_UART((uint8_t)CopySetPara[PARA_DEVICE_ID], Fun_Received, (ByteCount+1));
            DataArrLoc = 0;
            ByteCount = 0;
            break;
          }
          case 0x10:
          {
              Start_Add_High = RecieveArray[2];
              Start_Add_Low = RecieveArray[3];
              Start_Add = (Start_Add_Low) + (Start_Add_High<<8);
              NoOfBytes_High = RecieveArray[4];
              NoOfBytes_Low = RecieveArray[5];
              NoOfBytes = (NoOfBytes_Low) + (NoOfBytes_High<<8);
              
              if((Start_Add >= 4100)&&(Start_Add <= 4100+2)&&(!(Start_Add %2)))
              {
                Start_Add -= 4100;
                //Start_Add +=2; // To remove system configuration
                CheckPasswordEdit(Start_Add/2,NoOfBytes);
                break;
              } 
              else if((Start_Add >= 5000)&&(Start_Add <= 5000+(MAX_PARAM_LIMIT*2))&&(!(Start_Add %2)))
              {
                Start_Add -= 5000;
                //Start_Add +=2; // To remove system configuration
                ModbusUpdateParameter(Start_Add/2,NoOfBytes);
                break;
              }
              else if (Start_Add == 6000 && NoOfBytes == 2)
              {
                  g_DisableLoadOnGridSeconds =
                      ((uint32_t)RecieveArray[10]) +
                      ((uint32_t)RecieveArray[9]<<8) +
                      ((uint32_t)RecieveArray[8]<<16)+
                      ((uint32_t)RecieveArray[7]<<24);
                  memcpy(Mod_TransmitFrame.Data_Array, &RecieveArray[2], 4);
                  SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,4);
                  break;
              }
              else  ///// Illegal Data Address //////
              {

                Fun_Received |= 0x80;
                Mod_TransmitFrame.Data_Array[0] = 0x02;
                SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
                Fun_Received &=~ 0x80;
                break;
              }
              break;
            }
            case 0x1 /*MODBUS_READ_COIL_STATUS_CODE*/:
            case 0x2 /*MODBUS_READ_INPUT_STATUS_CODE*/:
            {
                Start_Add_High = RecieveArray[2];
                Start_Add_Low = RecieveArray[3];
                Start_Add = (Start_Add_Low) + (Start_Add_High<<8);
                NoOfBytes_High = RecieveArray[4];
                NoOfBytes_Low = RecieveArray[5];
                NoOfBytes = (NoOfBytes_Low) + (NoOfBytes_High<<8);
                
                struct MapEntry
                {
                    uint16_t address;
                    uint16_t numEntries;
                    uint8_t* buffer;
                };

                struct MapEntry mapEntries[] = 
                {
                    {
                        0,
                        sizeof(g_DigInputs),
                        (uint8_t*)(&g_DigInputs)
                    },
                    {
                        100,
                        sizeof(g_Alarms),
                        (uint8_t*)(&g_Alarms)
                    },
                    {
                        200,
                        sizeof(g_powerSupplyStatus),
                        (uint8_t*)(&g_powerSupplyStatus)
                    },
                    {
                        20000,
                        sizeof(g_DigInputs),
                        (uint8_t*)(&g_DigInputs)
                    },
                    {
                        30000,
                        sizeof(g_DigOutputs),
                        (uint8_t*)(&g_DigOutputs)
                    },
                    {
                        40000,
                        sizeof(g_testingStatus),
                        (uint8_t*)(&g_testingStatus),
                    },
                    {
                        60000,
                        sizeof(g_LedStatus),
                        (uint8_t*)(&g_LedStatus),
                    }
                };

                bool found = false;

                for (uint8_t i = 0; i < ARRAY_SIZE(mapEntries); i++)
                {
                    if (!(Start_Add >= mapEntries[i].address &&
                            Start_Add + NoOfBytes <= mapEntries[i].address + mapEntries[i].numEntries))
                    {
                        continue;
                    }

                    found = true;

                    // We need to shift certain bits if Start_Add != 0
                    //
                    uint8_t sourceBuffer[64] = {0};
                    uint8_t destBuffer[64] = {0};

                    uint8_t* source = mapEntries[i].buffer;
                    for (int j = 0; j < mapEntries[i].numEntries; j++)
                    {
                        if (source[j])
                        {
                            int sourceByteIdx = j / 8;
                            int sourceBitIdx = j - 8 * sourceByteIdx;
                            sourceBuffer[sourceByteIdx] |= (1 << sourceBitIdx);
                        }
                    }
                    for (int bitIdx = 0; bitIdx < NoOfBytes; bitIdx++)
                    {
                        int sourceBitIdx = bitIdx + Start_Add - mapEntries[i].address;
                        int sourceByteIdx = sourceBitIdx / 8;
                        int sourceBitOffset = sourceBitIdx - sourceByteIdx * 8;

                        int destBitIdx = bitIdx;
                        int destByteIdx = destBitIdx / 8;
                        int destBitOffset = destBitIdx - destByteIdx * 8;

                        destBuffer[destByteIdx] |=
                            (((sourceBuffer[sourceByteIdx] >> sourceBitOffset) & 0x1) << destBitOffset);
                    }

                    uint8_t numBytesToSend = ROUNDUP_POW2(NoOfBytes, 8) / 8;
                    ModbusSendData(destBuffer, numBytesToSend, Fun_Received);
                }

                if (!found)
                {
                    Fun_Received |= 0x80;
                    Mod_TransmitFrame.Data_Array[0] = 0x02;
                    SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
                    Fun_Received &=~ 0x80;
                    break;
                }
                break;
            }
            case 0x05: /*MODBUS_WRITE_SINGLE_COIL_STATUS_CODE*/
            {
                Start_Add_High = RecieveArray[2];
                Start_Add_Low = RecieveArray[3];
                Start_Add = (Start_Add_Low) + (Start_Add_High<<8);
                NoOfBytes_High = RecieveArray[4];
                NoOfBytes_Low = RecieveArray[5];
                NoOfBytes = (NoOfBytes_Low) + (NoOfBytes_High<<8);

                // Only 0xFF00 & 0x0000 are valid values
                //
                if (NoOfBytes != 0x0000 && NoOfBytes != 0xFF00)
                {
                    Fun_Received |= 0x80;
                    Mod_TransmitFrame.Data_Array[0] = 0x03;
                    SendData_UART((uint8_t)CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
                    Fun_Received &=~ 0x80;
                    break;
                }

                bool enable = (NoOfBytes == 0xFF00);

                if (Start_Add >= 30000 && Start_Add < 30000 + sizeof(g_DigOutputs))
                {
                    g_DigOutputs.Relays[Start_Add - 30000] = enable;
                }
                else if (Start_Add >= 40000 && Start_Add < 40000 + sizeof(g_testingStatus))
                {
                    g_testingStatus.Status[Start_Add - 40000] = enable;
                }
                else if (Start_Add == 0)
                {
                    NVIC_SystemReset();
                }
                else if (Start_Add >= 60000 && Start_Add < 60000 + sizeof(g_LedStatus))
                {
                    g_LedStatus.Status[Start_Add - 60000] = enable;
                }
                else
                {
                    Fun_Received |= 0x80;
                    Mod_TransmitFrame.Data_Array[0] = 0x02;
                    SendData_UART((uint8_t)CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
                    Fun_Received &=~ 0x80;
                    break;
                }
                memcpy(Mod_TransmitFrame.Data_Array, &RecieveArray[2], 4);
                SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,4);
                break;
            }
       ///// Exception Response for Illegal Function //////    
          default:
          {
            if((Fun_Received != 0x03))
            {
              Fun_Received |= 0x80;
              Mod_TransmitFrame.Data_Array[0] = 0x01;
              SendData_UART((uint8_t)CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
              Fun_Received &=~ 0x80;
            }
            break;
          }
       }
     }
     ReceiveLength=0;
   }
   else if((Timer.ParityError) && (Timer.End_Frame))
   {
     Timer.End_Frame=0;
     Timer.ParityError = 0;
     ReceiveLength=0;
   }
}
#endif


/*
Inf: Modbus CRC calculation 
Inp: String value and length
Ret: crc
*/
uint16_t ModBusCRCCalculation(uint8_t * str,uint8_t length)
{
  uint8_t i,j;
  uint16_t check_sum=0xffff;
  for (i=0; i<length; i++)
  {
    check_sum ^= (uint16_t)str[i];
    for (j=8; j!=0; j--)
    {
      if ((check_sum & 0x01) != 0)
      {
        check_sum >>= 1;
        check_sum ^= 0xA001;
      }
      else check_sum >>= 1;
    }
  }
  return check_sum;
}

 
/*
Inf: UART transmit modbus date API 
Inp: Address, function code and Length.
Ret: None
*/
void SendData_UART(uint8_t Add, uint8_t Func,  uint8_t Send_Length)
  {
      uint16_t Temp_CRC_Value;

      Mod_TransmitFrame.Slave_Address = Add;
      Mod_TransmitFrame.Function_Code = Func;
      Temp_CRC_Value = ModBusCRCCalculation(&Mod_TransmitFrame.Slave_Address,(Send_Length+2));   
      Mod_TransmitFrame.Data_Array[Send_Length] = Temp_CRC_Value;
      Mod_TransmitFrame.Data_Array[Send_Length+1] = Temp_CRC_Value>>8;
      if (!g_testingStatus.TestingModeEnabled)
      {
        g_LedStatus.Comm = true;
      }
      RS485Transmit;
      Delay1Msec12Mhz(4);
      DMA1_Channel7->CCR &=~ 0x01;
      DMA1_Channel7->CMAR = (unsigned int) &Mod_TransmitFrame.Slave_Address;
      DMA1_Channel7->CNDTR = (uint32_t)(Send_Length+4);
      DMA1_Channel7->CCR |= 0x0A;
      DMA1_Channel7->CCR |= 0x01;
      

  }

/*
Inf: Update Edit parameters through modbus
Inp: Address and number of bytes to edit
Ret: None
*/

uint16_t ModCopySetPara[MAX_PARAM_LIMIT+1];
void ModbusUpdateParameter(uint16_t Address,uint16_t NoOfBytes)
{
    uint32_t Temp32,TempParaSetting;//TempReset;
    uint8_t error = 0;
    
    for( Temp32=0;Temp32<MAX_PARAM_LIMIT;Temp32++)ModCopySetPara[Temp32]=CopySetPara[Temp32];
    for(uint16_t CopyLength = 0; CopyLength<NoOfBytes/2; CopyLength++)
    {
      
      Temp32=RecieveArray[CopyLength*4+10]+RecieveArray[CopyLength*4+9]*256;
      ModCopySetPara[CopyLength+Address] = (uint16_t)Temp32;    
    }
    Temp32=0;
    while(Temp32<(MAX_PARAM_LIMIT))
    {
      TempParaSetting=ModCopySetPara[Temp32];
      if(TempParaSetting !=0xffff)
      {
        if((TempParaSetting < EditParameters[Temp32].MinValue)||(TempParaSetting > EditParameters[Temp32].MaxValue))
          error = 1;
      }
      else if(EditParameters[Temp32].DisableType ==0)error = 1;
      Temp32++;
    }
    if(error || (!EditParaPassStatus))
    {
      Mod_TransmitFrame.Data_Array[0] = 0x03;
      Fun_Received |= 0x80;
      SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
      Fun_Received &=~ 0x80;
      return;
    }
      
    for( Temp32=0;Temp32<MAX_PARAM_LIMIT;Temp32++)CopySetPara[Temp32]=ModCopySetPara[Temp32];
    UpdateEditSettings();  
    Mod_TransmitFrame.Data_Array[0] = 5;     // Ack
    Delay1Msec12Mhz(20);
    EditParaPassStatus = 0;
    
    SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
}
 
/*
Inf: Check password for  Edit parameters through modbus
Inp: Address and number of bytes to compare with meter password
Ret: None
*/

void CheckPasswordEdit(uint16_t Address,uint16_t NoOfBytes)
{
   uint8_t error = 0;
    
    uint16_t ReadPass;
    ReadPass=RecieveArray[10]+RecieveArray[9]*256;
    //for( Temp32=0;Temp32<MAX_PARAM_LIMIT;Temp32++)ModCopySetPara[Temp32]=CopySetPara[Temp32];
    //for(uint16_t CopyLength = 0; CopyLength<NoOfBytes/2; CopyLength++)
    //{
      
     // Temp32=RecieveArray[CopyLength*4+10]+RecieveArray[CopyLength*4+9]*256;
     // ModCopySetPara[CopyLength+Address] = (uint16_t)Temp32;    
    //}
    //Temp32=0;
    if(ReadPass != MeterPassword)
    {
      error = 1;
    }
    if(error)
    {
      Mod_TransmitFrame.Data_Array[0] = 0x03;
      Fun_Received |= 0x80;
      SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
      Fun_Received &=~ 0x80;
      return;
    }
    Mod_TransmitFrame.Data_Array[0] = 5;     // Ack
    EditParaPassStatus=1;
    Delay1Msec12Mhz(20);
    
    SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
}

#endif // #define MODEL_RS485
