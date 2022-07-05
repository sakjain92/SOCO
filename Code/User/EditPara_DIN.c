/*
******************************************************************************
  * @file    editpara_din.c
  * @author  MRM R&D Team
  * @version V1.0.0
  * @date    07-04-2022
  * @brief   This file contains all the functions definitions 
             for the user settable parameters
******************************************************************************
*/



#include "CommFlagDef.h"
#include "EditPara_DIN.h"
#include "Parameter_DIN.h"
#include "Struct.h"
#include "stm32f37x_flash.h"
#include "extern_includes.h"


extern const uint8_t   SCREEN_SEQ_OLD_DATA[];

void EditAllParameters(uint8_t KeyPressed);
void EditDisplayParameter(void);
void VerifyPassword(uint8_t KeyPressed);
void ViewAllParameters(uint8_t KeyPressed) ;
void ViewSystemParameter(uint8_t KeyPressed);
extern void ParaSettingUpdate(void);
extern void DisplayVariable(uint8_t DV_Decimal,uint8_t DV_NoDigit,uint8_t DV_Row,uint8_t DV_Digit, uint32_t DV_Variable,uint8_t LED_Type);

void CheckKey(void);
void FindInitailCondition(void);
void ScrollParaBlock(uint8_t KeyPressed);
uint16_t PassWordVaraiable,EditFirstRow;
uint8_t SubParaIndex,ChkPassRow;

void CheckKey(void)
{
        
  if(SwFlag & SW_NEW_VALUE_FLAG)
  {
    SwFlag=SwFlag & ~SW_NEW_VALUE_FLAG;
    if(ParaBlockIndex )
    {
      KeyPressedCounter=60;
      if(ParaBlockIndex & 0x80)  // When any one of para to edit selected ParaBlockIndexMSB is set High
      {
        StartSelectingBlock[ParaBlockIndex &0x7F].Function(SwPressed);
      }
      else 
      {
        ScrollParaBlock(SwPressed);
      }
    }
    else if(SwPressed==KEY_EDIT)
    {          
      ParaBlockIndex=0;
      EditFirstRow=0;
      EditIndex=0;
      SubParaIndex=0;
      //SWITCH_OFF_LED17;
      ScrollParaBlock(SwPressed);                 
    }
    else 
    {
  
      if ((SwPressed==KEY_NEXT)&&(FlagDirectCalibration==0))
      {
        DisplaySetup.DisplayScanPage++;
        if(DisplayParameterBuffer[DisplaySetup.DisplayScanPage]==0xff)DisplaySetup.DisplayScanPage=0;
        if(DisplaySetup.DisplayScrollStatus==DISPLAY_SCROLL_OFF)ScrollSaveCounter=0;
        DisplayScrollCounter=10;
        DisplayUpdate();
      }
      else if ((SwPressed==KEY_DEC)&&(FlagDirectCalibration==0))
      {
        if(DisplaySetup.DisplayScanPage>0)DisplaySetup.DisplayScanPage--;
         else DisplaySetup.DisplayScanPage=DisParalast;
        DisplayScrollCounter=10;
        DisplayUpdate();
      }
    }
  }
}

void OutOfEdit(void)
{
    ParaBlockIndex=0;
    EditFirstRow=0;
    EditIndex=0;
    DisplaySetup.DisplayScanPage=0;
    DisplayScrollCounter=10;
    DisplayUpdate();
}

// This is the First entry point 
void ScrollParaBlock(uint8_t KeyPressed)
{
  uint8_t i;
  if((KeyPressed==KEY_EDIT)&& (ParaBlockIndex==0))
  {
    ParaBlockIndex=1;
    for (i=0;i<16;i++)BufferToDisplay[i]=0;
    DisplayString(ROW_TOP,DIGIT_4,StartSelectingBlock[ParaBlockIndex].FirstLine,0);
    
    UpdateDisplay=1;
  }
  else if(ParaBlockIndex)
  {
     if(KeyPressed==KEY_INC)
     {
      ParaBlockIndex++;
      if(*StartSelectingBlock[ParaBlockIndex].FirstLine==0)OutOfEdit();
      
      else
      {
        for (i=0;i<16;i++)BufferToDisplay[i]=0;
        DisplayString(ROW_TOP,DIGIT_4,StartSelectingBlock[ParaBlockIndex].FirstLine,0);
        
        UpdateDisplay=1;
      }
     }
     else if(KeyPressed==KEY_NEXT)
     {
      ParaBlockIndex |=0x80;
      EditFirstRow=0;
      DisplaySetup.DisplayScanPage=0;
      DisplayScrollCounter=10;
      StartSelectingBlock[ParaBlockIndex &0x7F].Function(SwPressed);

     }
     
  }
    
}

// Displays the various parameters in edit mode
// Parameters could be numeric value or
// String
void EditDisplayParameter(void)
{
  
  uint16_t TempEditDisVar;
  uint8_t  *p;
  TempEditDisVar=CopySetPara[EditIndex];
  switch(EditParameters[EditIndex].DecimalPosition)
  {
   
    
    case EDIT_DEC_KVA_TYPE:  
        
        p=(uint8_t *)DisKvaType[TempEditDisVar];
        DisplayString(ROW_TOP,3, p,0);

        break;
    case EDIT_DEC_SYSTEM_CONFIG:  
        
        p=(uint8_t *)DisSysConfig[TempEditDisVar];
        DisplayString(ROW_TOP,3, p,0);
        break; 
        

    case EDIT_DEC_PARITY:  
        
        p=(uint8_t *)DisParity[TempEditDisVar];
        DisplayString(ROW_TOP,3, p,0);
        break; 
    case EDIT_DEC_BAUD_RATE:  
        
        p=(uint8_t *)DisBaudRate[TempEditDisVar];
        DisplayString(ROW_TOP,3, p,0);
        if(TempEditDisVar>3)BufferToDisplay[14]=96;
        
        break;  
   case EDIT_DEC_STOP_BIT:  
        
        p=(uint8_t *)DisStopBits[TempEditDisVar];
        DisplayString(ROW_TOP,3, p,0);
        break;  
        
   case EDIT_DEC_ENERGY_TYPE:  
        
        p=(uint8_t *)DisEneType[TempEditDisVar];
        DisplayString(ROW_TOP,3, p,0);
        break;       
        
 // case EDIT_DEC_TRIP_CONFIG:  
        
       // p=(uint8_t *)DisBaudRate[TempEditDisVar];
      //  DisplayString8x16(DISPLAY_PAGE7,0,p);
   //     break;
   case EDIT_ENDIAN_TYPE:    
        p=(uint8_t *)DisEndTyp[TempEditDisVar];
        DisplayString(ROW_TOP,3, p,0);
        break;

  }
  UpdateDisplay=1;
  
}
 
void ViewOldData (uint8_t KeyPressed)
{
  if(EditFirstRow==0)for(uint8_t i=0;i<50;i++)
  {
    DisplayParameterBuffer[i]= SCREEN_SEQ_OLD_DATA[i];
    DisplaySetup.DisplayScanPage=0;
    EditFirstRow=1;
  }
  else if(KeyPressed==KEY_NEXT)
  {
    DisplaySetup.DisplayScanPage++;
    if(DisplayParameterBuffer[DisplaySetup.DisplayScanPage]==0XFF)
    {
      OutOfEdit();
      DisplayParameterUpdate();      
      
    }
  }
  DisplayUpdate();  
}

void ViewSystemParameter(uint8_t KeyPressed)
{
  uint8_t i;
  if(EditFirstRow==0)
  {
    for (i=0;i<16;i++)BufferToDisplay[i]=0;
    while(EditParameters[EditIndex].DisableType==EDITDIS_SKIP_ALWAYS)EditIndex++;
    //EditFirstRow=1;
    if(SubParaIndex == 0)
    {
        DisplayString(ROW_TOP,DIGIT_4,(uint8_t *)EditParameters[EditIndex].DesArrayTop,0);
        SubParaIndex++;
    }
    else
    {
      if(EditParameters[EditIndex].DecimalPosition<10)FindInitailCondition();    
      else EditDisplayParameter();
      SubParaIndex = 0;
      EditFirstRow=1;
    }
    UpdateDisplay=1;
    ViewAllParameters(0);
  }
  else ViewAllParameters(KeyPressed);

}
void ViewAllParameters(uint8_t KeyPressed)   
{
  uint16_t i;
  if((KeyPressed==KEY_NEXT)&& (EditIndex==StartSelectingBlock[ParaBlockIndex & 0x7f].EndIndex)&& (SubParaIndex == 1))OutOfEdit();
  else
  {
    if(KeyPressed==KEY_NEXT)
    {
      if(SubParaIndex==0)SubParaIndex++;   //******
      else 
      {
        SubParaIndex=0;
        if(EditParameters[EditIndex].DataWithDecUnit==PARA_ONLY_DATA)EditIndex +=3;
        else EditIndex++;
       }
      
      while(EditParameters[EditIndex].DisableType==EDITDIS_SKIP_ALWAYS)
      {
        EditIndex++;
        if(EditIndex==StartSelectingBlock[ParaBlockIndex & 0x7f].EndIndex)
        {
            OutOfEdit();
            return;
        }
      }
      for (i=0;i<16;i++)BufferToDisplay[i]=0;
      if(SubParaIndex == 0)
        DisplayString(ROW_TOP,DIGIT_4,(uint8_t *)EditParameters[EditIndex].DesArrayTop,0);
      else
      {
        if(EditParameters[EditIndex].DecimalPosition<10)FindInitailCondition();    
        else EditDisplayParameter();
      }
      UpdateDisplay=1;
    }
  }   
}

void EditSystemParameter(uint8_t KeyPressed)
{
  uint8_t i;
   
   if(!(InterruptFlag  & PASSWORD_VERIFIED))
   {
     VerifyPassword(KeyPressed);
   }
   else 
   {
        
     if(EditFirstRow==0)
     {
        for (i=0;i<16;i++)BufferToDisplay[i]=0;
        while(EditParameters[EditIndex].DisableType==EDITDIS_SKIP_ALWAYS)EditIndex++;
        EditFirstRow=1;
        DisplayString(ROW_TOP,DIGIT_4,(uint8_t *)EditParameters[EditIndex].DesArrayTop,0);
        UpdateDisplay=1;
        ViewAllParameters(0);
        EditAllParameters(0);
     }
       
     else EditAllParameters(KeyPressed);
   }
}
void FindInitailCondition(void)
{
  uint8_t i=0,j=0;
  uint32_t EditTempData;
  
  if(EditParameters[EditIndex].DataWithDecUnit==PARA_ONLY_DATA)
  {
    j=CopySetPara[EditIndex+2]; // Kilo digit
    i=CopySetPara[EditIndex+1]; // decimal data
    EditTempData=CopySetPara[EditIndex];
  }
  else if(EditParameters[EditIndex].DataWithDecUnit==PARA_WITH_DEC)
  {
    j=CopySetPara[EditIndex+1]; // Kilo digit
    i=CopySetPara[EditIndex]; // decimal data
     EditTempData=CopySetPara[EditIndex-1];
  }
  else if(EditParameters[EditIndex].DataWithDecUnit==PARA_WITH_UNIT)
  {
    j=CopySetPara[EditIndex]; // Kilo digit
    i=CopySetPara[EditIndex-1]; // decimal data
    EditTempData=CopySetPara[EditIndex-2];
  }
  else 
  {
     EditTempData=CopySetPara[EditIndex];
     i=EditParameters[EditIndex].DecimalPosition;
  }
  //for (k=0;k<5;k++)BufferToDisplay[k]=0;
  if(j)BufferToDisplay[13] |=1;
  else if(j==2)BufferToDisplay[11] |=1;
  else if(j==3)
  {
    BufferToDisplay[11] |=1;
    BufferToDisplay[13] |=1;
  }
  DisplayVariable(i,EditParameters[EditIndex].ParaMeterLength,ROW_TOP,DIGIT_1,EditTempData,0);  
  UpdateDisplay=1;  
  
}

void UpdateEditSettings(void)
{
   uint8_t i;
   for ( i=0;i<16;i++)BufferToDisplay[i]=0;
   UpdateDisplay=1; 
   Delay1Msec12Mhz(2);
   StorageBuffer.EnergyDispUnit=EnergyDisplayUnit;
   SetMeterParameters();
   if(EnergyDisplayUnit==MAX_ENERGY_SET_GWh)
   {
     DisplayString(ROW_TOP,DIGIT_6,(uint8_t *)DIS_CT_PT,0);
     UpdateDisplay=1;
     while(SwPressed!=KEY_INC)
     {
       RESET_WATCH_DOG;
     }
     NVIC_SystemReset();
   }
   else
   {
  #ifdef MODEL_DATA_SAVE
      uint8_t i,k=0;
      
      InterruptFlag |=INT_DATA_SAVING_EEPROM;
      __no_operation();
      I2CRead(PROGRAM_DATA_LOC1_START,20,EXT_EEPROM,LcdEpromBuffer );    
      InterruptFlag &=~INT_DATA_SAVING_EEPROM;
      for(i=0;i<9;i++)
      { 
        if(CopySetPara[i] != *((uint16_t *)(LcdEpromBuffer+i*2)))k=1;
      }
       
      if(k==1)
      {
        RESET_WATCH_DOG;
        //__disable_interrupt();
       //__no_operation();
        SaveOldData();
        //SaveTripData();
        DisplaySetup.DisplayScanPage=0;
        DisplaySetup.DisplayScrollStatus=0;
        EepromWrite(SCROLL_LOCK_LOC,2,EXT_EEPROM,(uint8_t *)&DisplaySetup.DisplayScrollStatus );
        DisplayDisabled();
       // __enable_interrupt();
      }     
  #endif
      ParaSettingUpdate();
   }
    OutOfEdit();
    InterruptFlag &=~(PASSWORD_CHECK_ON+PASSWORD_VERIFIED+PASSWORD_FOR_VIEW);
}
  
void EditAllParameters(uint8_t KeyPressed) 
{
  uint16_t i;
  int32_t EditTempData;
  EditTempData=CopySetPara[EditParameters[EditIndex].VariableIndex];
  if((KeyPressed==KEY_NEXT)&& (EditIndex==StartSelectingBlock[ParaBlockIndex & 0x7f].EndIndex) && (SubParaIndex == 1))
  {
    UpdateEditSettings();
    SubParaIndex=0;
  }
  else
  {

    if(KeyPressed==KEY_NEXT)
    {
      
      if(SubParaIndex==0)SubParaIndex++;   //******
      else
      {
          EditIndex++;
          SubParaIndex=0;
      }
      while(EditParameters[EditIndex].DisableType==EDITDIS_SKIP_ALWAYS)
      {
        EditIndex++;
        if(EditIndex==StartSelectingBlock[ParaBlockIndex & 0x7f].EndIndex)
        {
          
          UpdateEditSettings();
          return;
        }
      }
      for (i=0;i<16;i++)BufferToDisplay[i]=0;
      if(SubParaIndex == 0)
            DisplayString(ROW_TOP,DIGIT_4,(uint8_t *)EditParameters[EditIndex].DesArrayTop,0);
      else
      {
        if(EditParameters[EditIndex].DecimalPosition<10)FindInitailCondition();    
        else EditDisplayParameter();
      }
    }
    else if((KeyPressed==KEY_INC)&& (SubParaIndex==1))     //*******
    {
      if(EditParameters[EditIndex].DataWithDecUnit==PARA_WITH_DEC)
      {
        if(CopySetPara[EditIndex+1]==1)BufferToDisplay[13] |=1;              // Kilo digit
        else BufferToDisplay[13] &=~1; 
        EditTempData=CopySetPara[EditIndex];
        BufferToDisplay[EditTempData+10]&=~0x100;
        EditTempData++;
        if(EditTempData>EditParameters[EditIndex].MaxValue)EditTempData=EditParameters[EditIndex].MinValue;
        CopySetPara[EditIndex]=EditTempData;
        BufferToDisplay[EditTempData+10]|=0x100;
      }
      else if(EditParameters[EditIndex].DataWithDecUnit==PARA_WITH_UNIT)
      {
        if(CopySetPara[EditIndex]==1)
        {
          CopySetPara[EditIndex]=0;
          BufferToDisplay[13] &=~1;  
        }
        else
        {
          CopySetPara[EditIndex]=1;
          BufferToDisplay[13] |=1;
        }  
      }
      else 
      {
         for (i=8;i<16;i++)BufferToDisplay[i]=0; 
         EditTempData=CopySetPara[EditIndex];
         EditTempData +=SwValue;
         if(EditTempData>=EditParameters[EditIndex].MaxValue)EditTempData=EditParameters[EditIndex].MaxValue;
         CopySetPara[EditIndex]=EditTempData;
         //for (k=0;k<5;k++)BufferToDisplay[k]=0;
         if(EditParameters[EditIndex].DecimalPosition<10)
         {
           if(EditParameters[EditIndex].DataWithDecUnit)
           {
            if(CopySetPara[EditIndex+2])BufferToDisplay[13] |=1; // Kilo digit
            i=CopySetPara[EditIndex+1]; // decimal data
           }
           else i=0;
           DisplayVariable(i,EditParameters[EditIndex].ParaMeterLength,ROW_TOP,DIGIT_1,EditTempData,0);   
         }
         else EditDisplayParameter();
      }
      
    }
    else if((KeyPressed==KEY_DEC)&& (SubParaIndex==1))
    {
      if(EditParameters[EditIndex].DataWithDecUnit==PARA_WITH_DEC)
      {
        if(CopySetPara[EditIndex+1]==1)BufferToDisplay[13] |=1;
        else BufferToDisplay[13] &=~1;                   // Kilo digit
        EditTempData=CopySetPara[EditIndex];
        BufferToDisplay[EditTempData+10]&=~0x100;
        if(EditTempData==EditParameters[EditIndex].MinValue)EditTempData=EditParameters[EditIndex].MaxValue;
        else EditTempData--;
        CopySetPara[EditIndex]=EditTempData;
        BufferToDisplay[EditTempData+10]|=0x100;
      }
      else if(EditParameters[EditIndex].DataWithDecUnit==PARA_WITH_UNIT)
      {
        if(CopySetPara[EditIndex]==1)
        {
          CopySetPara[EditIndex]=0;
          BufferToDisplay[13] &=~1; 
        }
        else
        {
          BufferToDisplay[13] |=1;
          CopySetPara[EditIndex] =1;
        }  
      }
      else 
      {
         for (i=8;i<16;i++)BufferToDisplay[i]=0; 
         EditTempData=CopySetPara[EditIndex];
         EditTempData -=SwValue;
         if(EditTempData<=EditParameters[EditIndex].MinValue)EditTempData=EditParameters[EditIndex].MinValue;
         CopySetPara[EditIndex]=EditTempData;
         if(EditParameters[EditIndex].DecimalPosition<10)
         {
           if(EditParameters[EditIndex].DataWithDecUnit)
           {
             if(CopySetPara[EditIndex+2])BufferToDisplay[13] |=1; // Kilo digit
             i=CopySetPara[EditIndex+1]; // decimal data
           }
           else i=0;
           DisplayVariable(i,EditParameters[EditIndex].ParaMeterLength,ROW_TOP,DIGIT_1,EditTempData,0);   
         }
         else EditDisplayParameter();
      }
    }
    UpdateDisplay=1;
  }  
      
}

   
void VerifyPassword(uint8_t KeyPressed)
{
  uint8_t i;
  
  for (i=0;i<16;i++)BufferToDisplay[i]=0;
  if(!(InterruptFlag & PASSWORD_CHECK_ON))
  {
   
    PassWordVaraiable=1;
    DisplayString(ROW_TOP,DIGIT_6,(uint8_t *)DIS_CUR_PASS,0);
    InterruptFlag |=PASSWORD_CHECK_ON;
    UpdateDisplay=1;
  }
  else
  {
    if((KeyPressed==KEY_NEXT) && (ChkPassRow>=1)&& (SubParaIndex == 1))
    {
     
        if(PassWordVaraiable==MeterPassword)
        {
        
          InterruptFlag |=PASSWORD_VERIFIED;
          for (i=0;i<16;i++)BufferToDisplay[i]=0;
          DisplayString(ROW_TOP,DIGIT_4,(uint8_t *)DIS_OK,0);
          UpdateDisplay=1;
          
        }
    
        else
        {
          InterruptFlag &=~(PASSWORD_CHECK_ON+PASSWORD_VERIFIED+PASSWORD_FOR_VIEW);
          for (i=0;i<16;i++)BufferToDisplay[i]=0;
          DisplayString(ROW_TOP,DIGIT_4,(uint8_t *)DIS_NOK,0);
          UpdateDisplay=1;
          ParaBlockIndex=0;
          EditIndex=0;
          DisplaySetup.DisplayScanPage=0;
          DisplayScrollCounter=10;
        }
        SubParaIndex = 0;
        EditFirstRow=0;
   }
  else
  {
    if(KeyPressed==KEY_NEXT)
    {
        DisplayVariable(0,3,ROW_TOP,DIGIT_1,PassWordVaraiable,0);
        ChkPassRow++;
        SubParaIndex=1;
        UpdateDisplay=1;
    }
    else if((KeyPressed==KEY_INC)&&(PassWordVaraiable<999)&& (SubParaIndex == 1))
    {
        PassWordVaraiable+=SwValue;
        if(PassWordVaraiable>999)PassWordVaraiable=999;
        DisplayVariable(0,3,ROW_TOP,DIGIT_1,PassWordVaraiable,0);
        UpdateDisplay=1;
    }
    else if((KeyPressed==KEY_DEC)&&(PassWordVaraiable>0)&& (SubParaIndex == 1))
    {
        if(PassWordVaraiable>=(SwValue+1))PassWordVaraiable -=SwValue;
        else PassWordVaraiable=1;
        DisplayVariable(0,3,ROW_TOP,DIGIT_1,PassWordVaraiable,0);
        UpdateDisplay=1;
    }
    
  
  }
  
  }

}



         
void EditResetPassword(uint8_t KeyPressed)
{
   uint8_t i;
   if(!(InterruptFlag  & PASSWORD_VERIFIED))
   {
     VerifyPassword(KeyPressed);
     EditFirstRow=0;
   }
   else 
   {
      
      for (i=0;i<16;i++)BufferToDisplay[i]=0;
      if(SubParaIndex == 0)
      {
        DisplayString(ROW_TOP,DIGIT_6,(uint8_t *)DIS_SET_NEW,0);
        UpdateDisplay=1;
        SubParaIndex++;
      }
      else
      {
        if((KeyPressed==KEY_INC)&&(PassWordVaraiable<999))
        {
          PassWordVaraiable+=SwValue;
          if(PassWordVaraiable>999)PassWordVaraiable=999;
        }
        else if((KeyPressed==KEY_DEC)&&(PassWordVaraiable>0))
        {
          if(PassWordVaraiable>=SwValue)PassWordVaraiable -=SwValue;
          else PassWordVaraiable=0;
        }
        DisplayVariable(0,3,ROW_TOP,DIGIT_1,PassWordVaraiable,0);
        UpdateDisplay=1;
      }
       if((KeyPressed==KEY_NEXT)&&(EditFirstRow>=2))
        {
          MeterPassword=PassWordVaraiable;
          InterruptFlag |=INT_DATA_SAVING_EEPROM;
          __no_operation();
          EepromWrite(PASSWORD_SAV_LOC,2,EXT_EEPROM,(uint8_t *)&PassWordVaraiable );
          InterruptFlag &=~INT_DATA_SAVING_EEPROM;
          for (i=0;i<16;i++)BufferToDisplay[i]=0;
          DisplayString(ROW_TOP,DIGIT_4,(uint8_t *)DIS_UPDT,0);
          UpdateDisplay=1;
          InterruptFlag &=~(PASSWORD_CHECK_ON+PASSWORD_VERIFIED);
          ParaBlockIndex=0;
          EditFirstRow=0;
          EditIndex=0;
          SubParaIndex=0;
          DisplaySetup.DisplayScanPage=0;
          DisplayScrollCounter=10;
        }
        
      EditFirstRow++;
   }
}
      
        
 
             
 
void FillDefaultValue(void)
{
  uint8_t i;
  for(i=0;i<MAX_PARAM_LIMIT;i++) CopySetPara[i]=EditParameters[i].DefaultValue;
}
