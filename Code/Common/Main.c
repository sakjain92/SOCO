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
#include "extern_includes.h"

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

  if(StorageBuffer.RunningMode==RUNNING_MODE_IMPORT)StorageBuffer.SolarImportInterruptions++;
  else StorageBuffer.SolarExportInterruptions++;

  FillCurrentGainArray();
  IntTimerCount.TimerNewValue=3750;
  IntTimerCount.TimerPresentValue=3750;
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
          EepromRead(PROGRAM_DATA_LOC1_START,(MAX_PARAM_LIMIT*2+2),EXT_EEPROM,(uint8_t *)CopySetPara );
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
  // UNDONE: We don't read frequency currently hence delete this code
  // and read values at constant 3.2kHz frequency only
  //
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

void SwitchOffContactorRPhaseGridHealthy()
{
    SWITCH_OFF_CONTACTOR_R_PHASE_GRID_HEALTHY;
}
void SwitchOnContactorRPhaseGridHealthy()
{
    SWITCH_ON_CONTACTOR_R_PHASE_GRID_HEALTHY;
}
void SwitchOffContactorYPhaseGridHealthy()
{
    SWITCH_OFF_CONTACTOR_Y_PHASE_GRID_HEALTHY;
}
void SwitchOnContactorYPhaseGridHealthy()
{
    SWITCH_ON_CONTACTOR_Y_PHASE_GRID_HEALTHY;
}
void SwitchOffContactorBPhaseGridHealthy()
{
    SWITCH_OFF_CONTACTOR_B_PHASE_GRID_HEALTHY;
}
void SwitchOnContactorBPhaseGridHealthy()
{
    SWITCH_ON_CONTACTOR_B_PHASE_GRID_HEALTHY;
}
void SwitchOffContactorLoadOnSolar()
{
    SWITCH_OFF_CONTACTOR_LOAD_ON_SOLAR;
}
void SwitchOnContactorLoadOnSolar()
{
    SWITCH_ON_CONTACTOR_LOAD_ON_SOLAR;
}
void SwitchOffContactorLoadOnGrid()
{
    SWITCH_OFF_CONTACTOR_LOAD_ON_GRID;
}
void SwitchOnContactorLoadOnGrid()
{
    SWITCH_ON_CONTACTOR_LOAD_ON_GRID;
}

// Controls the logic controlling the relays outputs
// This function is called once every 1 second
// DEVNOTE: This function should consider the possibility of contactors
// getting stuck and not following the state as driven by the relays
//
// UNDONE: Check all logic here thoroughly. There is a bug here that 
// if contactor feedback is not matching it's actual state we are trying to
// set it to, the contactors can change simuntaneously whereas there should
// always be some delay when changing one or more contactors that impact
// each other
//
void ProcessRelays()
{
    enum StuckState
    {
        CONTACTOR_NOT_STUCK,
        CONTACTOR_STUCK_IN_OPEN,
        CONTACTOR_STUCK_IN_CLOSED,
    };

    struct PhaseVolState
    {
        bool isHealthy;
        uint8_t overVoltParaIdx;
        uint8_t underVoltParaIdx;
        uint8_t overVoltResetParaIdx;
        uint8_t underVoltResetParaIdx;
        uint8_t failDelayParaIdx;
        uint8_t returnDelayParaIdx;
        uint16_t TimeLeft;
        float* vol;
        void (*turnContactorOn)(void);
        void (*turnContactorOff)(void);
        bool* contactorFeedback;
        bool shouldContactorBeOn;
        uint16_t ContactorChangeTimeLeft;
        bool isPrevContactorOn; // Records previous state of contactor before transition
                                // till we don't register the actual contactor change.
                                // This is to allow contactors to first turn off before
                                // turning other contactors (after some delay)
        bool* stuckOpen;
        bool* stuckClosed;
    };

    static bool initialized = false;

    // UNDONE: Make this a setting
    //
#define MAX_CONTACTOR_CHANGE_LATENCY    5

    // By default, contactor is turned on when the controller is off
    // (Contactor coil tied to NC of the controller's relay).
    // So assume healthy state initially (except Load On Solar contactor)
    //
    static struct PhaseVolState pvsArray[] = 
    {
        // Mains R Phase
        {
            .isHealthy = true,
            .overVoltParaIdx = PARA_MAINS_OVER_VOLT,
            .underVoltParaIdx = PARA_MAINS_UNDER_VOLT,
            .overVoltResetParaIdx = PARA_MAINS_OVER_VOLT_RESET,
            .underVoltResetParaIdx = PARA_MAINS_UNDER_VOLT_RESET,
            .failDelayParaIdx = PARA_MAINS_FAIL_DELAY,
            .returnDelayParaIdx = PARA_MAINS_RETURN_DELAY,
            .TimeLeft = 0,
            .vol = &InstantPara.VolR,
            .turnContactorOn = SwitchOnContactorRPhaseGridHealthy,
            .turnContactorOff = SwitchOffContactorRPhaseGridHealthy,
            .contactorFeedback = &g_DigInputs.MainsRPhaseContactorOn,
            .shouldContactorBeOn = true,
            .ContactorChangeTimeLeft = MAX_CONTACTOR_CHANGE_LATENCY,
            .isPrevContactorOn = true,
            .stuckOpen = &g_Alarms.MainsRPhaseContactorStuckOpen,
            .stuckClosed = &g_Alarms.MainsRPhaseContactorStuckClosed,
        },
        // Mains Y Phase
        {
            .isHealthy = true,
            .overVoltParaIdx = PARA_MAINS_OVER_VOLT,
            .underVoltParaIdx = PARA_MAINS_UNDER_VOLT,
            .overVoltResetParaIdx = PARA_MAINS_OVER_VOLT_RESET,
            .underVoltResetParaIdx = PARA_MAINS_UNDER_VOLT_RESET,
            .failDelayParaIdx = PARA_MAINS_FAIL_DELAY,
            .returnDelayParaIdx = PARA_MAINS_RETURN_DELAY,
            .TimeLeft = 0,
            .vol = &InstantPara.VolY,
            .turnContactorOn = SwitchOnContactorYPhaseGridHealthy,
            .turnContactorOff = SwitchOffContactorYPhaseGridHealthy,
            .contactorFeedback = &g_DigInputs.MainsYPhaseContactorOn,
            .shouldContactorBeOn = true,
            .ContactorChangeTimeLeft = MAX_CONTACTOR_CHANGE_LATENCY,
            .isPrevContactorOn = true,
            .stuckOpen = &g_Alarms.MainsYPhaseContactorStuckOpen,
            .stuckClosed = &g_Alarms.MainsYPhaseContactorStuckClosed,
        },
        // Mains B Phase
        {
            .isHealthy = true,
            .overVoltParaIdx = PARA_MAINS_OVER_VOLT,
            .underVoltParaIdx = PARA_MAINS_UNDER_VOLT,
            .overVoltResetParaIdx = PARA_MAINS_OVER_VOLT_RESET,
            .underVoltResetParaIdx = PARA_MAINS_UNDER_VOLT_RESET,
            .failDelayParaIdx = PARA_MAINS_FAIL_DELAY,
            .returnDelayParaIdx = PARA_MAINS_RETURN_DELAY,
            .TimeLeft = 0,
            .vol = &InstantPara.VolB,
            .turnContactorOn = SwitchOnContactorBPhaseGridHealthy,
            .turnContactorOff = SwitchOffContactorBPhaseGridHealthy,
            .contactorFeedback = &g_DigInputs.MainsBPhaseContactorOn,
            .shouldContactorBeOn = true,
            .ContactorChangeTimeLeft = MAX_CONTACTOR_CHANGE_LATENCY,
            .isPrevContactorOn = true,
            .stuckOpen = &g_Alarms.MainsBPhaseContactorStuckOpen,
            .stuckClosed = &g_Alarms.MainsBPhaseContactorStuckClosed,
        },
        // Load On Solar
        //
        {
            .isHealthy = false,
            .overVoltParaIdx = PARA_SOLAR_OVER_VOLT,
            .underVoltParaIdx = PARA_SOLAR_UNDER_VOLT,
            .overVoltResetParaIdx = PARA_SOLAR_OVER_VOLT_RESET,
            .underVoltResetParaIdx = PARA_SOLAR_UNDER_VOLT_RESET,
            .failDelayParaIdx = PARA_SOLAR_FAIL_DELAY,
            .returnDelayParaIdx = PARA_SOLAR_RETURN_DELAY,
            .TimeLeft = 0,
            // UNDONE: Change this to actual solar R Phase
            //
            .vol = &InstantPara.VolRSolar,
            .turnContactorOn = SwitchOnContactorLoadOnSolar,
            .turnContactorOff = SwitchOffContactorLoadOnSolar,
            .contactorFeedback = &g_DigInputs.LoadOnSolarContactorOn,
            .shouldContactorBeOn = false,
            .ContactorChangeTimeLeft = MAX_CONTACTOR_CHANGE_LATENCY,
            .isPrevContactorOn = false,
            .stuckOpen = &g_Alarms.LoadOnSolarContactorStuckOpen,
            .stuckClosed = &g_Alarms.LoadOnSolarContactorStuckClosed,
        },
        // Load On Grid
        //
        {
            .isHealthy = true,
            .overVoltParaIdx = PARA_MAINS_OVER_VOLT,
            .underVoltParaIdx = PARA_MAINS_UNDER_VOLT,
            .overVoltResetParaIdx = PARA_MAINS_OVER_VOLT_RESET,
            .underVoltResetParaIdx = PARA_MAINS_UNDER_VOLT_RESET,
            .failDelayParaIdx = PARA_MAINS_FAIL_DELAY,
            .returnDelayParaIdx = PARA_MAINS_RETURN_DELAY,
            .TimeLeft = 0,
            .vol = &InstantPara.VolR,
            .turnContactorOn = SwitchOnContactorLoadOnGrid,
            .turnContactorOff = SwitchOffContactorLoadOnGrid,
            .contactorFeedback = &g_DigInputs.LoadOnGridContactorOn,
            .shouldContactorBeOn = true,
            .ContactorChangeTimeLeft = MAX_CONTACTOR_CHANGE_LATENCY,
            .isPrevContactorOn = true,
            .stuckOpen = &g_Alarms.LoadOnGridContactorStuckOpen,
            .stuckClosed = &g_Alarms.LoadOnGridContactorStuckClosed,
        }
    };

    if (!initialized)
    {
        initialized = true;
        pvsArray[0].TimeLeft = pvsArray[1].TimeLeft = pvsArray[2].TimeLeft =
            pvsArray[4].TimeLeft = CopySetPara[PARA_MAINS_FAIL_DELAY];
        pvsArray[3].TimeLeft = CopySetPara[PARA_SOLAR_RETURN_DELAY];
    }

    bool isAnyPhaseContactorOn = 
        pvsArray[0].isPrevContactorOn ||
        pvsArray[1].isPrevContactorOn ||
        pvsArray[2].isPrevContactorOn;

    // DEVNOTE: Data in CopySetPara[] can change at any point of time
    //
    for (uint8_t i = 0; i < ARRAY_SIZE(pvsArray); i++)
    {
        struct PhaseVolState* pvs = &pvsArray[i];

        if (pvs->isHealthy)
        {
            bool curUnhealthy = 
                (*pvs->vol > CopySetPara[pvs->overVoltParaIdx]) ||
                (*pvs->vol < CopySetPara[pvs->underVoltParaIdx]);

            if (curUnhealthy)
            {
                if (pvs->TimeLeft)
                {
                    pvs->TimeLeft--;
                }
                if (!pvs->TimeLeft)
                {
                    pvs->isHealthy = false;
                    pvs->TimeLeft = CopySetPara[pvs->returnDelayParaIdx];
                }
            }
            else
            {
                pvs->TimeLeft = CopySetPara[pvs->failDelayParaIdx];
            }
        }
        else 
        {
            bool curHealthy = 
                (*pvs->vol <= CopySetPara[pvs->overVoltResetParaIdx]) &&
                (*pvs->vol >= CopySetPara[pvs->underVoltResetParaIdx]);
            if (curHealthy)
            {
                if (pvs->TimeLeft)
                {
                    pvs->TimeLeft--;
                }
                if (!pvs->TimeLeft)
                {
                    pvs->isHealthy = true;
                    pvs->TimeLeft = CopySetPara[pvs->failDelayParaIdx];
                }
            }
            else
            {
                pvs->TimeLeft = CopySetPara[pvs->returnDelayParaIdx];
            }
        }
    }

    // We can't read solar's voltages when any of the R/Y/B contactor is still
    // on
    //
    if (isAnyPhaseContactorOn)
    {
        pvsArray[3].isHealthy = false;
        pvsArray[3].TimeLeft = CopySetPara[pvsArray[3].returnDelayParaIdx]; 
    }

    // Don't turn the Mains Phase Contactor On if
    // a) the Load is stuck on solar
    // as then the neutral is connected to local earth as we don't want
    // to connect grid's netural to local earth 
    // or 
    // b) if R Phase of Grid is unhealthy (as we wouldn't be able to drive
    // the load on Mains as R phase controlls that contactor). So hence
    // better to drive the solar contactor instead
    //
    // Don't put the load on Solar if any phase contactor is on
    // (As we don't want to connect grid neutral to local earth
    // which is done when Solar contactor is turned on)
    // OR
    //  if the DG is running
    // (For the edge case where the Mains is not available and Solar isn't
    // able to drive the load. In this case, if the load is kept on solar,
    // DG will not start as solar voltage will always be available but
    // solar will keep tripping when trying to start the load)
    //
    // No sense of turning B/Y phase of mains on incase the R phase of mains
    // is unhealthy as R Phase unhealthy indicates Load can't be put on mains.
    // So hence should put load on solar. For that, need to shut off the
    // phase contactors
    //
    // UNDONE: We can have an optimization that if we detect that K5
    // is stuck in open condition, we can shift load to solar once it's available.
    // For now, not doing this optimization as it will require turning off K5
    // but remembering that it is stuck.
    //
    pvsArray[0].shouldContactorBeOn = pvsArray[0].isHealthy &&
                                        !pvsArray[3].isPrevContactorOn &&
                                        (g_DisableLoadOnGridSeconds == 0);
    pvsArray[1].shouldContactorBeOn = pvsArray[0].isHealthy &&
                                        pvsArray[1].isHealthy &&
                                        !pvsArray[3].isPrevContactorOn &&
                                        (g_DisableLoadOnGridSeconds == 0);
    pvsArray[2].shouldContactorBeOn = pvsArray[0].isHealthy &&
                                        pvsArray[2].isHealthy &&
                                        !pvsArray[3].isPrevContactorOn &&
                                        (g_DisableLoadOnGridSeconds == 0);
    // If 48V is not present, DG PFC is not reliable.
    // But if DG is running, 48V should be available.
    // So if 48V is not present we assume DG is not running.
    //
    // Also, can't turn on Solar contactor if any of the mains contactor is stuck
    // in open condition
    //
    pvsArray[3].shouldContactorBeOn = pvsArray[3].isHealthy &&
                                      (!pvsArray[4].isHealthy || g_DisableLoadOnGridSeconds) &&
                                        (g_DigInputs.DGOff || !g_DigInputs.DC48Available) &&
                                        !pvsArray[0].isPrevContactorOn &&
                                        !pvsArray[1].isPrevContactorOn &&
                                        !pvsArray[2].isPrevContactorOn &&
                                        !pvsArray[4].isPrevContactorOn;
    pvsArray[4].shouldContactorBeOn = pvsArray[4].isHealthy &&
                                        (g_DisableLoadOnGridSeconds == 0) &&
                                        !pvsArray[3].isPrevContactorOn;

    if (g_DisableLoadOnGridSeconds)
    {
        g_DisableLoadOnGridSeconds--;
    }

    // DEVNOTE: Follow break before make for all contactors
    // DEVNOTE: There should be gap between turning off a contactor & then
    // turning on other contactors
    //
    for (uint8_t i = 0; i < ARRAY_SIZE(pvsArray); i++)
    {
        // UNDONE: Not sure if "break before make" is being followed here
        //
        if (!pvsArray[i].shouldContactorBeOn)
        {
            pvsArray[i].turnContactorOff();
        }
        else
        {
            pvsArray[i].turnContactorOn();
        }
        
        // Even if feedback is not available, wait for some time for contactor
        // to actually change before marking it as changed
        //
        bool feedbackNotMatches =
            ((*pvsArray[i].contactorFeedback != pvsArray[i].shouldContactorBeOn) &&
                g_DigInputs.DC48Available);
        bool contactorStateChanged =
            pvsArray[i].shouldContactorBeOn != pvsArray[i].isPrevContactorOn;

        // UNDONE: If feedback doesn't match (i.e. suddenly the contactor
        // goes stuck-open or stuck-close), we will have some delay before
        // we do anything about it
        //
        if (contactorStateChanged)
        {
            if ((!*pvsArray[i].stuckOpen && !*pvsArray[i].stuckClosed) ||
                !feedbackNotMatches)
            {
                if (pvsArray[i].ContactorChangeTimeLeft)
                {
                    pvsArray[i].ContactorChangeTimeLeft--;
                }
            }
            else
            {
                pvsArray[i].ContactorChangeTimeLeft = MAX_CONTACTOR_CHANGE_LATENCY;
            }

            if (!pvsArray[i].ContactorChangeTimeLeft)
            {
                pvsArray[i].ContactorChangeTimeLeft = MAX_CONTACTOR_CHANGE_LATENCY;
                if (feedbackNotMatches)
                {
                    pvsArray[i].isPrevContactorOn = *pvsArray[i].contactorFeedback;
                    if (*pvsArray[i].contactorFeedback)
                    {
                        *pvsArray[i].stuckOpen = false;
                        *pvsArray[i].stuckClosed = true;
                    }
                    else
                    {
                        *pvsArray[i].stuckOpen = true;
                        *pvsArray[i].stuckClosed = false;
                    }
                }
                else
                {
                    pvsArray[i].isPrevContactorOn = pvsArray[i].shouldContactorBeOn;
                    *pvsArray[i].stuckOpen = false;
                    *pvsArray[i].stuckClosed = false;
                }
            }
        }
        else if (!feedbackNotMatches)
        {
            pvsArray[i].ContactorChangeTimeLeft = MAX_CONTACTOR_CHANGE_LATENCY;
            pvsArray[i].isPrevContactorOn = pvsArray[i].shouldContactorBeOn;
            *pvsArray[i].stuckOpen = false;
            *pvsArray[i].stuckClosed = false;
        }
        else
        {
            // UNDONE: Add logic here
            //
        }
    }
#undef MAX_CONTACTOR_CHANGE_LATENCY
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
 
  // Long press KEY_NEXT causes issue when inside calibration
  // Also, it causes issue when we are editting settings
  //
  if(FlagDirectCalibration==0 && ParaBlockIndex==0)CheckAutoScroll();

  if (FlagDirectCalibration==0)ProcessRelays();
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
// DEVNOTE: This function is called once every second
//
void StartCalibration(void)
{

   if((SwPressed==KEY_DIR_CAL)&&(FlagDirectCalibration==0))
   {
      FlagDirectCalibration=CALIBRATE_OUT_1;
   }

   if (FlagDirectCalibration >= CALIBRATE_OUT_1 &&
       FlagDirectCalibration <= CALIBRATE_OUT_5)
   {
       // UNDONE: This logic here might not be working
       // Not able to test outputs properly
       //
       void (*outputs[])(void) = 
       {
           SwitchOnContactorRPhaseGridHealthy,
           SwitchOnContactorYPhaseGridHealthy,
           SwitchOnContactorBPhaseGridHealthy,
           SwitchOnContactorLoadOnSolar,
           SwitchOnContactorLoadOnGrid
       };
       uint8_t idx = FlagDirectCalibration - CALIBRATE_OUT_1;
       DisplayOutputX(idx + 1);
       outputs[idx]();
       if (SwPressed==KEY_NEXT)
       {
           FlagDirectCalibration++;
       }
   }

   if (FlagDirectCalibration >= CALIBRATE_IN_START &&
        FlagDirectCalibration <= CALIBRATE_IN_8)
   {
       SwitchOffContactorRPhaseGridHealthy();
       SwitchOffContactorYPhaseGridHealthy();
       SwitchOffContactorBPhaseGridHealthy();
       SwitchOffContactorLoadOnSolar();
       SwitchOffContactorLoadOnGrid();

       const bool* inputs[] = 
       {
           &g_DigInputs.MainsRPhaseContactorOn,
           &g_DigInputs.MainsYPhaseContactorOn,
           &g_DigInputs.MainsBPhaseContactorOn,
           &g_DigInputs.LoadOnSolarContactorOn,
           &g_DigInputs.LoadOnGridContactorOn,
           &g_DigInputs.SPDFailed,
           &g_DigInputs.DGOff,
           &g_DigInputs.DC48Available
       };
       if (FlagDirectCalibration == CALIBRATE_IN_START)
       {
            for (uint8_t i = 0; i < ARRAY_SIZE(inputs); i++)
            {
                if (*inputs[i])
                {
                    DisplayImproperSettings();
                }
            }
            FlagDirectCalibration = CALIBRATE_IN_1;
       }

       uint8_t idx = FlagDirectCalibration - CALIBRATE_IN_1;
       DisplayInputX(idx + 1);
       if (*inputs[idx])
       {
           FlagDirectCalibration++;
           if (FlagDirectCalibration == CALIBRATE_VOL_CUR_START)
           {
             SetDefaultCalCoeff();
             DisplaySetHighPF();
             FlagDirectCalibration=CALIBRATE_DIS_H_VI;  
           }
       }
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

     WorkingCopyGain.VR_SOLAR_GAIN=1.0f;
     WorkingCopyGain.VY_SOLAR_GAIN=1.0f;
     WorkingCopyGain.VB_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IR_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IY_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IB_SOLAR_GAIN=1.0f;
     WorkingCopyGain.PR_SOLAR_ALFA=1;
     WorkingCopyGain.PR_SOLAR_BETA=0;
     WorkingCopyGain.PY_SOLAR_ALFA=1;
     WorkingCopyGain.PY_SOLAR_BETA=0;
     WorkingCopyGain.PB_SOLAR_ALFA=1;
     WorkingCopyGain.PB_SOLAR_BETA=0;
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

        WorkingCopyGain.IR_SOLAR_GAIN=CalibrationCoeff.IR_SOLAR_HIGH_GAIN;
        WorkingCopyGain.IY_SOLAR_GAIN=CalibrationCoeff.IY_SOLAR_HIGH_GAIN;
        WorkingCopyGain.IB_SOLAR_GAIN=CalibrationCoeff.IB_SOLAR_HIGH_GAIN;
        WorkingCopyGain.VR_SOLAR_GAIN=CalibrationCoeff.VR_SOLAR_240_GAIN;
        WorkingCopyGain.VY_SOLAR_GAIN=CalibrationCoeff.VY_SOLAR_240_GAIN;
        WorkingCopyGain.VB_SOLAR_GAIN=CalibrationCoeff.VB_SOLAR_240_GAIN;
        WorkingCopyGain.PR_SOLAR_ALFA=1;
        WorkingCopyGain.PR_SOLAR_BETA=0;
        WorkingCopyGain.PY_SOLAR_ALFA=1;
        WorkingCopyGain.PY_SOLAR_BETA=0;
        WorkingCopyGain.PB_SOLAR_ALFA=1;
        WorkingCopyGain.PB_SOLAR_BETA=0;
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

     WorkingCopyGain.IR_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IY_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IB_SOLAR_GAIN=1.0f;
     WorkingCopyGain.VR_SOLAR_GAIN=CalibrationCoeff.VR_SOLAR_240_GAIN;
     WorkingCopyGain.VY_SOLAR_GAIN=CalibrationCoeff.VY_SOLAR_240_GAIN;
     WorkingCopyGain.VB_SOLAR_GAIN=CalibrationCoeff.VB_SOLAR_240_GAIN;
     WorkingCopyGain.PR_SOLAR_ALFA=1;
     WorkingCopyGain.PR_SOLAR_BETA=0;
     WorkingCopyGain.PY_SOLAR_ALFA=1;
     WorkingCopyGain.PY_SOLAR_BETA=0;
     WorkingCopyGain.PB_SOLAR_ALFA=1;
     WorkingCopyGain.PB_SOLAR_BETA=0;

       
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

        WorkingCopyGain.IR_SOLAR_GAIN=CalibrationCoeff.IR_SOLAR_MID_GAIN;
        WorkingCopyGain.IY_SOLAR_GAIN=CalibrationCoeff.IY_SOLAR_MID_GAIN;
        WorkingCopyGain.IB_SOLAR_GAIN=CalibrationCoeff.IB_SOLAR_MID_GAIN;
        WorkingCopyGain.VR_SOLAR_GAIN=CalibrationCoeff.VR_SOLAR_240_GAIN;
        WorkingCopyGain.VY_SOLAR_GAIN=CalibrationCoeff.VY_SOLAR_240_GAIN;
        WorkingCopyGain.VB_SOLAR_GAIN=CalibrationCoeff.VB_SOLAR_240_GAIN;
        WorkingCopyGain.PR_SOLAR_ALFA=1;
        WorkingCopyGain.PR_SOLAR_BETA=0;
        WorkingCopyGain.PY_SOLAR_ALFA=1;
        WorkingCopyGain.PY_SOLAR_BETA=0;
        WorkingCopyGain.PB_SOLAR_ALFA=1;
        WorkingCopyGain.PB_SOLAR_BETA=0;
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

     WorkingCopyGain.IR_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IY_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IB_SOLAR_GAIN=1.0f;
     WorkingCopyGain.VR_SOLAR_GAIN=CalibrationCoeff.VR_SOLAR_240_GAIN;
     WorkingCopyGain.VY_SOLAR_GAIN=CalibrationCoeff.VY_SOLAR_240_GAIN;
     WorkingCopyGain.VB_SOLAR_GAIN=CalibrationCoeff.VB_SOLAR_240_GAIN;
     WorkingCopyGain.PR_SOLAR_ALFA=1;
     WorkingCopyGain.PR_SOLAR_BETA=0;
     WorkingCopyGain.PY_SOLAR_ALFA=1;
     WorkingCopyGain.PY_SOLAR_BETA=0;
     WorkingCopyGain.PB_SOLAR_ALFA=1;
     WorkingCopyGain.PB_SOLAR_BETA=0;

       
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

        WorkingCopyGain.IR_SOLAR_GAIN=CalibrationCoeff.IR_SOLAR_LOW_GAIN;
        WorkingCopyGain.IY_SOLAR_GAIN=CalibrationCoeff.IY_SOLAR_LOW_GAIN;
        WorkingCopyGain.IB_SOLAR_GAIN=CalibrationCoeff.IB_SOLAR_LOW_GAIN;
        WorkingCopyGain.VR_SOLAR_GAIN=CalibrationCoeff.VR_SOLAR_240_GAIN;
        WorkingCopyGain.VY_SOLAR_GAIN=CalibrationCoeff.VY_SOLAR_240_GAIN;
        WorkingCopyGain.VB_SOLAR_GAIN=CalibrationCoeff.VB_SOLAR_240_GAIN;
        WorkingCopyGain.PR_SOLAR_ALFA=1;
        WorkingCopyGain.PR_SOLAR_BETA=0;
        WorkingCopyGain.PY_SOLAR_ALFA=1;
        WorkingCopyGain.PY_SOLAR_BETA=0;
        WorkingCopyGain.PB_SOLAR_ALFA=1;
        WorkingCopyGain.PB_SOLAR_BETA=0;
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
        FlagDirectCalibration = CALIBRATE_END;
     }
   }
   if (FlagDirectCalibration > CALIBRATE_END)
   {
     DisplayDoneCal();
     NVIC_SystemReset();
   }
   // UNDONE: Do a formal check of keys and also the 5V input incase
   // we add support for measuring 5V.
   // UNDONE: We need to test RS485 also here or better if we make the
   // testing automated over RS485 itself
   // UNDONE: We haven't tested USB here
   // Also, we haven't tested few peripherals such as 5V from 48V converted
   // and 5V from combined power supply.
   // We do use those inputs in our product but we aren't testing those in
   // software
   //
}
 
/*
Inf: Calculate current gain after calibration 
Inp: None
Ret: None
*/ 
void FillCurrentGainArray(void)
{
  // UNDONE: Check if we need to make the phase error correction more fine grained
  // as compared to multiples of CUR_LOW_CAL_POINT
  //
  uint8_t  i;
  StepHigh=(float)(CUR_HIGH_CAL_POINT-CUR_MID_CAL_POINT)/(float)(CUR_LOW_CAL_POINT);
  StepLow=(float)(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT)/(float)(CUR_LOW_CAL_POINT);
  
  for(i=0;i<StepHigh;i++)BufferBetaR[i]=CalibrationCoeff.IR_HIGH_PH_ERROR+(CalibrationCoeff.IR_MID_PH_ERROR-CalibrationCoeff.IR_HIGH_PH_ERROR)*i/StepHigh;
  for(i=0;i<StepHigh;i++)BufferBetaY[i]=CalibrationCoeff.IY_HIGH_PH_ERROR+(CalibrationCoeff.IY_MID_PH_ERROR-CalibrationCoeff.IY_HIGH_PH_ERROR)*i/StepHigh;
  for(i=0;i<StepHigh;i++)BufferBetaB[i]=CalibrationCoeff.IB_HIGH_PH_ERROR+(CalibrationCoeff.IB_MID_PH_ERROR-CalibrationCoeff.IB_HIGH_PH_ERROR)*i/StepHigh;
  
  for(i=0;i<StepLow+1;i++)BufferBetaR[i+StepHigh]=CalibrationCoeff.IR_MID_PH_ERROR+(CalibrationCoeff.IR_LOW_PH_ERROR-CalibrationCoeff.IR_MID_PH_ERROR)*i/StepLow;
  for(i=0;i<StepLow+1;i++)BufferBetaY[i+StepHigh]=CalibrationCoeff.IY_MID_PH_ERROR+(CalibrationCoeff.IY_LOW_PH_ERROR-CalibrationCoeff.IY_MID_PH_ERROR)*i/StepLow;
  for(i=0;i<StepLow+1;i++)BufferBetaB[i+StepHigh]=CalibrationCoeff.IB_MID_PH_ERROR+(CalibrationCoeff.IB_LOW_PH_ERROR-CalibrationCoeff.IB_MID_PH_ERROR)*i/StepLow;
  
  for(i=0;i<50;i++)CalPF(BufferBetaR[i],&BufferAlfaR[i],&BufferBetaR[i]);
  for(i=0;i<50;i++)CalPF(BufferBetaY[i],&BufferAlfaY[i],&BufferBetaY[i]);
  for(i=0;i<50;i++)CalPF(BufferBetaB[i],&BufferAlfaB[i],&BufferBetaB[i]);

  for(i=0;i<StepHigh;i++)BufferBetaRSolar[i]=CalibrationCoeff.IR_SOLAR_HIGH_PH_ERROR+(CalibrationCoeff.IR_SOLAR_MID_PH_ERROR-CalibrationCoeff.IR_SOLAR_HIGH_PH_ERROR)*i/StepHigh;
  for(i=0;i<StepHigh;i++)BufferBetaYSolar[i]=CalibrationCoeff.IY_SOLAR_HIGH_PH_ERROR+(CalibrationCoeff.IY_SOLAR_MID_PH_ERROR-CalibrationCoeff.IY_SOLAR_HIGH_PH_ERROR)*i/StepHigh;
  for(i=0;i<StepHigh;i++)BufferBetaBSolar[i]=CalibrationCoeff.IB_SOLAR_HIGH_PH_ERROR+(CalibrationCoeff.IB_SOLAR_MID_PH_ERROR-CalibrationCoeff.IB_SOLAR_HIGH_PH_ERROR)*i/StepHigh;
  
  for(i=0;i<StepLow+1;i++)BufferBetaRSolar[i+StepHigh]=CalibrationCoeff.IR_SOLAR_MID_PH_ERROR+(CalibrationCoeff.IR_SOLAR_LOW_PH_ERROR-CalibrationCoeff.IR_SOLAR_MID_PH_ERROR)*i/StepLow;
  for(i=0;i<StepLow+1;i++)BufferBetaYSolar[i+StepHigh]=CalibrationCoeff.IY_SOLAR_MID_PH_ERROR+(CalibrationCoeff.IY_SOLAR_LOW_PH_ERROR-CalibrationCoeff.IY_SOLAR_MID_PH_ERROR)*i/StepLow;
  for(i=0;i<StepLow+1;i++)BufferBetaBSolar[i+StepHigh]=CalibrationCoeff.IB_SOLAR_MID_PH_ERROR+(CalibrationCoeff.IB_SOLAR_LOW_PH_ERROR-CalibrationCoeff.IB_SOLAR_MID_PH_ERROR)*i/StepLow;
  
  for(i=0;i<50;i++)CalPF(BufferBetaRSolar[i],&BufferAlfaRSolar[i],&BufferBetaRSolar[i]);
  for(i=0;i<50;i++)CalPF(BufferBetaYSolar[i],&BufferAlfaYSolar[i],&BufferBetaYSolar[i]);
  for(i=0;i<50;i++)CalPF(BufferBetaBSolar[i],&BufferAlfaBSolar[i],&BufferBetaBSolar[i]);

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

  WorkingCopyGain.VR_GAIN=CalibrationCoeff.VR_240_GAIN;
  WorkingCopyGain.VY_GAIN=CalibrationCoeff.VY_240_GAIN;
  WorkingCopyGain.VB_GAIN=CalibrationCoeff.VB_240_GAIN;
  WorkingCopyGain.VR_SOLAR_GAIN=CalibrationCoeff.VR_SOLAR_240_GAIN;
  WorkingCopyGain.VY_SOLAR_GAIN=CalibrationCoeff.VY_SOLAR_240_GAIN;
  WorkingCopyGain.VB_SOLAR_GAIN=CalibrationCoeff.VB_SOLAR_240_GAIN;

  if(InstantPara.CurrentR>=CUR_MID_CAL_POINT)
  {
    if(InstantPara.CurrentR>=CUR_HIGH_CAL_POINT)WorkingCopyGain.IR_GAIN=CalibrationCoeff.IR_HIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IR_HIGH_GAIN-CalibrationCoeff.IR_MID_GAIN)/(CUR_HIGH_CAL_POINT-CUR_MID_CAL_POINT);
      Offset=CalibrationCoeff.IR_HIGH_GAIN-CUR_HIGH_CAL_POINT*Slope;
      WorkingCopyGain.IR_GAIN=Offset+Slope*InstantPara.CurrentR;
    }
  }
  else if(InstantPara.CurrentR>=CUR_LOW_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IR_MID_GAIN-CalibrationCoeff.IR_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IR_MID_GAIN-Slope;
    WorkingCopyGain.IR_GAIN=Offset+Slope*InstantPara.CurrentR;
  }
  else WorkingCopyGain.IR_GAIN=CalibrationCoeff.IR_LOW_GAIN;
 
  if(InstantPara.CurrentY>=CUR_MID_CAL_POINT)
  {
    if(InstantPara.CurrentY>CUR_HIGH_CAL_POINT)WorkingCopyGain.IY_GAIN=CalibrationCoeff.IY_HIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IY_HIGH_GAIN-CalibrationCoeff.IY_MID_GAIN)/(CUR_HIGH_CAL_POINT-CUR_MID_CAL_POINT);
      Offset=CalibrationCoeff.IY_HIGH_GAIN-CUR_HIGH_CAL_POINT*Slope;
      WorkingCopyGain.IY_GAIN=Offset+Slope*InstantPara.CurrentY;
    }
  }
  else if(InstantPara.CurrentY>=CUR_LOW_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IY_MID_GAIN-CalibrationCoeff.IY_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IY_MID_GAIN-Slope;
    WorkingCopyGain.IY_GAIN=Offset+Slope*InstantPara.CurrentY;
  }
  else WorkingCopyGain.IY_GAIN=CalibrationCoeff.IY_LOW_GAIN;
  if(InstantPara.CurrentB>=CUR_MID_CAL_POINT)
  {
    if(InstantPara.CurrentB>=CUR_HIGH_CAL_POINT)WorkingCopyGain.IB_GAIN=CalibrationCoeff.IB_HIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IB_HIGH_GAIN-CalibrationCoeff.IB_MID_GAIN)/(CUR_HIGH_CAL_POINT-CUR_MID_CAL_POINT);
      Offset=CalibrationCoeff.IB_HIGH_GAIN-CUR_HIGH_CAL_POINT*Slope;
      WorkingCopyGain.IB_GAIN=Offset+Slope*InstantPara.CurrentB;
    }
  }
  else if(InstantPara.CurrentB>=CUR_LOW_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IB_MID_GAIN-CalibrationCoeff.IB_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IB_MID_GAIN-Slope;
    WorkingCopyGain.IB_GAIN=Offset+Slope*InstantPara.CurrentB;
  }
  else WorkingCopyGain.IB_GAIN=CalibrationCoeff.IB_LOW_GAIN;  
   
  // Solar
  if(InstantPara.CurrentRSolar>=CUR_MID_CAL_POINT)
  {
    if(InstantPara.CurrentRSolar>=CUR_HIGH_CAL_POINT)WorkingCopyGain.IR_SOLAR_GAIN=CalibrationCoeff.IR_SOLAR_HIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IR_SOLAR_HIGH_GAIN-CalibrationCoeff.IR_SOLAR_MID_GAIN)/(CUR_HIGH_CAL_POINT-CUR_MID_CAL_POINT);
      Offset=CalibrationCoeff.IR_SOLAR_HIGH_GAIN-CUR_HIGH_CAL_POINT*Slope;
      WorkingCopyGain.IR_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentRSolar;
    }
  }
  else if(InstantPara.CurrentRSolar>=CUR_LOW_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IR_SOLAR_MID_GAIN-CalibrationCoeff.IR_SOLAR_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IR_SOLAR_MID_GAIN-Slope;
    WorkingCopyGain.IR_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentRSolar;
  }
  else WorkingCopyGain.IR_SOLAR_GAIN=CalibrationCoeff.IR_SOLAR_LOW_GAIN;
 
  if(InstantPara.CurrentYSolar>=CUR_MID_CAL_POINT)
  {
    if(InstantPara.CurrentYSolar>CUR_HIGH_CAL_POINT)WorkingCopyGain.IY_SOLAR_GAIN=CalibrationCoeff.IY_SOLAR_HIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IY_SOLAR_HIGH_GAIN-CalibrationCoeff.IY_SOLAR_MID_GAIN)/(CUR_HIGH_CAL_POINT-CUR_MID_CAL_POINT);
      Offset=CalibrationCoeff.IY_SOLAR_HIGH_GAIN-CUR_HIGH_CAL_POINT*Slope;
      WorkingCopyGain.IY_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentYSolar;
    }
  }
  else if(InstantPara.CurrentYSolar>=CUR_LOW_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IY_SOLAR_MID_GAIN-CalibrationCoeff.IY_SOLAR_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IY_SOLAR_MID_GAIN-Slope;
    WorkingCopyGain.IY_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentYSolar;
  }
  else WorkingCopyGain.IY_SOLAR_GAIN=CalibrationCoeff.IY_SOLAR_LOW_GAIN;
  if(InstantPara.CurrentBSolar>=CUR_MID_CAL_POINT)
  {
    if(InstantPara.CurrentBSolar>=CUR_HIGH_CAL_POINT)WorkingCopyGain.IB_SOLAR_GAIN=CalibrationCoeff.IB_SOLAR_HIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IB_SOLAR_HIGH_GAIN-CalibrationCoeff.IB_SOLAR_MID_GAIN)/(CUR_HIGH_CAL_POINT-CUR_MID_CAL_POINT);
      Offset=CalibrationCoeff.IB_SOLAR_HIGH_GAIN-CUR_HIGH_CAL_POINT*Slope;
      WorkingCopyGain.IB_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentBSolar;
    }
  }
  else if(InstantPara.CurrentBSolar>=CUR_LOW_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IB_SOLAR_MID_GAIN-CalibrationCoeff.IB_SOLAR_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IB_SOLAR_MID_GAIN-Slope;
    WorkingCopyGain.IB_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentBSolar;
  }
  else WorkingCopyGain.IB_SOLAR_GAIN=CalibrationCoeff.IB_SOLAR_LOW_GAIN;  

    
  TempChar=(uint8_t)(InstantPara.CurrentR/(float)(CUR_LOW_CAL_POINT));
  if(TempChar>=50)TempChar=50;
  else if(TempChar==0)TempChar=1;
  TempChar=50-TempChar;
  WorkingCopyGain.PR_ALFA=BufferAlfaR[TempChar];
  WorkingCopyGain.PR_BETA=BufferBetaR[TempChar];
  
  TempChar=(uint8_t)(InstantPara.CurrentY/(float)(CUR_LOW_CAL_POINT));
  if(TempChar>=50)TempChar=50;
  else if(TempChar==0)TempChar=1;
  TempChar=50-TempChar;
  WorkingCopyGain.PY_ALFA=BufferAlfaY[TempChar];
  WorkingCopyGain.PY_BETA=BufferBetaY[TempChar];

  TempChar=(uint8_t)(InstantPara.CurrentB/(float)(CUR_LOW_CAL_POINT));
  if(TempChar>=50)TempChar=50;
  else if(TempChar==0)TempChar=1;
  TempChar=50-TempChar;
  WorkingCopyGain.PB_ALFA=BufferAlfaB[TempChar];
  WorkingCopyGain.PB_BETA=BufferBetaB[TempChar];

  // Solar
  //
  TempChar=(uint8_t)(InstantPara.CurrentRSolar/(float)(CUR_LOW_CAL_POINT));
  if(TempChar>=50)TempChar=50;
  else if(TempChar==0)TempChar=1;
  TempChar=50-TempChar;
  WorkingCopyGain.PR_SOLAR_ALFA=BufferAlfaRSolar[TempChar];
  WorkingCopyGain.PR_SOLAR_BETA=BufferBetaRSolar[TempChar];
  
  TempChar=(uint8_t)(InstantPara.CurrentYSolar/(float)(CUR_LOW_CAL_POINT));
  if(TempChar>=50)TempChar=50;
  else if(TempChar==0)TempChar=1;
  TempChar=50-TempChar;
  WorkingCopyGain.PY_SOLAR_ALFA=BufferAlfaYSolar[TempChar];
  WorkingCopyGain.PY_SOLAR_BETA=BufferBetaYSolar[TempChar];

  TempChar=(uint8_t)(InstantPara.CurrentBSolar/(float)(CUR_LOW_CAL_POINT));
  if(TempChar>=50)TempChar=50;
  else if(TempChar==0)TempChar=1;
  TempChar=50-TempChar;
  WorkingCopyGain.PB_SOLAR_ALFA=BufferAlfaBSolar[TempChar];
  WorkingCopyGain.PB_SOLAR_BETA=BufferBetaBSolar[TempChar];
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
    StorageBuffer.SolarRunHourImport++;
    {
      if((InstantPara.CurrentR>0)||(InstantPara.CurrentY>0)||
        (InstantPara.CurrentB>0))
      {
         StorageBuffer.LoadHourImport++;
      }
      if((InstantPara.CurrentRSolar>0)||(InstantPara.CurrentYSolar>0)||
        (InstantPara.CurrentBSolar>0))
      {
         StorageBuffer.SolarLoadHourImport++;
      }
    }
  
    if(DataSaveCounter>=DATA_SAVE_DURATION)
    {
      DataSaveCounter=0;
      InterruptFlag |=INT_DATA_SAVING_EEPROM;
      StorageBuffer.StorageCounter++;
      StorageBuffer.StorageLocation+=MAX_DATA_SAVE_SIZE;
      if(StorageBuffer.StorageLocation >=DATA_SAVE_END_LOC)StorageBuffer.StorageLocation=DATA_SAVE_START_LOC;
      Tempointer=offsetof(struct STORE,StoreCRC);
      StorageBuffer.StoreCRC=CRCCalculation((uint16_t *)&StorageBuffer,Tempointer/2);
      EepromWrite(StorageBuffer.StorageLocation,sizeof(StorageBuffer),EXT_EEPROM,(uint8_t *)&StorageBuffer );
      InterruptFlag &=~INT_DATA_SAVING_EEPROM;
    }
  }
#endif





  
