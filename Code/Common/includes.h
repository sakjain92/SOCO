#ifndef INCLUDES_H
#define INCLUDES_H

#include "CommFlagDef.h"
#include "Struct.h"
#include "stm32f37x_flash.h"
#include <Math.h>
#include <stdio.h>
#include <string.h>

/* Local functions included */
void DisplayParameterUpdate(void); 
void ProtectionReset(void); 
void WriteFlashData(void);
void ClearInterruptVariables(void);
void UpdateDisplayIndex();
void StartCheckCTPolarity(void);
void RefreshLcd(void);
void CheckAutoScroll(void);
void DisplayDisabled(void);
void ReSetDisplayParameter(void);
void CalPF(float Error, float * CalGainBufferPointer,float * CalBetaBufferPointer);
void EepromWrite(uint16_t DataLocation,uint8_t NoOfBytes,uint8_t DeviceAddress,uint8_t *DataArray );
void I2CRead(uint16_t DataLocation,uint8_t NoOfBytes,uint8_t DeviceAddress,uint8_t *DataArray );
void CheckEpromFree(uint8_t DeviceAddress);
void DisplayString(uint8_t DV_Row,uint8_t DV_Digit, uint8_t *DV_String,uint8_t LED_Type);
void DisplayCalHighVI(void);
void DisplayCalMidVI(void);
void DisplayCalLowVI(void);
void DisplaySetHighPF(void);
void DisplaySetMidPF(void);
void DisplaySetLowPF(void);
void DisplayCalHighPF(void);
void DisplayCalMidPF(void);
void DisplayCalLowPF(void);
void InitUart(uint8_t baud,uint8_t parity1,uint8_t Stopbit1);
void ClearCalAccumulatedData(void);
void DisplayUpdate(void);
void InitializeHardware(void);
void DirectCalibration(void);
void PowerDownDataSave(void);
void AccumulateDataForCalibration(void);
void FillCurrentGainArray(void);
void SetWorkingGainBuffer(void);  
void SaveData(void);
void ProcessFreq(void);
void ProcessDemand(void);
extern void Metrology(void);
void StartCalibration(void);
void ProcessIntCycleOver(void);
void ProcessMFreq(void);
void CheckKey(void);
void InitWatchDog(void);
void DisplayImproperSettings(void);
void UpdateEditSettings(void);
uint16_t CRCCalculation(uint16_t * str,uint8_t length);
void MeterInit(void);
void SetInitialDisplay(void);
void SetVersionDisplay(void);
void DisplayCheckEnd(void);
void SaveFlashData(void);
void ModBusCommunication(void);
void Process1SecOver(void);
void Delay1Msec12Mhz( uint32_t Delay);
void ProcessMainInterrupt(void);
void Metrology(void);
void CalCtPtRatio(void);
void SaveOldData(void);
void SetMeterParameters(void);
void FillDefaultValue(void);
void InterruptDisplayRefresh(void);


/* Local variables Declarations */
struct STORE StorageBuffer;
struct STORE OldData;
struct PH_COEFF_STRUCT CalBuffer;
struct INT_DATA_SAVE   IntDataSave;
struct INT_DATA_SUM    IntDataSum;
struct OFFSET          VIOffset;
struct INST_PARA       InstantPara;
struct FFT_STRUCT      FftSampleData;
struct GAIN_WC         WorkingCopyGain; 
struct INT_TIMER_COUNT IntTimerCount;
struct STORE WCopyTripData;
struct DEMAND Demand; 
struct Object Timer;
struct DIS_SCROLL DisplaySetup ;
float DemandArray[122];
float CtRatio, PtRatio,CtPtRatio,StartingCurrent;
float BufferAlfaR[51],BufferAlfaY[51],BufferAlfaB[51];
float BufferBetaR[51],BufferBetaY[51],BufferBetaB[51]; 
double EnergyOverflowLimit;
uint64_t SumWattPerPulseTick;
uint32_t WattPerPulseTick,IntWattPerPulseTick;
uint32_t CaptureValueSum;
uint32_t InterruptFlag,RYFreqMeasDuration,SaveFreqMeasDuration;
uint16_t PreviousCaptureCount,CaptureValueSav,CaptureSumCounter,LastChangeFreq;
float IntRPrevSample,IntYPrevSample,IntBPrevSample;
uint16_t CopySetPara[MAX_PARAM_LIMIT+1],DataSaveCounter;
uint16_t AdcDataInArray[24],SampleCounter;
uint16_t OneSecFlag,MeterPassword;
uint16_t SwPressed,SwPrev,SwTimer,SwFlag,SwValue,SwTimeOverflow,SwTimeOverflow2,SwTimeOverflow3;
int16_t PrevSampleVol,FilOut_1,FilOut_2;
volatile uint16_t CounterSendComplete;
uint8_t ParaBlockIndex;
uint8_t Dec2DCBArray[16],DisplayScrollCounter;
uint8_t LcdEpromBuffer[128];
uint8_t SwitchPressed;
uint8_t CommIndexRS232,discard,CommFlagRS232,TransmitLengthRS232,DiscardRS232;
uint8_t KeyPressedCounter,EditIndex;
// FreqSampleFlag: Defines which voltage phase to use to detect frequency
uint8_t ParaBlockIndex,FreqSampleFlag;
uint8_t PowerFailCounter,ScrollChangeCounter;
uint8_t  Tc20ms,KWhLedOnCounter,UpdateDisplay;
uint8_t FreqFlag,CycleCounter;
uint8_t FlagDirectCalibration,CalibrationGapCounter,DisplayDebarCounter;
uint8_t OperatingSatatus,PowerUpCounter=0,SecondSourceActive,GenratorOnCounter;
uint8_t RecieveArray[MAX_BYTE_TO_RECIEVE];
uint16_t TimerCounter,NoOfBytes;
uint8_t ReceiveLength;
uint16_t LoadExportOrGenerator;
uint8_t  StepHigh,StepLow,ScrollSaveCounter=61;
uint8_t  RCurPrevStatus,YCurPrevStatus,BCurPrevStatus;
uint8_t pwrDlyFlag,StartDelayCount,EditPassCount;
uint8_t EditParaPassStatus;
float Reserved;
uint32_t Check485DirCount,Check485DirCounter;
uint16_t BufferToDisplay[24];
uint8_t DisplayParameterBuffer[50],dispStartCnt,DisParalast;

struct DigInputs g_DigInputs;

#endif
