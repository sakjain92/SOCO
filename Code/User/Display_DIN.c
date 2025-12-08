/*
******************************************************************************
  * @file    display_din.c
  * @author  MRM R&D Team
  * @version V1.0.0
  * @date    07-06-2022
  * @brief   This file contains all the functions definitions 
             for the display
******************************************************************************
*/


#include "stm32f37x.h"
#include <stdio.h>
#include "DIN_Struct.h"
#include "CommFlagDef.h"
#include "stm32f37x_flash.h"
#include "Display_DIN.h"
#include "fonts_DIN.h"
#include "extern_includes.h"

void DisplayPF(float TempFloat,uint8_t ScrIndex, uint8_t DisIndex );
void DisplayOldEnergy(double Data,uint8_t ScrIndex, uint8_t DisIndex,uint8_t led );
void DisplayEnergy(double Data,uint8_t ScrIndex, uint8_t DisIndex,uint8_t led );
void DisplayString(uint8_t DV_Row,uint8_t DV_Digit, uint8_t *DV_String,uint8_t LED_Type);
void DisplayVariable(uint8_t DV_Decimal,uint8_t DV_NoDigit,uint8_t DV_Row,uint8_t DV_Digit, uint32_t DV_Variable,uint8_t LED_Type);
uint32_t  ResolveDataForDisplay(float Data,uint8_t ScrIndex, uint8_t DisIndex );
void DisplayCurrentVol(float TempFloat,uint8_t ScrIndex, uint8_t DisIndex );
void DisplayPower(float TempFloat,uint8_t ScrIndex, uint8_t DisIndex );
void DisplayRunHr(uint32_t Data,uint8_t ScrIndex, uint8_t DisIndex );
void SetVersionDisplay(void);
void ConvertToBCB(int32_t Data);
void DisplayInterruption(uint32_t Data,uint8_t ScrIndex, uint8_t DisIndex );


uint8_t ResolveDisplayDecimal;  

extern const uint16_t CHAR_NEU_14_SEG[];
extern const uint16_t CHAR_ALFA_14_SEG[];
extern  const uint8_t CHAR_NEU_7_SEG[];
extern  const uint8_t CHAR_ALFA_7_SEG[];
uint16_t BufferForDisplay[24],ComInUsed;
void LedProcess(uint8_t LED_Type);

void DisplayUpdate(void)
{
  
  uint8_t ScreenIndex,DisplayIndex,i;
  for (i=0;i<16;i++)BufferToDisplay[i]=0;
  ScreenIndex=0;
 
  DisplayIndex=DisplayParameterBuffer[DisplaySetup.DisplayScanPage];
  while ( Screen[DisplayIndex][ScreenIndex].DataType !=0xff)
  {
    switch(Screen[DisplayIndex][ScreenIndex].DataType)
    {
      case DATA_TYPE_VARIABLE:
           if(Screen[DisplayIndex][ScreenIndex].DecPos<11)
           {
             DisplayVariable(Screen[DisplayIndex][ScreenIndex].DecPos,
                             Screen[DisplayIndex][ScreenIndex].NumOfDigits,
                             ROW_TOP,
                             Screen[DisplayIndex][ScreenIndex].DigitAdd,
                             (uint32_t)*Screen[DisplayIndex][ScreenIndex].Variable,Screen[DisplayIndex][ScreenIndex].LedType);
           }
           else
           {
               switch(Screen[DisplayIndex][ScreenIndex].DecPos)
               {
               
               case DIS_VAL_CURRENT:
                 DisplayCurrentVol(*Screen[DisplayIndex][ScreenIndex].Variable*CtRatio,ScreenIndex,DisplayIndex);
                 break;
               case DIS_VAL_VOLTAGE:
                 DisplayCurrentVol(*Screen[DisplayIndex][ScreenIndex].Variable*PtRatio,ScreenIndex,DisplayIndex);
                 break;
               case DIS_VAL_PF:
                 DisplayPF(*Screen[DisplayIndex][ScreenIndex].Variable,ScreenIndex,DisplayIndex);;
                 break;  
               case DIS_VAL_ANGLE:
                 DisplayVariable(DECIMAL_DIGIT_4,
                             Screen[DisplayIndex][ScreenIndex].NumOfDigits,
                             ROW_TOP,
                             Screen[DisplayIndex][ScreenIndex].DigitAdd,
                             (uint32_t)(*Screen[DisplayIndex][ScreenIndex].Variable*10),Screen[DisplayIndex][ScreenIndex].LedType);
                 break;   
               case DIS_VAL_POWER:
                 DisplayPower(*Screen[DisplayIndex][ScreenIndex].Variable,ScreenIndex,DisplayIndex);
                 break;
               case DIS_VAL_POW_VA:
                 DisplayPower(*Screen[DisplayIndex][ScreenIndex].Variable,ScreenIndex,DisplayIndex);
                 break;
               case DIS_VAL_POW_VAR:
                 DisplayPower(*Screen[DisplayIndex][ScreenIndex].Variable,ScreenIndex,DisplayIndex);
                 break;  
               case DIS_RUN_HR_IMP:
                 DisplayRunHr(StorageBuffer.RunHourImport,ScreenIndex,DisplayIndex);
                 break; 
               case DIS_LH_IMP:
                 DisplayRunHr(StorageBuffer.LoadHourImport,ScreenIndex,DisplayIndex);
                 break;
               case DIS_W_IMP:
                 DisplayEnergy(StorageBuffer.ImportWh,ScreenIndex,DisplayIndex,Screen[DisplayIndex][ScreenIndex].LedType);
                 break;  
               case DIS_VA_IMP:    
                 DisplayEnergy(StorageBuffer.ImportVAh,ScreenIndex,DisplayIndex,Screen[DisplayIndex][ScreenIndex].LedType);
                 break;  
               case DIS_VAR_POS_IMP:
                 DisplayEnergy(StorageBuffer.ImportVarhPos,ScreenIndex,DisplayIndex,Screen[DisplayIndex][ScreenIndex].LedType);
                 break;  
               case DIS_VAR_NEG_IMP:
                 DisplayEnergy(-StorageBuffer.ImportVarhNeg,ScreenIndex,DisplayIndex,Screen[DisplayIndex][ScreenIndex].LedType);
                 break;    
               case DIS_INT_IMP:
                 DisplayInterruption(StorageBuffer.ImportInterruptions,ScreenIndex,DisplayIndex);
                 break;  
 // Old Data
                 
                 case DIS_RUN_HR_IMP_OLD:
                 DisplayRunHr(OldData.RunHourImport,ScreenIndex,DisplayIndex);
                 break; 
               case DIS_LH_IMP_OLD:
                 DisplayRunHr(OldData.LoadHourImport,ScreenIndex,DisplayIndex);
                 break;
               case DIS_W_IMP_OLD:
                    
                 DisplayOldEnergy(OldData.ImportWh,ScreenIndex,DisplayIndex,Screen[DisplayIndex][ScreenIndex].LedType);
                 break;  
               case DIS_VA_IMP_OLD:
                   
                 DisplayOldEnergy(OldData.ImportVAh,ScreenIndex,DisplayIndex,Screen[DisplayIndex][ScreenIndex].LedType);
                 break;  
               case DIS_VAR_POS_IMP_OLD:
                 
                 DisplayOldEnergy(OldData.ImportVarhPos,ScreenIndex,DisplayIndex,Screen[DisplayIndex][ScreenIndex].LedType);
                 break;  
               case DIS_VAR_NEG_IMP_OLD:
                 
                 DisplayOldEnergy(-OldData.ImportVarhNeg,ScreenIndex,DisplayIndex,Screen[DisplayIndex][ScreenIndex].LedType);
                 break;    
               case DIS_INT_IMP_OLD:
                 DisplayInterruption(OldData.ImportInterruptions,ScreenIndex,DisplayIndex);
                 break;    
                 
               case DIS_VAL_FREQ:
                 DisplayVariable(DECIMAL_DIGIT_5,
                             Screen[DisplayIndex][ScreenIndex].NumOfDigits,
                             ROW_TOP,
                             Screen[DisplayIndex][ScreenIndex].DigitAdd,
                             (uint32_t)(*Screen[DisplayIndex][ScreenIndex].Variable*100),Screen[DisplayIndex][ScreenIndex].LedType);
              
                 break;   
               case DIS_VAL_UNB:
                 DisplayVariable(DECIMAL_DIGIT_4,
                             Screen[DisplayIndex][ScreenIndex].NumOfDigits,
                             ROW_TOP,
                             Screen[DisplayIndex][ScreenIndex].DigitAdd,
                             (uint32_t)(*Screen[DisplayIndex][ScreenIndex].Variable*10),Screen[DisplayIndex][ScreenIndex].LedType);
                 break;
                     
               }
           }         
           
           ScreenIndex++;
           break;
  
      case DATA_TYPE_STRING:
         DisplayString(ROW_TOP,
                         Screen[DisplayIndex][ScreenIndex].DigitAdd,
                         (uint8_t *)Screen[DisplayIndex][ScreenIndex].IconStringArray,Screen[DisplayIndex][ScreenIndex].LedType);
           ScreenIndex++;
           break;
     
      
    }
  }  
  
  UpdateDisplay=1;
  
  
}


const uint32_t POWER_ARRAY[11]={0,1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};

void ConvertToBCB(int32_t Data)
{
  uint8_t i,j;
  if(Data<0)
  {
    Data=-Data;
    Dec2DCBArray[10]='-';
  }
  j=0;
  for(i=10;i>0;i--)
  {
    while(Data>=POWER_ARRAY[i])
    {
      Data=Data-POWER_ARRAY[i];
      j++;
    }
    Dec2DCBArray[10-i]=j;
    j=0;
  }
}

void LedProcess(uint8_t LED_Type)
{
  switch(LED_Type)
  {
    case LED_OFF:
      SWITCH_OFF_LED2_R;
      SWITCH_OFF_LED2_G;
      SWITCH_OFF_LED2_B;
      break;
    case LED_R:
      SWITCH_ON_LED2_R;
      SWITCH_OFF_LED2_G;
      SWITCH_OFF_LED2_B;
      break;
    case LED_Y:
      SWITCH_ON_LED2_R;
      SWITCH_ON_LED2_G;
      SWITCH_OFF_LED2_B;
      break;
    case LED_B:
      SWITCH_OFF_LED2_R;
      SWITCH_OFF_LED2_G;
      SWITCH_ON_LED2_B;
      break;
    default:
      SWITCH_OFF_LED2_R;
      SWITCH_OFF_LED2_G;
      SWITCH_OFF_LED2_B;
    
  }
  
}

void DisplayVariable(uint8_t DV_Decimal,uint8_t DV_NoDigit,uint8_t DV_Row,uint8_t DV_Digit, uint32_t DV_Variable,uint8_t LED_Type)
{
   uint16_t *StringPointer,i,j;
   ConvertToBCB(DV_Variable);
   
   //LED Routine
   LedProcess(LED_Type);
   StringPointer=BufferToDisplay+10+DV_Digit;
   //if(DV_Row==ROW_TOP) StringPointer=BufferToDisplay+5+DV_Digit;
   //else if(DV_Row==ROW_MID) StringPointer=BufferToDisplay+10+DV_Digit;
   //else StringPointer=BufferToDisplay+DV_Digit;
   
   j=0;
   for(i=0;i<DV_NoDigit;i++)
   {
     if(DV_Digit==DIGIT_5)
     {
      *StringPointer|=CHAR_NEU_7_SEG[Dec2DCBArray[9-i]];
      *StringPointer|=(256*CHAR_NEU_7_SEG[Dec2DCBArray[9-i-1]]);
      break;
     }
     else *StringPointer|=CHAR_NEU_14_SEG[Dec2DCBArray[9-i]];
     StringPointer++;
     j++;
     DV_Digit++;
   }
   
   if((DV_Decimal>=DECIMAL_DIGIT_2)&&(DV_Decimal<=DECIMAL_DIGIT_6))
   {
     StringPointer=BufferToDisplay;
     StringPointer +=10;
     //if(DV_Row==ROW_TOP) StringPointer +=5;
     //else if(DV_Row==ROW_MID)StringPointer +=10;
    
     
     if(DV_Decimal==DECIMAL_DIGIT_6)*(StringPointer+4)|=0x100;
     else if (DV_Decimal==DECIMAL_DIGIT_5)*(StringPointer+4)|=1;
     else *(StringPointer+DV_Decimal)|=0x100;
     
   } 
   
}


uint32_t  ResolveDataForDisplay(float Data,uint8_t ScrIndex, uint8_t DisIndex )
{
  // decimal place has to be resolved here
  uint8_t K;
  K=Screen[DisIndex][ScrIndex].DigitAdd;
  if(Data>1000)
  {
    ResolveDisplayDecimal=0;
  }
  else if(Data>100)
  {
    Data =Data*10;
    ResolveDisplayDecimal=K+1;
  }
  else if(Data>10)
  {
    Data =Data*100;
    ResolveDisplayDecimal=K+2;
  }
  else 
  {
    Data =Data*1000;
    ResolveDisplayDecimal=K+3;
  }
  return (uint32_t)Data;
}

void DisplayCurrentVol(float TempFloat,uint8_t ScrIndex, uint8_t DisIndex )
{
  
  uint32_t TempVariable;
  LcdEpromBuffer[0]=0;
  if(TempFloat>=1000)
  {
   TempFloat=TempFloat/1000; 
   LcdEpromBuffer[0]='K';
  }
  TempVariable=ResolveDataForDisplay(TempFloat,ScrIndex,DisIndex );
  DisplayVariable(ResolveDisplayDecimal,Screen[DisIndex][ScrIndex].NumOfDigits,
               ROW_TOP,
               Screen[DisIndex][ScrIndex].DigitAdd,
               TempVariable,Screen[DisIndex][ScrIndex].LedType);
  if( LcdEpromBuffer[0]=='K')
  {
   
    BufferToDisplay[13] |=1;
        
  }
}

  
void DisplayPower(float TempFloat,uint8_t ScrIndex, uint8_t DisIndex )
{
  uint8_t TempFlag=0;
  uint32_t TempVariable;
  TempFloat=TempFloat*CtPtRatio;
  if(TempFloat<0)
  {
    TempFloat=-TempFloat;
    TempFlag |=0x01;   // -ve sign
  }
  if(TempFloat>=1e9)
  {
   TempFloat=TempFloat/1E9; 
   TempFlag |=0x02; // display k 
   TempFlag |=0x04; //  display  M 
  }
  else if(TempFloat>=1e6)
  {
   TempFloat=TempFloat/1E6; 
   TempFlag |=0x04; //  display  M 
  }
  else if(TempFloat>=1e3)
  {
   TempFloat=TempFloat/1E3; 
   TempFlag |=0x02; // display k 
  }
  TempVariable=ResolveDataForDisplay(TempFloat,ScrIndex,DisIndex );
  DisplayVariable(ResolveDisplayDecimal,Screen[DisIndex][ScrIndex].NumOfDigits,
               ROW_TOP,
               Screen[DisIndex][ScrIndex].DigitAdd,
               TempVariable,Screen[DisIndex][ScrIndex].LedType);
  
  if( TempFlag & 0x01)
  {
    BufferToDisplay[12] |=1;
    
  }
  if(TempFlag & 0x02)
  {
    BufferToDisplay[13] |=1;
  }
  if(TempFlag & 0x04)
  {
    BufferToDisplay[11] |=1;
    
  }
  
  
}

  
void DisplayPF(float TempFloat,uint8_t ScrIndex, uint8_t DisIndex )
{
  uint8_t TempFlag=0;
  uint8_t K;
  K=Screen[DisIndex][ScrIndex].DigitAdd;
  
  if(TempFloat<0)
  {
    TempFloat=-TempFloat;
    TempFlag =0x01;   // -ve sign
  }
  DisplayVariable(K+3,Screen[DisIndex][ScrIndex].NumOfDigits,
               ROW_TOP,
               Screen[DisIndex][ScrIndex].DigitAdd,
                (uint32_t)(TempFloat*1000),Screen[DisIndex][ScrIndex].LedType);
   
  if( TempFlag)
  {
    BufferToDisplay[12] |=1;
   
  }
}

    

void DisplayRunHr(uint32_t Data,uint8_t ScrIndex, uint8_t DisIndex )
{
  uint32_t TempInt;
  uint32_t Sec,Min;
  TempInt=Data/60;
  Sec=Data-(TempInt*60);
  Data=TempInt/60;
  Min=TempInt-(Data*60);
  
  DisplayVariable(0,6,ROW_TOP,DIGIT_1,Data,0);
  //DisplayVariable(0,2,ROW_BOT,DIGIT_1,Sec);
  //DisplayVariable(0,2,ROW_TOP,DIGIT_1,Sec);
  Sec=0;
  while(Min>=10)
  {
    Min -=10;
    Sec++;
  }
  
  //DisplayVariable(0,1,ROW_BOT,DIGIT_4,Min);
  //DisplayVariable(0,1,ROW_BOT,DIGIT_5,Sec);
  //DisplayString(ROW_BOT,DIGIT_3,(uint8_t*)StrDash);
 // DisplayVariable(0,1,ROW_TOP,DIGIT_4,Min);
  //DisplayVariable(0,1,ROW_TOP,DIGIT_5,Sec);
  //DisplayString(ROW_TOP,DIGIT_3,(uint8_t*)StrDash);
  
  BufferToDisplay[4] &=0x00ff; 
  //BufferToDisplay[14] &=0x00ff; 
}


void DisplayInterruption(uint32_t Data,uint8_t ScrIndex, uint8_t DisIndex )
{
  //DisplayVariable(0,6,ROW_MID,DIGIT_1,Data);
  DisplayVariable(0,6,ROW_TOP,DIGIT_1,Data,0);
}

void DisplayTHD(float TempFloat,uint8_t ScrIndex, uint8_t DisIndex )
{
  if(TempFloat>9999)TempFloat=0;
  DisplayVariable(DECIMAL_DIGIT_5,Screen[DisIndex][ScrIndex].NumOfDigits,
                  ROW_TOP, Screen[DisIndex][ScrIndex].DigitAdd,
                  (uint32_t)TempFloat,Screen[DisIndex][ScrIndex].LedType);
}

  
void DisplayEnergy(double Data,uint8_t ScrIndex, uint8_t DisIndex, uint8_t led )
{
 // double TempData;
  uint8_t TempFlag=0;
  uint32_t Temp32;
  if( Data<0)
  {
    Data=-1*Data;
    TempFlag |=0x01;
  }
  if(Data>1e12)
  {
    Data=Data/1e6;
    TempFlag |=0x04; //  display  M 
    TempFlag |=0x02; // display k    
  }
  if(Data>1e9)
  {
    Data=Data/1e3;
    TempFlag |=0x04; //  display  M 
  }
  else TempFlag |=0x02; // display k 
  
  Temp32=(uint32_t)(Data/1000);
  //DisplayVariable(0,6,ROW_MID,DIGIT_1,(uint32_t)(Temp32));
  DisplayVariable(0,6,ROW_TOP,DIGIT_1,(uint32_t)(Temp32),led);
  Temp32=(uint32_t)(Data-(float)Temp32*1e3);
  //DisplayVariable(DECIMAL_DIGIT_4,3,ROW_BOT,DIGIT_1,Temp32);
  
  if( TempFlag & 0X01)
  {
    BufferToDisplay[12] |=1;
   }
  if(TempFlag & 0x02)
  {
    BufferToDisplay[13] |=1;
   }
  if(TempFlag & 0x04)
  {
   BufferToDisplay[11] |=1;
   
  }
}


void DisplayOldEnergy(double Data,uint8_t ScrIndex, uint8_t DisIndex,uint8_t led )
{
 // double TempData;
  uint8_t TempFlag=0;
  uint32_t Temp32;
  if( Data<0)
  {
    Data=-1*Data;
    TempFlag |=0x01;
  }

  if(Data>1e12)
  {
    Data=Data/1e6;
    TempFlag |=0x04; //  display  M 
    TempFlag |=0x02; // display k  
  }
  if(Data>1e9)
  {
    Data=Data/1e3;
    TempFlag |=0x04; //  display  M 
  }
  else TempFlag |=0x02; // display k 
  
  Temp32=(uint32_t)(Data/1000);
  //DisplayVariable(0,6,ROW_MID,DIGIT_1,(uint32_t)(Temp32));
  DisplayVariable(0,6,ROW_TOP,DIGIT_1,(uint32_t)(Temp32),led);
  Temp32=(uint32_t)(Data-(float)Temp32*1e3);
  //DisplayVariable(DECIMAL_DIGIT_4,3,ROW_BOT,DIGIT_1,Temp32);
  if( TempFlag & 0X01)
  {
    BufferToDisplay[12] |=1;
    
  }
  if(TempFlag & 0x02)
  {
    BufferToDisplay[13] |=1;
    
  }
  if(TempFlag & 0x04)
  {
    BufferToDisplay[11] |=1;
    
  }
  

}



void DisplayString(uint8_t DV_Row,uint8_t DV_Digit, uint8_t *DV_String,uint8_t LED_Type)
{
  uint16_t *StringPointer,CountCharacter=0;
  
  //LED Routine
   LedProcess(LED_Type);
   
  StringPointer=BufferToDisplay+10+DV_Digit;
   //if(DV_Row==ROW_TOP) StringPointer=BufferToDisplay+5+DV_Digit;
   //else if(DV_Row==ROW_MID) StringPointer=BufferToDisplay+10+DV_Digit;
   //else StringPointer=BufferToDisplay+DV_Digit;
   
   
   while(*(DV_String+CountCharacter)!=0) CountCharacter++; 
   if(CountCharacter>4)
   {
      //StringPointer--;
      if(*DV_String !=' ')
        *StringPointer|=256*CHAR_ALFA_7_SEG[*DV_String-0X41];
      DV_String++;
      if(*DV_String !=' ')
       *StringPointer|=(CHAR_ALFA_7_SEG[*DV_String-0X41]);
      StringPointer--;
      DV_String++;
      
   }
   while(*DV_String!=0)  
   {
     
     if(*(uint8_t *)DV_String==' ')
     {
       *StringPointer |=0;
     }
     else 
     {
        *StringPointer |=CHAR_ALFA_14_SEG[*(uint8_t *)DV_String-0x30];
     }
     StringPointer--;
     DV_String++;
   }
}


void CheckAutoScroll(void)
{
  if(SwPressed==KEY_NEXT)ScrollChangeCounter++;
  else ScrollChangeCounter=0;
  if(ScrollChangeCounter==2)
  {
   ScrollChangeCounter=0;
   for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
   DisplayScrollCounter=5;
   if(DisplaySetup.DisplayScrollStatus==DISPLAY_SCROLL_ON)
   {
     DisplaySetup.DisplayScrollStatus=DISPLAY_SCROLL_OFF;
     DisplayString(ROW_TOP,DIGIT_6,(uint8_t *)DIS_SCRLOFF,0);
     

   }
   else
   {
     DisplaySetup.DisplayScrollStatus=DISPLAY_SCROLL_ON;
     DisplayString(ROW_TOP,DIGIT_6,(uint8_t *)DIS_SCRLON,0);
   }
   InterruptFlag |=INT_DATA_SAVING_EEPROM;
   __no_operation();
   EepromWrite(SCROLL_LOCK_LOC,2,EXT_EEPROM,(uint8_t *)&DisplaySetup.DisplayScrollStatus );
   InterruptFlag &=~INT_DATA_SAVING_EEPROM;
   UpdateDisplay=1;
   DisplayScrollCounter=10;
  }
}

void DisplaySetHighPF(void)
{
    for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
    DisplayString(ROW_TOP,DIGIT_5,(uint8_t *)DIS_SETHIGHPF,0);
    UpdateDisplay=1;
}
void DisplayCalHighPF(void)
{
  for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
  DisplayString(ROW_TOP,DIGIT_5,(uint8_t *)DIS_HIGHCALPF,0);
  UpdateDisplay=1;
  DisplayScrollCounter=10; 

}
void DisplaySetMidPF(void)
{
    for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
    DisplayString(ROW_TOP,DIGIT_5,(uint8_t *)DIS_SETMIDPF,0);
    UpdateDisplay=1;
    //DisplayScrollCounter=10; 
}
void DisplayCalMidPF(void)
{
  for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
  DisplayString(ROW_TOP,DIGIT_5,(uint8_t *)DIS_MIDCALPF,0);
  UpdateDisplay=1;
  DisplayScrollCounter=10; 

}

void DisplaySetLowPF(void)
{
    for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
    DisplayString(ROW_TOP,DIGIT_5,(uint8_t *)DIS_SETLOWPF,0);
    UpdateDisplay=1;
    //DisplayScrollCounter=10; 
}

void DisplayCalLowPF(void)
{
  for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
  DisplayString(ROW_TOP,DIGIT_5,(uint8_t *)DIS_LOWCALPF,0);
  UpdateDisplay=1;
  DisplayScrollCounter=10; 
}

void DisplayInputX(uint8_t idx)
{
  for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
  DisplayString(ROW_TOP,DIGIT_4,(uint8_t *)"In",0);
  DisplayVariable(0, 1, ROW_TOP, DIGIT_1, idx, LED_OFF);
  UpdateDisplay=1;
  DisplayScrollCounter=10; 
}

void DisplayOutputX(uint8_t idx)
{
  for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
  DisplayString(ROW_TOP,DIGIT_4,(uint8_t *)"Out",0);
  DisplayVariable(0, 1, ROW_TOP, DIGIT_1, idx, LED_OFF);
  UpdateDisplay=1;
  DisplayScrollCounter=10; 
}

void DisplayCalHighVI(void)
{
  for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
  DisplayString(ROW_TOP,DIGIT_5,(uint8_t *)DIS_CALHIGHVI,0);
  UpdateDisplay=1;
  DisplayScrollCounter=10; 
}
  
void DisplayCalMidVI(void)
{
  for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
  DisplayString(ROW_TOP,DIGIT_5,(uint8_t *)DIS_CALMIDVI,0);
  UpdateDisplay=1;
  DisplayScrollCounter=10; 
}
void DisplayCalLowVI(void)
{
  for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
  DisplayString(ROW_TOP,DIGIT_5,(uint8_t *)DIS_CALLOWVI,0);
  UpdateDisplay=1;
  DisplayScrollCounter=10; 
}

void SetInitialDisplay(void)
{
    for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
    DisplayString(ROW_TOP,DIGIT_4,(uint8_t *)DIS_SOCO,0);
    UpdateDisplay=1;

}
void SetVersionDisplay(void)
{
    for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
    DisplayVariable(DECIMAL_DIGIT_3,3,ROW_TOP,DIGIT_1,VERSION_NO,0);
    BufferToDisplay[13]=3096;
    UpdateDisplay=1;

}


// UNDONE: This doesn't show up on the screen due to while(1)
//
void DisplayImproperSettings(void)
{
  for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
  DisplayString(ROW_TOP,DIGIT_6,(uint8_t *)DIS_CHECK_IN,0);
  CalibrationGapCounter=0;
  FlagDirectCalibration=0;
  SwPressed=0;
  UpdateDisplay=1;
  while(1)RESET_WATCH_DOG;

}

// UNDONE: This doesn't show up on the screen due to while (1)
//
void DisplayDoneCal(void)
{
  for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
  DisplayString(ROW_TOP,DIGIT_6,(uint8_t *)DIS_CAL_DONE,0);
  while(1)RESET_WATCH_DOG;    
}

#pragma optimize=none
void InterruptDisplayRefresh(void)
{
    uint16_t i,Data;
    if(UpdateDisplay==1)
    {
      for(i=0;i<24;i++)BufferForDisplay[i]=BufferToDisplay[i];
      UpdateDisplay=0;
    }
    ComInUsed++;
    if(ComInUsed==5)ComInUsed=0; 
      
    if((BufferForDisplay[12]==64512)&&(BufferForDisplay[13]==64512))
    i=2;
    Data=BufferForDisplay[ComInUsed];
    Data =~Data;
    for(i=0;i<16;i++)
    {
      if(Data & 0x01)DIS_DATA_OUT_HIGH; 
      else DIS_DATA_OUT_LOW;
      
      SHIFT_REGISTER_CLOCK_HIGH;
      Data=Data>>1;
      SHIFT_REGISTER_CLOCK_LOW;
      
    }
    Data=BufferForDisplay[ComInUsed+5];
     Data =~Data;
    for(i=0;i<16;i++)
    {
      if(Data & 0x01)DIS_DATA_OUT_HIGH ; 
      else DIS_DATA_OUT_LOW;
      SHIFT_REGISTER_CLOCK_HIGH;
      Data=Data>>1;
      SHIFT_REGISTER_CLOCK_LOW;
    }
    Data=BufferForDisplay[ComInUsed+10];
     Data =~Data;
    for(i=0;i<16;i++)
    {
      if(Data & 0x01)DIS_DATA_OUT_HIGH ; 
      else DIS_DATA_OUT_LOW;
      
      SHIFT_REGISTER_CLOCK_HIGH;
      Data=Data>>1;
      SHIFT_REGISTER_CLOCK_LOW; 
    }
    DISP_COM1_DIS;
    DISP_COM2_DIS;
    DISP_COM3_DIS;
    DISP_COM4_DIS;
    DISP_COM5_DIS;
    OUTPUT_REGISTER_CLOCK_HIGH;
    OUTPUT_REGISTER_CLOCK_LOW;  
    if(ComInUsed==0)DISP_COM1_ENB;
    else if(ComInUsed==1)DISP_COM2_ENB;
    else if(ComInUsed==2)DISP_COM3_ENB;
    else if(ComInUsed==3)DISP_COM4_ENB;
    else DISP_COM5_ENB;
    DISP_SR_ON;


    
}



void DisplayDisabled(void)
{
  for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
  DISP_SR_OFF;
  //UpdateDisplay=1;
};
  
void StartCheckCTPolarity(void)
{
  
  for (uint8_t i=0;i<16;i++)BufferToDisplay[i]=0;
  if(InterruptFlag  & INT_R_PHASE_REV)
    DisplayString(ROW_TOP,DIGIT_3,(uint8_t *)DIS_INV_R,LED_R);
  if(InterruptFlag  & INT_Y_PHASE_REV);
    DisplayString(ROW_TOP,DIGIT_2,(uint8_t *)DIS_INV_Y,LED_Y);
  if(InterruptFlag  & INT_B_PHASE_REV);
    DisplayString(ROW_TOP,DIGIT_1,(uint8_t *)DIS_INV_B,LED_B);
  UpdateDisplay=1;
  DisplayScrollCounter=10; 
}
