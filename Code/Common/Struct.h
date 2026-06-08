#ifndef STRUCT_H
#define STRUCT_H

#include "stm32f37x.h"
#include <stdbool.h>
#include "bootloader.h"
#include "FlagDef_DIN.h"

#define RUNNING_MODE_IMPORT  0

#define TO_BOOL(x)      (!!(x))

#define ARRAY_SIZE(a)           (sizeof(a) / sizeof(a[0]))

// Rounds up X (integer) to an upper integer multiple of 'factor' where 'factor' is power of two
//
#define ROUNDUP_POW2(x, factor)  ((((x)+(factor)-1) & ~((factor) - 1)))

#define COMPILE_ASSERT(cond)    extern char dummy_assertion_array[(cond) ? 1 : -1]

struct STORE 
{
    uint32_t  StorageCounter;
    uint16_t  StorageLocation;

    // Running Mode, Run Hour, Load Hours, Interrupts are currently not 
    // valid in this codebase
    //
    uint16_t  RunningMode;

    uint32_t  RunHourImport;
    uint32_t  RunHourExport;
    uint32_t  LoadHourImport;   // For dual source import is mains(S1) and export is generator(S2)
    uint32_t  LoadHourExport;
    double    ImportWh;    // least count is 1e-7whr and hence max 999,999,999,999.999,999,9 whr sec term
    double    ImportVAh;
    double    ImportVarhPos;
    double    ImportVarhNeg;
    double    ExportWh;
    double    ExportVAh;
    double    ExportVarhPos;
    double    ExportVarhNeg; 
    uint16_t  ImportInterruptions;
    uint16_t  ExportInterruptions;  // Interruption cant be import/export only

    uint32_t  SolarRunHourImport;
    uint32_t  SolarRunHourExport;
    uint32_t  SolarLoadHourImport;   // For dual source import is mains(S1) and export is generator(S2)
    uint32_t  SolarLoadHourExport;
    double    SolarImportWh;    // least count is 1e-7whr and hence max 999,999,999,999.999,999,9 whr sec term
    double    SolarImportVAh;
    double    SolarImportVarhPos;
    double    SolarImportVarhNeg;
    double    SolarExportWh;
    double    SolarExportVAh;
    double    SolarExportVarhPos;
    double    SolarExportVarhNeg; 
    uint16_t  SolarImportInterruptions;
    uint16_t  SolarExportInterruptions;  // Interruption cant be import/export only
    
    uint8_t   Spare[128];

    uint16_t  EnergyDispUnit;
    uint16_t  StoreCRC;
};


struct INT_DATA_SUM
{
  float        VolRPhase;
  float        VolYPhase;
  float        VolBPhase;
  float        CurRPhase;
  float        CurYPhase;
  float        CurBPhase;
  float        RPhasePower;
  float        YPhasePower;
  float        BPhasePower;
  float        YRPower3P3W;
  float        YBPower3P3W;
  float        CurNeutral;
  float        VolRYPhPh;
  float        VolYBPhPh;
  float        VolBRPhPh;
  float        OffsetVolRPhase;
  float        OffsetVolYPhase;
  float        OffsetVolBPhase;
  float        OffsetCurRPhase;
  float        OffsetCurYPhase;
  float        OffsetCurBPhase;

  float        VolRSolarPhase;
  float        VolYSolarPhase;
  float        VolBSolarPhase;
  float        CurRSolarPhase;
  float        CurYSolarPhase;
  float        CurBSolarPhase;
  float        RSolarPhasePower;
  float        YSolarPhasePower;
  float        BSolarPhasePower;
  float        YRSolarPower3P3W;
  float        YBSolarPower3P3W;
  float        CurNeutralSolar;
  float        VolRYSolarPhPh;
  float        VolYBSolarPhPh;
  float        VolBRSolarPhPh;
  float        OffsetVolRSolarPhase;
  float        OffsetVolYSolarPhase;
  float        OffsetVolBSolarPhase;
  float        OffsetCurRSolarPhase;
  float        OffsetCurYSolarPhase;
  float        OffsetCurBSolarPhase;

  float        Fan1Current;
  float        Fan2Current;
  float        OffsetFan1Current;
  float        OffsetFan2Current;

  float        AmbientTemperature;
  float        VRefInt;
};


struct INT_DATA_SAVE
{
  float     VolRPhase;
  float     VolYPhase;
  float     VolBPhase;
  float     CurRPhase;
  float     CurYPhase;
  float     CurBPhase;
  float     RPhasePower;
  float     YPhasePower;
  float     BPhasePower;
  float     YRPower3P3W;
  float     YBPower3P3W;
  float     CurNeutral;
  float     VolRYPhPh;
  float     VolYBPhPh;
  float     VolBRPhPh;
  float     OffsetVolRPhase;
  float     OffsetVolYPhase;
  float     OffsetVolBPhase;
  float     OffsetCurRPhase;
  float     OffsetCurYPhase;
  float     OffsetCurBPhase;
  uint16_t  TimerCountValue;

  float     VolRSolarPhase;
  float     VolYSolarPhase;
  float     VolBSolarPhase;
  float     CurRSolarPhase;
  float     CurYSolarPhase;
  float     CurBSolarPhase;
  float     RSolarPhasePower;
  float     YSolarPhasePower;
  float     BSolarPhasePower;
  float     YRSolarPower3P3W;
  float     YBSolarPower3P3W;
  float     CurNeutralSolar;
  float     VolRYSolarPhPh;
  float     VolYBSolarPhPh;
  float     VolBRSolarPhPh;
  float     OffsetVolRSolarPhase;
  float     OffsetVolYSolarPhase;
  float     OffsetVolBSolarPhase;
  float     OffsetCurRSolarPhase;
  float     OffsetCurYSolarPhase;
  float     OffsetCurBSolarPhase;

  float     Fan1Current;
  float     Fan2Current;
  float     OffsetFan1Current;
  float     OffsetFan2Current;

  float     AmbientTemperature;
  float     VRefInt;
};

struct INT_TIMER_COUNT
{
  uint16_t     TimerNewValue;
  uint16_t     TimerPresentValue;
};

struct INST_PARA
{
  float     VolR;
  float     VolY;
  float     VolB;
  float     VolRY;
  float     VolYB;
  float     VolBR;
  float     CurrentR;
  float     CurrentY;
  float     CurrentB;
  float     CurrentN;
  float     TotalPowerR;
  float     TotalPowerY;
  float     TotalPowerB;
  float     SumTotalPower;
  float     FunPowerR;
  float     FunPowerY;
  float     FunPowerB;
  float     SumFunPower;
  float     AppPowerR;
  float     AppPowerY;
  float     AppPowerB;
  float     TotalAppPower;
  float     ReactPowerR;
  float     ReactPowerY;
  float     ReactPowerB;
  float     TotalReactPower;
  float     PowerFactorR;
  float     PowerFactorY;
  float     PowerFactorB;
  float     TotalPowerFactor;
  float     Frequency;
  float     FrequencyR;
  float     FrequencyY;
  float     FrequencyB;
  float     FunRCurr;
  float     FunYCurr;
  float     FunBCurr;
  float     FunNCurr;
  float     FunRVol;
  float     FunYVol;
  float     FunBVol;
  float     Fan1Current;
  float     Fan2Current;
  float     AmbientTemperature;
 
  float     HarRPhaseVol;
  float     HarYPhaseVol;
  float     HarBPhaseVol;
  float     HarRPhaseCur;
  float     HarYPhaseCur;
  float     HarBPhaseCur;
  
  float     ThdRPhaseVol;
  float     ThdYPhaseVol;
  float     ThdBPhaseVol;
  float     ThdRPhaseCur;
  float     ThdYPhaseCur;
  float     ThdBPhaseCur;
  
  float     AvgCurr;
  float     CurUnb;
  float     VolUnb;
  float     AngleRPhase;
  float     AngleYPhase;
  float     AngleBPhase;
  float     VolLLAvg;
  float     VolLNAvg;
  float     CurrentTotal;
  float     VrMin;
  float     VrMax;
  float     VrSum;
  float     VyMin;
  float     VyMax;
  float     VySum;
  float     VbMin;
  float     VbMax;
  float     VbSum;
  
  float     IrMin;
  float     IrMax;
  float     IrSum;
  float     IyMin;
  float     IyMax;
  float     IySum;
  float     IbMin;
  float     IbMax;
  float     IbSum;

  float     VolRSolar;
  float     VolYSolar;
  float     VolBSolar;
  float     VolRYSolar;
  float     VolYBSolar;
  float     VolBRSolar;
  float     CurrentRSolar;
  float     CurrentYSolar;
  float     CurrentBSolar;
  float     CurrentNSolar;
  float     TotalPowerRSolar;
  float     TotalPowerYSolar;
  float     TotalPowerBSolar;
  float     SumTotalPowerSolar;
  float     AppPowerRSolar;
  float     AppPowerYSolar;
  float     AppPowerBSolar;
  float     TotalAppPowerSolar;
  float     ReactPowerRSolar;
  float     ReactPowerYSolar;
  float     ReactPowerBSolar;
  float     TotalReactPowerSolar;
  float     PowerFactorRSolar;
  float     PowerFactorYSolar;
  float     PowerFactorBSolar;
  float     TotalPowerFactorSolar;
  float     FrequencyRSolar;
  float     FrequencyYSolar;
  float     FrequencyBSolar;
  float     FunRSolarCurr;
  float     FunYSolarCurr;
  float     FunBSolarCurr;
  float     FunNSolarCurr;
  float     FunPowerSolarR;
  float     FunPowerSolarY;
  float     FunPowerSolarB;
};

// Per-phase frequency zero-crossing detector state
//
struct FreqMeasState
{
    uint32_t MeasDuration;
    uint32_t SaveMeasDuration;
    int16_t  PrevSampleVol;
    uint8_t  Flag;
    uint8_t  CycleCounter;
};

struct FreqMeasStates
{
    struct FreqMeasState RPhase;
    struct FreqMeasState YPhase;
    struct FreqMeasState BPhase;
    struct FreqMeasState RSolarPhase;
    struct FreqMeasState YSolarPhase;
    struct FreqMeasState BSolarPhase;
};

struct OFFSET
{
 
  float     VolRPhase;
  float     VolYPhase;
  float     VolBPhase;
  float     CurRPhase;
  float     CurYPhase;
  float     CurBPhase;

  float     VolRSolarPhase;
  float     VolYSolarPhase;
  float     VolBSolarPhase;
  float     CurRSolarPhase;
  float     CurYSolarPhase;
  float     CurBSolarPhase;

  float     Fan1Current;
  float     Fan2Current;
};

struct FFT_STRUCT
{
  float    FFT_RVolSinSum[50];
  float    FFT_RVolCosSum[50];
  float    FFT_YVolSinSum[50];
  float    FFT_YVolCosSum[50];
  float    FFT_BVolSinSum[50];
  float    FFT_BVolCosSum[50];
  
  float    FFT_RCurSinSum[50];
  float    FFT_RCurCosSum[50];
  float    FFT_YCurSinSum[50];
  float    FFT_YCurCosSum[50];
  float    FFT_BCurSinSum[50];
  float    FFT_BCurCosSum[50];
  float    FFT_NeuCurSinSum[50];
  float    FFT_NeuCurCosSum[50];
  
  float    FFT_RVolSinSave[50];
  float    FFT_RVolCosSave[50];
  float    FFT_YVolSinSave[50];
  float    FFT_YVolCosSave[50];
  float    FFT_BVolSinSave[50];
  float    FFT_BVolCosSave[50];
  
  float    FFT_RCurSinSave[50];
  float    FFT_RCurCosSave[50];
  float    FFT_YCurSinSave[50];
  float    FFT_YCurCosSave[50];
  float    FFT_BCurSinSave[50];
  float    FFT_BCurCosSave[50];
  float    FFT_NeuCurSinSave[50];
  float    FFT_NeuCurCosSave[50];

  float    FFT_RSolarVolSinSum[50];
  float    FFT_RSolarVolCosSum[50];
  float    FFT_YSolarVolSinSum[50];
  float    FFT_YSolarVolCosSum[50];
  float    FFT_BSolarVolSinSum[50];
  float    FFT_BSolarVolCosSum[50];
  
  float    FFT_RSolarCurSinSum[50];
  float    FFT_RSolarCurCosSum[50];
  float    FFT_YSolarCurSinSum[50];
  float    FFT_YSolarCurCosSum[50];
  float    FFT_BSolarCurSinSum[50];
  float    FFT_BSolarCurCosSum[50];
  float    FFT_NeuSolarCurSinSum[50];
  float    FFT_NeuSolarCurCosSum[50];
  
  float    FFT_RSolarVolSinSave[50];
  float    FFT_RSolarVolCosSave[50];
  float    FFT_YSolarVolSinSave[50];
  float    FFT_YSolarVolCosSave[50];
  float    FFT_BSolarVolSinSave[50];
  float    FFT_BSolarVolCosSave[50];
  
  float    FFT_RSolarCurSinSave[50];
  float    FFT_RSolarCurCosSave[50];
  float    FFT_YSolarCurSinSave[50];
  float    FFT_YSolarCurCosSave[50];
  float    FFT_BSolarCurSinSave[50];
  float    FFT_BSolarCurCosSave[50];
  float    FFT_NeuSolarCurSinSave[50];
  float    FFT_NeuSolarCurCosSave[50];

  uint16_t FFT_Counter;
  uint16_t FFT_CounterIndex;
};

/*

struct HARMONICS
{
  float         RPhaseVol[64];
  float         YPhaseVol[64];
  float         BPhaseVol[64];
  float         RPhaseCur[64];
  float         YPhaseCur[64];
  float         BPhaseCur[64];
  float         NPhaseCur[64];
};
*/

struct PH_COEFF_STRUCT
{
float     VR_240_GAIN;
float     VY_240_GAIN;
float     VB_240_GAIN;

float     IR_XHIGH_GAIN;
float     IR_HIGH_GAIN;
float     IR_MID_GAIN;
float     IR_LOW_GAIN;

float     IY_XHIGH_GAIN;
float     IY_HIGH_GAIN;
float     IY_MID_GAIN;
float     IY_LOW_GAIN;

float     IB_XHIGH_GAIN;
float     IB_HIGH_GAIN;
float     IB_MID_GAIN;
float     IB_LOW_GAIN;

float     IR_XHIGH_PH_ERROR;
float     IR_HIGH_PH_ERROR;
float     IR_MID_PH_ERROR;
float     IR_LOW_PH_ERROR;

float     IY_XHIGH_PH_ERROR;
float     IY_HIGH_PH_ERROR;
float     IY_MID_PH_ERROR;
float     IY_LOW_PH_ERROR;

float     IB_XHIGH_PH_ERROR;
float     IB_HIGH_PH_ERROR;
float     IB_MID_PH_ERROR;
float     IB_LOW_PH_ERROR;

float     VR_SOLAR_240_GAIN;
float     VY_SOLAR_240_GAIN;
float     VB_SOLAR_240_GAIN;

float     IR_SOLAR_XHIGH_GAIN;
float     IR_SOLAR_HIGH_GAIN;
float     IR_SOLAR_MID_GAIN;
float     IR_SOLAR_LOW_GAIN;

float     IY_SOLAR_XHIGH_GAIN;
float     IY_SOLAR_HIGH_GAIN;
float     IY_SOLAR_MID_GAIN;
float     IY_SOLAR_LOW_GAIN;

float     IB_SOLAR_XHIGH_GAIN;
float     IB_SOLAR_HIGH_GAIN;
float     IB_SOLAR_MID_GAIN;
float     IB_SOLAR_LOW_GAIN;

float     IR_SOLAR_XHIGH_PH_ERROR;
float     IR_SOLAR_HIGH_PH_ERROR;
float     IR_SOLAR_MID_PH_ERROR;
float     IR_SOLAR_LOW_PH_ERROR;

float     IY_SOLAR_XHIGH_PH_ERROR;
float     IY_SOLAR_HIGH_PH_ERROR;
float     IY_SOLAR_MID_PH_ERROR;
float     IY_SOLAR_LOW_PH_ERROR;

float     IB_SOLAR_XHIGH_PH_ERROR;
float     IB_SOLAR_HIGH_PH_ERROR;
float     IB_SOLAR_MID_PH_ERROR;
float     IB_SOLAR_LOW_PH_ERROR;

float     FAN1_GAIN;
float     FAN2_GAIN;

// Inter-phase voltage phase-shift error for grid mains line-line voltage.
// Signed phase deviation (radians) of the actual phase difference from the
// ideal 120deg, measured during CALIBRATE_XH_PF via the sin identity
// RMS(V_LL) = 2*240*|sin(phi/2)|. Sign encodes which channel of the pair
// gets the runtime delay (positive => first letter of pair, negative =>
// second letter). Bound: |error| <= ~10.8deg (5% V_LL tolerance).
//
// At runtime, FillCurrentGainArray()-equivalent path converts each PH_ERROR
// into ALFA / BETA / INT_DELAY in WorkingCopyGain via the same fractional-
// delay FIR coefficients used by PR_/PY_/PB_. Applied only to V_LL
// accumulator; per-phase RMS, V*I power, FFT, PR_/PY_/PB_ are untouched.
//
float     VLL_RY_PH_ERROR;
float     VLL_YB_PH_ERROR;
float     VLL_BR_PH_ERROR;

// Same as VLL_*_PH_ERROR above but for solar line-line voltages. Computed
// at CALIBRATE_XH_PF from CalVolRYSolar / CalVolYBSolar / CalVolBRSolar.
//
float     VLL_RY_SOLAR_PH_ERROR;
float     VLL_YB_SOLAR_PH_ERROR;
float     VLL_BR_SOLAR_PH_ERROR;

float     unused[12];

uint16_t     INIT_DATA1;
uint16_t     INIT_DATA2  ;

} ;


struct GAIN_WC
{
  float VR_GAIN;
  float VY_GAIN;
  float VB_GAIN;
  
  float IR_GAIN;
  float IY_GAIN;
  float IB_GAIN;
  
  float PR_ALFA;
  float PY_ALFA;
  float PB_ALFA;
  
  float PR_BETA;
  float PY_BETA;
  float PB_BETA;
  
  float VR_SOLAR_GAIN;
  float VY_SOLAR_GAIN;
  float VB_SOLAR_GAIN;

  float IR_SOLAR_GAIN;
  float IY_SOLAR_GAIN;
  float IB_SOLAR_GAIN;
  
  float PR_SOLAR_ALFA;
  float PY_SOLAR_ALFA;
  float PB_SOLAR_ALFA;
  
  float PR_SOLAR_BETA;
  float PY_SOLAR_BETA;
  float PB_SOLAR_BETA;

  float FAN1_GAIN;
  float FAN2_GAIN;

  // Inter-phase voltage phase-shift FIR for grid mains line-line voltage.
  // Same 2-tap fractional-delay structure as PR_ALFA / PR_BETA / PR_INT_DELAY:
  //   y[n] = ALFA * ( x[n - int_d] + BETA * x[n - int_d - 1] )
  // INT_DELAY sign selects which channel of the pair gets the delay:
  //   >= 0 : delay applied to first letter of pair (R for V_RY pair, Y for
  //          V_YB pair, B for V_BR pair). int_d magnitude in 0..3.
  //   <  0 : delay applied to second letter (Y / B / R). int_d magnitude
  //          = |value| - 1, in 0..3 (bias of 1 disambiguates the 0 case).
  //
  float VLL_RY_ALFA;
  float VLL_YB_ALFA;
  float VLL_BR_ALFA;

  float VLL_RY_BETA;
  float VLL_YB_BETA;
  float VLL_BR_BETA;

  int8_t VLL_RY_INT_DELAY;
  int8_t VLL_YB_INT_DELAY;
  int8_t VLL_BR_INT_DELAY;

  // Solar V_LL phase-shift FIR. Same structure as the grid VLL_*_ALFA /
  // VLL_*_BETA / VLL_*_INT_DELAY block above, applied only to the solar
  // V_LL accumulator (VolRYSolarPhPh / VolYBSolarPhPh / VolBRSolarPhPh).
  //
  float VLL_RY_SOLAR_ALFA;
  float VLL_YB_SOLAR_ALFA;
  float VLL_BR_SOLAR_ALFA;

  float VLL_RY_SOLAR_BETA;
  float VLL_YB_SOLAR_BETA;
  float VLL_BR_SOLAR_BETA;

  int8_t VLL_RY_SOLAR_INT_DELAY;
  int8_t VLL_YB_SOLAR_INT_DELAY;
  int8_t VLL_BR_SOLAR_INT_DELAY;

  // Per-channel I_N (neutral current) phase-alignment FIR. Used ONLY by
  // the I_N accumulator in the ISR; per-phase RMS, V*I power, FFT and the
  // per-phase PF FIR are untouched. Coefficients derived at boot from the
  // existing V_LL and per-phase PF cal data via DeriveNeutralFir() - no
  // dedicated cal step. INT_DELAY is always >= 0 (FIR is delay-only) so
  // there is no first-letter/second-letter sign convention; INT_DELAY in
  // 0..3 selects the integer-sample tap and ALFA/BETA carry the
  // fractional part. Identity (ALFA=1, BETA=0, INT_DELAY=0) when the
  // computed delay exceeds the 4-sample (~22.5 deg) FIR budget.
  //
  float I_N_R_ALFA;
  float I_N_Y_ALFA;
  float I_N_B_ALFA;

  float I_N_R_BETA;
  float I_N_Y_BETA;
  float I_N_B_BETA;

  int8_t I_N_R_INT_DELAY;
  int8_t I_N_Y_INT_DELAY;
  int8_t I_N_B_INT_DELAY;

  // Same per-channel I_N FIR for solar.
  //
  float I_N_R_SOLAR_ALFA;
  float I_N_Y_SOLAR_ALFA;
  float I_N_B_SOLAR_ALFA;

  float I_N_R_SOLAR_BETA;
  float I_N_Y_SOLAR_BETA;
  float I_N_B_SOLAR_BETA;

  int8_t I_N_R_SOLAR_INT_DELAY;
  int8_t I_N_Y_SOLAR_INT_DELAY;
  int8_t I_N_B_SOLAR_INT_DELAY;

  // Integer sample delay for phase correction. Sign selects which signal
  // the correction is applied to; magnitude selects the integer tap.
  //   >= 0: delay applied to voltage, integer part = value (0..3 samples).
  //    < 0: delay applied to current, integer part = |value| - 1 (0..3
  //         samples). The bias of 1 avoids an encoding collision at 0.
  // In both cases the fractional part is handled by PR_ALFA/PR_BETA.
  // CT phase errors that drive displayed PF > actual PF at cal time fall
  // into the negative branch; CT phase errors that drive displayed PF <
  // actual PF fall into the non-negative branch (the historical path).
  //
  int8_t PR_INT_DELAY;
  int8_t PY_INT_DELAY;
  int8_t PB_INT_DELAY;
  int8_t PR_SOLAR_INT_DELAY;
  int8_t PY_SOLAR_INT_DELAY;
  int8_t PB_SOLAR_INT_DELAY;
};

  
struct ICON
{
  uint32_t Location;  // absolute bit position as per the lcd module and connection on PCB
  uint16_t CommAdd;   // as per LCD->RAM[] array location. 
};

struct Field
{
  uint8_t Slave_Address;
  uint8_t Function_Code;
  uint8_t Data_Array[250];
};

struct Object
{
  volatile uint8_t Start;
  volatile uint8_t Error;
  volatile uint8_t End_Frame;
  volatile uint8_t ParityError;
  volatile uint8_t transfercomplete;
  volatile uint8_t FaultEventCheck;
  volatile uint8_t SettingError;
  volatile uint8_t FrameRecieved;
  volatile uint8_t USBCommunication;
  volatile uint8_t USBPending;
  volatile uint8_t DoubleData;
  volatile uint8_t TransmissionFailed;
};
#if (defined MODBUS_MAP_PROCOM)
struct ModBusParameter
{
   uint8_t    *Variable;
   uint16_t     DataType;
   uint16_t     lookup;
   uint8_t      Multiply;
   
};
#endif

struct ParaArray
{
  uint16_t Mod_Add;
  uint16_t  BlockLength;
};
struct DEMAND
{
  float RisingDemand;
  float PresentDemand;
  float MaxDemand;
  float DemandSumValue;
  float Demand15SecSum;
  float RisingDemandSum;  // only for sliding window
  uint32_t MDtime;
  uint32_t MDdate;
  uint16_t Demand15SecCounter;
  uint16_t DemandCounter;
  uint16_t DemandPointerLastCount;
  uint16_t TimeElapsed;
  
};

struct SAMPLE
{
  float PrevIn_1;
  float PrevIn_2;
  float PrevOut_1;
  float PrevOut_2;
};

struct DIS_SCROLL
{
  uint8_t DisplayScrollStatus;
  uint8_t DisplayScanPage;
  uint8_t DisplayScanPageEne;
  uint8_t Spare2;
}
;

struct LCD_ARRAY
{
  uint8_t RAM[50];
};

// All digital inputs:
// DEVNOTE: Keep them such that each of these inputs are off when nothing
// is attached to the controller (When the input is open, the corresponding
// entry in this struct should be 0)
//
// DEVNOTE: This is sent over modbus. Keep digital inputs in order.
// Also, using one bool per input
//
#define NUMBER_OF_INPUTS    8
struct DigInputs
{
    union
    {
        bool Inputs[NUMBER_OF_INPUTS];
        struct
        {
            bool MainsRPhaseContactorOn; 
            bool MainsYPhaseContactorOn;
            bool MainsBPhaseContactorOn;
            bool LoadOnSolarContactorOn;
            bool LoadOnGridContactorOn;
            bool SolarNeutralEarthContactorOn;
            bool DGOff;
            bool unused;
        };
    };
};

COMPILE_ASSERT(sizeof(struct DigInputs) == NUMBER_OF_INPUTS);
// DisplayHexByte() packs inputs into one byte for hex display
//
COMPILE_ASSERT(NUMBER_OF_INPUTS <= 8);

// All power supply on/off status
// DEVNOTE: This is sent over modbus. Keep them in order.
// Also, using one bool per power supply status
//
#define NUMBER_OF_POWER_SUPPLIES    2
struct PowerSupplyStatus
{
    union
    {
        bool Status[NUMBER_OF_POWER_SUPPLIES];
        struct
        {
            bool DCPowerSupplyStatus;
            bool ACPowerSupplyStatus;
        };
    };
};

COMPILE_ASSERT(sizeof(struct PowerSupplyStatus) == NUMBER_OF_POWER_SUPPLIES);

// Defines various abnormal conditions detected
//
// DEVNOTE: This is sent over modbus. Keep them in order.
// Also, using one bool per alarm
struct Alarms
{
    bool MainsRPhaseContactorStuckOpen;
    bool MainsRPhaseContactorStuckClosed;
    bool MainsYPhaseContactorStuckOpen;
    bool MainsYPhaseContactorStuckClosed;
    bool MainsBPhaseContactorStuckOpen;
    bool MainsBPhaseContactorStuckClosed;
    bool LoadOnSolarContactorStuckOpen;
    bool LoadOnSolarContactorStuckClosed;
    bool LoadOnGridContactorStuckOpen;
    bool LoadOnGridContactorStuckClosed;
    bool SolarNeutralEarthContactorStuckOpen;
    bool SolarNeutralEarthContactorStuckClosed;
};

// Voltage health status flags (true = fault condition active)
//
// DEVNOTE: This is sent over modbus. Keep them in order.
//
struct VoltageHealth
{
    bool GridRPhaseUnderVoltage;
    bool GridRPhaseOverVoltage;
    bool GridYPhaseUnderVoltage;
    bool GridYPhaseOverVoltage;
    bool GridBPhaseUnderVoltage;
    bool GridBPhaseOverVoltage;
    bool SolarRPhaseUnderVoltage;
    bool SolarRPhaseOverVoltage;
    bool SolarYPhaseUnderVoltage;
    bool SolarYPhaseOverVoltage;
    bool SolarBPhaseUnderVoltage;
    bool SolarBPhaseOverVoltage;
    // Phase loss = voltage below MIN_VOL_LIMIT (clamped to 0 by Metrology).
    // Kept distinct from UnderVoltage so Modbus consumers can tell a missing
    // phase apart from a low-but-present phase.
    //
    bool GridRPhaseLoss;
    bool GridYPhaseLoss;
    bool GridBPhaseLoss;
    bool SolarRPhaseLoss;
    bool SolarYPhaseLoss;
    bool SolarBPhaseLoss;
    // Confirmed per-phase health (true = unhealthy), with the same fail /
    // return delay hysteresis the contactor logic uses. Grid R/Y/B mirror
    // the contactor health machines; solar R comes straight from the
    // IDX_SOLAR machine, which is forced UNDER_VOLTAGE while any mains phase
    // contactor is closed (grid back-feed ties the solar R sense to grid R).
    // So 322 reads unhealthy throughout grid mode and reflects genuine solar
    // R health only once the mains contactors are open. Modbus 319-322.
    //
    bool GridRPhaseUnhealthy;
    bool GridYPhaseUnhealthy;
    bool GridBPhaseUnhealthy;
    bool SolarRPhaseUnhealthy;
};

// Load status flags (true = condition active), customer addresses 801-818.
//
// Implements the decision trees in
// Document/CustomerFacing/SOCO_Load_Status_Logic.docx. Exactly one flag of
// the GRID group {801,803,804,810,811,812,813} and exactly one flag of the
// SOLAR group {805,807,808,809,814,815,816,817,818} is set at any time.
// GridDisabledByUser (802) and SolarDisabledByUser (806) are independent of
// both groups. Group addresses are not contiguous because 801-809 keep
// their legacy positions for backward compatibility; new flags are
// appended at 810+.
//
// DEVNOTE: This is sent over modbus. Keep them in order.
//
struct LoadStatus
{
    bool LoadOnGridGridRHealthy;                  // 801: G1 on grid, healthy
    bool GridDisabledByUser;                      // 802: independent, = (HR6001 > 0)
    bool LoadNotOnGridGridRUnhealthy;             // 803: G4
    bool LoadNotOnGridSolarContactorStuckClosed;  // 804: G5 K4 stuck closed
    bool LoadOnSolarSolarRHealthy;                // 805: S1 on solar, healthy
    bool SolarDisabledByUser;                     // 806: independent, = (HR16001 > 0)
    bool LoadNotOnSolarSolarRUnhealthy;           // 807: S5
    bool LoadNotOnSolarGridAvailable;             // 808: S3 grid is available
    bool LoadNotOnSolarDGRunning;                 // 809: S6
    bool LoadOnGridContactorStuckClosed;          // 810: G2 K5 stuck closed
    bool LoadNotOnGridDisabledByUser;             // 811: G3
    bool LoadNotOnGridContactorStuckOpen;         // 812: G6 K5 stuck open
    bool LoadNotOnGridTransient;                  // 813: G7 transferring
    bool LoadOnSolarContactorStuckClosed;         // 814: S2 K4/K6 stuck closed
    bool LoadNotOnSolarDisabledByUser;            // 815: S4
    bool LoadNotOnSolarGridContactorStuckClosed;  // 816: S7 K1/K2/K3/K5 stuck closed
    bool LoadNotOnSolarContactorStuckOpen;        // 817: S8 K4/K6 stuck open
    bool LoadNotOnSolarTransient;                 // 818: S9 transferring
};

// Fan fault flags (true = fault condition active)
//
// DEVNOTE: This is sent over modbus. Keep them in order.
//
struct FanFaults
{
    bool Fan1ShortCircuit;
    bool Fan1OpenCircuit;
    bool Fan2ShortCircuit;
    bool Fan2OpenCircuit;
};

// Defines various relay status
//
// DEVNOTE: This is sent over modbus. Keep them in order.
// Also, using one bool per relay (with true = Relay On)
//
#define NUMBER_OF_RELAYS    6
struct DigOutputs
{
    union
    {
        bool Relays[NUMBER_OF_RELAYS];
        struct
        {
            bool RPhaseGridUnhealthyOutput; // NC used for R Grid Healthy
            bool YPhaseGridUnhealthyOutput; // NC used for Y Grid Healthy
            bool BPhaseGridUnhealthyOutput; // NC used for B Grid Healthy
            bool LoadOnSolarOutput;         // NO used for Load On Solar
            bool LoadOffGridOutput;         // NC used for Load On Grid
            bool FansOff;                   // NC used for Fans On
        };
    };
};
COMPILE_ASSERT(sizeof(struct DigOutputs) == NUMBER_OF_RELAYS);
// DisplayHexByte() packs outputs into one byte for hex display
//
COMPILE_ASSERT(NUMBER_OF_RELAYS <= 8);

// Defines various LED status
//
// DEVNOTE: This is sent over modbus. Keep them in order.
// Also, using one bool per LED
//
#define NUMBER_OF_LEDS    8
struct LedStatus
{
    union
    {
        bool Status[NUMBER_OF_LEDS];
        struct
        {
            bool R;
            bool G;
            bool B;
            bool Grid;
            bool Solar;
            bool LoadOnGrid;
            bool LoadOnSolar;
            bool Comm;
        };
    };
};
COMPILE_ASSERT(sizeof(struct LedStatus) == NUMBER_OF_LEDS);


// Persistent product information stored in 1 EEPROM page (64 bytes)
// at PRODUCT_INFO_LOC. All fields are zero-initialized at first use.
//
struct ProductInfo
{
    uint32_t SerialNumber[2];          // 8 bytes: lower and upper halves
    uint32_t FunctionallyTestedFlag;   // 4 bytes: non-zero = product has been tested
    uint32_t CalibratedFlag;           // 4 bytes: non-zero = product has been calibrated
    uint8_t  Reserved[48];             // 48 bytes: reserved for future use
};
COMPILE_ASSERT(sizeof(struct ProductInfo) == PRODUCT_INFO_SIZE);


// Defines the various status related to testing/calibration
//
// DEVNOTE: This is sent over modbus. Keep them in order.
// Also, using one bool per status (with true = status = true)
//
struct TestingStatus
{
    union
    {
        bool Status[1];
        struct
        {
            bool TestingModeEnabled;
        };
    };
};

// FOTA = Firmware Over-The-Air upgrade
//
#define FOTA_STATUS_IDLE     0
#define FOTA_STATUS_READY    1
#define FOTA_STATUS_ERROR    127

struct FotaState
{
    uint32_t status;          // FOTA_STATUS_*
    uint32_t chunksReceived;  // next expected record number
    uint32_t bytesWritten;    // total firmware bytes written to EEPROM
    uint32_t firmwareSize;    // totalRecords * CHUNK_SIZE - HEADER_SIZE
    uint16_t totalRecords;    // from file header
    uint16_t version;         // from file header
    uint16_t expectedCrc;     // CRC-16 of plaintext firmware, from header
    uint16_t runningCrc;      // running CRC-16 computed over decrypted chunks
    uint16_t prngSeed;        // initial seed, passed to FotaFlashInfo
    uint16_t prngState;       // current PRNG state (advances per word)
};

COMPILE_ASSERT(sizeof(struct FotaFlashInfo) == 128);

#endif
