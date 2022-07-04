/*
******************************************************************************
  * @file    setmeterpara.c
  * @author  MRM R&D Team
  * @version V1.0.0
  * @date    07-04-2022
  * @brief   This file contains all the functions definitions 
             for the initialize meter parameters
******************************************************************************
*/


#include "CommFlagDef.h"
#include "Struct.h"
#include "stm32f37x_flash.h"
#include "extern_includes.h"
#include "math.h"

float PTPrimary, CTPrimary,PTSecondary,CTSecondary;

extern const uint8_t SCREEN_SEQ_3P_MAIN[];
extern const uint8_t SCREEN_SEQ_1P_MAIN[];

void SetMeterParameters(void)
{
  CalCtPtRatio();
  I2CRead(SCROLL_LOCK_LOC,2,EXT_EEPROM,(uint8_t *)&DisplaySetup.DisplayScrollStatus );
  I2CRead(PASSWORD_SAV_LOC,2,EXT_EEPROM,(uint8_t *)&MeterPassword );
 
  //I2CRead(TRIP_DATA_LOC,sizeof(StorageBuffer),EXT_EEPROM,(uint8_t *)&TripData );
  I2CRead(OLD_DATA_LOC,sizeof(StorageBuffer),EXT_EEPROM,(uint8_t *)&OldData );  // read old data
#ifdef MODEL_RS485
  InitUart(CopySetPara[PARA_BAUD_RATE],CopySetPara[PARA_PARITY],CopySetPara[PARA_STOP_BIT]);
#endif
}

void CalCtPtRatio(void)
{
  float MaxPower;
   CTPrimary= (float)CopySetPara[PARA_CT_PRIMARY];
  if(CopySetPara[PARA_CT_PRI_UNIT]==1)CTPrimary =CTPrimary*1000;  // uint is kilo
  if(CopySetPara[PARA_CT_PRIMARY_DEC]==2)CTPrimary =CTPrimary/100;  // uint is kilo
  else if(CopySetPara[PARA_CT_PRIMARY_DEC]==1)CTPrimary =CTPrimary/10;  // uint is kilo4
  CTSecondary=(float)CopySetPara[PARA_CT_SECONDARY];
  CtRatio=CTPrimary/CTSecondary;
  
  PTPrimary= (float)CopySetPara[PARA_PT_PRIMARY];
  if(CopySetPara[PARA_PT_PRI_UNIT]==1)PTPrimary =PTPrimary*1000;  // uint is kilo
  if(CopySetPara[PARA_PT_PRIMARY_DEC]==2)PTPrimary =PTPrimary/100;  // uint is kilo
  else if(CopySetPara[PARA_PT_PRIMARY_DEC]==1)PTPrimary =PTPrimary/10;  // uint is kilo
  PTSecondary=(float)CopySetPara[PARA_PT_SECONDARY];
  PtRatio=PTPrimary/PTSecondary;
  CtPtRatio=CtRatio*PtRatio;
  MaxPower=sqrt(3)*CTPrimary*PTPrimary; 
  if(MaxPower>100e6)
  {
    EnergyDisplayUnit=MAX_ENERGY_SET_GWh;
  }
  else if(MaxPower>100e3)
  {
    EnergyDisplayUnit=MAX_ENERGY_SET_MWh;
  }
  else 
  {
    EnergyDisplayUnit=MAX_ENERGY_SET_KWh;
  }
  switch(EnergyDisplayUnit)
  {
  case MAX_ENERGY_SET_Wh:
    EnergyOverflowLimit=ENERGY_OVER_FLOW_LIMIT;
    break;
  case MAX_ENERGY_SET_KWh:
    EnergyOverflowLimit=ENERGY_OVER_FLOW_LIMIT*1000;
    break;
  case MAX_ENERGY_SET_MWh: 
    EnergyOverflowLimit=ENERGY_OVER_FLOW_LIMIT*1e6;
    break;  
  case MAX_ENERGY_SET_GWh:
    EnergyOverflowLimit=ENERGY_OVER_FLOW_LIMIT*1e9;
    break;  
  }
}
  
void DisplayParameterUpdate(void)
{

  uint8_t i;
  if(CopySetPara[PARA_SYSTEM_CONFIG]!=SYSTEM_CONFIG_1P)
 
    for(i=0;i<50;i++)DisplayParameterBuffer[i]= SCREEN_SEQ_3P_MAIN[i];
    
  else
    for(i=0;i<50;i++)DisplayParameterBuffer[i]= SCREEN_SEQ_1P_MAIN[i];  
   i=0;
 DisParalast=0;
 while( DisplayParameterBuffer[i]!=0xff)
 {
   DisParalast++;
   i++;
 }
 
 DisParalast--;   
  
}
void UpdateDisplayIndex(void)
{
    if(DisplaySetup.DisplayScrollStatus==DISPLAY_SCROLL_ON)
    {
      if(DisplayScrollCounter)DisplayScrollCounter--;    
      if(DisplayScrollCounter==0)
      {
        DisplayScrollCounter=10;
        if(dispStartCnt == 0)
        {
          DisplaySetup.DisplayScanPage = 0;
          dispStartCnt = 1;
        }
        else DisplaySetup.DisplayScanPage++;
        if(DisplayParameterBuffer[DisplaySetup.DisplayScanPage]==0xff)DisplaySetup.DisplayScanPage=0;

      }
    }
}
