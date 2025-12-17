
#include "CommFlagDef.h"


extern struct DEMAND Demand; 

extern struct STORE StorageBuffer;
extern struct STORE OldData;
extern  struct STORE WCopyTripData;
extern struct INST_PARA InstantPara;
extern uint16_t CopySetPara[MAX_PARAM_LIMIT+1];
extern struct Object Timer;
//extern struct Field Mod_TransmitFrame;
extern uint16_t LoadExportOrGenerator;
extern float PTPrimary, CTPrimary,PTSecondary,CTSecondary;
extern float ModbusDummyFloatRegister;

#define CT_RATIO       2
#define PT_RATIO       3
#define CT_PT_RATIO    4

// None of these should be more than 60 as we allow user to read all
// registers in a block at a time
//
#define InstPara_NewAll         58
#define InstPara_THD            6
#define InstPara_ENERGYIMPORT   7
#define InstPara_ENERGYEXPORT   7
#define InstPara_ENERGYOLD      14
#define InstPara_ENERGYTrip     14
#define InstPara_TotalRMS       9
#define InstPara_RphaseRMS      11
#define InstPara_YphaseRMS      11
#define InstPara_BphaseRMS      11
#define InstPara_Demand         18
#define InstPara_LoadOnGridDisableSec 1
#define InstPara_NewSolarAll    58
#define InstPara_Solar_ENERGYIMPORT   7
#define InstPara_Solar_ENERGYEXPORT   7

#define InstPara_NewAll_Block1     27
#define InstPara_NewAll_Block2     31

#define DATA_TYPE_16            2
#define DATA_TYPE_32            4
#define DATA_TYPE_Double        90
#define DATA_TYPE_Float		91
					
					
const struct ModBusParameter BlockAll[]=							
{							
  { (uint8_t *)&LoadExportOrGenerator	,	       DATA_TYPE_16	,	1	,	1	        },
#ifdef MODBUS_VOL							
  { (uint8_t *)&InstantPara.VolR	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolY	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolB	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolRY	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolYB	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolBR	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.VolR	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolY	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolB	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	}, 
  { (uint8_t *)&InstantPara.VolRY	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolYB	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolBR	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	},
#endif
							
  
#ifdef MODBUS_CUR							
  { (uint8_t *)&InstantPara.CurrentR	,	       DATA_TYPE_Float	,	1	,	CT_RATIO	},
  { (uint8_t *)&InstantPara.CurrentY	,	       DATA_TYPE_Float	,	1	,	CT_RATIO	},
  { (uint8_t *)&InstantPara.CurrentB	,	       DATA_TYPE_Float	,	1	,	CT_RATIO	},
#else
  { (uint8_t *)&InstantPara.CurrentR	,	       DATA_TYPE_Float	,	0	,	CT_RATIO	},
  { (uint8_t *)&InstantPara.CurrentY	,	       DATA_TYPE_Float	,	0	,	CT_RATIO	},
  { (uint8_t *)&InstantPara.CurrentB	,	       DATA_TYPE_Float	,	0	,	CT_RATIO	},
#endif
#ifdef  MODBUS_CUR_NEU
  { (uint8_t *)&InstantPara.CurrentN	,	       DATA_TYPE_Float	,	1	,	CT_RATIO	},
#else
  { (uint8_t *)&InstantPara.CurrentN	,	       DATA_TYPE_Float	,	0	,	CT_RATIO	},
#endif
#ifdef MODBUS_ACTIVE_POWER						
  { (uint8_t *)&InstantPara.TotalPowerR	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalPowerY	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalPowerB	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.SumTotalPower,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.TotalPowerR	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalPowerY	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalPowerB	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.SumTotalPower,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_FUN_POWER							
  { (uint8_t *)&InstantPara.FunPowerR	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.FunPowerY	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.FunPowerB	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.SumFunPower	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.FunPowerR	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.FunPowerY	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.FunPowerB	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.SumFunPower	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef	MODBUS_APP_POWER						
  { (uint8_t *)&InstantPara.AppPowerR	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.AppPowerY	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.AppPowerB	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalAppPower,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.AppPowerR	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.AppPowerY	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.AppPowerB	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalAppPower,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef	MODBUS_REACT_POWER							
  { (uint8_t *)&InstantPara.ReactPowerR	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.ReactPowerY	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.ReactPowerB	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalReactPower,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.ReactPowerR	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.ReactPowerY	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.ReactPowerB	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalReactPower,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////27
#ifdef MODBUS_POWER_FACTOR							
  { (uint8_t *)&InstantPara.PowerFactorR	,       DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.PowerFactorY	,       DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.PowerFactorB	,	DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.TotalPowerFactor	,	DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.PowerFactorR	,       DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.PowerFactorY	,       DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.PowerFactorB	,	DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.TotalPowerFactor	,	DATA_TYPE_Float	,	0	,	1	},
#endif
#ifdef 	MODBUS_FREQ						
  { (uint8_t *)&InstantPara.Frequency	,	       DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.Frequency	,	       DATA_TYPE_Float	,	0	,	1	},
#endif
#ifdef MODBUS_FUN_CUR
  { (uint8_t *)&InstantPara.FunRCurr	,	       DATA_TYPE_Float	,	1	,	CT_RATIO	},
  { (uint8_t *)&InstantPara.FunYCurr	,	       DATA_TYPE_Float	,	1	,	CT_RATIO	},
  { (uint8_t *)&InstantPara.FunBCurr	,	       DATA_TYPE_Float	,	1	,	CT_RATIO	},
#else
  { (uint8_t *)&InstantPara.FunRCurr	,	       DATA_TYPE_Float	,	0	,	CT_RATIO	},
  { (uint8_t *)&InstantPara.FunYCurr	,	       DATA_TYPE_Float	,	0	,	CT_RATIO	},
  { (uint8_t *)&InstantPara.FunBCurr	,	       DATA_TYPE_Float	,	0	,	CT_RATIO	},
#endif
#ifdef MODBUS_FUN_CUR_NEU  
  { (uint8_t *)&InstantPara.FunNCurr	,	       DATA_TYPE_Float	,	1	,	CT_RATIO	},
#else
  { (uint8_t *)&InstantPara.FunNCurr	,	       DATA_TYPE_Float	,	0	,	CT_RATIO	},
#endif
#ifdef MODBUS_FUN_VOL							
  { (uint8_t *)&InstantPara.FunRVol	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.FunYVol	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.FunBVol	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.FunRVol	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.FunYVol	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.FunBVol	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	},
#endif
#ifdef MODBUS_AVG_CUR								
  { (uint8_t *)&InstantPara.AvgCurr	,	       DATA_TYPE_Float	,	1	,	CT_RATIO	},
#else
  { (uint8_t *)&InstantPara.AvgCurr	,	       DATA_TYPE_Float	,	0	,	CT_RATIO	},
#endif
#ifdef MODBUS_CUR_UNBAL
  { (uint8_t *)&InstantPara.CurUnb	,	       DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.CurUnb	,	       DATA_TYPE_Float	,	0	,	1	},
#endif
#ifdef MODBUS_VOL_UNBAL  
  { (uint8_t *)&InstantPara.VolUnb	,	       DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.VolUnb	,	       DATA_TYPE_Float	,	0	,	1	},
#endif
#ifdef MODBUS_ANGLE							
  { (uint8_t *)&InstantPara.AngleRPhase	,	       DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.AngleYPhase	,	       DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.AngleBPhase	,	       DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.AngleRPhase	,	       DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.AngleYPhase	,	       DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.AngleBPhase	,	       DATA_TYPE_Float	,	0	,	1	},
#endif

  ////////////////////////     THD    /////////////////////////////	18					
#ifdef MODBUS_THD 							
  { (uint8_t *)&InstantPara.ThdRPhaseVol	,	DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.ThdYPhaseVol	,	DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.ThdBPhaseVol	,	DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.ThdRPhaseCur	,	DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.ThdYPhaseCur	,	DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.ThdBPhaseCur	,	DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.ThdRPhaseVol	,	DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.ThdYPhaseVol	,	DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.ThdBPhaseVol	,	DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.ThdRPhaseCur	,	DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.ThdYPhaseCur	,	DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.ThdBPhaseCur	,	DATA_TYPE_Float	,	0	,	1	},
#endif
/////////////////////////////////////////////////////////////////////	6
#ifdef MODBUS_AVG_VOL
  { (uint8_t *)&InstantPara.VolLLAvg	,	       DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.VolLNAvg	,	       DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.VolLLAvg	,	       DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.VolLNAvg	,	       DATA_TYPE_Float	,	0	,	1	},
#endif
  
#ifdef MODBUS_CURR_TOTAL
  { (uint8_t *)&InstantPara.CurrentTotal,	       DATA_TYPE_Float	,	1	,	1	},
#else
    { (uint8_t *)&InstantPara.CurrentTotal,	       DATA_TYPE_Float	,	0	,	1	},
#endif
  
#ifdef MODBUS_CTPT_Ratio
  { (uint8_t *)&PTPrimary	,	       DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&PTSecondary	,	       DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&CTPrimary	,	       DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&CTSecondary	,	       DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&PTPrimary	,	       DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&PTSecondary	,	       DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&CTPrimary	,	       DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&CTSecondary	,	       DATA_TYPE_Float	,	0	,	1	},
#endif 
  
  
  ////////////////////////     THD    ////////////////////////////////////////////	7					
#ifdef MODBUS_THD 							
  { (uint8_t *)&InstantPara.ThdRPhaseVol	,	DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.ThdYPhaseVol	,	DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.ThdBPhaseVol	,	DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.ThdRPhaseCur	,	DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.ThdYPhaseCur	,	DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.ThdBPhaseCur	,	DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.ThdRPhaseVol	,	DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.ThdYPhaseVol	,	DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.ThdBPhaseVol	,	DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.ThdRPhaseCur	,	DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.ThdYPhaseCur	,	DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.ThdBPhaseCur	,	DATA_TYPE_Float	,	0	,	1	},
#endif  
  
  
  ////////////////////////    ENERGY IMPORT  /////////////////////////////	6						
#ifdef MODBUS_RH_IMPORT								
  { (uint8_t *)&StorageBuffer.RunHourImport	,	  DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.RunHourImport	,	  DATA_TYPE_32	,	0	,	1	},
#endif
#ifdef MODBUS_LH_IMPORT
  { (uint8_t *)&StorageBuffer.LoadHourImport	,	  DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.LoadHourImport	,	  DATA_TYPE_32	,	0	,	1	},
#endif

#ifdef MODBUS_WH_IMPORT
  { (uint8_t *)&StorageBuffer.ImportWh	        ,	  DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.ImportWh	        ,	  DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_VAH_IMPORT
  { (uint8_t *)&StorageBuffer.ImportVAh	        ,	  DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.ImportVAh	        ,	  DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_VARH_IMPORT
  { (uint8_t *)&StorageBuffer.ImportVarhPos	,	  DATA_TYPE_Double	,	1	,	1	},
  { (uint8_t *)&StorageBuffer.ImportVarhNeg	,	  DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.ImportVarhPos	,	  DATA_TYPE_Double	,	0	,	1	},
  { (uint8_t *)&StorageBuffer.ImportVarhNeg	,	  DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_INTERRUPT_IMPORT
  { (uint8_t *)&StorageBuffer.ImportInterruptions,	  DATA_TYPE_16	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.ImportInterruptions,	  DATA_TYPE_16	,	0	,	1	},
#endif
  							
  ////////////////////////    ENERGY EXPORT  /////////////////////////////							
#ifdef MODBUS_RH_EXPORT					
  { (uint8_t *)&StorageBuffer.RunHourExport	,	   DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.RunHourExport	,	   DATA_TYPE_32	,	0	,	1	},
#endif
#ifdef MODBUS_LH_EXPORT	  
  { (uint8_t *)&StorageBuffer.LoadHourExport	,	   DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.LoadHourExport	,	   DATA_TYPE_32	,	0	,	1	},
#endif
#ifdef MODBUS_WH_EXPORT  
  { (uint8_t *)&StorageBuffer.ExportWh	,	           DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.ExportWh	,	           DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_VAH_EXPORT
  { (uint8_t *)&StorageBuffer.ExportVAh	,	           DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.ExportVAh	,	           DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_VARH_EXPORT  
  { (uint8_t *)&StorageBuffer.ExportVarhPos	,	   DATA_TYPE_Double	,	1	,	1	},
  { (uint8_t *)&StorageBuffer.ExportVarhNeg	,	   DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.ExportVarhPos	,	   DATA_TYPE_Double	,	0	,	1	},
  { (uint8_t *)&StorageBuffer.ExportVarhNeg	,	   DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_INTERRUPT_EXPORT  
  { (uint8_t *)&StorageBuffer.ExportInterruptions,	   DATA_TYPE_16	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.ExportInterruptions,	   DATA_TYPE_16	,	0	,	1	},
#endif
    							
  							
  //////////////////////////  OLD Energy      //////////////////////////////							
  							
#ifdef MODBUS_OLD_RH_IMPORT								
  { (uint8_t *)&OldData.RunHourImport	,	  DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&OldData.RunHourImport	,	  DATA_TYPE_32	,	0	,	1	},
#endif
#ifdef MODBUS_OLD_LH_IMPORT
  { (uint8_t *)&OldData.LoadHourImport	,	  DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&OldData.LoadHourImport	,	  DATA_TYPE_32	,	0	,	1	},
#endif

#ifdef MODBUS_OLD_WH_IMPORT
  { (uint8_t *)&OldData.ImportWh	,	  DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&OldData.ImportWh	,	  DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_OLD_VAH_IMPORT
  { (uint8_t *)&OldData.ImportVAh	,	  DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&OldData.ImportVAh	,	  DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_OLD_VARH_IMPORT
  { (uint8_t *)&OldData.ImportVarhPos	,	  DATA_TYPE_Double	,	1	,	1	},
  { (uint8_t *)&OldData.ImportVarhNeg	,	  DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&OldData.ImportVarhPos	,	  DATA_TYPE_Double	,	0	,	1	},
  { (uint8_t *)&OldData.ImportVarhNeg	,	  DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_OLD_INTERRUPT_IMPORT
  { (uint8_t *)&OldData.ImportInterruptions,	  DATA_TYPE_16	,	1	,	1	},
#else
  { (uint8_t *)&OldData.ImportInterruptions,	  DATA_TYPE_16	,	0	,	1	},
#endif
  							
							
#ifdef MODBUS_OLD_RH_EXPORT					
  { (uint8_t *)&OldData.RunHourExport	,	   DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&OldData.RunHourExport	,	   DATA_TYPE_32	,	0	,	1	},
#endif
#ifdef MODBUS_OLD_LH_EXPORT	  
  { (uint8_t *)&OldData.LoadHourExport	,	   DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&OldData.LoadHourExport	,	   DATA_TYPE_32	,	0	,	1	},
#endif
#ifdef MODBUS_OLD_WH_EXPORT  
  { (uint8_t *)&OldData.ExportWh	,	   DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&OldData.ExportWh	,	   DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_OLD_VAH_EXPORT
  { (uint8_t *)&OldData.ExportVAh	,	   DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&OldData.ExportVAh	,	   DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_OLD_VARH_EXPORT  
  { (uint8_t *)&OldData.ExportVarhPos	,	   DATA_TYPE_Double	,	1	,	1	},
  { (uint8_t *)&OldData.ExportVarhNeg	,	   DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&OldData.ExportVarhPos	,	   DATA_TYPE_Double	,	0	,	1	},
  { (uint8_t *)&OldData.ExportVarhNeg	,	   DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_OLD_INTERRUPT_EXPORT  
  { (uint8_t *)&OldData.ExportInterruptions,	   DATA_TYPE_16	,	1	,	1	},
#else
  { (uint8_t *)&OldData.ExportInterruptions,	   DATA_TYPE_16	,	0	,	1	},
#endif
  							
  							
  /////////////////////////  Trip Energy   ////////////////////////////							
  							
#ifdef MODBUS_TRIP_RH_IMPORT								
  { (uint8_t *)&WCopyTripData.RunHourImport	,	  DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&WCopyTripData.RunHourImport	,	  DATA_TYPE_32	,	0	,	1	},
#endif
#ifdef MODBUS_TRIP_LH_IMPORT
  { (uint8_t *)&WCopyTripData.LoadHourImport	,	  DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&WCopyTripData.LoadHourImport	,	  DATA_TYPE_32	,	0	,	1	},
#endif

#ifdef MODBUS_TRIP_WH_IMPORT
  { (uint8_t *)&WCopyTripData.ImportWh	,	         DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&WCopyTripData.ImportWh	,	         DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_TRIP_VAH_IMPORT
  { (uint8_t *)&WCopyTripData.ImportVAh	,	         DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&WCopyTripData.ImportVAh	,	         DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_TRIP_VARH_IMPORT
  { (uint8_t *)&WCopyTripData.ImportVarhPos	,	  DATA_TYPE_Double	,	1	,	1	},
  { (uint8_t *)&WCopyTripData.ImportVarhNeg	,	  DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&WCopyTripData.ImportVarhPos	,	  DATA_TYPE_Double	,	0	,	1	},
  { (uint8_t *)&WCopyTripData.ImportVarhNeg	,	  DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_TRIP_INTERRUPT_IMPORT
  { (uint8_t *)&WCopyTripData.ImportInterruptions,	  DATA_TYPE_16	,	1	,	1	},
#else
  { (uint8_t *)&WCopyTripData.ImportInterruptions,	  DATA_TYPE_16	,	0	,	1	},
#endif
  							
							
#ifdef MODBUS_TRIP_RH_EXPORT					
  { (uint8_t *)&WCopyTripData.RunHourExport	,	   DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&WCopyTripData.RunHourExport	,	   DATA_TYPE_32	,	0	,	1	},
#endif
#ifdef MODBUS_TRIP_LH_EXPORT	  
  { (uint8_t *)&WCopyTripData.LoadHourExport	,	   DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&WCopyTripData.LoadHourExport	,	   DATA_TYPE_32	,	0	,	1	},
#endif
#ifdef MODBUS_TRIP_WH_EXPORT  
  { (uint8_t *)&WCopyTripData.ExportWh	,	           DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&WCopyTripData.ExportWh	,	           DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_TRIP_VAH_EXPORT
  { (uint8_t *)&WCopyTripData.ExportVAh	,	           DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&WCopyTripData.ExportVAh	,	           DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_TRIP_VARH_EXPORT  
  { (uint8_t *)&WCopyTripData.ExportVarhPos	,	   DATA_TYPE_Double	,	1	,	1	},
  { (uint8_t *)&WCopyTripData.ExportVarhNeg	,	   DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&WCopyTripData.ExportVarhPos	,	   DATA_TYPE_Double	,	0	,	1	},
  { (uint8_t *)&WCopyTripData.ExportVarhNeg	,	   DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_TRIP_INTERRUPT_EXPORT  
  { (uint8_t *)&WCopyTripData.ExportInterruptions,	   DATA_TYPE_16	,	1	,	1	},
#else
  { (uint8_t *)&WCopyTripData.ExportInterruptions,	   DATA_TYPE_16	,	0	,	1	},
#endif
    							
  							

  							
    ///////////////////////   TotalRMS   /////////////////////////////							
#ifdef MODBUS_ACTIVE_POWER  							
  { (uint8_t *)&InstantPara.SumTotalPower,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.SumTotalPower,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_FUN_POWER  							
   { (uint8_t *)&InstantPara.SumFunPower,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
   { (uint8_t *)&InstantPara.SumFunPower,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_APP_POWER  							
  { (uint8_t *)&InstantPara.TotalAppPower,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.TotalAppPower,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_REACT_POWER  							
  { (uint8_t *)&InstantPara.TotalReactPower,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.TotalReactPower,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_POWER_FACTOR  							
  { (uint8_t *)&InstantPara.TotalPowerFactor,	   DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.TotalPowerFactor,	   DATA_TYPE_Float	,	0	,	1	},
#endif
#ifdef 	MODBUS_FREQ  							
  { (uint8_t *)&InstantPara.Frequency,	           DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.Frequency,	           DATA_TYPE_Float	,	0	,	1	},
#endif
#ifdef MODBUS_AVG_CUR  							
  { (uint8_t *)&InstantPara.AvgCurr,	           DATA_TYPE_Float	,	1	,	CT_RATIO	},
#else
  { (uint8_t *)&InstantPara.AvgCurr,	           DATA_TYPE_Float	,	0	,	CT_RATIO	},
#endif
#ifdef MODBUS_CUR_UNBAL  							
  { (uint8_t *)&InstantPara.CurUnb,	           DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.CurUnb,	           DATA_TYPE_Float	,	0	,	1	},
#endif
#ifdef MODBUS_VOL_UNBAL  							
   { (uint8_t *)&InstantPara.VolUnb,	           DATA_TYPE_Float	,	1	,	1	},
#else
   { (uint8_t *)&InstantPara.VolUnb,	           DATA_TYPE_Float	,	0	,	1	},
#endif

  							
  //////////////////////   RphaseRMS  ///////////////////////////////	

#ifdef MODBUS_VOL  							
  { (uint8_t *)&InstantPara.VolR	,	   DATA_TYPE_Float	,	1	,	PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.VolR	,	   DATA_TYPE_Float	,	0	,	PT_RATIO	},
#endif
#ifdef MODBUS_VOL  							
   { (uint8_t *)&InstantPara.VolRY	,	   DATA_TYPE_Float	,	1	,	PT_RATIO	},
#else
   { (uint8_t *)&InstantPara.VolRY	,	   DATA_TYPE_Float	,	0	,	PT_RATIO	},
#endif
#ifdef MODBUS_CUR  							
  { (uint8_t *)&InstantPara.CurrentR	,	   DATA_TYPE_Float	,	1	,	CT_RATIO	},
#else
  { (uint8_t *)&InstantPara.CurrentR	,	   DATA_TYPE_Float	,	0	,	CT_RATIO	},
#endif
#ifdef MODBUS_ACTIVE_POWER  							
  { (uint8_t *)&InstantPara.TotalPowerR	,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.TotalPowerR	,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_FUN_POWER  							
  { (uint8_t *)&InstantPara.FunPowerR	,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.FunPowerR	,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_APP_POWER  							
  { (uint8_t *)&InstantPara.AppPowerR	,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.AppPowerR	,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_REACT_POWER  							
   { (uint8_t *)&InstantPara.ReactPowerR,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
   { (uint8_t *)&InstantPara.ReactPowerR,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_POWER_FACTOR  							
  { (uint8_t *)&InstantPara.PowerFactorR,	   DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.PowerFactorR,	   DATA_TYPE_Float	,	0	,	1	},
#endif
#ifdef MODBUS_FUN_CUR  							
  { (uint8_t *)&InstantPara.FunRCurr	,	   DATA_TYPE_Float	,	1	,	CT_RATIO	},
#else
  { (uint8_t *)&InstantPara.FunRCurr	,	   DATA_TYPE_Float	,	0	,	CT_RATIO	},
#endif
#ifdef MODBUS_FUN_VOL  							
  { (uint8_t *)&InstantPara.FunRVol	,	   DATA_TYPE_Float	,	1	,	PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.FunRVol	,	   DATA_TYPE_Float	,	0	,	PT_RATIO	},
#endif
#ifdef MODBUS_ANGLE  							
   { (uint8_t *)&InstantPara.AngleRPhase,	   DATA_TYPE_Float	,	1	,	1	},
#else
   { (uint8_t *)&InstantPara.AngleRPhase,	   DATA_TYPE_Float	,	0	,	1	},
#endif

//////////////////////   YphaseYMS  ///////////////////////////////	

#ifdef MODBUS_VOL  							
  { (uint8_t *)&InstantPara.VolY	,	   DATA_TYPE_Float	,	1	,	PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.VolY	,	   DATA_TYPE_Float	,	0	,	PT_RATIO	},
#endif
#ifdef MODBUS_VOL  							
   { (uint8_t *)&InstantPara.VolYB	,	   DATA_TYPE_Float	,	1	,	PT_RATIO	},
#else
   { (uint8_t *)&InstantPara.VolYB	,	   DATA_TYPE_Float	,	0	,	PT_RATIO	},
#endif
#ifdef MODBUS_CUR							
  { (uint8_t *)&InstantPara.CurrentY	,	   DATA_TYPE_Float	,	1	,	CT_RATIO	},
#else
  { (uint8_t *)&InstantPara.CurrentY	,	   DATA_TYPE_Float	,	0	,	CT_RATIO	},
#endif
#ifdef MODBUS_ACTIVE_POWER  							
  { (uint8_t *)&InstantPara.TotalPowerY	,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.TotalPowerY	,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_FUN_POWER							
  { (uint8_t *)&InstantPara.FunPowerY	,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.FunPowerY	,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_APP_POWER							
  { (uint8_t *)&InstantPara.AppPowerY	,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.AppPowerY	,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_REACT_POWER  							
   { (uint8_t *)&InstantPara.ReactPowerY,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
   { (uint8_t *)&InstantPara.ReactPowerY,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_POWER_FACTOR  							
  { (uint8_t *)&InstantPara.PowerFactorY,	   DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.PowerFactorY,	   DATA_TYPE_Float	,	0	,	1	},
#endif
#ifdef MODBUS_FUN_CUR 							
  { (uint8_t *)&InstantPara.FunYCurr	,	   DATA_TYPE_Float	,	1	,	CT_RATIO	},
#else
  { (uint8_t *)&InstantPara.FunYCurr	,	   DATA_TYPE_Float	,	0	,	CT_RATIO	},
#endif
#ifdef MODBUS_FUN_VOL  							
  { (uint8_t *)&InstantPara.FunYVol	,	   DATA_TYPE_Float	,	1	,	PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.FunYVol	,	   DATA_TYPE_Float	,	0	,	PT_RATIO	},
#endif
#ifdef MODBUS_ANGLE  							
   { (uint8_t *)&InstantPara.AngleYPhase,	   DATA_TYPE_Float	,	1	,	1	},
#else
   { (uint8_t *)&InstantPara.AngleYPhase,	   DATA_TYPE_Float	,	0	,	1	},
#endif


  							
//////////////////////   BphaseBMS  ///////////////////////////////	

#ifdef MODBUS_VOL  							
  { (uint8_t *)&InstantPara.VolB	,	   DATA_TYPE_Float	,	1	,	PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.VolB	,	   DATA_TYPE_Float	,	0	,	PT_RATIO	},
#endif
#ifdef MODBUS_VOL  							
   { (uint8_t *)&InstantPara.VolBR	,	   DATA_TYPE_Float	,	1	,	PT_RATIO	},
#else
   { (uint8_t *)&InstantPara.VolBR	,	   DATA_TYPE_Float	,	0	,	PT_RATIO	},
#endif
#ifdef MODBUS_CUR  							
  { (uint8_t *)&InstantPara.CurrentB	,	   DATA_TYPE_Float	,	1	,	CT_RATIO	},
#else
  { (uint8_t *)&InstantPara.CurrentB	,	   DATA_TYPE_Float	,	0	,	CT_RATIO	},
#endif
#ifdef MODBUS_ACTIVE_POWER  							
  { (uint8_t *)&InstantPara.TotalPowerB	,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.TotalPowerB	,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_FUN_POWER  							
  { (uint8_t *)&InstantPara.FunPowerB	,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.FunPowerB	,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_APP_POWER  							
  { (uint8_t *)&InstantPara.AppPowerB	,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.AppPowerB	,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_REACT_POWER  							
   { (uint8_t *)&InstantPara.ReactPowerB,	   DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
   { (uint8_t *)&InstantPara.ReactPowerB,	   DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef MODBUS_POWER_FACTOR  							
  { (uint8_t *)&InstantPara.PowerFactorB,	   DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.PowerFactorB,	   DATA_TYPE_Float	,	0	,	1	},
#endif
#ifdef MODBUS_FUN_CUR  							
  { (uint8_t *)&InstantPara.FunBCurr	,	   DATA_TYPE_Float	,	1	,	CT_RATIO	},
#else
  { (uint8_t *)&InstantPara.FunBCurr	,	   DATA_TYPE_Float	,	0	,	CT_RATIO	},
#endif
#ifdef MODBUS_FUN_VOL  							
  { (uint8_t *)&InstantPara.FunBVol	,	   DATA_TYPE_Float	,	1	,	PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.FunBVol	,	   DATA_TYPE_Float	,	0	,	PT_RATIO	},
#endif
#ifdef MODBUS_ANGLE  							
   { (uint8_t *)&InstantPara.AngleBPhase,	   DATA_TYPE_Float	,	1	,	1	},
#else
   { (uint8_t *)&InstantPara.AngleBPhase,	   DATA_TYPE_Float	,	0	,	1	},
#endif


  
  /////////////////////////   Demand   //////////////////////////////////
#ifdef MODBUS_DEMAND 
  { (uint8_t *)&Demand.PresentDemand    ,          DATA_TYPE_Float,     1       ,       CT_PT_RATIO      },
  { (uint8_t *)&Demand.RisingDemand     ,          DATA_TYPE_Float,     1       ,       CT_PT_RATIO      },
  { (uint8_t *)&Demand.TimeElapsed      ,          DATA_TYPE_16	,       1       ,       1                },
  { (uint8_t *)&Demand.MaxDemand        ,          DATA_TYPE_Float,     1       ,       CT_PT_RATIO      },
  { (uint8_t *)&Demand.MDtime           ,          DATA_TYPE_32	,       1       ,       1                },
  { (uint8_t *)&Demand.MDdate           ,          DATA_TYPE_32	,       1       ,       1                },
#else
  { (uint8_t *)&Demand.PresentDemand    ,          DATA_TYPE_Float,     0       ,       CT_PT_RATIO      },
  { (uint8_t *)&Demand.RisingDemand     ,          DATA_TYPE_Float,     0       ,       CT_PT_RATIO      },
  { (uint8_t *)&Demand.TimeElapsed      ,          DATA_TYPE_16	,       0       ,       1                },
  { (uint8_t *)&Demand.MaxDemand        ,          DATA_TYPE_Float,     0       ,       CT_PT_RATIO      },
  { (uint8_t *)&Demand.MDtime           ,          DATA_TYPE_32	,       0       ,       1                },
  { (uint8_t *)&Demand.MDdate           ,          DATA_TYPE_32	,       0       ,       1                },
#endif
  
#ifdef MODBUS_MIN_MAX  	
 { (uint8_t *)&InstantPara.IrMin    ,          DATA_TYPE_Float,     1       ,       CT_RATIO      },
 { (uint8_t *)&InstantPara.IyMin    ,          DATA_TYPE_Float,     1       ,       CT_RATIO      },
 { (uint8_t *)&InstantPara.IbMin    ,          DATA_TYPE_Float,     1       ,       CT_RATIO      },
 { (uint8_t *)&InstantPara.IrMax    ,          DATA_TYPE_Float,     1       ,       CT_RATIO      },
 { (uint8_t *)&InstantPara.IyMax    ,          DATA_TYPE_Float,     1       ,       CT_RATIO      },
 { (uint8_t *)&InstantPara.IbMax    ,          DATA_TYPE_Float,     1       ,       CT_RATIO      },
 
 { (uint8_t *)&InstantPara.VrMin    ,          DATA_TYPE_Float,     1       ,       PT_RATIO      },
 { (uint8_t *)&InstantPara.VyMin    ,          DATA_TYPE_Float,     1       ,       PT_RATIO      },
 { (uint8_t *)&InstantPara.VbMin    ,          DATA_TYPE_Float,     1       ,       PT_RATIO      },
 { (uint8_t *)&InstantPara.VrMax    ,          DATA_TYPE_Float,     1       ,       PT_RATIO      },
 { (uint8_t *)&InstantPara.VyMax    ,          DATA_TYPE_Float,     1       ,       PT_RATIO      },
 { (uint8_t *)&InstantPara.VbMax    ,          DATA_TYPE_Float,     1       ,       PT_RATIO      },
#else
 { (uint8_t *)&InstantPara.IrMin    ,          DATA_TYPE_Float,     0       ,       CT_RATIO      },
 { (uint8_t *)&InstantPara.IyMin    ,          DATA_TYPE_Float,     0       ,       CT_RATIO      },
 { (uint8_t *)&InstantPara.IbMin    ,          DATA_TYPE_Float,     0       ,       CT_RATIO      },
 { (uint8_t *)&InstantPara.IrMax    ,          DATA_TYPE_Float,     0       ,       CT_RATIO      },
 { (uint8_t *)&InstantPara.IyMax    ,          DATA_TYPE_Float,     0       ,       CT_RATIO      },
 { (uint8_t *)&InstantPara.IbMax    ,          DATA_TYPE_Float,     0       ,       CT_RATIO      },
 
 { (uint8_t *)&InstantPara.VrMin    ,          DATA_TYPE_Float,     0       ,       PT_RATIO      },
 { (uint8_t *)&InstantPara.VyMin    ,          DATA_TYPE_Float,     0       ,       PT_RATIO      },
 { (uint8_t *)&InstantPara.VbMin    ,          DATA_TYPE_Float,     0       ,       PT_RATIO      },
 { (uint8_t *)&InstantPara.VrMax    ,          DATA_TYPE_Float,     0       ,       PT_RATIO      },
 { (uint8_t *)&InstantPara.VyMax    ,          DATA_TYPE_Float,     0       ,       PT_RATIO      },
 { (uint8_t *)&InstantPara.VbMax    ,          DATA_TYPE_Float,     0       ,       PT_RATIO      },
#endif
 
 /////////////////////Edit Para for Delta Model//////////////////////
#ifdef MODBUS_EDIT_PARA
 { (uint8_t *)&CopySetPara[PARA_MAINS_UNDER_VOLT]       ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_MAINS_OVER_VOLT]        ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_MAINS_UNDER_VOLT_RESET] ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_MAINS_OVER_VOLT_RESET]  ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_MAINS_FAIL_DELAY]       ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_MAINS_RETURN_DELAY]      ,         DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_SOLAR_UNDER_VOLT]       ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_SOLAR_OVER_VOLT]        ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_SOLAR_UNDER_VOLT_RESET] ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_SOLAR_OVER_VOLT_RESET]  ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_SOLAR_FAIL_DELAY]       ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_SOLAR_RETURN_DELAY]      ,         DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_DEVICE_ID]              ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_BAUD_RATE]              ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_PARITY]                 ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_STOP_BIT]               ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_ENDIAN]                 ,          DATA_TYPE_16,     1       ,       1      },
#else
 { (uint8_t *)&CopySetPara[PARA_MAINS_UNDER_VOLT]       ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_MAINS_OVER_VOLT]        ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_MAINS_UNDER_VOLT_RESET] ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_MAINS_OVER_VOLT_RESET]  ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_MAINS_FAIL_DELAY]       ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_MAINS_RETURN_DELAY]      ,         DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_SOLAR_UNDER_VOLT]       ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_SOLAR_OVER_VOLT]        ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_SOLAR_UNDER_VOLT_RESET] ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_SOLAR_OVER_VOLT_RESET]  ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_SOLAR_FAIL_DELAY]       ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_SOLAR_RETURN_DELAY]      ,         DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_DEVICE_ID]              ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_BAUD_RATE]              ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_PARITY]                 ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_STOP_BIT]               ,          DATA_TYPE_16,     1       ,       1      },
 { (uint8_t *)&CopySetPara[PARA_ENDIAN]                 ,          DATA_TYPE_16,     1       ,       1      },
#endif
 
 {(uint8_t *)&g_DisableLoadOnGridSeconds                ,          DATA_TYPE_32,     1       ,       1      },

 // SOLAR
 //
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
#ifdef MODBUS_VOL							
  { (uint8_t *)&InstantPara.VolRSolar	,	        DATA_TYPE_Float	,	1	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolYSolar	,	        DATA_TYPE_Float	,	1	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolBSolar	,	        DATA_TYPE_Float	,	1	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolRYSolar	,	        DATA_TYPE_Float	,	1	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolYBSolar	,	        DATA_TYPE_Float	,	1	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolBRSolar	,	        DATA_TYPE_Float	,	1	,	PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.VolRSolar	,	        DATA_TYPE_Float	,	0	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolYSolar	,	        DATA_TYPE_Float	,	0	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolBSolar	,	        DATA_TYPE_Float	,	0	,	PT_RATIO	}, 
  { (uint8_t *)&InstantPara.VolRYSolar	,	        DATA_TYPE_Float	,	0	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolYBSolar	,	        DATA_TYPE_Float	,	0	,	PT_RATIO	},
  { (uint8_t *)&InstantPara.VolBRSolar	,	        DATA_TYPE_Float	,	0	,	PT_RATIO	},
#endif
							
  
#ifdef MODBUS_CUR							
  { (uint8_t *)&InstantPara.CurrentRSolar	,	    DATA_TYPE_Float	,	1	,	CT_RATIO	},
  { (uint8_t *)&InstantPara.CurrentYSolar	,	    DATA_TYPE_Float	,	1	,	CT_RATIO	},
  { (uint8_t *)&InstantPara.CurrentBSolar	,	    DATA_TYPE_Float	,	1	,	CT_RATIO	},
#else
  { (uint8_t *)&InstantPara.CurrentRSolar	,	    DATA_TYPE_Float	,	0	,	CT_RATIO	},
  { (uint8_t *)&InstantPara.CurrentYSolar	,	    DATA_TYPE_Float	,	0	,	CT_RATIO	},
  { (uint8_t *)&InstantPara.CurrentBSolar	,	    DATA_TYPE_Float	,	0	,	CT_RATIO	},
#endif
#ifdef  MODBUS_CUR_NEU
  { (uint8_t *)&InstantPara.CurrentNSolar	,	    DATA_TYPE_Float	,	1	,	CT_RATIO	},
#else
  { (uint8_t *)&InstantPara.CurrentNSolar	,	    DATA_TYPE_Float	,	0	,	CT_RATIO	},
#endif
#ifdef MODBUS_ACTIVE_POWER						
  { (uint8_t *)&InstantPara.TotalPowerRSolar	,	DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalPowerYSolar	,	DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalPowerBSolar	,	DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.SumTotalPowerSolar,	    DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.TotalPowerR	,	        DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalPowerY	,	        DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalPowerB	,	        DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.SumTotalPower,	        DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
  { (uint8_t *)&ModbusDummyFloatRegister	,	    DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&ModbusDummyFloatRegister	,	    DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&ModbusDummyFloatRegister	,	    DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&ModbusDummyFloatRegister	,	    DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#ifdef	MODBUS_APP_POWER						
  { (uint8_t *)&InstantPara.AppPowerRSolar	,	    DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.AppPowerYSolar	,	    DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.AppPowerBSolar	,	    DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalAppPowerSolar,	    DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.AppPowerRSolar	,	    DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.AppPowerYSolar	,	    DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.AppPowerBSolar	,	    DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalAppPowerSolar,	    DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
#ifdef	MODBUS_REACT_POWER							
  { (uint8_t *)&InstantPara.ReactPowerRSolar	,	DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.ReactPowerYSolar	,	DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.ReactPowerBSolar	,	DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalReactPowerSolar,	DATA_TYPE_Float	,	1	,	CT_PT_RATIO	},
#else
  { (uint8_t *)&InstantPara.ReactPowerRSolar	,	DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.ReactPowerYSolar	,	DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.ReactPowerBSolar	,	DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
  { (uint8_t *)&InstantPara.TotalReactPowerSolar,	DATA_TYPE_Float	,	0	,	CT_PT_RATIO	},
#endif
  
#ifdef MODBUS_POWER_FACTOR							
  { (uint8_t *)&InstantPara.PowerFactorRSolar	,   DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.PowerFactorYSolar	,   DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.PowerFactorBSolar	,	DATA_TYPE_Float	,	1	,	1	},
  { (uint8_t *)&InstantPara.TotalPowerFactorSolar	,	DATA_TYPE_Float	,	1	,	1	},
#else
  { (uint8_t *)&InstantPara.PowerFactorRSolar	,   DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.PowerFactorYSolar	,   DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.PowerFactorBSolar	,	DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&InstantPara.TotalPowerFactorSolar	,	DATA_TYPE_Float	,	0	,	1	},
#endif

  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},
  { (uint8_t *)&ModbusDummyFloatRegister,           DATA_TYPE_Float	,	0	,	1	},

  ////////////////////////  SOLAR ENERGY IMPORT  /////////////////////////////	6						
#ifdef MODBUS_RH_IMPORT								
  { (uint8_t *)&StorageBuffer.SolarRunHourImport	,	  DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.SolarRunHourImport	,	  DATA_TYPE_32	,	0	,	1	},
#endif
#ifdef MODBUS_LH_IMPORT
  { (uint8_t *)&StorageBuffer.SolarLoadHourImport	,	  DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.SolarLoadHourImport	,	  DATA_TYPE_32	,	0	,	1	},
#endif

#ifdef MODBUS_WH_IMPORT
  { (uint8_t *)&StorageBuffer.SolarImportWh	        ,	  DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.SolarImportWh	        ,	  DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_VAH_IMPORT
  { (uint8_t *)&StorageBuffer.SolarImportVAh	    ,	  DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.SolarImportVAh	    ,	  DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_VARH_IMPORT
  { (uint8_t *)&StorageBuffer.SolarImportVarhPos	,	  DATA_TYPE_Double	,	1	,	1	},
  { (uint8_t *)&StorageBuffer.SolarImportVarhNeg	,	  DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.SolarImportVarhPos	,	  DATA_TYPE_Double	,	0	,	1	},
  { (uint8_t *)&StorageBuffer.SolarImportVarhNeg	,	  DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_INTERRUPT_IMPORT
  { (uint8_t *)&StorageBuffer.SolarImportInterruptions,	  DATA_TYPE_16	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.SolarImportInterruptions,	  DATA_TYPE_16	,	0	,	1	},
#endif
  							
  ////////////////////////   SOLAR ENERGY EXPORT  /////////////////////////////							
#ifdef MODBUS_RH_EXPORT					
  { (uint8_t *)&StorageBuffer.SolarRunHourExport	,	   DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.SolarRunHourExport	,	   DATA_TYPE_32	,	0	,	1	},
#endif
#ifdef MODBUS_LH_EXPORT	  
  { (uint8_t *)&StorageBuffer.SolarLoadHourExport	,	   DATA_TYPE_32	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.SolarLoadHourExport	,	   DATA_TYPE_32	,	0	,	1	},
#endif
#ifdef MODBUS_WH_EXPORT  
  { (uint8_t *)&StorageBuffer.SolarExportWh	,	           DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.SolarExportWh	,	           DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_VAH_EXPORT
  { (uint8_t *)&StorageBuffer.SolarExportVAh	,	       DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.SolarExportVAh	,	       DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_VARH_EXPORT  
  { (uint8_t *)&StorageBuffer.SolarExportVarhPos	,	   DATA_TYPE_Double	,	1	,	1	},
  { (uint8_t *)&StorageBuffer.SolarExportVarhNeg	,	   DATA_TYPE_Double	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.SolarExportVarhPos	,	   DATA_TYPE_Double	,	0	,	1	},
  { (uint8_t *)&StorageBuffer.SolarExportVarhNeg	,	   DATA_TYPE_Double	,	0	,	1	},
#endif
#ifdef MODBUS_INTERRUPT_EXPORT  
  { (uint8_t *)&StorageBuffer.SolarExportInterruptions,	   DATA_TYPE_16	,	1	,	1	},
#else
  { (uint8_t *)&StorageBuffer.SolarExportInterruptions,	   DATA_TYPE_16	,	0	,	1	},
#endif

};

// DEVNOTE: Digital inputs are read using read input status
