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

// This struct keeps track of FIRMWARE Over-The-Air Upgrade feature
//
struct FotaState g_fota = {0};

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
        16000, InstPara_LoadOnSolarDisableSec,
    },
    {
        20000, InstPara_FanTemp,
    },
    {
        30000, MAX_PARAM_LIMIT
    },
    {
        40000, InstPara_FotaStatus
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
  if (!g_testingStatus.TestingModeEnabled && PROTECTION_BIT_LOW)
  {
      NVIC_SystemReset();
  }
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
Ret: True on success, else false
*/
bool DirectCalibration(void)
{
 float TempFloat;

 if(FlagDirectCalibration==CALIBRATE_L_PF)
 {
   // UNDONE: Should the guard rails be on average or instant parameters
   //
   if(
      (InstantPara.VolR >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolR<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.VolY >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolY<VOLTAGE_HIGHER_LIMIT)&&
      (InstantPara.VolB >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolB<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.CurrentR >I_L_LOWER_LIMIT) && (InstantPara.CurrentR<I_L_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_L_LOWER_LIMIT)&& (InstantPara.CurrentY<I_L_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_L_LOWER_LIMIT) && (InstantPara.CurrentB<I_L_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_L_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_L_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_L_LOWER_LIMIT)&&

      (InstantPara.VolRSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolRSolar<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.VolYSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolYSolar<VOLTAGE_HIGHER_LIMIT)&&
      (InstantPara.VolBSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolBSolar<VOLTAGE_HIGHER_LIMIT) &&
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

      if (CalBuffer.IR_LOW_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IR_LOW_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IY_LOW_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IY_LOW_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IB_LOW_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IB_LOW_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IR_SOLAR_LOW_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IR_SOLAR_LOW_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IY_SOLAR_LOW_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IY_SOLAR_LOW_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IB_SOLAR_LOW_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IB_SOLAR_LOW_PH_ERROR<PH_ERROR_MIN)
      {
          FlagDirectCalibration = CALIBRATE_ERROR;
      }

   }
   else
   {
       FlagDirectCalibration = CALIBRATE_ERROR;
   }
 }

 if(FlagDirectCalibration==CALIBRATE_L_VI)
 {
   if(
      (InstantPara.VolR >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolR<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.VolY >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolY<VOLTAGE_HIGHER_LIMIT)&&
      (InstantPara.VolB >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolB<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.CurrentR >I_L_LOWER_LIMIT) && (InstantPara.CurrentR<I_L_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_L_LOWER_LIMIT)&& (InstantPara.CurrentY<I_L_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_L_LOWER_LIMIT) && (InstantPara.CurrentB<I_L_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_L_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_L_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_L_LOWER_LIMIT)&&

      (InstantPara.VolRSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolRSolar<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.VolYSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolYSolar<VOLTAGE_HIGHER_LIMIT)&&
      (InstantPara.VolBSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolBSolar<VOLTAGE_HIGHER_LIMIT) &&
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
       FlagDirectCalibration = CALIBRATE_ERROR;
   }
 }

 else if(FlagDirectCalibration==CALIBRATE_M_PF)
 {
   if(
      (InstantPara.VolR >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolR<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.VolY >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolY<VOLTAGE_HIGHER_LIMIT)&&
      (InstantPara.VolB >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolB<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.CurrentR >I_M_LOWER_LIMIT) && (InstantPara.CurrentR<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_M_LOWER_LIMIT)&& (InstantPara.CurrentY<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_M_LOWER_LIMIT) && (InstantPara.CurrentB<I_M_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_M_LOWER_LIMIT)&&

      (InstantPara.VolRSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolRSolar<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.VolYSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolYSolar<VOLTAGE_HIGHER_LIMIT)&&
      (InstantPara.VolBSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolBSolar<VOLTAGE_HIGHER_LIMIT) &&
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

      if (CalBuffer.IR_MID_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IR_MID_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IY_MID_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IY_MID_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IB_MID_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IB_MID_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IR_SOLAR_MID_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IR_SOLAR_MID_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IY_SOLAR_MID_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IY_SOLAR_MID_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IB_SOLAR_MID_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IB_SOLAR_MID_PH_ERROR<PH_ERROR_MIN)
      {
          FlagDirectCalibration = CALIBRATE_ERROR;
      }
   }
   else
   {
       FlagDirectCalibration = CALIBRATE_ERROR;
   }
 }

 else if(FlagDirectCalibration==CALIBRATE_M_VI)
 {
   if(
      (InstantPara.VolR >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolR<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.VolY >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolY<VOLTAGE_HIGHER_LIMIT)&&
      (InstantPara.VolB >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolB<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.CurrentR >I_M_LOWER_LIMIT) && (InstantPara.CurrentR<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_M_LOWER_LIMIT)&& (InstantPara.CurrentY<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_M_LOWER_LIMIT) && (InstantPara.CurrentB<I_M_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_M_LOWER_LIMIT)&&

      (InstantPara.VolRSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolRSolar<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.VolYSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolYSolar<VOLTAGE_HIGHER_LIMIT)&&
      (InstantPara.VolBSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolBSolar<VOLTAGE_HIGHER_LIMIT) &&
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
       FlagDirectCalibration = CALIBRATE_ERROR;
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

      if (CalBuffer.IR_HIGH_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IR_HIGH_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IY_HIGH_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IY_HIGH_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IB_HIGH_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IB_HIGH_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IR_SOLAR_HIGH_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IR_SOLAR_HIGH_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IY_SOLAR_HIGH_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IY_SOLAR_HIGH_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IB_SOLAR_HIGH_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IB_SOLAR_HIGH_PH_ERROR<PH_ERROR_MIN)
      {
          FlagDirectCalibration = CALIBRATE_ERROR;
      }
   }
   else
   {
       FlagDirectCalibration = CALIBRATE_ERROR;
   }
 }

  else if(FlagDirectCalibration==CALIBRATE_XH_PF)
  {
    if(
      (InstantPara.VolR >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolR<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.VolY >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolY<VOLTAGE_HIGHER_LIMIT)&&
      (InstantPara.VolB >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolB<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.CurrentR >I_XH_LOWER_LIMIT) && (InstantPara.CurrentR<I_XH_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_XH_LOWER_LIMIT)&& (InstantPara.CurrentY<I_XH_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_XH_LOWER_LIMIT) && (InstantPara.CurrentB<I_XH_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_XH_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_XH_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_XH_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_XH_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_XH_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_XH_LOWER_LIMIT)&&

      (InstantPara.VolRSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolRSolar<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.VolYSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolYSolar<VOLTAGE_HIGHER_LIMIT)&&
      (InstantPara.VolBSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolBSolar<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.CurrentRSolar >I_XH_LOWER_LIMIT) && (InstantPara.CurrentRSolar<I_XH_UPPER_LIMIT)&&
      (InstantPara.CurrentYSolar >I_XH_LOWER_LIMIT)&& (InstantPara.CurrentYSolar<I_XH_UPPER_LIMIT)&&
      (InstantPara.CurrentBSolar >I_XH_LOWER_LIMIT) && (InstantPara.CurrentBSolar<I_XH_UPPER_LIMIT)&&
      (InstantPara.TotalPowerRSolar<PF_POWER_XH_UPPER_LIMIT)&&(InstantPara.TotalPowerRSolar>PF_POWER_XH_LOWER_LIMIT)&&
      (InstantPara.TotalPowerYSolar<PF_POWER_XH_UPPER_LIMIT)&&(InstantPara.TotalPowerYSolar>PF_POWER_XH_LOWER_LIMIT)&&
      (InstantPara.TotalPowerBSolar<PF_POWER_XH_UPPER_LIMIT)&&(InstantPara.TotalPowerBSolar>PF_POWER_XH_LOWER_LIMIT)
    )
   {

      ProtectionReset();
      __disable_interrupt();
      __no_operation();
      RESET_WATCH_DOG;
      SaveFlashData();

      TempFloat=(CalPowR-CAL_PF_POWER_SETTING_XHIGH)/CAL_PF_POWER_SETTING_XHIGH;
      CalBuffer.IR_XHIGH_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowY-CAL_PF_POWER_SETTING_XHIGH)/CAL_PF_POWER_SETTING_XHIGH;
      CalBuffer.IY_XHIGH_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowB-CAL_PF_POWER_SETTING_XHIGH)/CAL_PF_POWER_SETTING_XHIGH;
      CalBuffer.IB_XHIGH_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowRSolar-CAL_PF_POWER_SETTING_XHIGH)/CAL_PF_POWER_SETTING_XHIGH;
      CalBuffer.IR_SOLAR_XHIGH_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowYSolar-CAL_PF_POWER_SETTING_XHIGH)/CAL_PF_POWER_SETTING_XHIGH;
      CalBuffer.IY_SOLAR_XHIGH_PH_ERROR=TempFloat-0.001;

      TempFloat=(CalPowBSolar-CAL_PF_POWER_SETTING_XHIGH)/CAL_PF_POWER_SETTING_XHIGH;
      CalBuffer.IB_SOLAR_XHIGH_PH_ERROR=TempFloat-0.001;

      if (CalBuffer.IR_XHIGH_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IR_XHIGH_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IY_XHIGH_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IY_XHIGH_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IB_XHIGH_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IB_XHIGH_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IR_SOLAR_XHIGH_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IR_SOLAR_XHIGH_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IY_SOLAR_XHIGH_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IY_SOLAR_XHIGH_PH_ERROR<PH_ERROR_MIN ||
          CalBuffer.IB_SOLAR_XHIGH_PH_ERROR>PH_ERROR_MAX ||
          CalBuffer.IB_SOLAR_XHIGH_PH_ERROR<PH_ERROR_MIN)
      {
          FlagDirectCalibration = CALIBRATE_ERROR;
      }

      // Grid V_LL phase calibration. By this point V_R / V_Y / V_B gains
      // are applied (loaded at end of XH_VI) so InstantPara.VolR/Y/B are
      // in calibrated volts at ~240V and InstantPara.VolRY/YB/BR are in
      // calibrated volts at ~sqrt(3)*240 = 415.69V. PR_/PY_/PB_ are still
      // at identity, so V_LL is not biased by per-phase phase correction.
      //
      // Two equal-amplitude sinusoids with phase shift phi produce a
      // difference whose RMS is 2*A*|sin(phi/2)|. With A = 240V:
      //   phi      = 2 * arcsin(V_LL / 480)
      //   PH_ERROR = phi - 2*pi/3            (signed deviation from 120deg)
      //
      // PH_ERROR sign encodes which channel of the pair will be delayed at
      // runtime (positive => first letter, negative => second letter), via
      // the same FIR coefficients used by PR_/PY_/PB_; the conversion to
      // ALFA / BETA / INT_DELAY happens at SetWorkingGainBuffer() time via
      // CalPhaseLag().
      //
      // Saturation gate: V_LL must be within VOLTAGE_TOLERANCE-style band
      // V_LL_LOWER_LIMIT..V_LL_HIGHER_LIMIT (+/- 5%). Equivalent to
      // |PH_ERROR| <~ 10.8deg, well inside the 4-sample FIR budget.
      //
      {
          // Phase voltages are 120 degree apart
          //
          const float IDEAL_PHI = 2.0f * 3.14159265f / 3.0f;
          const float TWO_A     = 2.0f * CAL_VOLTAGE_SETTING_HIGH;

          float vRY = CalVolRY / NO_OF_CAL_ACCUMULATION_POW;
          float vYB = CalVolYB / NO_OF_CAL_ACCUMULATION_POW;
          float vBR = CalVolBR / NO_OF_CAL_ACCUMULATION_POW;

          if (vRY > V_LL_HIGHER_LIMIT || vRY < V_LL_LOWER_LIMIT ||
              vYB > V_LL_HIGHER_LIMIT || vYB < V_LL_LOWER_LIMIT ||
              vBR > V_LL_HIGHER_LIMIT || vBR < V_LL_LOWER_LIMIT)
          {
              FlagDirectCalibration = CALIBRATE_ERROR;
          }
          else
          {
              float r;

              r = vRY / TWO_A;
              if (r > 1.0f) r = 1.0f;
              if (r < 0.0f) r = 0.0f;
              CalBuffer.VLL_RY_PH_ERROR = 2.0f * asinf(r) - IDEAL_PHI;

              r = vYB / TWO_A;
              if (r > 1.0f) r = 1.0f;
              if (r < 0.0f) r = 0.0f;
              CalBuffer.VLL_YB_PH_ERROR = 2.0f * asinf(r) - IDEAL_PHI;

              r = vBR / TWO_A;
              if (r > 1.0f) r = 1.0f;
              if (r < 0.0f) r = 0.0f;
              CalBuffer.VLL_BR_PH_ERROR = 2.0f * asinf(r) - IDEAL_PHI;
          }
      }

      // Solar V_LL phase calibration. Same math as the grid block above:
      // the solar V_R/V_Y/V_B gains are already applied (loaded at end of
      // XH_VI) so InstantPara.VolR/Y/BSolar are at ~240V and
      // InstantPara.VolRY/YB/BRSolar are at ~sqrt(3)*240. The solar
      // PR_/PY_/PB_SOLAR FIRs are still at identity here, so V_LL is not
      // biased by the per-phase phase correction.
      //
      {
          const float IDEAL_PHI = 2.0f * 3.14159265f / 3.0f;
          const float TWO_A     = 2.0f * CAL_VOLTAGE_SETTING_HIGH;

          float vRY = CalVolRYSolar / NO_OF_CAL_ACCUMULATION_POW;
          float vYB = CalVolYBSolar / NO_OF_CAL_ACCUMULATION_POW;
          float vBR = CalVolBRSolar / NO_OF_CAL_ACCUMULATION_POW;

          if (vRY > V_LL_HIGHER_LIMIT || vRY < V_LL_LOWER_LIMIT ||
              vYB > V_LL_HIGHER_LIMIT || vYB < V_LL_LOWER_LIMIT ||
              vBR > V_LL_HIGHER_LIMIT || vBR < V_LL_LOWER_LIMIT)
          {
              FlagDirectCalibration = CALIBRATE_ERROR;
          }
          else
          {
              float r;

              r = vRY / TWO_A;
              if (r > 1.0f) r = 1.0f;
              if (r < 0.0f) r = 0.0f;
              CalBuffer.VLL_RY_SOLAR_PH_ERROR = 2.0f * asinf(r) - IDEAL_PHI;

              r = vYB / TWO_A;
              if (r > 1.0f) r = 1.0f;
              if (r < 0.0f) r = 0.0f;
              CalBuffer.VLL_YB_SOLAR_PH_ERROR = 2.0f * asinf(r) - IDEAL_PHI;

              r = vBR / TWO_A;
              if (r > 1.0f) r = 1.0f;
              if (r < 0.0f) r = 0.0f;
              CalBuffer.VLL_BR_SOLAR_PH_ERROR = 2.0f * asinf(r) - IDEAL_PHI;
          }
      }
   }
   else
   {
       FlagDirectCalibration = CALIBRATE_ERROR;
   }
  }

  else if(FlagDirectCalibration==CALIBRATE_XH_VI)
  {
    if(
      (InstantPara.VolR >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolR<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.VolY >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolY<VOLTAGE_HIGHER_LIMIT)&&
      (InstantPara.VolB >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolB<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.CurrentR >I_XH_LOWER_LIMIT) && (InstantPara.CurrentR<I_XH_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_XH_LOWER_LIMIT)&& (InstantPara.CurrentY<I_XH_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_XH_LOWER_LIMIT) && (InstantPara.CurrentB<I_XH_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_XH_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_XH_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_XH_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_XH_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_XH_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_XH_LOWER_LIMIT) &&

      (InstantPara.VolRSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolRSolar<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.VolYSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolYSolar<VOLTAGE_HIGHER_LIMIT)&&
      (InstantPara.VolBSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolBSolar<VOLTAGE_HIGHER_LIMIT) &&
      (InstantPara.CurrentRSolar >I_XH_LOWER_LIMIT) && (InstantPara.CurrentRSolar<I_XH_UPPER_LIMIT)&&
      (InstantPara.CurrentYSolar >I_XH_LOWER_LIMIT)&& (InstantPara.CurrentYSolar<I_XH_UPPER_LIMIT)&&
      (InstantPara.CurrentBSolar >I_XH_LOWER_LIMIT) && (InstantPara.CurrentBSolar<I_XH_UPPER_LIMIT)&&
      (InstantPara.TotalPowerRSolar<PF_POWER_XH_UPPER_LIMIT)&&(InstantPara.TotalPowerRSolar>PF_POWER_XH_LOWER_LIMIT)&&
      (InstantPara.TotalPowerYSolar<PF_POWER_XH_UPPER_LIMIT)&&(InstantPara.TotalPowerYSolar>PF_POWER_XH_LOWER_LIMIT)&&
      (InstantPara.TotalPowerBSolar<PF_POWER_XH_UPPER_LIMIT)&&(InstantPara.TotalPowerBSolar>PF_POWER_XH_LOWER_LIMIT)
    )

   {

      ProtectionReset();
      __disable_interrupt();
      __no_operation();
      RESET_WATCH_DOG;
      SaveFlashData();

      // Voltage gain is calibrated once at the highest current point (best
      // SNR) and reused for the remaining V/I/PF cal steps which all run at
      // V = 240 V.
      //
      CalBuffer.VR_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH)/CalVolR);
      CalBuffer.VY_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH)/CalVolY);
      CalBuffer.VB_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH)/CalVolB);
      CalBuffer.IR_XHIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_XHIGH)/CalCurR);
      CalBuffer.IY_XHIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_XHIGH)/CalCurY);
      CalBuffer.IB_XHIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_XHIGH)/CalCurB);

      CalBuffer.VR_SOLAR_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH)/CalVolRSolar);
      CalBuffer.VY_SOLAR_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH)/CalVolYSolar);
      CalBuffer.VB_SOLAR_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH)/CalVolBSolar);
      CalBuffer.IR_SOLAR_XHIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_XHIGH)/CalCurRSolar);
      CalBuffer.IY_SOLAR_XHIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_XHIGH)/CalCurYSolar);
      CalBuffer.IB_SOLAR_XHIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_XHIGH)/CalCurBSolar);
   }
   else
   {
       FlagDirectCalibration = CALIBRATE_ERROR;
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
      (InstantPara.TotalPowerBSolar<PF_POWER_H_UPPER_LIMIT)&&(InstantPara.TotalPowerBSolar>PF_POWER_H_LOWER_LIMIT)
    )

   {

      ProtectionReset();
      __disable_interrupt();
      __no_operation();
      RESET_WATCH_DOG;
      SaveFlashData();
      CalBuffer.IR_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH)/CalCurR);
      CalBuffer.IY_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH)/CalCurY);
      CalBuffer.IB_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH)/CalCurB);

      CalBuffer.IR_SOLAR_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH)/CalCurRSolar);
      CalBuffer.IY_SOLAR_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH)/CalCurYSolar);
      CalBuffer.IB_SOLAR_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH)/CalCurBSolar);
   }
   else
   {
       FlagDirectCalibration = CALIBRATE_ERROR;
   }
  }

  else if(FlagDirectCalibration==CALIBRATE_FAN)
  {
    if(
      (InstantPara.Fan1Current<FAN_CUR_HIGHER_LIMIT)&&(InstantPara.Fan1Current>FAN_CUR_LOWER_LIMIT)&&
      (InstantPara.Fan2Current<FAN_CUR_HIGHER_LIMIT)&&(InstantPara.Fan2Current>FAN_CUR_LOWER_LIMIT)
    )
   {

      ProtectionReset();
      __disable_interrupt();
      __no_operation();
      RESET_WATCH_DOG;
      SaveFlashData();

      CalBuffer.FAN1_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_FAN_CUR_SETTING)/CalFan1Current);
      CalBuffer.FAN2_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_FAN_CUR_SETTING)/CalFan2Current);
   }
   else
   {
       FlagDirectCalibration = CALIBRATE_ERROR;
   }
  }

  RESET_WATCH_DOG;
  if (FlagDirectCalibration != CALIBRATE_ERROR)
  {
    WriteFlashData();
  }
  RESET_WATCH_DOG;
  ClearInterruptVariables();
  __enable_interrupt();

  if (FlagDirectCalibration == CALIBRATE_ERROR)
  {
      return false;
  }
  return true;
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
   CalVolRY+=InstantPara.VolRY;
   CalVolYB+=InstantPara.VolYB;
   CalVolBR+=InstantPara.VolBR;
   CalCurR+=InstantPara.CurrentR;
   CalCurY+=InstantPara.CurrentY;
   CalCurB+=InstantPara.CurrentB;
   CalPowR+=InstantPara.TotalPowerR;
   CalPowY+=InstantPara.TotalPowerY;
   CalPowB+=InstantPara.TotalPowerB;

   CalVolRSolar+=InstantPara.VolRSolar;
   CalVolYSolar+=InstantPara.VolYSolar;
   CalVolBSolar+=InstantPara.VolBSolar;
   CalVolRYSolar+=InstantPara.VolRYSolar;
   CalVolYBSolar+=InstantPara.VolYBSolar;
   CalVolBRSolar+=InstantPara.VolBRSolar;
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
   CalVolRY=0;
   CalVolYB=0;
   CalVolBR=0;
   CalCurR=0;
   CalCurY=0;
   CalCurB=0;
   CalPowR=0;
   CalPowY=0;
   CalPowB=0;

   CalVolRSolar=0;
   CalVolYSolar=0;
   CalVolBSolar=0;
   CalVolRYSolar=0;
   CalVolYBSolar=0;
   CalVolBRSolar=0;
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
void CalPF(float Error, float * CalGainBufferPointer, float * CalBetaBufferPointer, int8_t * CalIntDelayPointer)
{
    // Phase correction: converts PH_ERROR to an integer sample delay +
    // fractional FIR coefficients (A, B).
    //
    // Sign of Error selects which signal the correction is applied to:
    //   Error <= 0: delay applied to voltage; *CalIntDelayPointer in 0..3.
    //   Error >  0: delay applied to current; *CalIntDelayPointer in -1..-4
    //               (bias of 1 so 0 unambiguously means V-side). Sign of
    //               Error is flipped before computing D so the same FIR
    //               coefficients can be reused on the other channel.
    //
    // Valid input range: PH_ERROR_MIN <= Error <= PH_ERROR_MAX (enforced
    // by the bounds check in DirectCalibration before values are stored).
    //
    bool delayOnI = (Error > 0.0f);
    if (delayOnI) Error = -Error;

    W=(3.14159265f)/32;  // 2*freq*PI/sampling freq
    PhasenRadian=acos(0.5f*(1+Error))-(3.14159265f/3);
    D=PhasenRadian/W;

    uint8_t intD = (uint8_t)D;
    float fracD = D - (float)intD;

    B=-((1-2*fracD)*cos(W)-sqrt(((1-2*fracD)*(1-2*fracD)*cos(W)*cos(W))+(4*fracD*(1-fracD))))/(2*(1-fracD));
    A=1/(sqrt(((cos(W)+B)*(cos(W)+B))+sin(W)*sin(W)));
    *CalGainBufferPointer=A;
    *CalBetaBufferPointer=B;
    *CalIntDelayPointer = delayOnI ? -(int8_t)(intD + 1) : (int8_t)intD;
}

/*
Inf: Compute V_LL phase-shift FIR coefficients (ALFA, BETA, INT_DELAY)
     from a signed phase-error in radians, mirroring the CalPF() math.
Inp: SignedPhaseRad: deviation of measured V_LL phase from ideal 120 deg.
                     Positive => delay applied to first letter of the V_LL
                     pair (R for V_RY, Y for V_YB, B for V_BR). Negative =>
                     delay applied to second letter (Y / B / R).
     CalGainBufferPointer / CalBetaBufferPointer / CalIntDelayPointer:
                     output ALFA, BETA, INT_DELAY for the FIR
                     y[n] = ALFA * (x[n - int_d] + BETA * x[n - int_d - 1])
                     applied at runtime in the V_LL accumulator.
Ret: None. Falls back to identity FIR (ALFA=1, BETA=0, INT_DELAY=0) if the
     requested delay exceeds the 4-sample FIR budget.
*/
void CalPhaseLag(float SignedPhaseRad, float * CalGainBufferPointer, float * CalBetaBufferPointer, int8_t * CalIntDelayPointer)
{
    // 2 * pi * f / fs at fundamental: 2*pi*50/3200 = pi/32.
    //
    const float W = 3.14159265f / 32.0f;

    bool  delayOnSecond = (SignedPhaseRad < 0.0f);
    float mag = delayOnSecond ? -SignedPhaseRad : SignedPhaseRad;
    float D   = mag / W;       // total fractional sample delay in [0, ...]

    // Saturation: outside the 4-sample FIR budget fall back to identity
    // FIR. The cal-time V_LL_*_LIMIT gate should already have caught this
    // (4 samples ~= 22.5 deg vs the 5% V_LL band ~= 10.8 deg), so this is
    // pure defence-in-depth for corrupt flash values.
    //
    if (D >= 4.0f)
    {
        *CalGainBufferPointer = 1.0f;
        *CalBetaBufferPointer = 0.0f;
        *CalIntDelayPointer   = 0;
        return;
    }

    uint8_t intD  = (uint8_t)D;
    float   fracD = D - (float)intD;

    // Same 2-tap fractional-delay FIR as CalPF(): chosen so the filter has
    // unit magnitude at W and a phase delay of (intD + fracD) samples.
    //
    float B = -((1.0f - 2.0f*fracD)*cosf(W) - sqrtf((1.0f - 2.0f*fracD)*(1.0f - 2.0f*fracD)*cosf(W)*cosf(W) + 4.0f*fracD*(1.0f - fracD))) / (2.0f*(1.0f - fracD));
    float A = 1.0f / sqrtf((cosf(W) + B)*(cosf(W) + B) + sinf(W)*sinf(W));

    *CalGainBufferPointer = A;
    *CalBetaBufferPointer = B;
    *CalIntDelayPointer   = delayOnSecond ? -(int8_t)(intD + 1) : (int8_t)intD;
}

/*
Inf: Derive 3 per-channel I_N FIR coefficients (ALFA/BETA/INT_DELAY) from
     existing V_LL and per-phase PF calibration data. No dedicated I_N
     cal step is required.

     The 6 V/I channels each have a fixed RC-filter / SDADC-slot delay
     (call them delta_VR, delta_VY, delta_VB, delta_IR, delta_IY, delta_IB,
     all in radians at 50 Hz). When a balanced 3-phase source is connected,
     these per-channel delays cause:
       - the measured V_R-V_Y angle to deviate from 120 deg by exactly
         (delta_VY - delta_VR), captured by V_LL cal as VllRyPhErr.
       - the measured V_R<->I_R angle (at PF=0.5 cal) to deviate from
         60 deg by exactly (delta_IR - delta_VR), call this eps_R.

     The I-channel inter-channel skew telescopes through V:
        delta_IY - delta_IR  =  (delta_IY - delta_VY)
                              + (delta_VY - delta_VR)
                              + (delta_VR - delta_IR)
                             =  eps_Y - eps_R + VllRyPhErr        (PH_ILL_RY)
        delta_IR - delta_IB  =  eps_R - eps_B + VllBrPhErr        (PH_ILL_BR)

     eps_X is recovered from the flash form
        I*_XHIGH_PH_ERROR = 2*cos(60 deg + eps_X) - 1
     via eps_X = acos((1 + flash) / 2) - pi/3 (signed). Argument clamped
     to [-1, +1] for safety against corrupt flash values.

     We then convert pair errors to 3 non-negative per-channel delays.
     Treating I_R as numeric reference, the relative I-channel delays are:
        rel_R = 0
        rel_Y = delta_IY - delta_IR =  PH_ILL_RY
        rel_B = delta_IB - delta_IR = -PH_ILL_BR
     Pick tau = max(0, rel_Y, rel_B) and apply
        d_R = tau,   d_Y = tau - rel_Y,   d_B = tau - rel_B
     so all three channels end up with the same total post-FIR delay -
     namely, the largest pre-FIR delay among the three (the most-lagging
     channel becomes the alignment target, and the others are delayed to
     catch up). FIR can only delay, never advance, so all d_X are
     non-negative by construction.

     CalPhaseLag() handles each d_X: it falls back to identity FIR
     internally if d_X exceeds the 4-sample (~22.5 deg) budget, which
     guards against pathological flash values where (VllRyPhErr +
     |eps_Y - eps_R|) might saturate.

Inp: VllRyPhErr   - VLL_RY_PH_ERROR from flash (signed radians)
     VllBrPhErr   - VLL_BR_PH_ERROR from flash (signed radians)
     IrXhPhErr    - IR_XHIGH_PH_ERROR from flash (TempFloat form)
     IyXhPhErr    - IY_XHIGH_PH_ERROR from flash (TempFloat form)
     IbXhPhErr    - IB_XHIGH_PH_ERROR from flash (TempFloat form)
     AlfaR/BetaR/IntDelayR - output FIR coefficients for I_R channel
     AlfaY/BetaY/IntDelayY - output FIR coefficients for I_Y channel
     AlfaB/BetaB/IntDelayB - output FIR coefficients for I_B channel
Ret: None.
*/
void DeriveNeutralFir(float VllRyPhErr, float VllBrPhErr,
                      float IrXhPhErr, float IyXhPhErr, float IbXhPhErr,
                      float * AlfaR, float * BetaR, int8_t * IntDelayR,
                      float * AlfaY, float * BetaY, int8_t * IntDelayY,
                      float * AlfaB, float * BetaB, int8_t * IntDelayB)
{
    const float PI_OVER_3 = 3.14159265f / 3.0f;

    // Step 1: recover signed eps per phase from the flash form.
    // flash = 2*cos(60 deg + eps) - 1  =>  eps = acos((1+flash)/2) - pi/3.
    // Clamp argument to acos's domain [-1, +1] in case of corrupt flash.
    //
    float r_R = (1.0f + IrXhPhErr) * 0.5f;
    float r_Y = (1.0f + IyXhPhErr) * 0.5f;
    float r_B = (1.0f + IbXhPhErr) * 0.5f;
    if (r_R >  1.0f) r_R =  1.0f;
    if (r_R < -1.0f) r_R = -1.0f;
    if (r_Y >  1.0f) r_Y =  1.0f;
    if (r_Y < -1.0f) r_Y = -1.0f;
    if (r_B >  1.0f) r_B =  1.0f;
    if (r_B < -1.0f) r_B = -1.0f;

    float eps_R = acosf(r_R) - PI_OVER_3;
    float eps_Y = acosf(r_Y) - PI_OVER_3;
    float eps_B = acosf(r_B) - PI_OVER_3;

    // Step 2: telescope to inter-channel I skew.
    //
    float PH_ILL_RY = VllRyPhErr + eps_Y - eps_R;     // delta_IY - delta_IR
    float PH_ILL_BR = VllBrPhErr + eps_R - eps_B;     // delta_IR - delta_IB

    // Step 3: pick the most-delayed channel as alignment target and derive
    // 3 non-negative per-channel delays.
    //
    float rel_Y = PH_ILL_RY;       // delta_IY - delta_IR
    float rel_B = -PH_ILL_BR;      // delta_IB - delta_IR

    float tau = 0.0f;
    if (rel_Y > tau) tau = rel_Y;
    if (rel_B > tau) tau = rel_B;

    float d_R = tau;
    float d_Y = tau - rel_Y;
    float d_B = tau - rel_B;

    // Step 4: convert to FIR coefficients. CalPhaseLag with positive input
    // takes the "delay first letter" branch (delayOnSecond=false), which
    // for our purposes simply means INT_DELAY is non-negative (0..3) and
    // selects the integer-sample tap directly. Saturation fallback to
    // identity FIR is built into CalPhaseLag.
    //
    CalPhaseLag(d_R, AlfaR, BetaR, IntDelayR);
    CalPhaseLag(d_Y, AlfaY, BetaY, IntDelayY);
    CalPhaseLag(d_B, AlfaB, BetaB, IntDelayB);
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
                      Mod_TransmitFrame.Data_Array[DataArrLoc+4] = *((uint8_t *)&TempDataTransfer);
                      Mod_TransmitFrame.Data_Array[DataArrLoc+3] = *((uint8_t *)&TempDataTransfer+1);
                      Mod_TransmitFrame.Data_Array[DataArrLoc+2] = *((uint8_t *)&TempDataTransfer+2);
                      Mod_TransmitFrame.Data_Array[DataArrLoc+1] = *((uint8_t *)&TempDataTransfer+3);
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
              else if (Start_Add == 16000 && NoOfBytes == 2)
              {
                  g_DisableLoadOnSolarSeconds =
                      ((uint32_t)RecieveArray[10]) +
                      ((uint32_t)RecieveArray[9]<<8) +
                      ((uint32_t)RecieveArray[8]<<16)+
                      ((uint32_t)RecieveArray[7]<<24);
                  memcpy(Mod_TransmitFrame.Data_Array, &RecieveArray[2], 4);
                  SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,4);
                  break;
              }
              else if((Start_Add >= 30000)&&(Start_Add <= 30000+(MAX_PARAM_LIMIT*2))&&(!(Start_Add %2)))
              {
                Start_Add -= 30000;
                //Start_Add +=2; // To remove system configuration
                ModbusUpdateParameter(Start_Add/2,NoOfBytes);
                break;
              }
              else if (Start_Add == 40000 && NoOfBytes == 2)
              {
                  uint32_t value =
                      ((uint32_t)RecieveArray[10]) +
                      ((uint32_t)RecieveArray[9]<<8) +
                      ((uint32_t)RecieveArray[8]<<16)+
                      ((uint32_t)RecieveArray[7]<<24);

                  if (value == 1)
                  {
                      memset(&g_fota, 0, sizeof(g_fota));
                      g_fota.runningCrc = 0xFFFF;
                      g_fota.status = FOTA_STATUS_READY;
                  }
                  else if (value == 2)
                  {
                      if (g_fota.status != FOTA_STATUS_READY ||
                          g_fota.chunksReceived != g_fota.totalRecords ||
                          g_fota.runningCrc != g_fota.expectedCrc ||
                          g_fota.version == 0)
                      {
                          g_fota.status = FOTA_STATUS_ERROR;
                          Fun_Received |= 0x80;
                          Mod_TransmitFrame.Data_Array[0] = 0x04;
                          SendData_UART((uint8_t)CopySetPara[PARA_DEVICE_ID],
                                        Fun_Received, 1);
                          Fun_Received &=~ 0x80;
                          break;
                      }

                      // Build the flash info in RAM, then program to flash.
                      // upgradePending is at the end of the struct so it
                      // is written last — a reset mid-write leaves it 0xFF
                      // (erased) and the bootloader ignores the partial write.
                      //
                      struct FotaFlashInfo info = {0};
                      info.firmwareSize = g_fota.firmwareSize;
                      info.prngSeed = g_fota.prngSeed;
                      info.expectedCrc = g_fota.runningCrc;
                      info.upgradePending = 0x01;

                      __disable_irq();
                      FLASH_Unlock();
                      FLASH_ErasePage((uint32_t)&g_fotaFlashInfo);
                      uint16_t *src = (uint16_t *)&info;
                      uint32_t base = (uint32_t)&g_fotaFlashInfo;
                      for (uint16_t i = 0;
                           i < sizeof(struct FotaFlashInfo) / 2;
                           i++)
                      {
                          FLASH_ProgramHalfWord(base + i * 2, src[i]);
                      }
                      FLASH_Lock();
                      __enable_irq();

                      memcpy(Mod_TransmitFrame.Data_Array, &RecieveArray[2], 4);
                      SendData_UART(CopySetPara[PARA_DEVICE_ID],
                                    Fun_Received, 4);
                      // Restart by watchdog: allow delay for modbus
                      // response to be sent
                      //
                      while(1);
                  }
                  else
                  {
                      Fun_Received |= 0x80;
                      Mod_TransmitFrame.Data_Array[0] = 0x03;
                      SendData_UART((uint8_t)CopySetPara[PARA_DEVICE_ID],
                                    Fun_Received, 1);
                      Fun_Received &=~ 0x80;
                      break;
                  }

                  memcpy(Mod_TransmitFrame.Data_Array, &RecieveArray[2], 4);
                  SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received, 4);
                  break;
              }
              else if((Start_Add == 50002) && (NoOfBytes == 4))
              {
                  // Write serial number (lower 32 bits at 50003, upper 32 bits
                  // at 50005). Only allowed when test mode is enabled.
                  //
                  if (!g_testingStatus.TestingModeEnabled)
                  {
                      Fun_Received |= 0x80;
                      Mod_TransmitFrame.Data_Array[0] = 0x03;
                      SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
                      Fun_Received &=~ 0x80;
                      break;
                  }
                  g_ProductInfo.SerialNumber[0] =
                      ((uint32_t)RecieveArray[10]) +
                      ((uint32_t)RecieveArray[9]<<8) +
                      ((uint32_t)RecieveArray[8]<<16)+
                      ((uint32_t)RecieveArray[7]<<24);
                  g_ProductInfo.SerialNumber[1] =
                      ((uint32_t)RecieveArray[14]) +
                      ((uint32_t)RecieveArray[13]<<8) +
                      ((uint32_t)RecieveArray[12]<<16)+
                      ((uint32_t)RecieveArray[11]<<24);
                  EepromWrite(PRODUCT_INFO_LOC,
                              sizeof(struct ProductInfo), EXT_EEPROM,
                              (uint8_t *)&g_ProductInfo);
                  memcpy(Mod_TransmitFrame.Data_Array, &RecieveArray[2], 4);
                  SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,4);
                  break;
              }
              else if((Start_Add == 50008) && (NoOfBytes == 2))
              {
                  // Write tested flag. Only allowed when test mode is enabled.
                  //
                  if (!g_testingStatus.TestingModeEnabled)
                  {
                      Fun_Received |= 0x80;
                      Mod_TransmitFrame.Data_Array[0] = 0x03;
                      SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
                      Fun_Received &=~ 0x80;
                      break;
                  }
                  g_ProductInfo.FunctionallyTestedFlag =
                      ((uint32_t)RecieveArray[10]) +
                      ((uint32_t)RecieveArray[9]<<8) +
                      ((uint32_t)RecieveArray[8]<<16)+
                      ((uint32_t)RecieveArray[7]<<24);
                  EepromWrite(PRODUCT_INFO_LOC,
                              sizeof(struct ProductInfo), EXT_EEPROM,
                              (uint8_t *)&g_ProductInfo);
                  memcpy(Mod_TransmitFrame.Data_Array, &RecieveArray[2], 4);
                  SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,4);
                  break;
              }
              else if((Start_Add == 50010) && (NoOfBytes == 2))
              {
                  // Write calibrated flag. Only allowed when test mode is
                  // enabled AND calibration has completed successfully.
                  //
                  if (!g_testingStatus.TestingModeEnabled ||
                      (FlagDirectCalibration != CALIBRATE_END))
                  {
                      Fun_Received |= 0x80;
                      Mod_TransmitFrame.Data_Array[0] = 0x03;
                      SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
                      Fun_Received &=~ 0x80;
                      break;
                  }
                  g_ProductInfo.CalibratedFlag =
                      ((uint32_t)RecieveArray[10]) +
                      ((uint32_t)RecieveArray[9]<<8) +
                      ((uint32_t)RecieveArray[8]<<16)+
                      ((uint32_t)RecieveArray[7]<<24);
                  EepromWrite(PRODUCT_INFO_LOC,
                              sizeof(struct ProductInfo), EXT_EEPROM,
                              (uint8_t *)&g_ProductInfo);
                  memcpy(Mod_TransmitFrame.Data_Array, &RecieveArray[2], 4);
                  SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,4);
                  break;
              }

              else if((Start_Add == 50012) && (NoOfBytes == 2))
              {
                  // Advance calibration state when calibrations state in a 
                  // wait point. Only allowed when test mode is
                  // enabled.
                  //
                  if (!g_testingStatus.TestingModeEnabled)
                  {
                      Fun_Received |= 0x80;
                      Mod_TransmitFrame.Data_Array[0] = 0x03;
                      SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
                      Fun_Received &=~ 0x80;
                      break;
                  }
                  ModbusAdvanceFlagDirectCalibration = true;
                  memcpy(Mod_TransmitFrame.Data_Array, &RecieveArray[2], 4);
                  SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,4);
                  break;
              }
              else if((Start_Add == 50014) && (NoOfBytes == 2))
              {
                  // Write scratch pad. Temporary 32-bit value held in RAM;
                  // resets to 0 on power-on reset.
                  //
                  g_ScratchPad =
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
                        600,
                        sizeof(g_Alarms),
                        (uint8_t*)(&g_Alarms)
                    },
                    {
                        200,
                        sizeof(g_powerSupplyStatus),
                        (uint8_t*)(&g_powerSupplyStatus)
                    },
                    {
                        300,
                        sizeof(g_voltageHealth),
                        (uint8_t*)(&g_voltageHealth)
                    },
                    {
                        400,
                        sizeof(g_DigOutputs),
                        (uint8_t*)(&g_DigOutputs)
                    },
                    {
                        800,
                        sizeof(g_LoadStatus),
                        (uint8_t*)(&g_LoadStatus)
                    },
                    {
                        900,
                        sizeof(g_FanFaults),
                        (uint8_t*)(&g_FanFaults)
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

                if ((Start_Add >= 30000 && Start_Add < 30000 + sizeof(g_DigOutputs))&&
                     g_testingStatus.TestingModeEnabled)
                {
                    g_DigOutputs.Relays[Start_Add - 30000] = enable;
                }
                else if (Start_Add >= 40000 && Start_Add < 40000 + sizeof(g_testingStatus))
                {
                    g_testingStatus.Status[Start_Add - 40000] = enable;
                }
                else if ((Start_Add == 45000) && g_testingStatus.TestingModeEnabled)
                {
                    memcpy(Mod_TransmitFrame.Data_Array, &RecieveArray[2], 4);
                    SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,4);
                    // Restart by watchdog: Allow delay for modbus response
                    // to be sent
                    //
                    while(1);
                }
                else if ((Start_Add >= 60000 && Start_Add < 60000 + sizeof(g_LedStatus)) &&
                         g_testingStatus.TestingModeEnabled)
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
            case 0x15: /* Write File Record -- FOTA firmware chunks */
            {
                // Frame: [addr][0x15][dataLen][refType][fileHi][fileLo]
                //        [recHi][recLo][lenHi][lenLo][data...][crcLo][crcHi]
                uint8_t dataLen   = RecieveArray[2];
                uint8_t refType   = RecieveArray[3];
                uint16_t fileNo   = ((uint16_t)RecieveArray[4] << 8) | RecieveArray[5];
                uint16_t recordNo = ((uint16_t)RecieveArray[6] << 8) | RecieveArray[7];
                uint16_t recLen   = ((uint16_t)RecieveArray[8] << 8) | RecieveArray[9];

                // Some hardcoded values defined in FOTA flowchart
                // Also, we need records to arrive sequentially
                //
                if (refType != 0x06 || fileNo != 1 || recLen != 100 ||
                    dataLen != 207 || g_fota.status != FOTA_STATUS_READY ||
                    recordNo >= FOTA_MAX_CHUNKS ||
                    recordNo != g_fota.chunksReceived ||
                    (recordNo > 0 && recordNo >= g_fota.totalRecords))
                {
                    Fun_Received |= 0x80;
                    Mod_TransmitFrame.Data_Array[0] = 0x02;
                    SendData_UART((uint8_t)CopySetPara[PARA_DEVICE_ID], Fun_Received, 1);
                    Fun_Received &=~ 0x80;
                    break;
                }

                // Record data starts at RecieveArray[10], 200 bytes total.
                // Record 0: [header 6 bytes][encrypted firmware 194 bytes]
                // Record N: [encrypted firmware 200 bytes]
                //
                // Copy firmware data to local buffer so it's safe from
                // UART ISR overwriting RecieveArray during EepromWrite
                //
                uint8_t fwBuf[FOTA_CHUNK_SIZE];
                uint8_t *recordData = &RecieveArray[10];
                uint16_t fwBytes;

                if (recordNo == 0)
                {
                    // Parse header (little-endian)
                    g_fota.totalRecords = *(uint16_t *)&recordData[0];
                    g_fota.version      = *(uint16_t *)&recordData[2];
                    g_fota.expectedCrc  = *(uint16_t *)&recordData[4];

                    if (g_fota.totalRecords == 0 ||
                        g_fota.totalRecords > FOTA_MAX_CHUNKS)
                    {
                        g_fota.status = FOTA_STATUS_ERROR;
                        Fun_Received |= 0x80;
                        Mod_TransmitFrame.Data_Array[0] = 0x02;
                        SendData_UART((uint8_t)CopySetPara[PARA_DEVICE_ID],
                                      Fun_Received, 1);
                        Fun_Received &=~ 0x80;
                        break;
                    }

                    g_fota.firmwareSize = (uint32_t)g_fota.totalRecords
                                          * FOTA_CHUNK_SIZE - FOTA_HEADER_SIZE;
                    g_fota.prngSeed = (uint16_t)(FOTA_SECRET_KEY
                                      ^ (g_fota.version * (uint16_t)FOTA_SEED_MIXER));
                    g_fota.prngState = g_fota.prngSeed;

                    fwBytes = FOTA_CHUNK_SIZE - FOTA_HEADER_SIZE;
                    memcpy(fwBuf, &recordData[FOTA_HEADER_SIZE], fwBytes);
                }
                else
                {
                    fwBytes = FOTA_CHUNK_SIZE;
                    memcpy(fwBuf, recordData, fwBytes);
                }

                // Decrypt each word and feed to running CRC.
                // Only the decrypted value is used for CRC; the encrypted
                // data goes to EEPROM untouched.
                //
                uint16_t numWords = fwBytes / 2;
                for (uint16_t w = 0; w < numWords; w++)
                {
                    g_fota.prngState = g_fota.prngState * FOTA_PRNG_MUL
                                       + FOTA_PRNG_INC;
                    uint16_t decrypted = *(uint16_t *)&fwBuf[w * 2]
                                         ^ g_fota.prngState;
                    g_fota.runningCrc = CRCCalculationSeeded(
                                            &decrypted, 1, g_fota.runningCrc);
                }

                // Write encrypted firmware (no header) to EEPROM
                uint32_t eepromAddr = FOTA_STAGING_START + g_fota.bytesWritten;
                EepromWrite(eepromAddr, fwBytes, EXT_EEPROM, fwBuf);
                g_fota.chunksReceived++;
                g_fota.bytesWritten += fwBytes;

                // Echo entire request as response (Modbus 0x15 spec)
                uint8_t echoLen = ReceiveLength - 4;
                memcpy(Mod_TransmitFrame.Data_Array, &RecieveArray[2], echoLen);
                SendData_UART((uint8_t)CopySetPara[PARA_DEVICE_ID],
                              Fun_Received, echoLen);
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
UNDONE: Do we need to restart the controller on any setting update?
Check carefully on this.
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
    
    // For now, we are removing the need to set a password to edit any parameter
    // over modbus
    //
    if(error /*|| (!EditParaPassStatus)*/)
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
