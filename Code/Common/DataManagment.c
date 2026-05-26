/*
******************************************************************************
  * @file    DataManagement.c
  * @author  MRM R&D Team
  * @version V1.0.0
  * @date    14-06-2022
  * @brief   This file contains all the functions definitions 
             for the meter Data management
******************************************************************************
*/


#include "stm32f37x.h"
#include <stdio.h>
#include "CommFlagDef.h"
#include "Struct.h"
#include "stm32f37x_flash.h"
#include "extern_includes.h"

COMPILE_ASSERT(sizeof(StorageBuffer) <= MAX_DATA_SAVE_SIZE);

/*
Inf: CRC-16 with caller-supplied initial value. Can be called repeatedly
     to compute a running CRC across multiple buffers by passing the
     previous return value as the seed for the next call.
Inp: str    - pointer to uint16_t data array
     length - number of uint16_t words to process
     seed   - initial CRC value (use 0xFFFF for the first call)
Ret: Updated CRC-16 value
*/
uint16_t CRCCalculationSeeded(uint16_t *str, uint8_t length, uint16_t seed)
{
  uint8_t i, j;
  uint16_t check_sum = seed;
  for (i = 0; i < length; i++)
  {
    check_sum ^= (uint16_t)str[i];
    for (j = 0; j < 8; j++)
    {
      if (check_sum & 1)
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
Inf: CRC-16 over a single buffer, starting from the standard initial
     value of 0xFFFF. Wrapper around CRCCalculationSeeded().
Inp: str    - pointer to uint16_t data array
     length - number of uint16_t words to process
Ret: CRC-16 value
*/
uint16_t CRCCalculation(uint16_t *str, uint8_t length)
{
  return CRCCalculationSeeded(str, length, 0xFFFF);
}
#ifdef MODEL_DATA_SAVE
void  SaveOldData(void)
{
  uint32_t Temp;
  uint16_t Tempointer;
  RESET_WATCH_DOG;
  PowerFailCounter=0;
  DataSaveCounter=0;
  CalCtPtRatio();
  PowerDownDataSave();
  Temp=StorageBuffer.StorageCounter;
  InterruptFlag &= ~INT_POWER_OK;
  EepromWrite(OLD_DATA_LOC,sizeof(StorageBuffer),EXT_EEPROM,(uint8_t *)&StorageBuffer );  // save old data
  for(uint16_t i=0;i<sizeof(StorageBuffer);i++)*((uint8_t *)&StorageBuffer+i)=0;
  StorageBuffer.ImportVarhNeg=0.00001;
  StorageBuffer.ExportVarhNeg=0.00001;
  StorageBuffer.SolarImportVarhNeg=0.00001;
  StorageBuffer.SolarExportVarhNeg=0.00001;
  StorageBuffer.StorageCounter=Temp+1;
  StorageBuffer.StorageLocation=DATA_SAVE_START_LOC;
  Tempointer=offsetof(struct STORE,StoreCRC);
  StorageBuffer.StoreCRC=CRCCalculation((uint16_t *)&StorageBuffer,Tempointer/2);
  EepromWrite(StorageBuffer.StorageLocation,sizeof(StorageBuffer),EXT_EEPROM,(uint8_t *)&StorageBuffer );
}
//extern void Delay1Msec12Mhz( uint32_t Delay);
void PowerDownDataSave(void)
{
  uint16_t Tempointer;
  RESET_WATCH_DOG;
  PowerFailCounter=0;
  DataSaveCounter=0;
  StorageBuffer.StorageCounter++;
  InterruptFlag |=INT_DATA_SAVING_EEPROM;
  Tempointer=offsetof(struct STORE,StoreCRC);
  StorageBuffer.StoreCRC=CRCCalculation((uint16_t *)&StorageBuffer,Tempointer/2);
  InterruptFlag &= ~INT_POWER_OK;
  EepromWrite(POWER_DN_SAVE_PAGE,sizeof(StorageBuffer),EXT_EEPROM,(uint8_t *)&StorageBuffer );
  Delay1Msec12Mhz(6);
  InterruptFlag &=~INT_DATA_SAVING_EEPROM;
}

/*
void SaveTripData(void)
{
  RESET_WATCH_DOG;
  EepromWrite(TRIP_DATA_LOC,sizeof(StorageBuffer),EXT_EEPROM,(uint8_t *)&StorageBuffer );
  TripData= StorageBuffer;
}*/
  
#endif //MODEL_DATA_SAVE
