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
void ProcessLeds();
void ProcessRelays();

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
  MeterInit(); 
  if(StorageBuffer.RunningMode==RUNNING_MODE_IMPORT)StorageBuffer.ImportInterruptions++;
  else StorageBuffer.ExportInterruptions++;

  // UNDONE: This is wrong. We should add running mode for solar separately
  //
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
    ProcessRelays();
    ProcessLeds();
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
  // UNDONE: Solar frequency is not used to set the frequency of the
  // interrupt handler it seems
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
    // Too high frequency can cause interrupts to fire too fast
    //
    float newFrequency = Frequency;
    if (newFrequency > MAX_FREQ_ALLOWED * 100) newFrequency = MAX_FREQ_ALLOWED * 100;
    if (newFrequency < MIN_FREQ_ALLOWED * 100) newFrequency = MIN_FREQ_ALLOWED * 100;
    IntTimerCount.TimerNewValue = (uint32_t)((5000*3750.0)/newFrequency);
  }
}

/*
Inf: Compute frequency from a per-phase zero-crossing state
Inp: FreqMeasState pointer
Ret: Frequency in Hz, or negative if no new measurement
*/
static float ComputeFreqFromState(struct FreqMeasState *s)
{
  if(s->Flag & FF_MEAS_OVER)
  {
    s->Flag &=~FF_MEAS_OVER;
    return (6e10/((float)s->SaveMeasDuration))/100;
  }
  return -1.0f;
}

/* UNDONE: Move this code to Metrology.c
 *
Inf: Per-phase frequency calculation from zero-crossing data
Inp: None
Ret: None
*/
void ProcessPerPhaseFreq(void)
{
  float freq;

  //
  // Grid R/Y/B phase frequencies
  //
  freq = ComputeFreqFromState(&g_FreqState.RPhase);
  if(freq >= 0) InstantPara.FrequencyR = freq;
  if (InstantPara.VolR < FREQ_LIMIT_VOL) InstantPara.FrequencyR = 0;

  freq = ComputeFreqFromState(&g_FreqState.YPhase);
  if(freq >= 0) InstantPara.FrequencyY = freq;
  if (InstantPara.VolY < FREQ_LIMIT_VOL) InstantPara.FrequencyY = 0;


  freq = ComputeFreqFromState(&g_FreqState.BPhase);
  if(freq >= 0) InstantPara.FrequencyB = freq;
  if (InstantPara.VolB < FREQ_LIMIT_VOL) InstantPara.FrequencyB = 0;

  //
  // Solar R/Y/B phase frequencies
  //
  freq = ComputeFreqFromState(&g_FreqState.RSolarPhase);
  if(freq >= 0) InstantPara.FrequencyRSolar = freq;
  if (InstantPara.VolRSolar < FREQ_LIMIT_VOL) InstantPara.FrequencyRSolar = 0;

  freq = ComputeFreqFromState(&g_FreqState.YSolarPhase);
  if(freq >= 0) InstantPara.FrequencyYSolar = freq;
  if (InstantPara.VolYSolar < FREQ_LIMIT_VOL) InstantPara.FrequencyYSolar = 0;

  freq = ComputeFreqFromState(&g_FreqState.BSolarPhase);
  if(freq >= 0) InstantPara.FrequencyBSolar = freq;
  if (InstantPara.VolBSolar < FREQ_LIMIT_VOL) InstantPara.FrequencyBSolar = 0;
}

void SwitchOffContactorRPhaseGridHealthy()
{
    g_DigOutputs.RPhaseGridUnhealthyOutput = true;
}
void SwitchOnContactorRPhaseGridHealthy()
{
    g_DigOutputs.RPhaseGridUnhealthyOutput = false;
}
void SwitchOffContactorYPhaseGridHealthy()
{
    g_DigOutputs.YPhaseGridUnhealthyOutput = true;
}
void SwitchOnContactorYPhaseGridHealthy()
{
    g_DigOutputs.YPhaseGridUnhealthyOutput = false;
}
void SwitchOffContactorBPhaseGridHealthy()
{
    g_DigOutputs.BPhaseGridUnhealthyOutput = true;
}
void SwitchOnContactorBPhaseGridHealthy()
{
    g_DigOutputs.BPhaseGridUnhealthyOutput = false;
}
// UNDONE: Should we have these LEDs turn on even incase of errors?
// Maybe yes. Have these LEDs turn on and let the error led show there is
// some system issue
//
void SwitchOffContactorLoadOnSolar()
{
    g_DigOutputs.LoadOnSolarOutput = false;
    g_LedStatus.LoadOnSolar = false;
}
void SwitchOnContactorLoadOnSolar()
{
    g_DigOutputs.LoadOnSolarOutput = true;
    g_LedStatus.LoadOnSolar = true;
}
void SwitchOffContactorLoadOnGrid()
{
    g_DigOutputs.LoadOffGridOutput = true;
    g_LedStatus.LoadOnGrid = false;
}
void SwitchOnContactorLoadOnGrid()
{
    g_DigOutputs.LoadOffGridOutput = false;
    g_LedStatus.LoadOnGrid = true;
}
void SwitchOffFans()
{
    g_DigOutputs.FansOff = true;
}
void SwitchOnFans()
{
    g_DigOutputs.FansOff = false;
}

// Changes the status of leds based on the internal state set for the LEDs
//
void ProcessLeds()
{
    if (g_LedStatus.Status[0])
    {
        SWITCH_ON_LED1;
    }
    else
    {
        SWITCH_OFF_LED1;
    }
    
    if (g_LedStatus.Status[1])
    {
        SWITCH_ON_LED2;
    }
    else
    {
        SWITCH_OFF_LED2;
    }

    if (g_LedStatus.Status[2])
    {
        SWITCH_ON_LED3;
    }
    else
    {
        SWITCH_OFF_LED3;
    }

    if (g_LedStatus.Status[3])
    {
        SWITCH_ON_LED4;
    }
    else
    {
        SWITCH_OFF_LED4;
    }

    if (g_LedStatus.Status[4])
    {
        SWITCH_ON_LED5;
    }
    else
    {
        SWITCH_OFF_LED5;
    }

    if (g_LedStatus.Status[5])
    {
        SWITCH_ON_LED6;
    }
    else
    {
        SWITCH_OFF_LED6;
    }

    if (g_LedStatus.Status[6])
    {
        SWITCH_ON_LED7;
    }
    else
    {
        SWITCH_OFF_LED7;
    }

    if (g_LedStatus.Status[7])
    {
        SWITCH_ON_LED8;
    }
    else
    {
        SWITCH_OFF_LED8;
    }
}

// Changes the status of relays based on the internal state set for the relays
//
void ProcessRelays()
{
    if (g_DigOutputs.Relays[0])
    {
        TURN_RELAY1_ON;
    }
    else
    {
        TURN_RELAY1_OFF;
    }
    if (g_DigOutputs.Relays[1])
    {
        TURN_RELAY2_ON;
    }
    else
    {
        TURN_RELAY2_OFF;
    }
    if (g_DigOutputs.Relays[2])
    {
        TURN_RELAY3_ON;
    }
    else
    {
        TURN_RELAY3_OFF;
    }
    if (g_DigOutputs.Relays[3])
    {
        TURN_RELAY4_ON;
    }
    else
    {
        TURN_RELAY4_OFF;
    }
    if (g_DigOutputs.Relays[4])
    {
        TURN_RELAY5_ON;
    }
    else
    {
        TURN_RELAY5_OFF;
    }
    if (g_DigOutputs.Relays[5])
    {
        TURN_RELAY6_ON;
    }
    else
    {
        TURN_RELAY6_OFF;
    }
}

// Controls contactor switching logic with safety, called every 1 second.
//
// Design:
// - acknowledgedOn tracks feedback-confirmed contactor state with settle
//   delay. All inter-contactor decisions use acknowledgedOn, providing
//   natural break-before-make without explicit sequencing.
// - settleTimer: feedback must disagree with acknowledgedOn for
//   SETTLE_SECONDS before acknowledgedOn updates.
// - stuckTimer: command must disagree with feedback for STUCK_SECONDS
//   before raising a stuck alarm. Covers both commanded transitions
//   and spontaneous contactor state changes.
//
void ProcessContactors()
{
#define SETTLE_SECONDS    3
#define STUCK_SECONDS     5
#define DRIVE_GAP_SECONDS 2   // Break-before-make gap: min seconds between any
                              // relay turning off and a new relay turning on
                              //
COMPILE_ASSERT(SETTLE_SECONDS < STUCK_SECONDS);
COMPILE_ASSERT(STUCK_SECONDS > DRIVE_GAP_SECONDS);

    enum { IDX_R = 0, IDX_Y, IDX_B, IDX_SOLAR, IDX_LOAD_GRID, IDX_SOLAR_NE, NUM_CONTACTORS };

    // Voltage-health state. HEALTHY → trips into UNDER_VOLTAGE or
    // OVER_VOLTAGE on its own trip threshold; each unhealthy state
    // recovers via its own reset threshold independently.
    //
    enum HealthState { HS_HEALTHY, HS_UNDER_VOLTAGE, HS_OVER_VOLTAGE };

    struct Contactor
    {
        // Configuration (set once)
        //
        uint8_t overVoltParam;
        uint8_t underVoltParam;
        uint8_t overVoltResetParam;
        uint8_t underVoltResetParam;
        uint8_t failDelayParam;
        uint8_t returnDelayParam;
        float* voltage;
        void (*driveOn)(void);
        void (*driveOff)(void);
        bool* feedback;
        bool* alarmStuckOpen;
        bool* alarmStuckClosed;

        // Runtime state
        //
        enum HealthState health;
        uint16_t healthTimer;
        bool wantOn;
        bool acknowledgedOn;   // Feedback-confirmed state (lags feedback by settleTimer)
        bool lastDriven;       // true if relay was driven on last cycle
        uint16_t settleTimer;  // Feedback debounce countdown
        uint16_t stuckTimer;   // Command-vs-feedback mismatch countdown
    };

    static bool initialized = false;

    // NC relay contactors default to on at power-up, NO default to off
    //
    static struct Contactor c[] =
    {
        // [IDX_R] Mains R Phase (NC relay)
        //
        {
            .overVoltParam       = PARA_MAINS_OVER_VOLT,
            .underVoltParam      = PARA_MAINS_UNDER_VOLT,
            .overVoltResetParam  = PARA_MAINS_OVER_VOLT_RESET,
            .underVoltResetParam = PARA_MAINS_UNDER_VOLT_RESET,
            .failDelayParam      = PARA_MAINS_FAIL_DELAY,
            .returnDelayParam    = PARA_MAINS_RETURN_DELAY,
            .voltage             = &InstantPara.VolR,
            .driveOn             = SwitchOnContactorRPhaseGridHealthy,
            .driveOff            = SwitchOffContactorRPhaseGridHealthy,
            .feedback            = &g_DigInputs.MainsRPhaseContactorOn,
            .alarmStuckOpen      = &g_Alarms.MainsRPhaseContactorStuckOpen,
            .alarmStuckClosed    = &g_Alarms.MainsRPhaseContactorStuckClosed,
            .health              = HS_HEALTHY,
            .wantOn              = true,
            .settleTimer         = SETTLE_SECONDS,
            .stuckTimer          = STUCK_SECONDS,
        },
        // [IDX_Y] Mains Y Phase (NC relay)
        //
        {
            .overVoltParam       = PARA_MAINS_OVER_VOLT,
            .underVoltParam      = PARA_MAINS_UNDER_VOLT,
            .overVoltResetParam  = PARA_MAINS_OVER_VOLT_RESET,
            .underVoltResetParam = PARA_MAINS_UNDER_VOLT_RESET,
            .failDelayParam      = PARA_MAINS_FAIL_DELAY,
            .returnDelayParam    = PARA_MAINS_RETURN_DELAY,
            .voltage             = &InstantPara.VolY,
            .driveOn             = SwitchOnContactorYPhaseGridHealthy,
            .driveOff            = SwitchOffContactorYPhaseGridHealthy,
            .feedback            = &g_DigInputs.MainsYPhaseContactorOn,
            .alarmStuckOpen      = &g_Alarms.MainsYPhaseContactorStuckOpen,
            .alarmStuckClosed    = &g_Alarms.MainsYPhaseContactorStuckClosed,
            .health              = HS_HEALTHY,
            .wantOn              = true,
            .settleTimer         = SETTLE_SECONDS,
            .stuckTimer          = STUCK_SECONDS,
        },
        // [IDX_B] Mains B Phase (NC relay)
        //
        {
            .overVoltParam       = PARA_MAINS_OVER_VOLT,
            .underVoltParam      = PARA_MAINS_UNDER_VOLT,
            .overVoltResetParam  = PARA_MAINS_OVER_VOLT_RESET,
            .underVoltResetParam = PARA_MAINS_UNDER_VOLT_RESET,
            .failDelayParam      = PARA_MAINS_FAIL_DELAY,
            .returnDelayParam    = PARA_MAINS_RETURN_DELAY,
            .voltage             = &InstantPara.VolB,
            .driveOn             = SwitchOnContactorBPhaseGridHealthy,
            .driveOff            = SwitchOffContactorBPhaseGridHealthy,
            .feedback            = &g_DigInputs.MainsBPhaseContactorOn,
            .alarmStuckOpen      = &g_Alarms.MainsBPhaseContactorStuckOpen,
            .alarmStuckClosed    = &g_Alarms.MainsBPhaseContactorStuckClosed,
            .health              = HS_HEALTHY,
            .wantOn              = true,
            .settleTimer         = SETTLE_SECONDS,
            .stuckTimer          = STUCK_SECONDS,
        },
        // [IDX_SOLAR] Load On Solar (NO relay — off by default)
        //
        {
            .overVoltParam       = PARA_SOLAR_OVER_VOLT,
            .underVoltParam      = PARA_SOLAR_UNDER_VOLT,
            .overVoltResetParam  = PARA_SOLAR_OVER_VOLT_RESET,
            .underVoltResetParam = PARA_SOLAR_UNDER_VOLT_RESET,
            .failDelayParam      = PARA_SOLAR_FAIL_DELAY,
            .returnDelayParam    = PARA_SOLAR_RETURN_DELAY,
            .voltage             = &InstantPara.VolRSolar,
            .driveOn             = SwitchOnContactorLoadOnSolar,
            .driveOff            = SwitchOffContactorLoadOnSolar,
            .feedback            = &g_DigInputs.LoadOnSolarContactorOn,
            .alarmStuckOpen      = &g_Alarms.LoadOnSolarContactorStuckOpen,
            .alarmStuckClosed    = &g_Alarms.LoadOnSolarContactorStuckClosed,
            .health              = HS_UNDER_VOLTAGE,
            .wantOn              = false,
            .settleTimer         = SETTLE_SECONDS,
            .stuckTimer          = STUCK_SECONDS,
        },
        // [IDX_LOAD_GRID] Load On Grid (NC relay)
        //
        {
            .overVoltParam       = PARA_MAINS_OVER_VOLT,
            .underVoltParam      = PARA_MAINS_UNDER_VOLT,
            .overVoltResetParam  = PARA_MAINS_OVER_VOLT_RESET,
            .underVoltResetParam = PARA_MAINS_UNDER_VOLT_RESET,
            .failDelayParam      = PARA_MAINS_FAIL_DELAY,
            .returnDelayParam    = PARA_MAINS_RETURN_DELAY,
            .voltage             = &InstantPara.VolR,
            .driveOn             = SwitchOnContactorLoadOnGrid,
            .driveOff            = SwitchOffContactorLoadOnGrid,
            .feedback            = &g_DigInputs.LoadOnGridContactorOn,
            .alarmStuckOpen      = &g_Alarms.LoadOnGridContactorStuckOpen,
            .alarmStuckClosed    = &g_Alarms.LoadOnGridContactorStuckClosed,
            .health              = HS_HEALTHY,
            .wantOn              = true,
            .settleTimer         = SETTLE_SECONDS,
            .stuckTimer          = STUCK_SECONDS,
        },
        // [IDX_SOLAR_NE] Solar Neutral/Earth K6 (NO relay — in parallel with K4,
        // shares same relay drive but has separate feedback)
        //
        {
            .overVoltParam       = PARA_SOLAR_OVER_VOLT,
            .underVoltParam      = PARA_SOLAR_UNDER_VOLT,
            .overVoltResetParam  = PARA_SOLAR_OVER_VOLT_RESET,
            .underVoltResetParam = PARA_SOLAR_UNDER_VOLT_RESET,
            .failDelayParam      = PARA_SOLAR_FAIL_DELAY,
            .returnDelayParam    = PARA_SOLAR_RETURN_DELAY,
            .voltage             = &InstantPara.VolRSolar,
            .driveOn             = SwitchOnContactorLoadOnSolar,
            .driveOff            = SwitchOffContactorLoadOnSolar,
            .feedback            = &g_DigInputs.SolarNeutralEarthContactorOn,
            .alarmStuckOpen      = &g_Alarms.SolarNeutralEarthContactorStuckOpen,
            .alarmStuckClosed    = &g_Alarms.SolarNeutralEarthContactorStuckClosed,
            .health              = HS_UNDER_VOLTAGE,
            .wantOn              = false,
            .settleTimer         = SETTLE_SECONDS,
            .stuckTimer          = STUCK_SECONDS,
        },
    };

    if (!initialized)
    {
        initialized = true;
        for (uint8_t i = 0; i < NUM_CONTACTORS; i++)
        {
            c[i].acknowledgedOn = *c[i].feedback;
            c[i].lastDriven = *c[i].feedback;
        }
        c[IDX_R].healthTimer = c[IDX_Y].healthTimer = c[IDX_B].healthTimer =
            c[IDX_LOAD_GRID].healthTimer = CopySetPara[PARA_MAINS_FAIL_DELAY];
        c[IDX_SOLAR].healthTimer = CopySetPara[PARA_SOLAR_RETURN_DELAY];
        c[IDX_SOLAR_NE].healthTimer = CopySetPara[PARA_SOLAR_RETURN_DELAY];
    }

    // ---- Phase 1: Voltage health check with hysteresis ----
    //
    // Three-state machine. Each unhealthy state recovers via its own
    // reset threshold only — clearing UV doesn't require voltage to
    // also be below the OV reset, so an artificial UV (e.g. solar
    // forced UV during grid mode) recovers cleanly when voltage is
    // anywhere within trip range, without needing the tighter inner
    // band a real trip would.
    //
    // DEVNOTE: Data in CopySetPara[] can change at any point of time
    //
    for (uint8_t i = 0; i < NUM_CONTACTORS; i++)
    {
        switch (c[i].health)
        {
            case HS_HEALTHY:
            {
                bool over  = (*c[i].voltage > CopySetPara[c[i].overVoltParam]);
                bool under = (*c[i].voltage < CopySetPara[c[i].underVoltParam]);

                if (over || under)
                {
                    if (c[i].healthTimer)
                        c[i].healthTimer--;
                    if (!c[i].healthTimer)
                    {
                        c[i].health = over ? HS_OVER_VOLTAGE : HS_UNDER_VOLTAGE;
                        c[i].healthTimer = CopySetPara[c[i].returnDelayParam];
                    }
                }
                else
                {
                    c[i].healthTimer = CopySetPara[c[i].failDelayParam];
                }
                break;
            }

            case HS_UNDER_VOLTAGE:
            {
                if (*c[i].voltage >= CopySetPara[c[i].underVoltResetParam])
                {
                    if (c[i].healthTimer)
                        c[i].healthTimer--;
                    if (!c[i].healthTimer)
                    {
                        c[i].health = HS_HEALTHY;
                        c[i].healthTimer = CopySetPara[c[i].failDelayParam];
                    }
                }
                else
                {
                    c[i].healthTimer = CopySetPara[c[i].returnDelayParam];
                }
                break;
            }

            case HS_OVER_VOLTAGE:
            {
                if (*c[i].voltage <= CopySetPara[c[i].overVoltResetParam])
                {
                    if (c[i].healthTimer)
                        c[i].healthTimer--;
                    if (!c[i].healthTimer)
                    {
                        c[i].health = HS_HEALTHY;
                        c[i].healthTimer = CopySetPara[c[i].failDelayParam];
                    }
                }
                else
                {
                    c[i].healthTimer = CopySetPara[c[i].returnDelayParam];
                }
                break;
            }
        }
    }

    // Solar voltage readings are invalid while any mains phase contactor
    // is acknowledged on (grid backfeed contaminates solar measurement).
    // Force UV (not OV) so that on grid leave, recovery is gated only
    // on the under-voltage reset threshold — solar reaches HEALTHY as
    // soon as voltage is anywhere in trip range for returnDelay seconds.
    //
    if (c[IDX_R].acknowledgedOn || c[IDX_Y].acknowledgedOn ||
        c[IDX_B].acknowledgedOn)
    {
        c[IDX_SOLAR].health = HS_UNDER_VOLTAGE;
        c[IDX_SOLAR].healthTimer = CopySetPara[c[IDX_SOLAR].returnDelayParam];
        c[IDX_SOLAR_NE].health = HS_UNDER_VOLTAGE;
        c[IDX_SOLAR_NE].healthTimer = CopySetPara[c[IDX_SOLAR_NE].returnDelayParam];
    }

    // ---- Voltage health status flags (exposed over modbus) ----
    //
    // Under-voltage is only flagged when the phase is present (voltage > 0)
    // but below the set limit. Phase loss (voltage clamped to 0 by Metrology
    // below MIN_VOL_LIMIT) is reported separately and must not trip the
    // under-voltage flag.
    //
    g_voltageHealth.GridRPhaseUnderVoltage =
        (InstantPara.VolR > 0) &&
        (InstantPara.VolR < CopySetPara[PARA_MAINS_UNDER_VOLT]);
    g_voltageHealth.GridRPhaseOverVoltage =
        (InstantPara.VolR > CopySetPara[PARA_MAINS_OVER_VOLT]);
    g_voltageHealth.GridYPhaseUnderVoltage =
        (InstantPara.VolY > 0) &&
        (InstantPara.VolY < CopySetPara[PARA_MAINS_UNDER_VOLT]);
    g_voltageHealth.GridYPhaseOverVoltage =
        (InstantPara.VolY > CopySetPara[PARA_MAINS_OVER_VOLT]);
    g_voltageHealth.GridBPhaseUnderVoltage =
        (InstantPara.VolB > 0) &&
        (InstantPara.VolB < CopySetPara[PARA_MAINS_UNDER_VOLT]);
    g_voltageHealth.GridBPhaseOverVoltage =
        (InstantPara.VolB > CopySetPara[PARA_MAINS_OVER_VOLT]);
    g_voltageHealth.SolarRPhaseUnderVoltage =
        (InstantPara.VolRSolar > 0) &&
        (InstantPara.VolRSolar < CopySetPara[PARA_SOLAR_UNDER_VOLT]);
    g_voltageHealth.SolarRPhaseOverVoltage =
        (InstantPara.VolRSolar > CopySetPara[PARA_SOLAR_OVER_VOLT]);
    g_voltageHealth.SolarYPhaseUnderVoltage =
        (InstantPara.VolYSolar > 0) &&
        (InstantPara.VolYSolar < CopySetPara[PARA_SOLAR_UNDER_VOLT]);
    g_voltageHealth.SolarYPhaseOverVoltage =
        (InstantPara.VolYSolar > CopySetPara[PARA_SOLAR_OVER_VOLT]);
    g_voltageHealth.SolarBPhaseUnderVoltage =
        (InstantPara.VolBSolar > 0) &&
        (InstantPara.VolBSolar < CopySetPara[PARA_SOLAR_UNDER_VOLT]);
    g_voltageHealth.SolarBPhaseOverVoltage =
        (InstantPara.VolBSolar > CopySetPara[PARA_SOLAR_OVER_VOLT]);

    // Phase loss: Metrology clamps any phase voltage below MIN_VOL_LIMIT to
    // exactly 0, so a zero reading indicates a missing / lost phase.
    //
    g_voltageHealth.GridRPhaseLoss  = (InstantPara.VolR      == 0);
    g_voltageHealth.GridYPhaseLoss  = (InstantPara.VolY      == 0);
    g_voltageHealth.GridBPhaseLoss  = (InstantPara.VolB      == 0);
    g_voltageHealth.SolarRPhaseLoss = (InstantPara.VolRSolar == 0);
    g_voltageHealth.SolarYPhaseLoss = (InstantPara.VolYSolar == 0);
    g_voltageHealth.SolarBPhaseLoss = (InstantPara.VolBSolar == 0);

    // ---- Phase 2: Contactor decision logic ----
    //
    // Grid phase contactors: on when healthy AND solar confirmed off AND
    // not disabled. Y and B also require R healthy (R controls the load
    // contactor — without R, load can't be on mains)
    //
    // UNDONE: We can have an optimization that if we detect that K5
    // is stuck in open condition, we can shift load to solar once it's
    // available. For now, not doing this optimization as it will require
    // turning off K5 but remembering that it is stuck.
    //
    c[IDX_R].wantOn = (c[IDX_R].health == HS_HEALTHY) &&
                      !c[IDX_SOLAR_NE].acknowledgedOn &&
                      (g_DisableLoadOnGridSeconds == 0);
    c[IDX_Y].wantOn = (c[IDX_R].health == HS_HEALTHY) &&
                      (c[IDX_Y].health == HS_HEALTHY) &&
                      !c[IDX_SOLAR_NE].acknowledgedOn &&
                      (g_DisableLoadOnGridSeconds == 0);
    c[IDX_B].wantOn = (c[IDX_R].health == HS_HEALTHY) &&
                      (c[IDX_B].health == HS_HEALTHY) &&
                      !c[IDX_SOLAR_NE].acknowledgedOn &&
                      (g_DisableLoadOnGridSeconds == 0);

    // Solar contactor: on when solar healthy AND grid not available or
    // disabled AND DG not running AND no mains/load contactors confirmed on.
    // If 48V absent DG PFC is unreliable, but running DG implies 48V,
    // so no-48V means DG is off.
    // If DG detection is disabled via PARA_DG_DETECT_DISABLED, treat DG
    // as always off (i.e. never block solar on account of DG).
    //
    c[IDX_SOLAR].wantOn =
        (c[IDX_SOLAR].health == HS_HEALTHY) &&
        ((c[IDX_LOAD_GRID].health != HS_HEALTHY) || g_DisableLoadOnGridSeconds) &&
        (CopySetPara[PARA_DG_DETECT_DISABLED] || g_DigInputs.DGOff) &&
        !c[IDX_R].acknowledgedOn &&
        !c[IDX_Y].acknowledgedOn &&
        !c[IDX_B].acknowledgedOn &&
        !c[IDX_LOAD_GRID].acknowledgedOn &&
        (g_DisableLoadOnSolarSeconds == 0);

    // Load on Grid: on when grid healthy AND solar confirmed off
    //
    c[IDX_LOAD_GRID].wantOn = (c[IDX_LOAD_GRID].health == HS_HEALTHY) &&
                              (g_DisableLoadOnGridSeconds == 0) &&
                              !c[IDX_SOLAR].acknowledgedOn;

    if (g_DisableLoadOnGridSeconds)
        g_DisableLoadOnGridSeconds--;
    if (g_DisableLoadOnSolarSeconds)
        g_DisableLoadOnSolarSeconds--;

    // K6 (Solar Neutral/Earth) is in parallel with K4 — always mirrors K4
    //
    c[IDX_SOLAR_NE].wantOn = c[IDX_SOLAR].wantOn;

    // Safety: grid always takes priority over solar. If both want to
    // be on (should not happen via decision logic above, but defends
    // against bugs or memory corruption), force solar off rather than
    // disconnecting everything — keeps load powered via grid.
    //
    if ((c[IDX_SOLAR].wantOn || c[IDX_SOLAR_NE].wantOn) &&
        (c[IDX_R].wantOn || c[IDX_Y].wantOn || c[IDX_B].wantOn ||
         c[IDX_LOAD_GRID].wantOn))
    {
        c[IDX_SOLAR].wantOn = false;
        c[IDX_SOLAR_NE].wantOn = false;
    }

    // ---- Load status flags (exposed over modbus) ----
    //
    // Indicate reasons why load is not on grid or solar. Each flag is
    // true when the corresponding condition is actively preventing load
    // from being on that source.
    //
    g_LoadStatus.LoadOnGrid = c[IDX_LOAD_GRID].acknowledgedOn;
    
    g_LoadStatus.LoadOnGridUserDisabled =
        !g_LoadStatus.LoadOnGrid &&
        (g_DisableLoadOnGridSeconds > 0);

    g_LoadStatus.LoadOnGridDisabledGridRPhaseUnhealthy =
        !g_LoadStatus.LoadOnGrid &&
        (c[IDX_R].health != HS_HEALTHY);

    g_LoadStatus.LoadOnGridDisabledSolarHealthy =
        !g_LoadStatus.LoadOnGrid &&
        c[IDX_SOLAR].acknowledgedOn;
    
    g_LoadStatus.LoadOnSolar = c[IDX_SOLAR].acknowledgedOn &&
                                c[IDX_SOLAR_NE].acknowledgedOn;

    g_LoadStatus.LoadOnSolarUserDisabled =
        !g_LoadStatus.LoadOnSolar &&
        (g_DisableLoadOnSolarSeconds > 0);

    // Only report solar unhealthy when we can actually measure solar
    // voltage (no mains phase contactor on). Otherwise, solar health is
    // forced to UNDER_VOLTAGE by the grid-backfeed override above and
    // does not reflect actual solar voltage state.
    //
    g_LoadStatus.LoadOnSolarDisabledSolarRPhaseUnhealthy =
        !g_LoadStatus.LoadOnSolar &&
        (c[IDX_SOLAR].health != HS_HEALTHY) &&
        !c[IDX_R].acknowledgedOn &&
        !c[IDX_Y].acknowledgedOn &&
        !c[IDX_B].acknowledgedOn;

    g_LoadStatus.LoadOnSolarDisabledGridHealthy =
        !g_LoadStatus.LoadOnSolar &&
        (c[IDX_R].acknowledgedOn ||
         c[IDX_Y].acknowledgedOn ||
         c[IDX_B].acknowledgedOn ||
         c[IDX_LOAD_GRID].acknowledgedOn);

    g_LoadStatus.LoadOnSolarDisabledDGRunning =
        !g_LoadStatus.LoadOnSolar &&
        !CopySetPara[PARA_DG_DETECT_DISABLED] && !g_DigInputs.DGOff;

    // ---- Phase 3: Drive relays with break-before-make gap ----
    //
    // If any contactor is newly turning off this cycle, start a gap timer.
    // New turn-ons are blocked until the gap expires, ensuring the
    // opening contactor has physically disconnected before the closing
    // contactor is energized. Contactors already being driven on are
    // not interrupted.
    //
    static uint16_t driveGapTimer = 0;

    bool anyNewTurnOff = false;
    for (uint8_t i = 0; i < NUM_CONTACTORS; i++)
    {
        if (!c[i].wantOn && c[i].lastDriven)
            anyNewTurnOff = true;
    }
    if (anyNewTurnOff)
        driveGapTimer = DRIVE_GAP_SECONDS;

    for (uint8_t i = 0; i < NUM_CONTACTORS; i++)
    {
        if (!c[i].wantOn)
        {
            c[i].driveOff();
            c[i].lastDriven = false;
        }
        else if (c[i].lastDriven || !driveGapTimer)
        {
            // Already driven on: keep driving. New turn-on: allowed
            // only after gap timer expires.
            //
            c[i].driveOn();
            c[i].lastDriven = true;
        }
        // else: new turn-on blocked — gap timer still active
        //
    }

    if (driveGapTimer)
        driveGapTimer--;

    // ---- Phase 4: Acknowledge feedback with settle delay ----
    // ---- Phase 5: Stuck detection ----
    //
    for (uint8_t i = 0; i < NUM_CONTACTORS; i++)
    {
        // Snapshot feedback once — ReadInputs() runs from ISR and can
        // toggle g_DigInputs between reads within this loop body
        //
        bool fb = *c[i].feedback;

        // Settle timer: update acknowledgedOn only after feedback
        // disagrees with it for SETTLE_SECONDS consecutive seconds
        //
        if (fb != c[i].acknowledgedOn)
        {
            if (c[i].settleTimer)
                c[i].settleTimer--;
            if (!c[i].settleTimer)
            {
                c[i].acknowledgedOn = fb;
                c[i].settleTimer = SETTLE_SECONDS;
            }
        }
        else
        {
            c[i].settleTimer = SETTLE_SECONDS;
        }

        // Stuck detection: command disagrees with feedback for
        // STUCK_SECONDS -> contactor is stuck
        //
        if (c[i].wantOn != fb)
        {
            if (c[i].stuckTimer)
                c[i].stuckTimer--;
            if (!c[i].stuckTimer)
            {
                // fb=true but wantOn=false -> stuck closed
                // fb=false but wantOn=true -> stuck open
                //
                *c[i].alarmStuckClosed = fb;
                *c[i].alarmStuckOpen = !fb;
            }
        }
        else
        {
            c[i].stuckTimer = STUCK_SECONDS;
            *c[i].alarmStuckOpen = false;
            *c[i].alarmStuckClosed = false;
        }
    }

#undef SETTLE_SECONDS
#undef STUCK_SECONDS
#undef DRIVE_GAP_SECONDS
}

// Controls fan relay (Relay 6, NC) based on ambient temperature
// with configurable over/under temperature thresholds and delay timers.
//
// Fan ON:  temperature > over-temp setting for over-temp delay seconds
// Fan OFF: temperature < under-temp setting for under-temp delay seconds
//
// Relay 6 is Normally Closed, so:
//   FansOff = false (relay de-energized) → contact closed → fans ON
//   FansOff = true  (relay energized)    → contact open   → fans OFF
//
// Called once per second from Process1SecOver()
//
void ProcessFanRelay()
{
    static uint16_t delayCounter = 0;
    static bool initialized = false;

    if (!initialized)
    {
        initialized = true;
        delayCounter = CopySetPara[PARA_OVER_TEMPERATURE_DELAY];
        SwitchOffFans();
    }

    if (CopySetPara[PARA_FAN_DISABLED])
    {
        SwitchOffFans();
        delayCounter = CopySetPara[PARA_OVER_TEMPERATURE_DELAY];
        return;
    }

    float temp = InstantPara.AmbientTemperature;

    if (g_DigOutputs.FansOff)
    {
        // Fans are currently OFF — check if temperature exceeds
        // over-temperature threshold
        //
        if (temp > (float)CopySetPara[PARA_OVER_TEMPERATURE])
        {
            if (delayCounter)
            {
                delayCounter--;
            }
            if (!delayCounter)
            {
                SwitchOnFans();
                delayCounter = CopySetPara[PARA_UNDER_TEMPERATURE_DELAY];
            }
        }
        else
        {
            // Temperature is below threshold — reset delay counter
            //
            delayCounter = CopySetPara[PARA_OVER_TEMPERATURE_DELAY];
        }
    }
    else
    {
        // Fans are currently ON — check if temperature drops below
        // under-temperature threshold
        //
        if (temp < (float)CopySetPara[PARA_UNDER_TEMPERATURE])
        {
            if (delayCounter)
            {
                delayCounter--;
            }
            if (!delayCounter)
            {
                SwitchOffFans();
                delayCounter = CopySetPara[PARA_OVER_TEMPERATURE_DELAY];
            }
        }
        else
        {
            // Temperature is above threshold — reset delay counter
            //
            delayCounter = CopySetPara[PARA_UNDER_TEMPERATURE_DELAY];
        }
    }
}

// Detects fan short-circuit and open-circuit faults based on measured
// fan current vs configurable thresholds.
//
// Short circuit: fan current > PARA_FAN_OVER_CURRENT (fans on or off)
// Open circuit:  fan current < PARA_FAN_UNDER_CURRENT when fans are ON
//
// Both fault set and fault clear require the condition to persist for
// FAN_FAULT_DELAY_SECONDS before the state changes.
//
// UNDONE: Make fan fault delay a user-configurable setting.
// UNDONE: Add over/under frequency protection for grid and solar as
// user-configurable settings with hysteresis, similar to voltage
// protection in ProcessContactors().
//
// Called once per second from Process1SecOver()
//
void ProcessFanFaults()
{
#define FAN_FAULT_DELAY_SECONDS 10

    struct FanCheck
    {
        float*  current;
        float   limit;
        bool    faultWhenAbove;  // true: fault if current > limit
                                 // false: fault if current < limit
        bool*   alarm;
        uint8_t timer;
    };

    // Parameters are in mA, InstantPara.FanXCurrent is in Amps
    //
    float overLimit  = (float)CopySetPara[PARA_FAN_OVER_CURRENT]  / 1000.0f;
    float underLimit = (float)CopySetPara[PARA_FAN_UNDER_CURRENT] / 1000.0f;
    bool fansOn = !g_DigOutputs.FansOff;

    static struct FanCheck checks[] =
    {
        { &InstantPara.Fan1Current, 0, true,  &g_FanFaults.Fan1ShortCircuit, FAN_FAULT_DELAY_SECONDS },
        { &InstantPara.Fan1Current, 0, false, &g_FanFaults.Fan1OpenCircuit,  FAN_FAULT_DELAY_SECONDS },
        { &InstantPara.Fan2Current, 0, true,  &g_FanFaults.Fan2ShortCircuit, FAN_FAULT_DELAY_SECONDS },
        { &InstantPara.Fan2Current, 0, false, &g_FanFaults.Fan2OpenCircuit,  FAN_FAULT_DELAY_SECONDS },
    };

    // Update limits each call as settings can change
    //
    for (uint8_t i = 0; i < ARRAY_SIZE(checks); i++)
        checks[i].limit = checks[i].faultWhenAbove ? overLimit : underLimit;

    for (uint8_t i = 0; i < ARRAY_SIZE(checks); i++)
    {
        bool faultCondition;
        if (checks[i].faultWhenAbove)
            faultCondition = (*checks[i].current > checks[i].limit);
        else
            faultCondition = (fansOn && *checks[i].current < checks[i].limit);

        // Steady state: condition matches alarm, nothing to do
        //
        if (faultCondition == *checks[i].alarm)
        {
            checks[i].timer = FAN_FAULT_DELAY_SECONDS;
        }
        // Mismatch: condition disagrees with alarm, count down to flip
        //
        else
        {
            if (checks[i].timer)
                checks[i].timer--;
            if (!checks[i].timer)
            {
                *checks[i].alarm = faultCondition;
                checks[i].timer = FAN_FAULT_DELAY_SECONDS;
            }
        }
    }

#undef FAN_FAULT_DELAY_SECONDS
}

/*
Inf: 1 sec process API
Inp: None
Ret: None
*/
void Process1SecOver(void)
{
  if(PROTECTION_BIT_HIGH ||
     ModbusAdvanceFlagDirectCalibration ||
     FlagDirectCalibration!=0)
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

  // Don't need to turn display on/off when testing after calibration again
  // as we need PDI inspector to check SOCO display
  //
  if (g_testingStatus.TestingModeEnabled && FlagDirectCalibration==0 && pwrDlyFlag
      && !g_ProductInfo.CalibratedFlag)
  {
    // Toggle display between all-on and all-off every 1 second
    // to visually indicate test mode. Skip during calibration so
    // the calibration display screens are not overwritten.
    // Also, enable toggling of display sometime after VERSION has been seen
    // on display.
    // On calibrated devices, fall through to normal display so parameters
    // are visible during test mode.
    //
    static bool testDisplayOn = false;
    testDisplayOn = !testDisplayOn;
    if (testDisplayOn)
    {
        DisplayAllOn();
    }
    else
    {
        DisplayAllOff();
    }
  }
  else if((ParaBlockIndex==0)&&
     (FlagDirectCalibration==0)&&
     (pwrDlyFlag))
  {
    UpdateDisplayIndex();
    DisplayUpdate();
  }
  if(DisplayDebarCounter>0)DisplayDebarCounter--;
 
  // Long press KEY_NEXT causes issue when inside calibration
  // Also, it causes issue when we are editting settings
  //
  if(FlagDirectCalibration==0 &&
      ParaBlockIndex==0 &&
      (!g_testingStatus.TestingModeEnabled || g_ProductInfo.CalibratedFlag))
  {
      CheckAutoScroll();
  }

  if (FlagDirectCalibration==0 &&
      !g_testingStatus.TestingModeEnabled)
  {
      ProcessContactors();
      ProcessFanRelay();
      ProcessFanFaults();
  }
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
    Timer.transfercomplete=0;
    CounterSendComplete=0;
  }
#endif
  InterruptFlag &= ~INT_CYCLE_OVER;
  ProcessFreq();
  ProcessPerPhaseFreq();
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

   // UNDONE: Test all LEDs and switches
   // UNDONE: Test DC Aux and AC Aux
   // UNDONE: Check CT polarity over modbus
   //
   //

   // If in an error state, stall. The Modbus script can read the error
   // code
   //
   if (FlagDirectCalibration == CALIBRATE_ERROR)
   {
       DisplayImproperSettings();
       return;
   }

   if (FlagDirectCalibration == CALIBRATE_END)
   {
       DisplayDoneCal();
       return;
   }

   if((SwPressed==KEY_DIR_CAL)&&(FlagDirectCalibration==0))
   {
      FlagDirectCalibration=CALIBRATE_OUT_1;
      for (uint8_t i = 0; i < NUMBER_OF_RELAYS; i++)
      {
        g_DigOutputs.Relays[i] = false;
      }
   }
   else if ((ModbusAdvanceFlagDirectCalibration) && (FlagDirectCalibration==0))
   {
       SetDefaultCalCoeff();
       DisplaySetXHighPF();
       FlagDirectCalibration = CALIBRATE_DIS_XH_VI;
   }

   bool advanceStateMachineInput = false;
   if ((SwPressed==KEY_NEXT) || (ModbusAdvanceFlagDirectCalibration))
   {
       advanceStateMachineInput = true;
       ModbusAdvanceFlagDirectCalibration = 0;
   }

   if (FlagDirectCalibration >= CALIBRATE_OUT_1 &&
       FlagDirectCalibration <= CALIBRATE_OUT_6)
   {
       // UNDONE: This logic here might not be working
       // Not able to test outputs properly
       //
       uint32_t idx = FlagDirectCalibration - CALIBRATE_OUT_1;
       DisplayOutputX(idx + 1);
       g_DigOutputs.Relays[idx] = true;
       if (SwPressed==KEY_NEXT)
       {
           FlagDirectCalibration++;
       }
   }

   if (FlagDirectCalibration >= CALIBRATE_IN_START &&
        FlagDirectCalibration <= CALIBRATE_IN_8)
   {
      for (uint8_t i = 0; i < NUMBER_OF_RELAYS; i++)
      {
        g_DigOutputs.Relays[i] = false;
      }

       if (FlagDirectCalibration == CALIBRATE_IN_START)
       {
            for (uint8_t i = 0; i < NUMBER_OF_INPUTS; i++)
            {
                if (g_DigInputs.Inputs[i])
                {
                    FlagDirectCalibration = CALIBRATE_ERROR;
                    return;
                }
            }
            FlagDirectCalibration = CALIBRATE_IN_1;
       }

       uint32_t idx = FlagDirectCalibration - CALIBRATE_IN_1;
       DisplayInputX(idx + 1);
       if (g_DigInputs.Inputs[idx])
       {
           FlagDirectCalibration++;
           if (FlagDirectCalibration == CALIBRATE_VOL_CUR_START)
           {
             SetDefaultCalCoeff();
             DisplaySetXHighPF();
             FlagDirectCalibration=CALIBRATE_DIS_XH_VI;
           }
       }
   }

   if(advanceStateMachineInput && (FlagDirectCalibration == CALIBRATE_DIS_XH_VI))
   {
     DisplayCalXHighVI();
     FlagDirectCalibration=CALIBRATE_XH_VI;
     ResetCalSys();
     WorkingCopyGain.VR_GAIN=1.0f;
     WorkingCopyGain.VY_GAIN=1.0f;
     WorkingCopyGain.VB_GAIN=1.0f;
     WorkingCopyGain.IR_GAIN=1.0f;
     WorkingCopyGain.IY_GAIN=1.0f;
     WorkingCopyGain.IB_GAIN=1.0f;
     WorkingCopyGain.PR_ALFA=1;
     WorkingCopyGain.PR_BETA=0;
     WorkingCopyGain.PR_INT_DELAY=0;
     WorkingCopyGain.PY_ALFA=1;
     WorkingCopyGain.PY_BETA=0;
     WorkingCopyGain.PY_INT_DELAY=0;
     WorkingCopyGain.PB_ALFA=1;
     WorkingCopyGain.PB_BETA=0;
     WorkingCopyGain.PB_INT_DELAY=0;

     WorkingCopyGain.VR_SOLAR_GAIN=1.0f;
     WorkingCopyGain.VY_SOLAR_GAIN=1.0f;
     WorkingCopyGain.VB_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IR_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IY_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IB_SOLAR_GAIN=1.0f;
     WorkingCopyGain.PR_SOLAR_ALFA=1;
     WorkingCopyGain.PR_SOLAR_BETA=0;
     WorkingCopyGain.PR_SOLAR_INT_DELAY=0;
     WorkingCopyGain.PY_SOLAR_ALFA=1;
     WorkingCopyGain.PY_SOLAR_BETA=0;
     WorkingCopyGain.PY_SOLAR_INT_DELAY=0;
     WorkingCopyGain.PB_SOLAR_ALFA=1;
     WorkingCopyGain.PB_SOLAR_BETA=0;
     WorkingCopyGain.PB_SOLAR_INT_DELAY=0;

     WorkingCopyGain.FAN1_GAIN=CalibrationCoeff.FAN1_GAIN;
     WorkingCopyGain.FAN2_GAIN=CalibrationCoeff.FAN2_GAIN;

     // Identity grid V_LL phase-shift FIR so V_RY/YB/BR are measured
     // uncorrected during the upcoming XH_PF step (which computes new
     // PH_ERROR values from the calibrated V_LL RMS via the sin formula).
     // Loaded back from flash at XH_PF success.
     //
     WorkingCopyGain.VLL_RY_ALFA = 1.0f; WorkingCopyGain.VLL_RY_BETA = 0.0f; WorkingCopyGain.VLL_RY_INT_DELAY = 0;
     WorkingCopyGain.VLL_YB_ALFA = 1.0f; WorkingCopyGain.VLL_YB_BETA = 0.0f; WorkingCopyGain.VLL_YB_INT_DELAY = 0;
     WorkingCopyGain.VLL_BR_ALFA = 1.0f; WorkingCopyGain.VLL_BR_BETA = 0.0f; WorkingCopyGain.VLL_BR_INT_DELAY = 0;
     WorkingCopyGain.VLL_RY_SOLAR_ALFA = 1.0f; WorkingCopyGain.VLL_RY_SOLAR_BETA = 0.0f; WorkingCopyGain.VLL_RY_SOLAR_INT_DELAY = 0;
     WorkingCopyGain.VLL_YB_SOLAR_ALFA = 1.0f; WorkingCopyGain.VLL_YB_SOLAR_BETA = 0.0f; WorkingCopyGain.VLL_YB_SOLAR_INT_DELAY = 0;
     WorkingCopyGain.VLL_BR_SOLAR_ALFA = 1.0f; WorkingCopyGain.VLL_BR_SOLAR_BETA = 0.0f; WorkingCopyGain.VLL_BR_SOLAR_INT_DELAY = 0;

     // Identity per-channel I_N FIR. The I_N FIR is derived at boot from
     // V_LL + per-phase PF cal data (DeriveNeutralFir), so during cal we
     // run uncorrected. Reloaded at next reboot from refreshed flash.
     //
     WorkingCopyGain.I_N_R_ALFA = 1.0f; WorkingCopyGain.I_N_R_BETA = 0.0f; WorkingCopyGain.I_N_R_INT_DELAY = 0;
     WorkingCopyGain.I_N_Y_ALFA = 1.0f; WorkingCopyGain.I_N_Y_BETA = 0.0f; WorkingCopyGain.I_N_Y_INT_DELAY = 0;
     WorkingCopyGain.I_N_B_ALFA = 1.0f; WorkingCopyGain.I_N_B_BETA = 0.0f; WorkingCopyGain.I_N_B_INT_DELAY = 0;
     WorkingCopyGain.I_N_R_SOLAR_ALFA = 1.0f; WorkingCopyGain.I_N_R_SOLAR_BETA = 0.0f; WorkingCopyGain.I_N_R_SOLAR_INT_DELAY = 0;
     WorkingCopyGain.I_N_Y_SOLAR_ALFA = 1.0f; WorkingCopyGain.I_N_Y_SOLAR_BETA = 0.0f; WorkingCopyGain.I_N_Y_SOLAR_INT_DELAY = 0;
     WorkingCopyGain.I_N_B_SOLAR_ALFA = 1.0f; WorkingCopyGain.I_N_B_SOLAR_BETA = 0.0f; WorkingCopyGain.I_N_B_SOLAR_INT_DELAY = 0;
   }
   if(FlagDirectCalibration == CALIBRATE_XH_VI)
   {
     CalibrationGapCounter++;
     if(CalibrationGapCounter<CAL_ACC_DELAY);

     else if(CalibrationGapCounter<(NO_OF_CAL_ACCUMULATION_VI+CAL_ACC_DELAY))AccumulateDataForCalibration();
     else
     {
        if (!DirectCalibration())
        {
            return;
        }
        FlagDirectCalibration=CALIBRATE_DIS_XH_PF;
        WorkingCopyGain.IR_GAIN=CalibrationCoeff.IR_XHIGH_GAIN;
        WorkingCopyGain.IY_GAIN=CalibrationCoeff.IY_XHIGH_GAIN;
        WorkingCopyGain.IB_GAIN=CalibrationCoeff.IB_XHIGH_GAIN;
        WorkingCopyGain.VR_GAIN=CalibrationCoeff.VR_240_GAIN;
        WorkingCopyGain.VY_GAIN=CalibrationCoeff.VY_240_GAIN;
        WorkingCopyGain.VB_GAIN=CalibrationCoeff.VB_240_GAIN;
        WorkingCopyGain.PR_ALFA=1;
        WorkingCopyGain.PR_BETA=0;
        WorkingCopyGain.PR_INT_DELAY=0;
        WorkingCopyGain.PY_ALFA=1;
        WorkingCopyGain.PY_BETA=0;
        WorkingCopyGain.PY_INT_DELAY=0;
        WorkingCopyGain.PB_ALFA=1;
        WorkingCopyGain.PB_BETA=0;
        WorkingCopyGain.PB_INT_DELAY=0;

        WorkingCopyGain.IR_SOLAR_GAIN=CalibrationCoeff.IR_SOLAR_XHIGH_GAIN;
        WorkingCopyGain.IY_SOLAR_GAIN=CalibrationCoeff.IY_SOLAR_XHIGH_GAIN;
        WorkingCopyGain.IB_SOLAR_GAIN=CalibrationCoeff.IB_SOLAR_XHIGH_GAIN;
        WorkingCopyGain.VR_SOLAR_GAIN=CalibrationCoeff.VR_SOLAR_240_GAIN;
        WorkingCopyGain.VY_SOLAR_GAIN=CalibrationCoeff.VY_SOLAR_240_GAIN;
        WorkingCopyGain.VB_SOLAR_GAIN=CalibrationCoeff.VB_SOLAR_240_GAIN;
        WorkingCopyGain.PR_SOLAR_ALFA=1;
        WorkingCopyGain.PR_SOLAR_BETA=0;
        WorkingCopyGain.PR_SOLAR_INT_DELAY=0;
        WorkingCopyGain.PY_SOLAR_ALFA=1;
        WorkingCopyGain.PY_SOLAR_BETA=0;
        WorkingCopyGain.PY_SOLAR_INT_DELAY=0;
        WorkingCopyGain.PB_SOLAR_ALFA=1;
        WorkingCopyGain.PB_SOLAR_BETA=0;
        WorkingCopyGain.PB_SOLAR_INT_DELAY=0;

        WorkingCopyGain.FAN1_GAIN=CalibrationCoeff.FAN1_GAIN;
        WorkingCopyGain.FAN2_GAIN=CalibrationCoeff.FAN2_GAIN;
     }
   }

   if(FlagDirectCalibration == CALIBRATE_DIS_XH_PF)
   {
     DisplayCalXHighPF();
     FlagDirectCalibration=CALIBRATE_XH_PF;
     ResetCalSys();
   }
   if(FlagDirectCalibration == CALIBRATE_XH_PF)
   {
     CalibrationGapCounter++;
     if(CalibrationGapCounter<CAL_ACC_DELAY);

     else if(CalibrationGapCounter<(NO_OF_CAL_ACCUMULATION_POW+CAL_ACC_DELAY))AccumulateDataForCalibration();
     else
     {
        if (!DirectCalibration())
        {
            return;
        }
        CalPhaseLag(CalibrationCoeff.VLL_RY_PH_ERROR, &WorkingCopyGain.VLL_RY_ALFA, &WorkingCopyGain.VLL_RY_BETA, &WorkingCopyGain.VLL_RY_INT_DELAY);
        CalPhaseLag(CalibrationCoeff.VLL_YB_PH_ERROR, &WorkingCopyGain.VLL_YB_ALFA, &WorkingCopyGain.VLL_YB_BETA, &WorkingCopyGain.VLL_YB_INT_DELAY);
        CalPhaseLag(CalibrationCoeff.VLL_BR_PH_ERROR, &WorkingCopyGain.VLL_BR_ALFA, &WorkingCopyGain.VLL_BR_BETA, &WorkingCopyGain.VLL_BR_INT_DELAY);
        CalPhaseLag(CalibrationCoeff.VLL_RY_SOLAR_PH_ERROR, &WorkingCopyGain.VLL_RY_SOLAR_ALFA, &WorkingCopyGain.VLL_RY_SOLAR_BETA, &WorkingCopyGain.VLL_RY_SOLAR_INT_DELAY);
        CalPhaseLag(CalibrationCoeff.VLL_YB_SOLAR_PH_ERROR, &WorkingCopyGain.VLL_YB_SOLAR_ALFA, &WorkingCopyGain.VLL_YB_SOLAR_BETA, &WorkingCopyGain.VLL_YB_SOLAR_INT_DELAY);
        CalPhaseLag(CalibrationCoeff.VLL_BR_SOLAR_PH_ERROR, &WorkingCopyGain.VLL_BR_SOLAR_ALFA, &WorkingCopyGain.VLL_BR_SOLAR_BETA, &WorkingCopyGain.VLL_BR_SOLAR_INT_DELAY);
        // I_N FIR is derived from VLL_*_PH_ERROR + I*_XHIGH_PH_ERROR which
        // are all freshly stored above; refresh the working copy now so
        // I_N stays correct without a reboot.
        //
        DeriveNeutralFir(CalibrationCoeff.VLL_RY_PH_ERROR, CalibrationCoeff.VLL_BR_PH_ERROR,
                         CalibrationCoeff.IR_XHIGH_PH_ERROR, CalibrationCoeff.IY_XHIGH_PH_ERROR, CalibrationCoeff.IB_XHIGH_PH_ERROR,
                         &WorkingCopyGain.I_N_R_ALFA, &WorkingCopyGain.I_N_R_BETA, &WorkingCopyGain.I_N_R_INT_DELAY,
                         &WorkingCopyGain.I_N_Y_ALFA, &WorkingCopyGain.I_N_Y_BETA, &WorkingCopyGain.I_N_Y_INT_DELAY,
                         &WorkingCopyGain.I_N_B_ALFA, &WorkingCopyGain.I_N_B_BETA, &WorkingCopyGain.I_N_B_INT_DELAY);
        DeriveNeutralFir(CalibrationCoeff.VLL_RY_SOLAR_PH_ERROR, CalibrationCoeff.VLL_BR_SOLAR_PH_ERROR,
                         CalibrationCoeff.IR_SOLAR_XHIGH_PH_ERROR, CalibrationCoeff.IY_SOLAR_XHIGH_PH_ERROR, CalibrationCoeff.IB_SOLAR_XHIGH_PH_ERROR,
                         &WorkingCopyGain.I_N_R_SOLAR_ALFA, &WorkingCopyGain.I_N_R_SOLAR_BETA, &WorkingCopyGain.I_N_R_SOLAR_INT_DELAY,
                         &WorkingCopyGain.I_N_Y_SOLAR_ALFA, &WorkingCopyGain.I_N_Y_SOLAR_BETA, &WorkingCopyGain.I_N_Y_SOLAR_INT_DELAY,
                         &WorkingCopyGain.I_N_B_SOLAR_ALFA, &WorkingCopyGain.I_N_B_SOLAR_BETA, &WorkingCopyGain.I_N_B_SOLAR_INT_DELAY);
        DisplaySetHighPF();
        FlagDirectCalibration=CALIBRATE_DIS_H_VI;
     }
   }

   if(advanceStateMachineInput && (FlagDirectCalibration == CALIBRATE_DIS_H_VI))
   {
     DisplayCalHighVI();
     FlagDirectCalibration=CALIBRATE_H_VI;
     ResetCalSys();
     WorkingCopyGain.IR_GAIN=1.0f;
     WorkingCopyGain.IY_GAIN=1.0f;
     WorkingCopyGain.IB_GAIN=1.0f;
     WorkingCopyGain.VR_GAIN=CalibrationCoeff.VR_240_GAIN;
     WorkingCopyGain.VY_GAIN=CalibrationCoeff.VY_240_GAIN;
     WorkingCopyGain.VB_GAIN=CalibrationCoeff.VB_240_GAIN;
     WorkingCopyGain.PR_ALFA=1;
     WorkingCopyGain.PR_BETA=0;
     WorkingCopyGain.PR_INT_DELAY=0;
     WorkingCopyGain.PY_ALFA=1;
     WorkingCopyGain.PY_BETA=0;
     WorkingCopyGain.PY_INT_DELAY=0;
     WorkingCopyGain.PB_ALFA=1;
     WorkingCopyGain.PB_BETA=0;
     WorkingCopyGain.PB_INT_DELAY=0;

     WorkingCopyGain.IR_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IY_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IB_SOLAR_GAIN=1.0f;
     WorkingCopyGain.VR_SOLAR_GAIN=CalibrationCoeff.VR_SOLAR_240_GAIN;
     WorkingCopyGain.VY_SOLAR_GAIN=CalibrationCoeff.VY_SOLAR_240_GAIN;
     WorkingCopyGain.VB_SOLAR_GAIN=CalibrationCoeff.VB_SOLAR_240_GAIN;
     WorkingCopyGain.PR_SOLAR_ALFA=1;
     WorkingCopyGain.PR_SOLAR_BETA=0;
     WorkingCopyGain.PR_SOLAR_INT_DELAY=0;
     WorkingCopyGain.PY_SOLAR_ALFA=1;
     WorkingCopyGain.PY_SOLAR_BETA=0;
     WorkingCopyGain.PY_SOLAR_INT_DELAY=0;
     WorkingCopyGain.PB_SOLAR_ALFA=1;
     WorkingCopyGain.PB_SOLAR_BETA=0;
     WorkingCopyGain.PB_SOLAR_INT_DELAY=0;

     WorkingCopyGain.FAN1_GAIN=CalibrationCoeff.FAN1_GAIN;
     WorkingCopyGain.FAN2_GAIN=CalibrationCoeff.FAN2_GAIN;
   }
   if(FlagDirectCalibration == CALIBRATE_H_VI)
   {
     CalibrationGapCounter++;
     if(CalibrationGapCounter<CAL_ACC_DELAY);
     
     else if(CalibrationGapCounter<(NO_OF_CAL_ACCUMULATION_VI+CAL_ACC_DELAY))AccumulateDataForCalibration();
     else
     {
        if (!DirectCalibration())
        {
            return;
        }
        FlagDirectCalibration=CALIBRATE_DIS_H_PF;
        WorkingCopyGain.IR_GAIN=CalibrationCoeff.IR_HIGH_GAIN;
        WorkingCopyGain.IY_GAIN=CalibrationCoeff.IY_HIGH_GAIN;
        WorkingCopyGain.IB_GAIN=CalibrationCoeff.IB_HIGH_GAIN;
        WorkingCopyGain.VR_GAIN=CalibrationCoeff.VR_240_GAIN;
        WorkingCopyGain.VY_GAIN=CalibrationCoeff.VY_240_GAIN;
        WorkingCopyGain.VB_GAIN=CalibrationCoeff.VB_240_GAIN;
        WorkingCopyGain.PR_ALFA=1;
        WorkingCopyGain.PR_BETA=0;
        WorkingCopyGain.PR_INT_DELAY=0;
        WorkingCopyGain.PY_ALFA=1;
        WorkingCopyGain.PY_BETA=0;
        WorkingCopyGain.PY_INT_DELAY=0;
        WorkingCopyGain.PB_ALFA=1;
        WorkingCopyGain.PB_BETA=0;
        WorkingCopyGain.PB_INT_DELAY=0;

        WorkingCopyGain.IR_SOLAR_GAIN=CalibrationCoeff.IR_SOLAR_HIGH_GAIN;
        WorkingCopyGain.IY_SOLAR_GAIN=CalibrationCoeff.IY_SOLAR_HIGH_GAIN;
        WorkingCopyGain.IB_SOLAR_GAIN=CalibrationCoeff.IB_SOLAR_HIGH_GAIN;
        WorkingCopyGain.VR_SOLAR_GAIN=CalibrationCoeff.VR_SOLAR_240_GAIN;
        WorkingCopyGain.VY_SOLAR_GAIN=CalibrationCoeff.VY_SOLAR_240_GAIN;
        WorkingCopyGain.VB_SOLAR_GAIN=CalibrationCoeff.VB_SOLAR_240_GAIN;
        WorkingCopyGain.PR_SOLAR_ALFA=1;
        WorkingCopyGain.PR_SOLAR_BETA=0;
        WorkingCopyGain.PR_SOLAR_INT_DELAY=0;
        WorkingCopyGain.PY_SOLAR_ALFA=1;
        WorkingCopyGain.PY_SOLAR_BETA=0;
        WorkingCopyGain.PY_SOLAR_INT_DELAY=0;
        WorkingCopyGain.PB_SOLAR_ALFA=1;
        WorkingCopyGain.PB_SOLAR_BETA=0;
        WorkingCopyGain.PB_SOLAR_INT_DELAY=0;

        WorkingCopyGain.FAN1_GAIN=CalibrationCoeff.FAN1_GAIN;
        WorkingCopyGain.FAN2_GAIN=CalibrationCoeff.FAN2_GAIN;
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
        if (!DirectCalibration())
        {
            return;
        }
        DisplaySetMidPF();
        FlagDirectCalibration=CALIBRATE_DIS_M_VI;
     }
   }
   if(advanceStateMachineInput && (FlagDirectCalibration == CALIBRATE_DIS_M_VI))
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
     WorkingCopyGain.PR_INT_DELAY=0;
     WorkingCopyGain.PY_ALFA=1;
     WorkingCopyGain.PY_BETA=0;
     WorkingCopyGain.PY_INT_DELAY=0;
     WorkingCopyGain.PB_ALFA=1;
     WorkingCopyGain.PB_BETA=0;
     WorkingCopyGain.PB_INT_DELAY=0;

     WorkingCopyGain.IR_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IY_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IB_SOLAR_GAIN=1.0f;
     WorkingCopyGain.VR_SOLAR_GAIN=CalibrationCoeff.VR_SOLAR_240_GAIN;
     WorkingCopyGain.VY_SOLAR_GAIN=CalibrationCoeff.VY_SOLAR_240_GAIN;
     WorkingCopyGain.VB_SOLAR_GAIN=CalibrationCoeff.VB_SOLAR_240_GAIN;
     WorkingCopyGain.PR_SOLAR_ALFA=1;
     WorkingCopyGain.PR_SOLAR_BETA=0;
     WorkingCopyGain.PR_SOLAR_INT_DELAY=0;
     WorkingCopyGain.PY_SOLAR_ALFA=1;
     WorkingCopyGain.PY_SOLAR_BETA=0;
     WorkingCopyGain.PY_SOLAR_INT_DELAY=0;
     WorkingCopyGain.PB_SOLAR_ALFA=1;
     WorkingCopyGain.PB_SOLAR_BETA=0;
     WorkingCopyGain.PB_SOLAR_INT_DELAY=0;

     WorkingCopyGain.FAN1_GAIN=CalibrationCoeff.FAN1_GAIN;
     WorkingCopyGain.FAN2_GAIN=CalibrationCoeff.FAN2_GAIN;
   }
   if(FlagDirectCalibration == CALIBRATE_M_VI)
   {
     CalibrationGapCounter++;
     if(CalibrationGapCounter<CAL_ACC_DELAY);
     
     else if(CalibrationGapCounter<(NO_OF_CAL_ACCUMULATION_VI+CAL_ACC_DELAY))AccumulateDataForCalibration();
     else
     {
        if (!DirectCalibration())
        {
            return;
        }
        
        FlagDirectCalibration=CALIBRATE_DIS_M_PF;
        WorkingCopyGain.IR_GAIN=CalibrationCoeff.IR_MID_GAIN;
        WorkingCopyGain.IY_GAIN=CalibrationCoeff.IY_MID_GAIN;
        WorkingCopyGain.IB_GAIN=CalibrationCoeff.IB_MID_GAIN;
        WorkingCopyGain.VR_GAIN=CalibrationCoeff.VR_240_GAIN;
        WorkingCopyGain.VY_GAIN=CalibrationCoeff.VY_240_GAIN;
        WorkingCopyGain.VB_GAIN=CalibrationCoeff.VB_240_GAIN;
        WorkingCopyGain.PR_ALFA=1;
        WorkingCopyGain.PR_BETA=0;
        WorkingCopyGain.PR_INT_DELAY=0;
        WorkingCopyGain.PY_ALFA=1;
        WorkingCopyGain.PY_BETA=0;
        WorkingCopyGain.PY_INT_DELAY=0;
        WorkingCopyGain.PB_ALFA=1;
        WorkingCopyGain.PB_BETA=0;
        WorkingCopyGain.PB_INT_DELAY=0;

        WorkingCopyGain.IR_SOLAR_GAIN=CalibrationCoeff.IR_SOLAR_MID_GAIN;
        WorkingCopyGain.IY_SOLAR_GAIN=CalibrationCoeff.IY_SOLAR_MID_GAIN;
        WorkingCopyGain.IB_SOLAR_GAIN=CalibrationCoeff.IB_SOLAR_MID_GAIN;
        WorkingCopyGain.VR_SOLAR_GAIN=CalibrationCoeff.VR_SOLAR_240_GAIN;
        WorkingCopyGain.VY_SOLAR_GAIN=CalibrationCoeff.VY_SOLAR_240_GAIN;
        WorkingCopyGain.VB_SOLAR_GAIN=CalibrationCoeff.VB_SOLAR_240_GAIN;
        WorkingCopyGain.PR_SOLAR_ALFA=1;
        WorkingCopyGain.PR_SOLAR_BETA=0;
        WorkingCopyGain.PR_SOLAR_INT_DELAY=0;
        WorkingCopyGain.PY_SOLAR_ALFA=1;
        WorkingCopyGain.PY_SOLAR_BETA=0;
        WorkingCopyGain.PY_SOLAR_INT_DELAY=0;
        WorkingCopyGain.PB_SOLAR_ALFA=1;
        WorkingCopyGain.PB_SOLAR_BETA=0;
        WorkingCopyGain.PB_SOLAR_INT_DELAY=0;

        WorkingCopyGain.FAN1_GAIN=CalibrationCoeff.FAN1_GAIN;
        WorkingCopyGain.FAN2_GAIN=CalibrationCoeff.FAN2_GAIN;
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
        if (!DirectCalibration())
        {
            return;
        }
        DisplaySetLowPF();
        FlagDirectCalibration=CALIBRATE_DIS_L_VI;
     }
   }
   
   if(advanceStateMachineInput && (FlagDirectCalibration == CALIBRATE_DIS_L_VI))
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
     WorkingCopyGain.PR_INT_DELAY=0;
     WorkingCopyGain.PY_ALFA=1;
     WorkingCopyGain.PY_BETA=0;
     WorkingCopyGain.PY_INT_DELAY=0;
     WorkingCopyGain.PB_ALFA=1;
     WorkingCopyGain.PB_BETA=0;
     WorkingCopyGain.PB_INT_DELAY=0;

     WorkingCopyGain.IR_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IY_SOLAR_GAIN=1.0f;
     WorkingCopyGain.IB_SOLAR_GAIN=1.0f;
     WorkingCopyGain.VR_SOLAR_GAIN=CalibrationCoeff.VR_SOLAR_240_GAIN;
     WorkingCopyGain.VY_SOLAR_GAIN=CalibrationCoeff.VY_SOLAR_240_GAIN;
     WorkingCopyGain.VB_SOLAR_GAIN=CalibrationCoeff.VB_SOLAR_240_GAIN;
     WorkingCopyGain.PR_SOLAR_ALFA=1;
     WorkingCopyGain.PR_SOLAR_BETA=0;
     WorkingCopyGain.PR_SOLAR_INT_DELAY=0;
     WorkingCopyGain.PY_SOLAR_ALFA=1;
     WorkingCopyGain.PY_SOLAR_BETA=0;
     WorkingCopyGain.PY_SOLAR_INT_DELAY=0;
     WorkingCopyGain.PB_SOLAR_ALFA=1;
     WorkingCopyGain.PB_SOLAR_BETA=0;
     WorkingCopyGain.PB_SOLAR_INT_DELAY=0;

     WorkingCopyGain.FAN1_GAIN=CalibrationCoeff.FAN1_GAIN;
     WorkingCopyGain.FAN2_GAIN=CalibrationCoeff.FAN2_GAIN;  
   }
   if(FlagDirectCalibration == CALIBRATE_L_VI)
   {
     CalibrationGapCounter++;
     if(CalibrationGapCounter<CAL_ACC_DELAY);
     
     else if(CalibrationGapCounter<(NO_OF_CAL_ACCUMULATION_VI+CAL_ACC_DELAY))AccumulateDataForCalibration();
     else
     {
        if (!DirectCalibration())
        {
            return;
        }

        FlagDirectCalibration=CALIBRATE_DIS_L_PF;
        WorkingCopyGain.IR_GAIN=CalibrationCoeff.IR_LOW_GAIN;
        WorkingCopyGain.IY_GAIN=CalibrationCoeff.IY_LOW_GAIN;
        WorkingCopyGain.IB_GAIN=CalibrationCoeff.IB_LOW_GAIN;
        WorkingCopyGain.VR_GAIN=CalibrationCoeff.VR_240_GAIN;
        WorkingCopyGain.VY_GAIN=CalibrationCoeff.VY_240_GAIN;
        WorkingCopyGain.VB_GAIN=CalibrationCoeff.VB_240_GAIN;
        WorkingCopyGain.PR_ALFA=1;
        WorkingCopyGain.PR_BETA=0;
        WorkingCopyGain.PR_INT_DELAY=0;
        WorkingCopyGain.PY_ALFA=1;
        WorkingCopyGain.PY_BETA=0;
        WorkingCopyGain.PY_INT_DELAY=0;
        WorkingCopyGain.PB_ALFA=1;
        WorkingCopyGain.PB_BETA=0;
        WorkingCopyGain.PB_INT_DELAY=0;

        WorkingCopyGain.IR_SOLAR_GAIN=CalibrationCoeff.IR_SOLAR_LOW_GAIN;
        WorkingCopyGain.IY_SOLAR_GAIN=CalibrationCoeff.IY_SOLAR_LOW_GAIN;
        WorkingCopyGain.IB_SOLAR_GAIN=CalibrationCoeff.IB_SOLAR_LOW_GAIN;
        WorkingCopyGain.VR_SOLAR_GAIN=CalibrationCoeff.VR_SOLAR_240_GAIN;
        WorkingCopyGain.VY_SOLAR_GAIN=CalibrationCoeff.VY_SOLAR_240_GAIN;
        WorkingCopyGain.VB_SOLAR_GAIN=CalibrationCoeff.VB_SOLAR_240_GAIN;
        WorkingCopyGain.PR_SOLAR_ALFA=1;
        WorkingCopyGain.PR_SOLAR_BETA=0;
        WorkingCopyGain.PR_SOLAR_INT_DELAY=0;
        WorkingCopyGain.PY_SOLAR_ALFA=1;
        WorkingCopyGain.PY_SOLAR_BETA=0;
        WorkingCopyGain.PY_SOLAR_INT_DELAY=0;
        WorkingCopyGain.PB_SOLAR_ALFA=1;
        WorkingCopyGain.PB_SOLAR_BETA=0;
        WorkingCopyGain.PB_SOLAR_INT_DELAY=0;

        WorkingCopyGain.FAN1_GAIN=CalibrationCoeff.FAN1_GAIN;
        WorkingCopyGain.FAN2_GAIN=CalibrationCoeff.FAN2_GAIN;
     }
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
        if (!DirectCalibration())
        {
            return;
        }
        DisplaySetFan();
        FlagDirectCalibration = CALIBRATE_DIS_FAN;
     }
   }

   if(advanceStateMachineInput && (FlagDirectCalibration == CALIBRATE_DIS_FAN))
   {
     DisplayCalFan();
     FlagDirectCalibration=CALIBRATE_FAN;
     ResetCalSys();
     WorkingCopyGain.FAN1_GAIN=1.0f;
     WorkingCopyGain.FAN2_GAIN=1.0f;
   }

   if(FlagDirectCalibration == CALIBRATE_FAN)
   {
     CalibrationGapCounter++;
     if(CalibrationGapCounter<CAL_ACC_DELAY);

     else if(CalibrationGapCounter<(NO_OF_CAL_ACCUMULATION_VI+CAL_ACC_DELAY))AccumulateDataForCalibration();
     else
     {
        if (!DirectCalibration())
        {
            return;
        }
        WorkingCopyGain.FAN1_GAIN=CalibrationCoeff.FAN1_GAIN;
        WorkingCopyGain.FAN2_GAIN=CalibrationCoeff.FAN2_GAIN;
        FlagDirectCalibration = CALIBRATE_END;
     }
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
  // Builds the per-current phase-error lookup table using piecewise-linear
  // interpolation across three segments with progressively finer step sizes:
  //   slots [0,         StepXHigh):              [HIGH,  XHIGH] in CUR_XHIGH_STEP
  //   slots [StepXHigh, StepXHigh+StepHigh):     [MID,   HIGH ] in CUR_HIGH_STEP
  //   slots [...,       StepXHigh+StepHigh+StepLow]: [LOW, MID] in CUR_LOW_STEP
  // Buffer is indexed in decreasing-current order (slot 0 = highest current).
  // Currents above XHIGH saturate to slot 0; below LOW saturate to last slot.
  //
  uint8_t  i;
  StepXHigh=(uint8_t)((CUR_XHIGH_CAL_POINT-CUR_HIGH_CAL_POINT)/CUR_XHIGH_STEP);
  StepHigh =(uint8_t)((CUR_HIGH_CAL_POINT -CUR_MID_CAL_POINT )/CUR_HIGH_STEP );
  StepLow  =(uint8_t)((CUR_MID_CAL_POINT  -CUR_LOW_CAL_POINT )/CUR_LOW_STEP  );

  for(i=0;i<StepXHigh;i++)BufferBetaR[i]=CalibrationCoeff.IR_XHIGH_PH_ERROR+(CalibrationCoeff.IR_HIGH_PH_ERROR-CalibrationCoeff.IR_XHIGH_PH_ERROR)*i/StepXHigh;
  for(i=0;i<StepXHigh;i++)BufferBetaY[i]=CalibrationCoeff.IY_XHIGH_PH_ERROR+(CalibrationCoeff.IY_HIGH_PH_ERROR-CalibrationCoeff.IY_XHIGH_PH_ERROR)*i/StepXHigh;
  for(i=0;i<StepXHigh;i++)BufferBetaB[i]=CalibrationCoeff.IB_XHIGH_PH_ERROR+(CalibrationCoeff.IB_HIGH_PH_ERROR-CalibrationCoeff.IB_XHIGH_PH_ERROR)*i/StepXHigh;

  for(i=0;i<StepHigh;i++)BufferBetaR[i+StepXHigh]=CalibrationCoeff.IR_HIGH_PH_ERROR+(CalibrationCoeff.IR_MID_PH_ERROR-CalibrationCoeff.IR_HIGH_PH_ERROR)*i/StepHigh;
  for(i=0;i<StepHigh;i++)BufferBetaY[i+StepXHigh]=CalibrationCoeff.IY_HIGH_PH_ERROR+(CalibrationCoeff.IY_MID_PH_ERROR-CalibrationCoeff.IY_HIGH_PH_ERROR)*i/StepHigh;
  for(i=0;i<StepHigh;i++)BufferBetaB[i+StepXHigh]=CalibrationCoeff.IB_HIGH_PH_ERROR+(CalibrationCoeff.IB_MID_PH_ERROR-CalibrationCoeff.IB_HIGH_PH_ERROR)*i/StepHigh;

  for(i=0;i<StepLow+1;i++)BufferBetaR[i+StepXHigh+StepHigh]=CalibrationCoeff.IR_MID_PH_ERROR+(CalibrationCoeff.IR_LOW_PH_ERROR-CalibrationCoeff.IR_MID_PH_ERROR)*i/StepLow;
  for(i=0;i<StepLow+1;i++)BufferBetaY[i+StepXHigh+StepHigh]=CalibrationCoeff.IY_MID_PH_ERROR+(CalibrationCoeff.IY_LOW_PH_ERROR-CalibrationCoeff.IY_MID_PH_ERROR)*i/StepLow;
  for(i=0;i<StepLow+1;i++)BufferBetaB[i+StepXHigh+StepHigh]=CalibrationCoeff.IB_MID_PH_ERROR+(CalibrationCoeff.IB_LOW_PH_ERROR-CalibrationCoeff.IB_MID_PH_ERROR)*i/StepLow;

  for(i=0;i<50;i++)CalPF(BufferBetaR[i],&BufferAlfaR[i],&BufferBetaR[i],&BufferIntDelayR[i]);
  for(i=0;i<50;i++)CalPF(BufferBetaY[i],&BufferAlfaY[i],&BufferBetaY[i],&BufferIntDelayY[i]);
  for(i=0;i<50;i++)CalPF(BufferBetaB[i],&BufferAlfaB[i],&BufferBetaB[i],&BufferIntDelayB[i]);

  for(i=0;i<StepXHigh;i++)BufferBetaRSolar[i]=CalibrationCoeff.IR_SOLAR_XHIGH_PH_ERROR+(CalibrationCoeff.IR_SOLAR_HIGH_PH_ERROR-CalibrationCoeff.IR_SOLAR_XHIGH_PH_ERROR)*i/StepXHigh;
  for(i=0;i<StepXHigh;i++)BufferBetaYSolar[i]=CalibrationCoeff.IY_SOLAR_XHIGH_PH_ERROR+(CalibrationCoeff.IY_SOLAR_HIGH_PH_ERROR-CalibrationCoeff.IY_SOLAR_XHIGH_PH_ERROR)*i/StepXHigh;
  for(i=0;i<StepXHigh;i++)BufferBetaBSolar[i]=CalibrationCoeff.IB_SOLAR_XHIGH_PH_ERROR+(CalibrationCoeff.IB_SOLAR_HIGH_PH_ERROR-CalibrationCoeff.IB_SOLAR_XHIGH_PH_ERROR)*i/StepXHigh;

  for(i=0;i<StepHigh;i++)BufferBetaRSolar[i+StepXHigh]=CalibrationCoeff.IR_SOLAR_HIGH_PH_ERROR+(CalibrationCoeff.IR_SOLAR_MID_PH_ERROR-CalibrationCoeff.IR_SOLAR_HIGH_PH_ERROR)*i/StepHigh;
  for(i=0;i<StepHigh;i++)BufferBetaYSolar[i+StepXHigh]=CalibrationCoeff.IY_SOLAR_HIGH_PH_ERROR+(CalibrationCoeff.IY_SOLAR_MID_PH_ERROR-CalibrationCoeff.IY_SOLAR_HIGH_PH_ERROR)*i/StepHigh;
  for(i=0;i<StepHigh;i++)BufferBetaBSolar[i+StepXHigh]=CalibrationCoeff.IB_SOLAR_HIGH_PH_ERROR+(CalibrationCoeff.IB_SOLAR_MID_PH_ERROR-CalibrationCoeff.IB_SOLAR_HIGH_PH_ERROR)*i/StepHigh;

  for(i=0;i<StepLow+1;i++)BufferBetaRSolar[i+StepXHigh+StepHigh]=CalibrationCoeff.IR_SOLAR_MID_PH_ERROR+(CalibrationCoeff.IR_SOLAR_LOW_PH_ERROR-CalibrationCoeff.IR_SOLAR_MID_PH_ERROR)*i/StepLow;
  for(i=0;i<StepLow+1;i++)BufferBetaYSolar[i+StepXHigh+StepHigh]=CalibrationCoeff.IY_SOLAR_MID_PH_ERROR+(CalibrationCoeff.IY_SOLAR_LOW_PH_ERROR-CalibrationCoeff.IY_SOLAR_MID_PH_ERROR)*i/StepLow;
  for(i=0;i<StepLow+1;i++)BufferBetaBSolar[i+StepXHigh+StepHigh]=CalibrationCoeff.IB_SOLAR_MID_PH_ERROR+(CalibrationCoeff.IB_SOLAR_LOW_PH_ERROR-CalibrationCoeff.IB_SOLAR_MID_PH_ERROR)*i/StepLow;

  for(i=0;i<50;i++)CalPF(BufferBetaRSolar[i],&BufferAlfaRSolar[i],&BufferBetaRSolar[i],&BufferIntDelayRSolar[i]);
  for(i=0;i<50;i++)CalPF(BufferBetaYSolar[i],&BufferAlfaYSolar[i],&BufferBetaYSolar[i],&BufferIntDelayYSolar[i]);
  for(i=0;i<50;i++)CalPF(BufferBetaBSolar[i],&BufferAlfaBSolar[i],&BufferBetaBSolar[i],&BufferIntDelayBSolar[i]);

}
  
// Maps a measured current value to a phase-error lookup buffer index.
// Buffer layout (decreasing current order, populated in FillCurrentGainArray):
//   slot [0,                            StepXHigh)            : XHIGH segment, CUR_XHIGH_STEP granularity
//   slot [StepXHigh,                    StepXHigh+StepHigh)   : HIGH  segment, CUR_HIGH_STEP  granularity
//   slot [StepXHigh+StepHigh,           StepXHigh+StepHigh+StepLow] : LOW segment, CUR_LOW_STEP  granularity
// Returns 0 for currents at or above XHIGH (saturated to highest knot)
// and the last LOW-segment slot for currents below the starting current.
//
static uint8_t PhaseBufferIndex(float I)
{
  if (I >= CUR_XHIGH_CAL_POINT) return 0;
  if (I >= CUR_HIGH_CAL_POINT)  return (uint8_t)((CUR_XHIGH_CAL_POINT - I) / CUR_XHIGH_STEP);
  if (I >= CUR_MID_CAL_POINT)   return StepXHigh + (uint8_t)((CUR_HIGH_CAL_POINT - I) / CUR_HIGH_STEP);
  if (I >= CUR_LOW_CAL_POINT)   return StepXHigh + StepHigh + (uint8_t)((CUR_MID_CAL_POINT - I) / CUR_LOW_STEP);
  return StepXHigh + StepHigh + StepLow;
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
  static bool initialized = false;

  if (!initialized)
  {
    WorkingCopyGain.VR_GAIN=CalibrationCoeff.VR_240_GAIN;
    WorkingCopyGain.VY_GAIN=CalibrationCoeff.VY_240_GAIN;
    WorkingCopyGain.VB_GAIN=CalibrationCoeff.VB_240_GAIN;
    CalPhaseLag(CalibrationCoeff.VLL_RY_PH_ERROR, &WorkingCopyGain.VLL_RY_ALFA, &WorkingCopyGain.VLL_RY_BETA, &WorkingCopyGain.VLL_RY_INT_DELAY);
    CalPhaseLag(CalibrationCoeff.VLL_YB_PH_ERROR, &WorkingCopyGain.VLL_YB_ALFA, &WorkingCopyGain.VLL_YB_BETA, &WorkingCopyGain.VLL_YB_INT_DELAY);
    CalPhaseLag(CalibrationCoeff.VLL_BR_PH_ERROR, &WorkingCopyGain.VLL_BR_ALFA, &WorkingCopyGain.VLL_BR_BETA, &WorkingCopyGain.VLL_BR_INT_DELAY);
    WorkingCopyGain.VR_SOLAR_GAIN=CalibrationCoeff.VR_SOLAR_240_GAIN;
    WorkingCopyGain.VY_SOLAR_GAIN=CalibrationCoeff.VY_SOLAR_240_GAIN;
    WorkingCopyGain.VB_SOLAR_GAIN=CalibrationCoeff.VB_SOLAR_240_GAIN;
    CalPhaseLag(CalibrationCoeff.VLL_RY_SOLAR_PH_ERROR, &WorkingCopyGain.VLL_RY_SOLAR_ALFA, &WorkingCopyGain.VLL_RY_SOLAR_BETA, &WorkingCopyGain.VLL_RY_SOLAR_INT_DELAY);
    CalPhaseLag(CalibrationCoeff.VLL_YB_SOLAR_PH_ERROR, &WorkingCopyGain.VLL_YB_SOLAR_ALFA, &WorkingCopyGain.VLL_YB_SOLAR_BETA, &WorkingCopyGain.VLL_YB_SOLAR_INT_DELAY);
    CalPhaseLag(CalibrationCoeff.VLL_BR_SOLAR_PH_ERROR, &WorkingCopyGain.VLL_BR_SOLAR_ALFA, &WorkingCopyGain.VLL_BR_SOLAR_BETA, &WorkingCopyGain.VLL_BR_SOLAR_INT_DELAY);
    WorkingCopyGain.FAN1_GAIN=CalibrationCoeff.FAN1_GAIN;
    WorkingCopyGain.FAN2_GAIN=CalibrationCoeff.FAN2_GAIN;

    // Derive per-channel I_N FIR from existing V_LL + per-phase PF cal
    // data. No dedicated I_N cal step; see DeriveNeutralFir() in Comm.c
    // for the telescoping math.
    //
    DeriveNeutralFir(CalibrationCoeff.VLL_RY_PH_ERROR, CalibrationCoeff.VLL_BR_PH_ERROR,
                     CalibrationCoeff.IR_XHIGH_PH_ERROR, CalibrationCoeff.IY_XHIGH_PH_ERROR, CalibrationCoeff.IB_XHIGH_PH_ERROR,
                     &WorkingCopyGain.I_N_R_ALFA, &WorkingCopyGain.I_N_R_BETA, &WorkingCopyGain.I_N_R_INT_DELAY,
                     &WorkingCopyGain.I_N_Y_ALFA, &WorkingCopyGain.I_N_Y_BETA, &WorkingCopyGain.I_N_Y_INT_DELAY,
                     &WorkingCopyGain.I_N_B_ALFA, &WorkingCopyGain.I_N_B_BETA, &WorkingCopyGain.I_N_B_INT_DELAY);
    DeriveNeutralFir(CalibrationCoeff.VLL_RY_SOLAR_PH_ERROR, CalibrationCoeff.VLL_BR_SOLAR_PH_ERROR,
                     CalibrationCoeff.IR_SOLAR_XHIGH_PH_ERROR, CalibrationCoeff.IY_SOLAR_XHIGH_PH_ERROR, CalibrationCoeff.IB_SOLAR_XHIGH_PH_ERROR,
                     &WorkingCopyGain.I_N_R_SOLAR_ALFA, &WorkingCopyGain.I_N_R_SOLAR_BETA, &WorkingCopyGain.I_N_R_SOLAR_INT_DELAY,
                     &WorkingCopyGain.I_N_Y_SOLAR_ALFA, &WorkingCopyGain.I_N_Y_SOLAR_BETA, &WorkingCopyGain.I_N_Y_SOLAR_INT_DELAY,
                     &WorkingCopyGain.I_N_B_SOLAR_ALFA, &WorkingCopyGain.I_N_B_SOLAR_BETA, &WorkingCopyGain.I_N_B_SOLAR_INT_DELAY);
    initialized = true;
  }

  if(InstantPara.CurrentR>=CUR_HIGH_CAL_POINT)
  {
    if(InstantPara.CurrentR>=CUR_XHIGH_CAL_POINT)WorkingCopyGain.IR_GAIN=CalibrationCoeff.IR_XHIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IR_XHIGH_GAIN-CalibrationCoeff.IR_HIGH_GAIN)/(CUR_XHIGH_CAL_POINT-CUR_HIGH_CAL_POINT);
      Offset=CalibrationCoeff.IR_XHIGH_GAIN-CUR_XHIGH_CAL_POINT*Slope;
      WorkingCopyGain.IR_GAIN=Offset+Slope*InstantPara.CurrentR;
    }
  }
  else if(InstantPara.CurrentR>=CUR_MID_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IR_HIGH_GAIN-CalibrationCoeff.IR_MID_GAIN)/(CUR_HIGH_CAL_POINT-CUR_MID_CAL_POINT);
    Offset=CalibrationCoeff.IR_HIGH_GAIN-CUR_HIGH_CAL_POINT*Slope;
    WorkingCopyGain.IR_GAIN=Offset+Slope*InstantPara.CurrentR;
  }
  else if(InstantPara.CurrentR>=CUR_LOW_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IR_MID_GAIN-CalibrationCoeff.IR_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IR_MID_GAIN-CUR_MID_CAL_POINT*Slope;
    WorkingCopyGain.IR_GAIN=Offset+Slope*InstantPara.CurrentR;
  }
  else WorkingCopyGain.IR_GAIN=CalibrationCoeff.IR_LOW_GAIN;
 
  if(InstantPara.CurrentY>=CUR_HIGH_CAL_POINT)
  {
    if(InstantPara.CurrentY>=CUR_XHIGH_CAL_POINT)WorkingCopyGain.IY_GAIN=CalibrationCoeff.IY_XHIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IY_XHIGH_GAIN-CalibrationCoeff.IY_HIGH_GAIN)/(CUR_XHIGH_CAL_POINT-CUR_HIGH_CAL_POINT);
      Offset=CalibrationCoeff.IY_XHIGH_GAIN-CUR_XHIGH_CAL_POINT*Slope;
      WorkingCopyGain.IY_GAIN=Offset+Slope*InstantPara.CurrentY;
    }
  }
  else if(InstantPara.CurrentY>=CUR_MID_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IY_HIGH_GAIN-CalibrationCoeff.IY_MID_GAIN)/(CUR_HIGH_CAL_POINT-CUR_MID_CAL_POINT);
    Offset=CalibrationCoeff.IY_HIGH_GAIN-CUR_HIGH_CAL_POINT*Slope;
    WorkingCopyGain.IY_GAIN=Offset+Slope*InstantPara.CurrentY;
  }
  else if(InstantPara.CurrentY>=CUR_LOW_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IY_MID_GAIN-CalibrationCoeff.IY_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IY_MID_GAIN-CUR_MID_CAL_POINT*Slope;
    WorkingCopyGain.IY_GAIN=Offset+Slope*InstantPara.CurrentY;
  }
  else WorkingCopyGain.IY_GAIN=CalibrationCoeff.IY_LOW_GAIN;
  if(InstantPara.CurrentB>=CUR_HIGH_CAL_POINT)
  {
    if(InstantPara.CurrentB>=CUR_XHIGH_CAL_POINT)WorkingCopyGain.IB_GAIN=CalibrationCoeff.IB_XHIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IB_XHIGH_GAIN-CalibrationCoeff.IB_HIGH_GAIN)/(CUR_XHIGH_CAL_POINT-CUR_HIGH_CAL_POINT);
      Offset=CalibrationCoeff.IB_XHIGH_GAIN-CUR_XHIGH_CAL_POINT*Slope;
      WorkingCopyGain.IB_GAIN=Offset+Slope*InstantPara.CurrentB;
    }
  }
  else if(InstantPara.CurrentB>=CUR_MID_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IB_HIGH_GAIN-CalibrationCoeff.IB_MID_GAIN)/(CUR_HIGH_CAL_POINT-CUR_MID_CAL_POINT);
    Offset=CalibrationCoeff.IB_HIGH_GAIN-CUR_HIGH_CAL_POINT*Slope;
    WorkingCopyGain.IB_GAIN=Offset+Slope*InstantPara.CurrentB;
  }
  else if(InstantPara.CurrentB>=CUR_LOW_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IB_MID_GAIN-CalibrationCoeff.IB_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IB_MID_GAIN-CUR_MID_CAL_POINT*Slope;
    WorkingCopyGain.IB_GAIN=Offset+Slope*InstantPara.CurrentB;
  }
  else WorkingCopyGain.IB_GAIN=CalibrationCoeff.IB_LOW_GAIN;

  // Solar
  if(InstantPara.CurrentRSolar>=CUR_HIGH_CAL_POINT)
  {
    if(InstantPara.CurrentRSolar>=CUR_XHIGH_CAL_POINT)WorkingCopyGain.IR_SOLAR_GAIN=CalibrationCoeff.IR_SOLAR_XHIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IR_SOLAR_XHIGH_GAIN-CalibrationCoeff.IR_SOLAR_HIGH_GAIN)/(CUR_XHIGH_CAL_POINT-CUR_HIGH_CAL_POINT);
      Offset=CalibrationCoeff.IR_SOLAR_XHIGH_GAIN-CUR_XHIGH_CAL_POINT*Slope;
      WorkingCopyGain.IR_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentRSolar;
    }
  }
  else if(InstantPara.CurrentRSolar>=CUR_MID_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IR_SOLAR_HIGH_GAIN-CalibrationCoeff.IR_SOLAR_MID_GAIN)/(CUR_HIGH_CAL_POINT-CUR_MID_CAL_POINT);
    Offset=CalibrationCoeff.IR_SOLAR_HIGH_GAIN-CUR_HIGH_CAL_POINT*Slope;
    WorkingCopyGain.IR_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentRSolar;
  }
  else if(InstantPara.CurrentRSolar>=CUR_LOW_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IR_SOLAR_MID_GAIN-CalibrationCoeff.IR_SOLAR_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IR_SOLAR_MID_GAIN-CUR_MID_CAL_POINT*Slope;
    WorkingCopyGain.IR_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentRSolar;
  }
  else WorkingCopyGain.IR_SOLAR_GAIN=CalibrationCoeff.IR_SOLAR_LOW_GAIN;

  if(InstantPara.CurrentYSolar>=CUR_HIGH_CAL_POINT)
  {
    if(InstantPara.CurrentYSolar>=CUR_XHIGH_CAL_POINT)WorkingCopyGain.IY_SOLAR_GAIN=CalibrationCoeff.IY_SOLAR_XHIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IY_SOLAR_XHIGH_GAIN-CalibrationCoeff.IY_SOLAR_HIGH_GAIN)/(CUR_XHIGH_CAL_POINT-CUR_HIGH_CAL_POINT);
      Offset=CalibrationCoeff.IY_SOLAR_XHIGH_GAIN-CUR_XHIGH_CAL_POINT*Slope;
      WorkingCopyGain.IY_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentYSolar;
    }
  }
  else if(InstantPara.CurrentYSolar>=CUR_MID_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IY_SOLAR_HIGH_GAIN-CalibrationCoeff.IY_SOLAR_MID_GAIN)/(CUR_HIGH_CAL_POINT-CUR_MID_CAL_POINT);
    Offset=CalibrationCoeff.IY_SOLAR_HIGH_GAIN-CUR_HIGH_CAL_POINT*Slope;
    WorkingCopyGain.IY_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentYSolar;
  }
  else if(InstantPara.CurrentYSolar>=CUR_LOW_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IY_SOLAR_MID_GAIN-CalibrationCoeff.IY_SOLAR_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IY_SOLAR_MID_GAIN-CUR_MID_CAL_POINT*Slope;
    WorkingCopyGain.IY_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentYSolar;
  }
  else WorkingCopyGain.IY_SOLAR_GAIN=CalibrationCoeff.IY_SOLAR_LOW_GAIN;

  if(InstantPara.CurrentBSolar>=CUR_HIGH_CAL_POINT)
  {
    if(InstantPara.CurrentBSolar>=CUR_XHIGH_CAL_POINT)WorkingCopyGain.IB_SOLAR_GAIN=CalibrationCoeff.IB_SOLAR_XHIGH_GAIN;
    else
    {
      Slope=(CalibrationCoeff.IB_SOLAR_XHIGH_GAIN-CalibrationCoeff.IB_SOLAR_HIGH_GAIN)/(CUR_XHIGH_CAL_POINT-CUR_HIGH_CAL_POINT);
      Offset=CalibrationCoeff.IB_SOLAR_XHIGH_GAIN-CUR_XHIGH_CAL_POINT*Slope;
      WorkingCopyGain.IB_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentBSolar;
    }
  }
  else if(InstantPara.CurrentBSolar>=CUR_MID_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IB_SOLAR_HIGH_GAIN-CalibrationCoeff.IB_SOLAR_MID_GAIN)/(CUR_HIGH_CAL_POINT-CUR_MID_CAL_POINT);
    Offset=CalibrationCoeff.IB_SOLAR_HIGH_GAIN-CUR_HIGH_CAL_POINT*Slope;
    WorkingCopyGain.IB_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentBSolar;
  }
  else if(InstantPara.CurrentBSolar>=CUR_LOW_CAL_POINT)
  {
    Slope=(CalibrationCoeff.IB_SOLAR_MID_GAIN-CalibrationCoeff.IB_SOLAR_LOW_GAIN)/(CUR_MID_CAL_POINT-CUR_LOW_CAL_POINT);
    Offset=CalibrationCoeff.IB_SOLAR_MID_GAIN-CUR_MID_CAL_POINT*Slope;
    WorkingCopyGain.IB_SOLAR_GAIN=Offset+Slope*InstantPara.CurrentBSolar;
  }
  else WorkingCopyGain.IB_SOLAR_GAIN=CalibrationCoeff.IB_SOLAR_LOW_GAIN;

    
  TempChar=PhaseBufferIndex(InstantPara.CurrentR);
  WorkingCopyGain.PR_ALFA=BufferAlfaR[TempChar];
  WorkingCopyGain.PR_BETA=BufferBetaR[TempChar];
  WorkingCopyGain.PR_INT_DELAY=BufferIntDelayR[TempChar];

  TempChar=PhaseBufferIndex(InstantPara.CurrentY);
  WorkingCopyGain.PY_ALFA=BufferAlfaY[TempChar];
  WorkingCopyGain.PY_BETA=BufferBetaY[TempChar];
  WorkingCopyGain.PY_INT_DELAY=BufferIntDelayY[TempChar];

  TempChar=PhaseBufferIndex(InstantPara.CurrentB);
  WorkingCopyGain.PB_ALFA=BufferAlfaB[TempChar];
  WorkingCopyGain.PB_BETA=BufferBetaB[TempChar];
  WorkingCopyGain.PB_INT_DELAY=BufferIntDelayB[TempChar];

  // Solar
  //
  TempChar=PhaseBufferIndex(InstantPara.CurrentRSolar);
  WorkingCopyGain.PR_SOLAR_ALFA=BufferAlfaRSolar[TempChar];
  WorkingCopyGain.PR_SOLAR_BETA=BufferBetaRSolar[TempChar];
  WorkingCopyGain.PR_SOLAR_INT_DELAY=BufferIntDelayRSolar[TempChar];

  TempChar=PhaseBufferIndex(InstantPara.CurrentYSolar);
  WorkingCopyGain.PY_SOLAR_ALFA=BufferAlfaYSolar[TempChar];
  WorkingCopyGain.PY_SOLAR_BETA=BufferBetaYSolar[TempChar];
  WorkingCopyGain.PY_SOLAR_INT_DELAY=BufferIntDelayYSolar[TempChar];

  TempChar=PhaseBufferIndex(InstantPara.CurrentBSolar);
  WorkingCopyGain.PB_SOLAR_ALFA=BufferAlfaBSolar[TempChar];
  WorkingCopyGain.PB_SOLAR_BETA=BufferBetaBSolar[TempChar];
  WorkingCopyGain.PB_SOLAR_INT_DELAY=BufferIntDelayBSolar[TempChar];
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





  
