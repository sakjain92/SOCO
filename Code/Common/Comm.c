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
      (InstantPara.TotalPowerB<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_L_LOWER_LIMIT)) 
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

      
   }
   else DisplayImproperSettings();
 } 
  
 if(FlagDirectCalibration==CALIBRATE_L_VI)
 {
   if(
     (InstantPara.CurrentR >I_L_LOWER_LIMIT) && (InstantPara.CurrentR<I_L_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_L_LOWER_LIMIT)&& (InstantPara.CurrentY<I_L_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_L_LOWER_LIMIT) && (InstantPara.CurrentB<I_L_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_L_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_L_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_L_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_L_LOWER_LIMIT))  
   {

      ProtectionReset();              
      __disable_interrupt();
      __no_operation();
      RESET_WATCH_DOG;
      SaveFlashData();  
            
      CalBuffer.IR_LOW_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_LOW*CalBuffer.IR_LOW_GAIN)/CalCurR);
     
      CalBuffer.IY_LOW_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_LOW*(float)CalBuffer.IY_LOW_GAIN)/CalCurY);
      
      CalBuffer.IB_LOW_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_LOW*(float)CalBuffer.IB_LOW_GAIN)/CalCurB);
      
   }
   else DisplayImproperSettings();
 } 
  
 else if(FlagDirectCalibration==CALIBRATE_M_PF)
 {
   if(
     (InstantPara.CurrentR >I_M_LOWER_LIMIT) && (InstantPara.CurrentR<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_M_LOWER_LIMIT)&& (InstantPara.CurrentY<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_M_LOWER_LIMIT) && (InstantPara.CurrentB<I_M_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_M_LOWER_LIMIT))  
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

      
   }
   else DisplayImproperSettings();
 } 
  
 else if(FlagDirectCalibration==CALIBRATE_M_VI)
 {
   if(
     (InstantPara.CurrentR >I_M_LOWER_LIMIT) && (InstantPara.CurrentR<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentY >I_M_LOWER_LIMIT)&& (InstantPara.CurrentY<I_M_UPPER_LIMIT)&&
      (InstantPara.CurrentB >I_M_LOWER_LIMIT) && (InstantPara.CurrentB<I_M_UPPER_LIMIT)&&
      (InstantPara.TotalPowerR<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerR>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerY<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerY>PF_POWER_M_LOWER_LIMIT)&&
      (InstantPara.TotalPowerB<PF_POWER_M_UPPER_LIMIT)&&(InstantPara.TotalPowerB>PF_POWER_M_LOWER_LIMIT))  
   {

      ProtectionReset();              
      __disable_interrupt();
      __no_operation();
      RESET_WATCH_DOG;
      SaveFlashData();  
            
      CalBuffer.IR_MID_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_MID*CalBuffer.IR_MID_GAIN)/CalCurR);
     
      CalBuffer.IY_MID_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_MID*(float)CalBuffer.IY_MID_GAIN)/CalCurY);
      
      CalBuffer.IB_MID_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_MID*(float)CalBuffer.IB_MID_GAIN)/CalCurB);
      
   }
   else DisplayImproperSettings();
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
      (InstantPara.VolBSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolBSolar<VOLTAGE_HIGHER_LIMIT)
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

   }
   else DisplayImproperSettings();
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
      (InstantPara.VolBSolar >VOLTAGE_LOWER_LIMIT) && (InstantPara.VolBSolar<VOLTAGE_HIGHER_LIMIT)
    )  

   {

      ProtectionReset();              
      __disable_interrupt();
      __no_operation();
      RESET_WATCH_DOG;
      SaveFlashData();  
      CalBuffer.VR_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH*CalBuffer.VR_240_GAIN)/CalVolR);
      
      CalBuffer.VY_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH*CalBuffer.VY_240_GAIN)/CalVolY);
      
      CalBuffer.VB_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH*CalBuffer.VB_240_GAIN)/CalVolB);
      
      CalBuffer.IR_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH*CalBuffer.IR_HIGH_GAIN)/CalCurR);
     
      CalBuffer.IY_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH*(float)CalBuffer.IY_HIGH_GAIN)/CalCurY);
      
      CalBuffer.IB_HIGH_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_CURRENT_SETTING_HIGH*(float)CalBuffer.IB_HIGH_GAIN)/CalCurB);

      CalBuffer.VR_SOLAR_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH*CalBuffer.VR_SOLAR_240_GAIN)/CalVolRSolar);
      
      CalBuffer.VY_SOLAR_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH*CalBuffer.VY_SOLAR_240_GAIN)/CalVolYSolar);
      
      CalBuffer.VB_SOLAR_240_GAIN=((NO_OF_CAL_ACCUMULATION_VI*CAL_VOLTAGE_SETTING_HIGH*CalBuffer.VB_SOLAR_240_GAIN)/CalVolBSolar);
   }
      
   else DisplayImproperSettings();
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
}

/*
Inf: Calibrate Power Factor 
Inp: Error, gain pointer, value pointer
Ret: None
*/  
float W,PhasenRadian,D,B,A;
void CalPF(float Error, float * CalGainBufferPointer,float * CalBetaBufferPointer)
{
    
    
    PhasenRadian=acos(0.5*(1+Error))-(3.14159265/3);
    W=(3.14159265)/32;  // 2*freq*PI/sampling freq
    D=PhasenRadian/W;
    B=-((1-2*D)*cos(W)-sqrt(((1-2*D)*(1-2*D)*cos(W)*cos(W))+(4*D*(1-D))))/(2*(1-D));
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
            
            
            if((Start_Add >= 1000)&&(Start_Add < 1000+(InstPara_NewAll*2))&&(!(Start_Add%2)))
            {
               Start_Add -= 1000;
               AvailableByte = InstPara_NewAll-Start_Add/2;
               ArrayIndex = Start_Add/2;
            }
            else if((Start_Add >=2000 )&&(Start_Add < 2000+(InstPara_THD*2))&&(!(Start_Add%2)))
            {
               Start_Add -= 2000;
               AvailableByte = InstPara_THD-Start_Add/2;
               ArrayIndex = InstPara_NewAll + Start_Add/2;
            }
            else if((Start_Add >=2100 )&&(Start_Add < 2100+(InstPara_ENERGYIMPORT*2))&&(!(Start_Add%2)))
            {
               Start_Add -= 2100;
               AvailableByte = InstPara_ENERGYIMPORT-Start_Add/2;
               ArrayIndex = InstPara_NewAll+ InstPara_THD + Start_Add/2;
            }
            else if((Start_Add >=2200 )&&(Start_Add < 2200+(InstPara_ENERGYEXPORT*2))&&(!(Start_Add%2)))
            {
               Start_Add -= 2200;
               AvailableByte = InstPara_ENERGYEXPORT-Start_Add/2;
               ArrayIndex = InstPara_NewAll + InstPara_THD + InstPara_ENERGYIMPORT + Start_Add/2;
            }
            else if((Start_Add >=2300 )&&(Start_Add < 2300+(InstPara_ENERGYOLD*2))&&(!(Start_Add%2)))
            {
               Start_Add -= 2300;
               AvailableByte = InstPara_ENERGYOLD-Start_Add/2;
               ArrayIndex = InstPara_NewAll + InstPara_THD + InstPara_ENERGYIMPORT + InstPara_ENERGYEXPORT + Start_Add/2;
            }
            else if((Start_Add >=2400 )&&(Start_Add <2400+(InstPara_ENERGYTrip*2))&&(!(Start_Add%2)))
            {
               Start_Add -= 2400;
               AvailableByte = InstPara_ENERGYTrip-Start_Add/2;
               ArrayIndex = InstPara_NewAll + InstPara_THD + InstPara_ENERGYIMPORT + InstPara_ENERGYEXPORT + InstPara_ENERGYOLD + Start_Add/2;
            }
            else if((Start_Add >=2500 )&&(Start_Add < 2500+(InstPara_TotalRMS*2))&&(!(Start_Add%2)))
            {
               Start_Add -= 2500;
               AvailableByte = InstPara_TotalRMS-Start_Add/2;
               ArrayIndex = InstPara_NewAll + InstPara_THD + InstPara_ENERGYIMPORT + InstPara_ENERGYEXPORT 
                          + InstPara_ENERGYOLD + InstPara_ENERGYTrip + Start_Add/2;
            }
            else if((Start_Add >=2600 )&&(Start_Add < 2600+(InstPara_RphaseRMS*2))&&(!(Start_Add%2)))
            {
               Start_Add -= 2600;
               AvailableByte = InstPara_RphaseRMS-Start_Add/2;
               ArrayIndex = InstPara_NewAll + InstPara_THD + InstPara_ENERGYIMPORT + InstPara_ENERGYEXPORT 
                          + InstPara_ENERGYOLD + InstPara_ENERGYTrip +InstPara_TotalRMS+ Start_Add/2;
            }
            else if((Start_Add >=2700 )&&(Start_Add < 2700+(InstPara_YphaseRMS*2))&&(!(Start_Add%2)))
            {
               Start_Add -= 2700;
               AvailableByte = InstPara_YphaseRMS-Start_Add/2;
               ArrayIndex = InstPara_NewAll + InstPara_THD + InstPara_ENERGYIMPORT + InstPara_ENERGYEXPORT 
                          + InstPara_ENERGYOLD + InstPara_ENERGYTrip +InstPara_TotalRMS+InstPara_RphaseRMS+ Start_Add/2;
            }
            else if((Start_Add >=2800 )&&(Start_Add < 2800+(InstPara_BphaseRMS*2))&&(!(Start_Add%2)))
            {
               Start_Add -= 2800;
               AvailableByte = InstPara_BphaseRMS-Start_Add/2;
               ArrayIndex = InstPara_NewAll + InstPara_THD + InstPara_ENERGYIMPORT + InstPara_ENERGYEXPORT 
                          + InstPara_ENERGYOLD + InstPara_ENERGYTrip +InstPara_TotalRMS+InstPara_RphaseRMS+InstPara_YphaseRMS+ Start_Add/2;
            }
            else if((Start_Add >=3000 )&&(Start_Add < 3000+(InstPara_Demand*2))&&(!(Start_Add%2)))
            {
               Start_Add -= 3000;
               AvailableByte = InstPara_Demand-Start_Add/2;
               ArrayIndex = InstPara_NewAll + InstPara_THD + InstPara_ENERGYIMPORT + InstPara_ENERGYEXPORT + InstPara_BphaseRMS
                          + InstPara_ENERGYOLD + InstPara_ENERGYTrip +InstPara_TotalRMS+InstPara_RphaseRMS+InstPara_YphaseRMS+ Start_Add/2;
            }
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            else if((Start_Add >=5000 )&&(Start_Add < 5000+(MAX_PARAM_LIMIT*2))&&(!(Start_Add%2)))
            {
               Start_Add -= 5000;
               AvailableByte = MAX_PARAM_LIMIT-Start_Add/2;
               ArrayIndex = InstPara_NewAll + InstPara_THD + InstPara_ENERGYIMPORT + InstPara_ENERGYEXPORT + InstPara_BphaseRMS
                          + InstPara_ENERGYOLD + InstPara_ENERGYTrip +InstPara_TotalRMS+InstPara_RphaseRMS+InstPara_YphaseRMS+ InstPara_Demand+Start_Add/2;
            }
            
            ////// Exception Response for Illegal Data Address //////
            else    
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
          case 0x10:
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
              }
              
              else if((Start_Add >= 5000)&&(Start_Add <= 5000+(MAX_PARAM_LIMIT*2))&&(!(Start_Add %2)))
              {
                Start_Add -= 5000;
                //Start_Add +=2; // To remove system configuration
                ModbusUpdateParameter(Start_Add/2,NoOfBytes);
              }
              else  ///// Illegal Data Address //////
              {

                Fun_Received |= 0x80;
                Mod_TransmitFrame.Data_Array[0] = 0x02;
                SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
                Fun_Received &=~ 0x80;
               
              }
              break;
            case 0x2 /*MODBUS_READ_INPUT_STATUS_CODE*/:
            {
                Start_Add_High = RecieveArray[2];
                Start_Add_Low = RecieveArray[3];
                Start_Add = (Start_Add_Low) + (Start_Add_High<<8);
                NoOfBytes_High = RecieveArray[4];
                NoOfBytes_Low = RecieveArray[5];
                NoOfBytes = (NoOfBytes_Low) + (NoOfBytes_High<<8);
                
                uint32_t endReg = (uint32_t)(Start_Add) + (uint32_t)(NoOfBytes) - 1;
                uint8_t maxRegs = sizeof(g_DigInputs);
                if (endReg >= maxRegs || NoOfBytes == 0)
                {
                    Fun_Received |= 0x80;
                    Mod_TransmitFrame.Data_Array[0] = 0x02;
                    SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
                    break;
                }

                // We need to shift certain bits if Start_Add != 0
                //
                uint8_t sourceBuffer[64] = {0};
                uint8_t destBuffer[64] = {0};

                uint8_t* source = (uint8_t*)(&g_DigInputs);
                for (int i = 0; i < sizeof(g_DigInputs); i++)
                {
                    if (source[i])
                    {
                        int sourceByteIdx = i / 8;
                        int sourceBitIdx = i - 8 * sourceByteIdx;
                        sourceBuffer[sourceByteIdx] |= (1 << sourceBitIdx);
                    }
                }
                for (int bitIdx = 0; bitIdx < NoOfBytes; bitIdx++)
                {
                    int sourceBitIdx = bitIdx + Start_Add;
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
                break;
            }

       ///// Exception Response for Illegal Function //////    
          default:
            if((Fun_Received != 0x03))
            {
              Fun_Received |= 0x80;
              Mod_TransmitFrame.Data_Array[0] = 0x01;
              SendData_UART((uint8_t)CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
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
      SWITCH_ON_LED_COMM;
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
      //SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
    }
    else
    {
      for( Temp32=0;Temp32<MAX_PARAM_LIMIT;Temp32++)CopySetPara[Temp32]=ModCopySetPara[Temp32];
      UpdateEditSettings();  
      Mod_TransmitFrame.Data_Array[0] = 5;     // Ack
      Delay1Msec12Mhz(20);
      EditParaPassStatus = 0;
    }
    
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
      //SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
    }
    else
    {
      Mod_TransmitFrame.Data_Array[0] = 5;     // Ack
      EditParaPassStatus=1;
      
      Delay1Msec12Mhz(20);
    }
    
    SendData_UART(CopySetPara[PARA_DEVICE_ID], Fun_Received,1);
}

#endif // #define MODEL_RS485
