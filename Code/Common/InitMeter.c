#include "CommFlagDef.h"
#include "Struct.h"
#include "stm32f37x_flash.h"
#include "extern_includes.h"
#include "math.h"


void ParaLocUpdate(uint16_t WriteAddress);
void StoredDataVerification(void);

// ---- EEPROM corruption recovery (boot) --------------------------------------
// On a settings/energy CRC failure we first RESTART up to 3 times. Only if
// the CRC still fails after 3 restarts do we treat it as real corruption and
// reset the affected part to defaults, recording it in ProductInfo (exposed and
// clearable over Modbus). This replaces the old InitilisationError() while(1)
// brick - the unit now always boots.
#define FAULT_SETTINGS       0x01u          // ProductInfo.EepromFaultFlag: settings reset
#define FAULT_DATA           0x02u          // ProductInfo.EepromFaultFlag: energy/data reset
#define EEPROM_RETRY_MAGIC   0x52545259u    // 'RTRY'
#define EEPROM_RETRY_STEP    0u
#define EEPROM_RETRY_CLEAR   1u

static void WriteDefaultSettings(void);
static void EepromRetry(uint8_t action);
static void SetEepromFaultFlag(uint8_t part);
#ifdef MODEL_DATA_SAVE
static void WriteDefaultEnergyData(void);
#endif


// Verify the external EEPROM is at least EEPROM_SIZE bytes and correctly
// addressable. I2C EEPROMs report no size; a part smaller than required folds
// high addresses onto low ones (address wrap), so we write a distinct marker to
// the last page of each 32KB block up to EEPROM_SIZE and read them all back. On
// a too-small chip the markers collide; on an absent / mis-soldered chip the
// reads NAK and return 0xFF - either way a readback mismatches. Goes through the
// normal EepromWrite/Read path, so a pass means FOTA's staging addresses (which
// use the same A16/A17 device-select addressing) are real and distinct.
// Non-destructive on a good board: the first-block marker (just below 32KB)
// sits in the unused metadata gap (zeroed moments later by NewMeterInit) and
// the rest in FOTA staging scratch.
// Returns true if the EEPROM is at least the required size, false otherwise.
static bool CheckEepromSize(void)
{
  // EEPROM parts step in size as 32 / 64 / 128 / 256 KB, so putting one marker
  // in every 32KB block up to the required size catches any too-small part.
  const uint32_t BLOCK_32KB = 32u * 1024u;
  const uint32_t numBlocks  = EEPROM_SIZE / BLOCK_32KB;   // e.g. 128KB -> 4 blocks
  uint32_t block, addr, marker, readback;

  // Marker in the last page of each 32KB block: 1*32KB, 2*32KB, ... numBlocks*32KB.
  for(block = 1u; block <= numBlocks; block++)
  {
    addr   = block * BLOCK_32KB - EEPROM_PAGE_LENGTH;   // last page of this 32KB block
    marker = addr ^ 0xA5A5A5A5u;                        // distinct per block, never 0x00 / 0xFF
    EepromWrite(addr, 4, EXT_EEPROM, (uint8_t *)&marker);
    RESET_WATCH_DOG;
  }
  for(block = 1u; block <= numBlocks; block++)
  {
    addr     = block * BLOCK_32KB - EEPROM_PAGE_LENGTH;
    marker   = addr ^ 0xA5A5A5A5u;
    readback = 0;
    EepromRead(addr, 4, EXT_EEPROM, (uint8_t *)&readback);
    RESET_WATCH_DOG;
    if(readback != marker) return false;
  }
  return true;
}

// Never returns: blink all front-panel LEDs together (~2.5 Hz) forever, feeding
// the watchdog so the unit stays in this visible fault state instead of
// resetting. Used when CheckEepromSize() fails at first boot - a clear "pull
// this board" signal on the production line. Runs with interrupts still off (the
// SysTick display refresh is not up yet), so it drives the LED GPIOs directly
// and busy-waits with Delay1Msec12Mhz.
static void EepromSizeErrorBlink(void)
{
  for(;;)
  {
    SWITCH_ON_LED1; SWITCH_ON_LED2; SWITCH_ON_LED3; SWITCH_ON_LED4; SWITCH_ON_LED5;
    SWITCH_ON_LED6; SWITCH_ON_LED7; SWITCH_ON_LED8; SWITCH_ON_LED9;
    RESET_WATCH_DOG;
    Delay1Msec12Mhz(200);

    SWITCH_OFF_LED1; SWITCH_OFF_LED2; SWITCH_OFF_LED3; SWITCH_OFF_LED4; SWITCH_OFF_LED5;
    SWITCH_OFF_LED6; SWITCH_OFF_LED7; SWITCH_OFF_LED8; SWITCH_OFF_LED9;
    RESET_WATCH_DOG;
    Delay1Msec12Mhz(200);
  }
}

void NewMeterInit(void)
{
  uint16_t i;
  uint16_t temp;
  RESET_WATCH_DOG;

  if((CalibrationCoeff.INIT_DATA1!=METER_INIT_VALUE)||(CalibrationCoeff.INIT_DATA2!=METER_INIT_VALUE))
  {
    // Fresh / uncalibrated unit (first-ever boot): confirm the external EEPROM
    // is the required size and addressable before initialising it. A too-small
    // or absent part (BOM / soldering error) makes FOTA staging wrap and corrupt
    // calibration, yet passes a normal functional test since all live data fits
    // in the first 32KB. Halt on failure so the defect is caught in production.
    // Runs ONLY on fresh units, so a transient EEPROM glitch in the field can
    // never brick a calibrated unit (the CRC retry / de-brick handles that).
    if(!CheckEepromSize())
    {
      EepromSizeErrorBlink();      // blinks all LEDs forever; never returns
    }
    ProtectionReset();
    for(i=0;i<64;i++)LcdEpromBuffer[i]=0; 
    for(i=0;i<MAX_METADATA_NUM_PAGES;i++) 
    {
      EepromWrite(i*64,64,EXT_EEPROM,LcdEpromBuffer );
      RESET_WATCH_DOG;
    } 
    temp=DEFAULT_METER_PASSWORD;
    EepromWrite(PASSWORD_SAV_LOC,2,EXT_EEPROM,(uint8_t *)&temp );
    WriteDefaultSettings();
    SaveFlashData();
    CalBuffer.INIT_DATA1=METER_INIT_VALUE;
    CalBuffer.INIT_DATA2=METER_INIT_VALUE;
    WriteFlashData();    
#ifdef  MODEL_DATA_SAVE
    WriteDefaultEnergyData();
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

// Reset the user parameter block (both program-data copies) to factory
// defaults. Reused by NewMeterInit (fresh unit) and the corruption recovery.
// Does NOT touch calibration (flash) or the keypad password.
static void WriteDefaultSettings(void)
{
  uint16_t temp;
  FillDefaultValue();
  temp = CRCCalculation(CopySetPara, MAX_PARAM_LIMIT);
  CopySetPara[MAX_PARAM_LIMIT] = temp;
  ParaLocUpdate(PROGRAM_DATA_LOC1_START);
  ParaLocUpdate(PROGRAM_DATA_LOC2_START);
}

#ifdef MODEL_DATA_SAVE
// Zero all energy/storage counters and persist. Reused by NewMeterInit (fresh
// unit) and the corruption recovery.
static void WriteDefaultEnergyData(void)
{
  uint16_t i, Tempointer;
  for(i=0;i<sizeof(StorageBuffer);i++)*((uint8_t *)&StorageBuffer+i)=0;
  StorageBuffer.ImportVarhNeg=0.00001;
  StorageBuffer.ExportVarhNeg=0.00001;
  StorageBuffer.SolarImportVarhNeg=0.00001;
  StorageBuffer.SolarExportVarhNeg=0.00001;
  StorageBuffer.StorageCounter=1;
  StorageBuffer.StorageLocation=DATA_SAVE_START_LOC;
  Tempointer=offsetof(struct STORE,StoreCRC);
  StorageBuffer.StoreCRC=CRCCalculation((uint16_t *)&StorageBuffer,Tempointer/2);
  EepromWrite(StorageBuffer.StorageLocation,sizeof(StorageBuffer),EXT_EEPROM,(uint8_t *)&StorageBuffer );
  PowerDownDataSave();
  SaveOldData();
}
#endif

// Boot EEPROM-retry counter. Survives our NVIC_SystemReset (a warm reset keeps
// __no_init RAM) but starts fresh on a true power-on (garbage RAM -> magic
// mismatch). All access goes through here so the statics stay function-local.
//   EEPROM_RETRY_STEP  : CRC failed -> restart (up to 3x) to free a wedged
//                        EEPROM; only returns once the 3 restarts are spent, so
//                        the caller then applies defaults.
//   EEPROM_RETRY_CLEAR : data read valid, or defaults written -> reset counter.
static void EepromRetry(uint8_t action)
{
  __no_init static volatile uint32_t magic;
  __no_init static volatile uint8_t  count;

  if (magic != EEPROM_RETRY_MAGIC) { magic = EEPROM_RETRY_MAGIC; count = 0; }

  if (action == EEPROM_RETRY_CLEAR) { count = 0; return; }

  if (count < 3u) { count++; NVIC_SystemReset(); }   // restart (does not return)
  /* count == 3: fall through -> caller writes defaults */
}

// Record + persist that an EEPROM corruption forced a reset. Writes ONLY the
// 4-byte EepromFaultFlag field (accumulated in RAM across this boot), never the
// whole struct - so a flaky EEPROM read can never land garbage over the
// (CRC-less) SerialNumber / Calibrated fields. The M24256 partial-page write
// touches just those 4 bytes; the rest of the ProductInfo page is untouched.
static void SetEepromFaultFlag(uint8_t part)
{
  static uint32_t accum = 0;               // faults this boot: settings | energy
  accum |= part;
  g_ProductInfo.EepromFaultFlag = accum;   // RAM copy so Modbus reflects it this session
  EepromWrite(PRODUCT_INFO_LOC + offsetof(struct ProductInfo, EepromFaultFlag),
              sizeof(g_ProductInfo.EepromFaultFlag), EXT_EEPROM,
              (uint8_t *)&g_ProductInfo.EepromFaultFlag);
}


// True until all three factory steps (calibration, serial number,
// functional test) have been recorded in g_ProductInfo. Used both at
// boot to decide whether to re-enter test mode and at runtime to drive
// the test-mode display blink. Both predicates must agree; otherwise a
// reset between two factory steps leaves the unit silently in test
// mode with no visual indicator.
//
bool IsUnitFactoryComplete(void)
{
  bool serialNumberWritten =
      (g_ProductInfo.SerialNumber[0] != 0) || (g_ProductInfo.SerialNumber[1] != 0);
  return serialNumberWritten &&
         g_ProductInfo.FunctionallyTestedFlag &&
         g_ProductInfo.CalibratedFlag;
}


void MeterInit(void)
{

  NewMeterInit();
  StoredDataVerification();
  SetMeterParameters();

  // Auto-enable test mode on uncalibrated/untested units so the
  // factory testing script can immediately drive outputs and calibrate
  // without a separate Modbus write to enter test mode.
  //
  // This must live in MeterInit() (one-shot at boot), not in
  // SetMeterParameters(), because SetMeterParameters() is also called
  // from UpdateEditSettings() and ParaSettingUpdate() after any
  // parameter write. Touching TestingModeEnabled there would silently
  // kick the factory script out of test mode whenever it writes a
  // parameter.
  //
#ifdef MODEL_RELEASED
  if (!IsUnitFactoryComplete())
  {
      g_testingStatus.TestingModeEnabled = true;
  }
  else
#endif
  {
      g_testingStatus.TestingModeEnabled = false;
  }
}
  
void StoredDataVerification(void)
{   
  
  uint16_t temp1;
  uint8_t temp,k; 
  uint16_t Tempointer;
  temp=0;
  RESET_WATCH_DOG;
  k=MAX_PARAM_LIMIT*2+2; // if k is more than 254 it needs to be read in 2 steps
  EepromRead(PROGRAM_DATA_LOC1_START,k,EXT_EEPROM,(uint8_t *)CopySetPara );
  temp1=CRCCalculation((uint16_t *)CopySetPara,MAX_PARAM_LIMIT+1);  
  if(temp1 !=0)temp=1;
  EepromRead(PROGRAM_DATA_LOC2_START,k,EXT_EEPROM,(uint8_t *)CopySetPara );
  temp1=CRCCalculation((uint16_t *)CopySetPara,MAX_PARAM_LIMIT+1);  
  if(temp1 !=0)temp=temp|0x02;
  if(temp!=0)
  {
      if(temp==1)
      {
         EepromRead(PROGRAM_DATA_LOC2_START,k,EXT_EEPROM,(uint8_t *)CopySetPara );
         ParaLocUpdate(PROGRAM_DATA_LOC1_START);
         RESET_WATCH_DOG;
      }
        
      else if(temp==2) 
      {
        EepromRead(PROGRAM_DATA_LOC1_START,k,EXT_EEPROM,(uint8_t *)CopySetPara );
        ParaLocUpdate(PROGRAM_DATA_LOC2_START);
         RESET_WATCH_DOG;
      }  
      else
      {
        // Both program-data copies failed CRC. Restart up to 3x to free a
        // wedged EEPROM; if still bad after that, reset settings to default.
        EepromRetry(EEPROM_RETRY_STEP);
        WriteDefaultSettings();
        SetEepromFaultFlag(FAULT_SETTINGS);
      } 
  }
  else EepromRead(PROGRAM_DATA_LOC2_START,k,EXT_EEPROM,(uint8_t *)CopySetPara );
#ifdef MODEL_DATA_SAVE  
  RESET_WATCH_DOG;
  uint16_t TempCrc,i;
  uint32_t TempStorageCounter=0,TempStorageLoc=0;
  k=0;
  // check validity of flash and eeprom all 40 locations.
  for(i=0;i<NUM_DATA_PAGES;i++)
  {
    EepromRead(DATA_SAVE_START_LOC+i*MAX_DATA_SAVE_SIZE,sizeof(StorageBuffer),EXT_EEPROM,LcdEpromBuffer);
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
  EepromRead(POWER_DN_SAVE_PAGE,sizeof(StorageBuffer),EXT_EEPROM,LcdEpromBuffer);
  
  temp=offsetof(struct STORE,StoreCRC)/2 +1;
  if((TempCrc=CRCCalculation((uint16_t *)LcdEpromBuffer,temp))==0)
  {
    if(*(uint32_t *)&LcdEpromBuffer[0]>TempStorageCounter)
    {
      EepromRead(POWER_DN_SAVE_PAGE,sizeof(StorageBuffer),EXT_EEPROM,(uint8_t*)&StorageBuffer);
    }
    else
    {
      if(k==0)
      {
        // No valid stored energy copy. Restart up to 3x to free a wedged
        // EEPROM; if still bad after that, reset energy data to default.
        EepromRetry(EEPROM_RETRY_STEP);
        WriteDefaultEnergyData();
        SetEepromFaultFlag(FAULT_DATA);
      } 
      else EepromRead(TempStorageLoc,sizeof(StorageBuffer),EXT_EEPROM,(uint8_t*)&StorageBuffer);
    }
  }
  else
  {
    if(k==0)
      {
        // No valid stored energy copy. Restart up to 3x to free a wedged
        // EEPROM; if still bad after that, reset energy data to default.
        EepromRetry(EEPROM_RETRY_STEP);
        WriteDefaultEnergyData();
        SetEepromFaultFlag(FAULT_DATA);
      } 
    else EepromRead(TempStorageLoc,sizeof(StorageBuffer),EXT_EEPROM,(uint8_t*)&StorageBuffer);
  }
#endif // MODEL_DATA_SAVE
  RESET_WATCH_DOG;;
  // Valid data reached (read OK, recovered from the 2nd copy, or defaults
  // written) -> clear the boot retry counter.
  EepromRetry(EEPROM_RETRY_CLEAR);
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

/*
Inf: Customer-initiated "reset every persistent setting to factory default"
     command, dispatched from the Modbus FC=0x10 handler at the dedicated
     reset-settings address. Rewrites all 24 PARA_* values back to their
     EditParameters[].DefaultValue, resets the keypad-EDIT password to the
     factory value (123), and persists both. UART settings (Device ID, Baud
     Rate, Parity, Stop Bit) re-apply via SetMeterParameters(): the active
     Modbus link WILL drop unless the master was already using defaults,
     and the master must reconnect at 9600/8N1/ID=1 to talk to the meter
     after this command. Energy counters are NOT touched here; use the
     dedicated reset-energy command for that.
Inp: None
Ret: None
*/
void ResetAllSettingsToDefault(void)
{
  uint16_t defaultPass = DEFAULT_METER_PASSWORD;

  RESET_WATCH_DOG;
  __disable_interrupt();
  __no_operation();

  // 1. Password back to factory value, both in RAM and EEPROM.
  EepromWrite(PASSWORD_SAV_LOC, 2, EXT_EEPROM, (uint8_t *)&defaultPass);
  MeterPassword = defaultPass;

  // 2. Parameters back to factory defaults, persisted to both program-data
  //    slots (PROGRAM_DATA_LOC1_START / PROGRAM_DATA_LOC2_START) by
  //    ParaSettingUpdate() so a torn write to one slot still has a good
  //    copy in the other.
  FillDefaultValue();

  __enable_interrupt();
  ParaSettingUpdate();
  RESET_WATCH_DOG;
}
