/*
******************************************************************************
  * @file    comm.c
  * @author  MRM R&D Team
  * @version V1.0.0
  * @date    07-04-2022
  * @brief   Main startup Functions
******************************************************************************
*/


#include "includes.h"

extern const struct PH_COEFF_STRUCT CalibrationCoeff;


/*
Inf: Main Application 
Inp: None
Ret: None
*/

void main(void)
{
  InitWatchDog();  
  RESET_WATCH_DOG;
  __disable_interrupt();
  InitializeHardware();
  DisplayDisabled();
  SetInitialDisplay();
  RS485Receive;
  SWITCH_OFF_LED_COMM;
  SWITCH_OFF_LED2_R;
  SWITCH_OFF_LED2_G;
  SWITCH_OFF_LED2_B;
  MeterInit(); 
  if(StorageBuffer.RunningMode==RUNNING_MODE_IMPORT)StorageBuffer.ImportInterruptions++;
  else StorageBuffer.ExportInterruptions++;
  FillCurrentGainArray();
  IntTimerCount.TimerNewValue=3750;
  IntTimerCount.TimerPresentValue=3750;
  WorkingCopyGain.VR_GAIN=CalibrationCoeff.VR_240_GAIN;
  WorkingCopyGain.VY_GAIN=CalibrationCoeff.VY_240_GAIN;
  WorkingCopyGain.VB_GAIN=CalibrationCoeff.VB_240_GAIN;
  SetWorkingGainBuffer();
  DISP_SR_ON;
  __enable_interrupt();
  
  
  
  while(1)
  {
    RESET_WATCH_DOG;
    CheckKey();    
    if(InterruptFlag & INT_CYCLE_OVER)
    {
      ProcessIntCycleOver();
      
    }
    if(OneSecFlag  & ONE_SEC_FLAG_1SEC_OVER)
    {
      OneSecFlag &=~ONE_SEC_FLAG_1SEC_OVER;  
        
      Process1SecOver();
      if(ParaBlockIndex)
      {
        if(KeyPressedCounter>0)KeyPressedCounter--;
        if(KeyPressedCounter==1)
        {
          RESET_WATCH_DOG;
          DisplayParameterUpdate();
          ParaBlockIndex=0;
          EditIndex=0;
          ParaBlockIndex=0;
          DisplaySetup.DisplayScanPage=0;
          I2CRead(PROGRAM_DATA_LOC1_START,(MAX_PARAM_LIMIT*2+2),EXT_EEPROM,(uint8_t *)CopySetPara );
          InterruptFlag &=~(PASSWORD_CHECK_ON+PASSWORD_VERIFIED+PASSWORD_FOR_VIEW);
          
          DisplayScrollCounter=10;
     
          DisplayUpdate();
        }
      }
#ifdef  MODEL_DATA_SAVE          
      SaveData();   
#endif
    }
#ifdef MODEL_RS485
     ModBusCommunication();
#endif
  }
}

/*
Inf: Frequency Process 
Inp: None
Ret: None
*/
void ProcessFreq(void)
{
  float Frequency=0;
  if(FreqSampleFlag==1)
  {
    if(CaptureValueSav>0)
    {
      Frequency=(uint16_t)(16e7/CaptureValueSav);
      CaptureValueSav=0;
      InstantPara.Frequency=Frequency/100;
      
    }
  }
  else if(FreqSampleFlag>1)
  {
    if(FreqFlag & FF_MEAS_OVER)
    {
      FreqFlag &=~FF_MEAS_OVER;
      Frequency=(6e10/((float)SaveFreqMeasDuration));
      InstantPara.Frequency=Frequency/100;
    }
    
  }
  else
  {
    Frequency=0;
    InstantPara.Frequency=Frequency;
  }
  if(Frequency==0)
  {
    IntTimerCount.TimerNewValue=3750;
  }
  else
  {
  //  IntTimerCount.TimerNewValue=3750;
    IntTimerCount.TimerNewValue=(uint16_t)((5000*3750.0)/(Frequency));
  }
}

/*
Inf: 1 sec process API
Inp: None
Ret: None
*/
void Process1SecOver(void)
{
  if(PROTECTION_BIT_HIGH)
  {
    StartCalibration(); 
    if(SwPressed==KEY_CT_CHK)StartCheckCTPolarity();
  }
  if(DisplaySetup.DisplayScrollStatus==DISPLAY_SCROLL_OFF)
  {
    if(ScrollSaveCounter<61)ScrollSaveCounter++;
    if(ScrollSaveCounter==60)
    {
       InterruptFlag |=INT_DATA_SAVING_EEPROM;
       __no_operation();
       EepromWrite(SCROLL_LOCK_LOC,2,EXT_EEPROM,(uint8_t *)&DisplaySetup.DisplayScrollStatus );
       InterruptFlag &=~INT_DATA_SAVING_EEPROM;
    }
  }
  if(StartDelayCount >= 100) SetVersionDisplay();
  if(StartDelayCount >= 200) pwrDlyFlag = 1;
  if(EditParaPassStatus==1)++EditPassCount;
  else EditPassCount = 0;
  
  if (EditPassCount >=30)
  {
    EditParaPassStatus = 0;
    EditPassCount = 0;
  }
  if((ParaBlockIndex==0)&&(FlagDirectCalibration==0)&&(pwrDlyFlag))
  {
    UpdateDisplayIndex();
    DisplayUpdate();
  }
  if(DisplayDebarCounter>0)DisplayDebarCounter--;
  
  if(ParaBlockIndex==0)CheckAutoScroll(); 
}

/*
Inf: After completing Interrupt Cycle
Inp: None
Ret: None
*/
void ProcessIntCycleOver(void)
{
#ifdef MODEL_RS485  
  if((USART2->CR1 & 0x0D)!=0X0D)
  {
    InitUart(CopySetPara[PARA_BAUD_RATE],CopySetPara[PARA_PARITY],CopySetPara[PARA_STOP_BIT]);
    RS485Receive;
    SWITCH_OFF_LED_COMM;
    Timer.transfercomplete=0;
    CounterSendComplete=0;
  }
#endif
  InterruptFlag &= ~INT_CYCLE_OVER;
  ProcessFreq();
  Metrology(); 
  if(FlagDirectCalibration==0)SetWorkingGainBuffer(); 
}
   
const float DefaultFlashCoeffVlaues[]=
{
  1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,
  0.0f,0.0f,0.0f,
  0.0f,0.0f,0.0f,
  0.0f,0.0f,0.0f,
  METER_INIT_VALUE,METER_INIT_VALUE
};

/*
Inf: Set Default calibration coefficient 
Inp: None
Ret: None
*/
void SetDefaultCalCoeff(void)
{
  ProtectionReset();              
  __disable_interrupt();
  __no_operation();
  RESET_WATCH_DOG;
  for (uint16_t i=0;i<sizeof(CalibrationCoeff);i++)*((uint8_t *)&CalBuffer+i)=*((uint8_t *)&CalibrationCoeff+i);
  WriteFlashData();   
  RESET_WATCH_DOG;
  ClearInterruptVariables();
  __enable_interrupt();
}

/*
Inf: Reset Calibration System
Inp: None
Ret: None
*/
void ResetCalSys(void)
{
  CalibrationGapCounter=0;
  ClearCalAccumulatedData();
  __disable_interrupt();
      __no_operation();
  RESET_WATCH_DOG;
  ClearInterruptVariables();
  __enable_interrupt();
}

/*
Inf: Calibration Process 
Inp: None
Ret: None
*/

void StartCalibration(void)
{

   if((SwPressed==KEY_DIR_CAL)&&(FlagDirectCalibration==0))
   {
      SetDefaultCalCoeff();
      DisplaySetHighPF();
      FlagDirectCalibration=CALIBRATE_DIS_H_VI;
    }
   if((SwPressed==KEY_NEXT)&& (FlagDirectCalibration == CALIBRATE_DIS_H_VI))
   {
     DisplayCalHighVI();
     FlagDirectCalibration=CALIBRATE_H_VI;
     ResetCalSys();
     WorkingCopyGain.VR_GAIN=1.0f;
     WorkingCopyGain.VY_GAIN=1.0f;
     WorkingCopyGain.VB_GAIN=1.0f;
     WorkingCopyGain.IR_GAIN=1.0f;
     WorkingCopyGain.IY_GAIN=1.0f;
     WorkingCopyGain.IB_GAIN=1.0f;
     WorkingCopyGain.PR_ALFA=1;
     WorkingCopyGain.PR_BETA=0;
     WorkingCopyGain.PY_ALFA=1;
     WorkingCopyGain.PY_BETA=0;
     WorkingCopyGain.PB_ALFA=1;
     WorkingCopyGain.PB_BETA=0;
       
   }
   if(FlagDirectCalibration == CALIBRATE_H_VI)
   {
     CalibrationGapCounter++;
     if(CalibrationGapCounter<CAL_ACC_DELAY);
     
     else if(CalibrationGapCounter<(NO_OF_CAL_ACCUMULATION_VI+CAL_ACC_DELAY))AccumulateDataForCalibration();
     else
     {
        DirectCalibration();
        FlagDirectCalibration=CALIBRATE_DIS_H_PF;
        WorkingCopyGain.IR_GAIN=CalibrationCoeff.IR_HIGH_GAIN;
        WorkingCopyGain.IY_GAIN=CalibrationCoeff.IY_HIGH_GAIN;
        WorkingCopyGain.IB_GAIN=CalibrationCoeff.IB_HIGH_GAIN;
        WorkingCopyGain.VR_GAIN=CalibrationCoeff.VR_240_GAIN;
        WorkingCopyGain.VY_GAIN=CalibrationCoeff.VY_240_GAIN;
        WorkingCopyGain.VB_GAIN=CalibrationCoeff.VB_240_GAIN;
        WorkingCopyGain.PR_ALFA=1;
        WorkingCopyGain.PR_BETA=0;
        WorkingCopyGain.PY_ALFA=1;
        WorkingCopyGain.PY_BETA=0;
        WorkingCopyGain.PB_ALFA=1;
        WorkingCopyGain.PB_BETA=0;
     }
   }
   
   if(FlagDirectCalibration == CALIBRATE_DIS_H_PF)
   {
     DisplayCalHighPF();
     FlagDirectCalibration=CALIBRATE_H_PF;
     ResetCalSys();
       
   }   
   if(FlagDirectCalibration == CALIBRATE_H_PF)
   {
     CalibrationGapCounter++;
     if(CalibrationGapCounter<CAL_ACC_DELAY);
     
     else if(CalibrationGapCounter<(NO_OF_CAL_ACCUMULATION_POW+CAL_ACC_DELAY))AccumulateDataForCalibration();
     else
     {
        DirectCalibration();
        DisplaySetMidPF();
        FlagDirectCalibration=CALIBRATE_DIS_M_VI;
     }
   }
   if((SwPressed==KEY_NEXT)&& (FlagDirectCalibration == CALIBRATE_DIS_M_VI))
   {
     DisplayCalMidVI();
     FlagDirectCalibration=CALIBRATE_M_VI;
     ResetCalSys();
     WorkingCopyGain.IR_GAIN=1.0f;
     WorkingCopyGain.IY_GAIN=1.0f;
     WorkingCopyGain.IB_GAIN=1.0f;
     WorkingCopyGain.VR_GAIN=CalibrationCoeff.VR_240_GAIN;
     WorkingCopyGain.VY_GAIN=CalibrationCoeff.VY_240_GAIN;
     WorkingCopyGain.VB_GAIN=CalibrationCoeff.VB_240_GAIN;
     
     WorkingCopyGain.PR_ALFA=1;
     WorkingCopyGain.PR_BETA=0;
     WorkingCopyGain.PY_ALFA=1;
     WorkingCopyGain.PY_BETA=0;
     WorkingCopyGain.PB_ALFA=1;
     WorkingCopyGain.PB_BETA=0;
       
   }
   if(FlagDirectCalibration == CALIBRATE_M_VI)
   {
     CalibrationGapCounter++;
     if(CalibrationGapCounter<CAL_ACC_DELAY);
     
     else if(CalibrationGapCounter<(NO_OF_CAL_ACCUMULATION_VI+CAL_ACC_DELAY))AccumulateDataForCalibration();
     else
     {
        DirectCalibration();
        FlagDirectCalibration=CALIBRATE_DIS_M_PF;
        WorkingCopyGain.IR_GAIN=CalibrationCoeff.IR_MID_GAIN;
        WorkingCopyGain.IY_GAIN=CalibrationCoeff.IY_MID_GAIN;
        WorkingCopyGain.IB_GAIN=CalibrationCoeff.IB_MID_GAIN;
        WorkingCopyGain.VR_GAIN=CalibrationCoeff.VR_240_GAIN;
        WorkingCopyGain.VY_GAIN=CalibrationCoeff.VY_240_GAIN;
        WorkingCopyGain.VB_GAIN=CalibrationCoeff.VB_240_GAIN;
        WorkingCopyGain.PR_ALFA=1;
        WorkingCopyGain.PR_BETA=0;
        WorkingCopyGain.PY_ALFA=1;
        WorkingCopyGain.PY_BETA=0;
        WorkingCopyGain.PB_ALFA=1;
        WorkingCopyGain.PB_BETA=0;

     }
   }
   if(FlagDirectCalibration == CALIBRATE_DIS_M_PF)
   {
     DisplayCalMidPF();
     FlagDirectCalibration=CALIBRATE_M_PF;
     ResetCalSys();
       
   }   
   if(FlagDirectCalibration == CALIBRATE_M_PF)
   {
     CalibrationGapCounter++;
     if(CalibrationGapCounter<CAL_ACC_DELAY);
     
     else if(CalibrationGapCounter<(NO_OF_CAL_ACCUMULATION_POW+CAL_ACC_DELAY))AccumulateDataForCalibration();
     else
     {
        DirectCalibration();
        DisplaySetLowPF();
        FlagDirectCalibration=CALIBRATE_DIS_L_VI;
     }
   }
   
   if((SwPressed==KEY_NEXT)&& (FlagDirectCalibration == CALIBRATE_DIS_L_VI))
   {
     DisplayCalLowVI();
     FlagDirectCalibration=CALIBRATE_L_VI;
     ResetCalSys();
     WorkingCopyGain.IR_GAIN=1.0f;
     WorkingCopyGain.IY_GAIN=1.0f;
     WorkingCopyGain.IB_GAIN=1.0f;
     WorkingCopyGain.VR_GAIN=CalibrationCoeff.VR_240_GAIN;
     WorkingCopyGain.VY_GAIN=CalibrationCoeff.VY_240_GAIN;
     WorkingCopyGain.VB_GAIN=CalibrationCoeff.VB_240_GAIN;
     
     
     WorkingCopyGain.PR_ALFA=1;
     WorkingCopyGain.PR_BETA=0;
     WorkingCopyGain.PY_ALFA=1;
     WorkingCopyGain.PY_BETA=0;
     WorkingCopyGain.PB_ALFA=1;
     WorkingCopyGain.PB_BETA=0;
       
   }
   if(FlagDirectCalibration == CALIBRATE_L_VI)
   {
     CalibrationGapCounter++;
     if(CalibrationGapCounter<CAL_ACC_DELAY);
     
     else if(CalibrationGapCounter<(NO_OF_CAL_ACCUMULATION_VI+CAL_ACC_DELAY))AccumulateDataForCalibration();
     else
     {
        DirectCalibration();
        FlagDirectCalibration=CALIBRATE_DIS_L_PF;
        WorkingCopyGain.IR_GAIN=CalibrationCoeff.IR_LOW_GAIN;
        WorkingCopyGain.IY_GAIN=CalibrationCoeff.IY_LOW_GAIN;
        WorkingCopyGain.IB_GAIN=CalibrationCoeff.IB_LOW_GAIN;
        WorkingCopyGain.VR_GAIN=CalibrationCoeff.VR_240_GAIN;
        WorkingCopyGain.VY_GAIN=CalibrationCoeff.VY_240_GAIN;
        WorkingCopyGain.VB_GAIN=CalibrationCoeff.VB_240_GAIN;
        WorkingCopyGain.PR_ALFA=1;
        WorkingCopyGain.PR_BETA=0;
        WorkingCopyGain.PY_ALFA=1;
        WorkingCopyGain.PY_BETA=0;
        WorkingCopyGain.PB_ALFA=1;
        WorkingCopyGain.PB_BETA=0;

     }
   }
   if(FlagDirectCalibration == CALIBRATE_DIS_L_PF)
   {
     DisplayCalLowPF();
     FlagDirectCalibration=CALIBRATE_L_PF;
     ResetCalSys();
   }
   
   if(FlagDirectCalibration == CALIBRATE_DIS_L_PF)
   {
     DisplayCalLowPF();
     FlagDirectCalibration=CALIBRATE_L_PF;
     ResetCalSys();
   }   
   if(FlagDirectCalibration == CALIBRATE_L_PF)
   {
     CalibrationGapCounter++;
     if(CalibrationGapCounter<CAL_ACC_DELAY);
     
     else if(CalibrationGapCounter<(NO_OF_CAL_ACCUMULATION_POW+CAL_ACC_DELAY))AccumulateDataForCalibration();
     else
     {
        DirectCalibration();
        NVIC_SystemReset();
     }
   }
}
 
/*
Inf: Calculate current gain after calibration 
Inp: None
Ret: None
*/ 
void FillCurrentGainArray(void)
{
  uint8_t  i;
  StepHigh=(5-CUR_MID_CAL_POINT)*10;
  StepLow=(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT)*10;
  
  for(i=0;i<StepHigh;i++)BufferBetaR[i]=CalibrationCoeff.IR_HIGH_PH_ERROR+(CalibrationCoeff.IR_MID_PH_ERROR-CalibrationCoeff.IR_HIGH_PH_ERROR)*i/StepHigh;
  for(i=0;i<StepHigh;i++)BufferBetaY[i]=CalibrationCoeff.IY_HIGH_PH_ERROR+(CalibrationCoeff.IY_MID_PH_ERROR-CalibrationCoeff.IY_HIGH_PH_ERROR)*i/StepHigh;
  for(i=0;i<StepHigh;i++)BufferBetaB[i]=CalibrationCoeff.IB_HIGH_PH_ERROR+(CalibrationCoeff.IB_MID_PH_ERROR-CalibrationCoeff.IB_HIGH_PH_ERROR)*i/StepHigh;
  
  for(i=0;i<StepLow+1;i++)BufferBetaR[i+StepHigh]=CalibrationCoeff.IR_MID_PH_ERROR+(CalibrationCoeff.IR_LOW_PH_ERROR-CalibrationCoeff.IR_MID_PH_ERROR)*i/StepLow;
  for(i=0;i<StepLow+1;i++)BufferBetaY[i+StepHigh]=CalibrationCoeff.IY_MID_PH_ERROR+(CalibrationCoeff.IY_LOW_PH_ERROR-CalibrationCoeff.IY_MID_PH_ERROR)*i/StepLow;
  for(i=0;i<StepLow+1;i++)BufferBetaB[i+StepHigh]=CalibrationCoeff.IB_MID_PH_ERROR+(CalibrationCoeff.IB_LOW_PH_ERROR-CalibrationCoeff.IB_MID_PH_ERROR)*i/StepLow;
  
  for(i=0;i<50;i++)CalPF(BufferBetaR[i],&BufferAlfaR[i],&BufferBetaR[i]);
  for(i=0;i<50;i++)CalPF(BufferBetaY[i],&BufferAlfaY[i],&BufferBetaY[i]);
  for(i=0;i<50;i++)CalPF(BufferBetaB[i],&BufferAlfaB[i],&BufferBetaB[i]);
}
  
/*
Inf: Calculate gain after calibration 
Inp: None
Ret: None
*/
void SetWorkingGainBuffer(void)
{
  uint8_t TempChar;
  float Slope,Offset;
  if(InstantPara.CurrentR>=1.0)
  {
    if(InstantPara.CurrentR>4.9)WorkingCopyGain.IR_GAIN=CalibrationCoeff.IR_HIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IR_HIGH_GAIN-CalibrationCoeff.IR_MID_GAIN)/(5-CUR_MID_CAL_POINT);
      Offset=CalibrationCoeff.IR_HIGH_GAIN-5*Slope;
      WorkingCopyGain.IR_GAIN=Offset+Slope*InstantPara.CurrentR;
    }
  }
  else if(InstantPara.CurrentR>=0.1)
  {
    Slope=(CalibrationCoeff.IR_MID_GAIN-CalibrationCoeff.IR_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IR_MID_GAIN-Slope;
    WorkingCopyGain.IR_GAIN=Offset+Slope*InstantPara.CurrentR;
  }
  else WorkingCopyGain.IR_GAIN=CalibrationCoeff.IR_LOW_GAIN;
 
  if(InstantPara.CurrentY>=1.0)
  {
    if(InstantPara.CurrentY>4.9)WorkingCopyGain.IY_GAIN=CalibrationCoeff.IY_HIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IY_HIGH_GAIN-CalibrationCoeff.IY_MID_GAIN)/(5-CUR_MID_CAL_POINT);
      Offset=CalibrationCoeff.IY_HIGH_GAIN-5*Slope;
      WorkingCopyGain.IY_GAIN=Offset+Slope*InstantPara.CurrentY;
    }
  }
  else if(InstantPara.CurrentY>=0.1)
  {
    Slope=(CalibrationCoeff.IY_MID_GAIN-CalibrationCoeff.IY_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IY_MID_GAIN-Slope;
    WorkingCopyGain.IY_GAIN=Offset+Slope*InstantPara.CurrentY;
  }
  else WorkingCopyGain.IY_GAIN=CalibrationCoeff.IY_LOW_GAIN;
  if(InstantPara.CurrentB>=1.0)
  {
    if(InstantPara.CurrentB>4.9)WorkingCopyGain.IB_GAIN=CalibrationCoeff.IB_HIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IB_HIGH_GAIN-CalibrationCoeff.IB_MID_GAIN)/(5-CUR_MID_CAL_POINT);
      Offset=CalibrationCoeff.IB_HIGH_GAIN-5*Slope;
      WorkingCopyGain.IB_GAIN=Offset+Slope*InstantPara.CurrentB;
    }
  }
  else if(InstantPara.CurrentB>=0.1)
  {
    Slope=(CalibrationCoeff.IB_MID_GAIN-CalibrationCoeff.IB_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IB_MID_GAIN-Slope;
    WorkingCopyGain.IB_GAIN=Offset+Slope*InstantPara.CurrentB;
  }
  else WorkingCopyGain.IB_GAIN=CalibrationCoeff.IB_LOW_GAIN;  
    
   
    
  TempChar=(uint8_t)(InstantPara.CurrentR*10);
  if(TempChar>=50)TempChar=50;
  else if(TempChar==0)TempChar=1;
  TempChar=50-TempChar;
  WorkingCopyGain.PR_ALFA=BufferAlfaR[TempChar];
  WorkingCopyGain.PR_BETA=BufferBetaR[TempChar];
  
  TempChar=(uint8_t)(InstantPara.CurrentY*10);
  if(TempChar>=50)TempChar=50;
  else if(TempChar==0)TempChar=1;
  TempChar=50-TempChar;
  WorkingCopyGain.PY_ALFA=BufferAlfaY[TempChar];
  WorkingCopyGain.PY_BETA=BufferBetaY[TempChar];

  TempChar=(uint8_t)(InstantPara.CurrentB*10);
  if(TempChar>=50)TempChar=50;
  else if(TempChar==0)TempChar=1;
  TempChar=50-TempChar;
  WorkingCopyGain.PB_ALFA=BufferAlfaB[TempChar];
  WorkingCopyGain.PB_BETA=BufferBetaB[TempChar];
 
}
  /*
Inf: Save Data in Eeprom
Inp: None
Ret: None
*/
 #ifdef MODEL_DATA_SAVE  
  void SaveData()
  {
    uint16_t Tempointer;
    if(PowerUpCounter<DATA_SAVE_DEBAR_TIME)PowerUpCounter++;
    if(InterruptFlag & INT_POWER_OK)DataSaveCounter++; 
    else DataSaveCounter=0;
    StorageBuffer.RunHourImport++;
    {
      if((InstantPara.CurrentR>0)||(InstantPara.CurrentY>0)||
        (InstantPara.CurrentB>0))
      {
         StorageBuffer.LoadHourImport++;
      }
    }
  
    if(DataSaveCounter>=DATA_SAVE_DURATION)
    {
      DataSaveCounter=0;
      InterruptFlag |=INT_DATA_SAVING_EEPROM;
      StorageBuffer.StorageCounter++;
      StorageBuffer.StorageLocation+=128;
      if(StorageBuffer.StorageLocation >=DATA_SAVE_END_LOC)StorageBuffer.StorageLocation=DATA_SAVE_START_LOC;
      Tempointer=offsetof(struct STORE,StoreCRC);
      StorageBuffer.StoreCRC=CRCCalculation((uint16_t *)&StorageBuffer,Tempointer/2);
      EepromWrite(StorageBuffer.StorageLocation,64,EXT_EEPROM,(uint8_t *)&StorageBuffer );
      EepromWrite(StorageBuffer.StorageLocation+64,sizeof(StorageBuffer)-64,EXT_EEPROM,(uint8_t *)&StorageBuffer+64 );
      InterruptFlag &=~INT_DATA_SAVING_EEPROM;
    }
  }
#endif





  
