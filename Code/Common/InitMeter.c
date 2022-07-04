#include "CommFlagDef.h"
#include "Struct.h"
#include "stm32f37x_flash.h"
#include "extern_includes.h"
#include "math.h"


void ParaLocUpdate(uint16_t WriteAddress);
void StoredDataVerification(void);
void InitilisationError(void);

void NewMeterInit(void)
{
  uint16_t i;
  uint16_t temp; 
  uint16_t Tempointer;
  RESET_WATCH_DOG;

  if((CalibrationCoeff.INIT_DATA1!=METER_INIT_VALUE)||(CalibrationCoeff.INIT_DATA2!=METER_INIT_VALUE))
  {
    //ProtectionReset(); 
    for(i=0;i<64;i++)LcdEpromBuffer[i]=0; 
    for(i=0;i<200;i++) 
    {
      EepromWrite(i*64,64,EXT_EEPROM,LcdEpromBuffer );
      RESET_WATCH_DOG;
    } 
    temp=123;
    //temp=1;
    EepromWrite(PASSWORD_SAV_LOC,2,EXT_EEPROM,(uint8_t *)&temp );
    FillDefaultValue();
    temp=CRCCalculation(CopySetPara,MAX_PARAM_LIMIT);
    CopySetPara[MAX_PARAM_LIMIT]=temp;
    ParaLocUpdate(PROGRAM_DATA_LOC1_START);
    ParaLocUpdate(PROGRAM_DATA_LOC2_START);   
    SaveFlashData();
    CalBuffer.INIT_DATA1=METER_INIT_VALUE;
    CalBuffer.INIT_DATA2=METER_INIT_VALUE;
    WriteFlashData();    
#ifdef  MODEL_DATA_SAVE     
    for(i=0;i<sizeof(StorageBuffer);i++)*((uint8_t *)&StorageBuffer+i)=0;
    StorageBuffer.ImportVarhNeg=0.00001;
    StorageBuffer.ExportVarhNeg=0.00001;
    i=sizeof(StorageBuffer);
    StorageBuffer.StorageCounter=1;
    StorageBuffer.StorageLocation=DATA_SAVE_START_LOC;
    Tempointer=offsetof(struct STORE,StoreCRC);
    StorageBuffer.StoreCRC=CRCCalculation((uint16_t *)&StorageBuffer,Tempointer/2);
    EepromWrite(StorageBuffer.StorageLocation,64,EXT_EEPROM,(uint8_t *)&StorageBuffer );
    EepromWrite(StorageBuffer.StorageLocation+64,sizeof(StorageBuffer)-64,EXT_EEPROM,(uint8_t *)&StorageBuffer+64 );
    
    PowerDownDataSave();
    //SaveTripData();
    SaveOldData();
#endif
    
#ifdef MODEL_RELEASED 
    if(!(FLASH->OBR & FLASH_OBR_RDPRT1))
    {
 
      FLASH_Unlock();
      FLASH_OB_Unlock();
      FLASH_OB_RDPConfig(OB_RDP_Level_1);
      FLASH_OB_UserConfig(OB_IWDG_HW,OB_STOP_RST,OB_STDBY_RST);
      FLASH_OB_Launch();
      FLASH_OB_Lock();
      
    }
#endif   
    
  }
}

void ParaLocUpdate(uint16_t WriteAddress)
{
  uint8_t i=0,k;
  k=MAX_PARAM_LIMIT*2+2;
  while(k)
  {
    EepromWrite(WriteAddress+i*64,64,EXT_EEPROM,(uint8_t *)CopySetPara+i*64 );
    i++;
    if(k>64)k=k-64;
    else k=0;
  }
}


void MeterInit(void)
{ 
  
  NewMeterInit();  
  StoredDataVerification(); 
  SetMeterParameters();

  
}
  
void StoredDataVerification(void)
{   
  
  uint16_t temp1;
  uint8_t temp,k; 
  uint16_t Tempointer;
  temp=0;
  RESET_WATCH_DOG;
  k=MAX_PARAM_LIMIT*2+2; // if k is more than 254 it needs to be read in 2 steps
  I2CRead(PROGRAM_DATA_LOC1_START,k,EXT_EEPROM,(uint8_t *)CopySetPara );
  temp1=CRCCalculation((uint16_t *)CopySetPara,MAX_PARAM_LIMIT+1);  
  if(temp1 !=0)temp=1;
  I2CRead(PROGRAM_DATA_LOC2_START,k,EXT_EEPROM,(uint8_t *)CopySetPara );
  temp1=CRCCalculation((uint16_t *)CopySetPara,MAX_PARAM_LIMIT+1);  
  if(temp1 !=0)temp=temp|0x02;
  if(temp!=0)
  {
      if(temp==1)
      {
         I2CRead(PROGRAM_DATA_LOC2_START,k,EXT_EEPROM,(uint8_t *)CopySetPara );
         ParaLocUpdate(PROGRAM_DATA_LOC1_START);
         RESET_WATCH_DOG;
      }
        
      else if(temp==2) 
      {
        I2CRead(PROGRAM_DATA_LOC1_START,k,EXT_EEPROM,(uint8_t *)CopySetPara );
        ParaLocUpdate(PROGRAM_DATA_LOC2_START);
         RESET_WATCH_DOG;
      }  
      else  InitilisationError(); 
  }
  else I2CRead(PROGRAM_DATA_LOC2_START,k,EXT_EEPROM,(uint8_t *)CopySetPara );
#ifdef MODEL_DATA_SAVE  
  RESET_WATCH_DOG;
  uint16_t TempCrc,i;
  uint32_t TempStorageCounter=0,TempStorageLoc=0;
  k=0;
  // check validity of flash and eeprom all 40 locations.
  for(i=0;i<40;i++)
  {
    I2CRead(DATA_SAVE_START_LOC+i*128,sizeof(StorageBuffer),EXT_EEPROM,LcdEpromBuffer);
    Tempointer=offsetof(struct STORE,StoreCRC);
    TempCrc=CRCCalculation((uint16_t *)&LcdEpromBuffer,(Tempointer/2)+1);
    RESET_WATCH_DOG;;
    if(TempCrc==0)
    {
      if(*(uint32_t *)&LcdEpromBuffer[0]>TempStorageCounter)
      {
        TempStorageCounter=*(uint32_t *)&LcdEpromBuffer[0];
        TempStorageLoc=*(uint32_t *)&LcdEpromBuffer[4];
        k=1;
      }
    }
  }
  // Power Down Recovery
  I2CRead(POWER_DN_SAVE_PAGE,sizeof(StorageBuffer),EXT_EEPROM,LcdEpromBuffer);
  
  temp=offsetof(struct STORE,StoreCRC)/2 +1;
  if((TempCrc=CRCCalculation((uint16_t *)LcdEpromBuffer,temp))==0)
  {
    if(*(uint32_t *)&LcdEpromBuffer[0]>TempStorageCounter)
    {
      I2CRead(POWER_DN_SAVE_PAGE,sizeof(StorageBuffer),EXT_EEPROM,(uint8_t*)&StorageBuffer);
    }
    else
    {
      if(k==0)InitilisationError(); 
      else I2CRead(TempStorageLoc,sizeof(StorageBuffer),EXT_EEPROM,(uint8_t*)&StorageBuffer);
    }
  }
  else
  {
    if(k==0)InitilisationError(); 
    else I2CRead(TempStorageLoc,sizeof(StorageBuffer),EXT_EEPROM,(uint8_t*)&StorageBuffer);
  }
#endif // MODEL_DATA_SAVE
  RESET_WATCH_DOG;;
  DisplayParameterUpdate();
  
} 

void ParaSettingUpdate(void)
{
  
  uint16_t temp; 
  RESET_WATCH_DOG;
  temp=CRCCalculation(CopySetPara,MAX_PARAM_LIMIT);
  CopySetPara[MAX_PARAM_LIMIT]=temp;
  __disable_interrupt();
  __no_operation();
  ParaLocUpdate(PROGRAM_DATA_LOC1_START); 
  ParaLocUpdate(PROGRAM_DATA_LOC2_START);
  SetMeterParameters();
  RESET_WATCH_DOG;
  DisplayParameterUpdate();
  DisplaySetup.DisplayScanPage=0;
  DisplayScrollCounter=10;
  __enable_interrupt();
  RESET_WATCH_DOG;
}

 



void InitilisationError(void)
{  
  
  while(1)
  {
      RESET_WATCH_DOG;
  }
}  

