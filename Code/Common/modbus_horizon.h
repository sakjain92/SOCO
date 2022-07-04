
#include "CommFlagDef.h"


extern struct DEMAND Demand; 

extern struct STORE StorageBuffer;
extern struct STORE OldData;
extern struct STORE WCopyTripData;
extern struct INST_PARA InstantPara;
extern struct Object Timer;
//extern struct Field Mod_TransmitFrame;
extern uint16_t LoadExportOrGenerator;
extern float Reserved;

#define CT_RATIO       2
#define PT_RATIO       3
#define CT_PT_RATIO    4

#define InstPara_Elmeasure      60

#define DATA_TYPE_16            2
#define DATA_TYPE_32            4
#define DATA_TYPE_Double        90
#define DATA_TYPE_Float		91



const struct ModBusParameter BlockAll[]=							
{
#ifdef MODBUS_ACTIVE_POWER			
  { (uint8_t *)&InstantPara.SumTotalPower,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO     ,0.1	},
  { (uint8_t *)&InstantPara.TotalPowerR	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.TotalPowerY	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.TotalPowerB	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	,0.1    },
#else
  { (uint8_t *)&InstantPara.SumTotalPower,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.TotalPowerR	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.TotalPowerY	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.TotalPowerB	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	,0.1    },
#endif
  
#ifdef	MODBUS_REACT_POWER		
  { (uint8_t *)&InstantPara.TotalReactPower,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.ReactPowerR	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.ReactPowerY	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.ReactPowerB	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	,0.1    },
  
#else
  { (uint8_t *)&InstantPara.TotalReactPower,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.ReactPowerR	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.ReactPowerY	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.ReactPowerB	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	,0.1    },
#endif
  
#ifdef MODBUS_POWER_FACTOR
  { (uint8_t *)&InstantPara.TotalPowerFactor	,	DATA_TYPE_Float	,	1	,	1	,1000    },
  { (uint8_t *)&InstantPara.PowerFactorR	,       DATA_TYPE_Float	,	1	,	1	,1000    },
  { (uint8_t *)&InstantPara.PowerFactorY	,       DATA_TYPE_Float	,	1	,	1	,1000    },
  { (uint8_t *)&InstantPara.PowerFactorB	,	DATA_TYPE_Float	,	1	,	1	,1000    },  
#else
  { (uint8_t *)&InstantPara.TotalPowerFactor	,	DATA_TYPE_Float	,	0	,	1	,1000    },
  { (uint8_t *)&InstantPara.PowerFactorR	,       DATA_TYPE_Float	,	0	,	1	,1000    },
  { (uint8_t *)&InstantPara.PowerFactorY	,       DATA_TYPE_Float	,	0	,	1	,1000    },
  { (uint8_t *)&InstantPara.PowerFactorB	,	DATA_TYPE_Float	,	0	,	1	,1000    },
#endif
  
#ifdef	MODBUS_APP_POWER	
  { (uint8_t *)&InstantPara.TotalAppPower,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.AppPowerR	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.AppPowerY	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.AppPowerB	,	       DATA_TYPE_Float	,	1	,	CT_PT_RATIO	,0.1    },
#else
  { (uint8_t *)&InstantPara.TotalAppPower,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.AppPowerR	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.AppPowerY	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	,0.1    },
  { (uint8_t *)&InstantPara.AppPowerB	,	       DATA_TYPE_Float	,	0	,	CT_PT_RATIO	,0.1    },
#endif
  
#ifdef MODBUS_VOL
  { (uint8_t *)&InstantPara.VolLLAvg	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.VolRY	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.VolYB	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.VolBR	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.VolLNAvg	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.VolR	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.VolY	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.VolB	,	       DATA_TYPE_Float	,	1	,	PT_RATIO	,1000   }, 
#else
  { (uint8_t *)&InstantPara.VolLLAvg	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.VolRY	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.VolYB	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.VolBR	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.VolLNAvg	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.VolR	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.VolY	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.VolB	,	       DATA_TYPE_Float	,	0	,	PT_RATIO	,1000   }, 
#endif

#ifdef MODBUS_CUR	
  { (uint8_t *)&InstantPara.AvgCurr     ,	       DATA_TYPE_Float	,	1	,	CT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.CurrentR	,	       DATA_TYPE_Float	,	1	,	CT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.CurrentY	,	       DATA_TYPE_Float	,	1	,	CT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.CurrentB	,	       DATA_TYPE_Float	,	1	,	CT_RATIO	,1000   },
#else
  { (uint8_t *)&InstantPara.AvgCurr     ,	       DATA_TYPE_Float	,	0	,	CT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.CurrentR	,	       DATA_TYPE_Float	,	0	,	CT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.CurrentY	,	       DATA_TYPE_Float	,	0	,	CT_RATIO	,1000   },
  { (uint8_t *)&InstantPara.CurrentB	,	       DATA_TYPE_Float	,	0	,	CT_RATIO	,1000   },
#endif
  
#ifdef 	MODBUS_FREQ						
  { (uint8_t *)&InstantPara.Frequency	,	       DATA_TYPE_Float	,	1	,	1	,1000   },
#else
  { (uint8_t *)&InstantPara.Frequency	,	       DATA_TYPE_Float	,	0	,	1	,1000   },
#endif
  
#ifdef MODBUS_WH_IMPORT
  { (uint8_t *)&StorageBuffer.ImportWh	,	  DATA_TYPE_Double	,	1	,	1	,0.001  },
#else
  { (uint8_t *)&StorageBuffer.ImportWh	,	  DATA_TYPE_Double	,	0	,	1	,0.001  },
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////               //30
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },
  
//////////////////////////////////////////////////////////////////////////////////////////////////////////              //28
#ifdef MODBUS_LH_IMPORT
  { (uint8_t *)&StorageBuffer.LoadHourImport	,	  DATA_TYPE_32	,	1	,	1	,1      },
#else
  { (uint8_t *)&StorageBuffer.LoadHourImport	,	  DATA_TYPE_32	,	0	,	1	,1      },     
#endif
  { (uint8_t *)&Reserved                	,	  DATA_TYPE_32	,	0	,	1	,1      },       //2
  
};




