#ifndef STRUCT_H
#define STRUCT_H

#include "stm32f37x.h"
#include <stdbool.h>

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
  float     FunRCurr;
  float     FunYCurr;
  float     FunBCurr;
  float     FunNCurr;
  float     FunRVol;
  float     FunYVol;
  float     FunBVol;
  float     Spare1;
  float     Spare2;
  float     Spare3;
 
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
  float     FunRSolarCurr;
  float     FunYSolarCurr;
  float     FunBSolarCurr;
  float     FunNSolarCurr;
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

float     IR_HIGH_GAIN;     // future for high accuracy meters
float     IR_MID_GAIN;
float     IR_LOW_GAIN;

float     IY_HIGH_GAIN;  
float     IY_MID_GAIN;
float     IY_LOW_GAIN; 

float     IB_HIGH_GAIN; 
float     IB_MID_GAIN;
float     IB_LOW_GAIN;  

float     IR_HIGH_PH_ERROR;
float     IR_MID_PH_ERROR;
float     IR_LOW_PH_ERROR;

float     IY_HIGH_PH_ERROR;
float     IY_MID_PH_ERROR;
float     IY_LOW_PH_ERROR;

float     IB_HIGH_PH_ERROR;
float     IB_MID_PH_ERROR;
float     IB_LOW_PH_ERROR;

float     VR_SOLAR_240_GAIN;             
float     VY_SOLAR_240_GAIN;
float     VB_SOLAR_240_GAIN;  

float     IR_SOLAR_HIGH_GAIN;     // future for high accuracy meters
float     IR_SOLAR_MID_GAIN;
float     IR_SOLAR_LOW_GAIN;

float     IY_SOLAR_HIGH_GAIN;  
float     IY_SOLAR_MID_GAIN;
float     IY_SOLAR_LOW_GAIN; 

float     IB_SOLAR_HIGH_GAIN; 
float     IB_SOLAR_MID_GAIN;
float     IB_SOLAR_LOW_GAIN;  

float     IR_SOLAR_HIGH_PH_ERROR;
float     IR_SOLAR_MID_PH_ERROR;
float     IR_SOLAR_LOW_PH_ERROR;

float     IY_SOLAR_HIGH_PH_ERROR;
float     IY_SOLAR_MID_PH_ERROR;
float     IY_SOLAR_LOW_PH_ERROR;

float     IB_SOLAR_HIGH_PH_ERROR;
float     IB_SOLAR_MID_PH_ERROR;
float     IB_SOLAR_LOW_PH_ERROR;


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
  unsigned Start        : 1;
  unsigned Error        : 1;
  unsigned End_Frame    : 1;
  unsigned ParityError  : 1;
  unsigned transfercomplete     : 1;
  unsigned FaultEventCheck      : 3;
  unsigned SettingError         : 1;
  unsigned FrameRecieved        : 1;
  unsigned USBCommunication     : 1;
  unsigned USBPending           : 1;
  unsigned DoubleData           :1;
  unsigned TransmissionFailed   :1;
  
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
// E.g. SPD has a NO contact which becomes NC when SPD Fails. So the entry
// "SPDFailed" is 0 when nothing is connected to the controller (as that indicates
// NO input which indicates SPD is healthy)
//
// DEVNOTE: This is sent over modbus. Keep digital inputs in order.
// Also, using one bool per input
//
struct DigInputs
{
    bool MainsRPhaseContactorOn; 
    bool MainsYPhaseContactorOn;
    bool MainsBPhaseContactorOn;
    bool LoadOnSolarContactorOn;
    bool LoadOnGridContactorOn;
    bool SPDFailed;             
    bool DGOff;              
    bool DC48Available;          
};

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
};

#endif
