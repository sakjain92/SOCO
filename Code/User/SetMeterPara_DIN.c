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
  CTPrimary = 1;
  CTSecondary = 1;
  PTPrimary = 1;
  PTSecondary = 1;
  CtRatio = 1;
  PtRatio = 1;
  CtPtRatio = 1;
  EnergyOverflowLimit=ENERGY_OVER_FLOW_LIMIT*1000;
}
  
void DisplayParameterUpdate(void)
{

  uint8_t i;
  for(i=0;i<50;i++)DisplayParameterBuffer[i]= SCREEN_SEQ_3P_MAIN[i];
    
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
