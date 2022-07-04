#include "Model_DIN.h"

/***************************************************************************
The Modbus Enable Disable Template is given below
The desired paramters should be selected as per the model

  #define MODBUS_VOL
  #define MODBUS_CUR
  #define MODBUS_CUR_NEU
  #define MODBUS_ACTIVE_POWER
  #define MODBUS_FUN_POWER
  #define MODBUS_APP_POWER
  #define MODBUS_REACT_POWER
  #define MODBUS_POWER_FACTOR
  #define MODBUS_FREQ
  #define MODBUS_FUN_CUR
  #define MODBUS_FUN_VOL
  #define MODBUS_FUN_CUR_NEU
  #define MODBUS_AVG_CUR
  #define MODBUS_CUR_UNBAL
  #define MODBUS_VOL_UNBAL
  #define MODBUS_ANGLE
  #define MODBUS_THD

  #define MODBUS_AVG_VOL
  #define MODBUS_CURR_TOTAL
  #define MODBUS_CTPT_Ratio


  #define MODBUS_RH_IMPORT
  #define MODBUS_LH_IMPORT
  #define MODBUS_WH_IMPORT
  #define MODBUS_VAH_IMPORT
  #define MODBUS_VARH_IMPORT
  #define MODBUS_INTERRUPT_IMPORT
  
  #define MODBUS_OLD_RH_IMPORT
  #define MODBUS_OLD_LH_IMPORT
  #define MODBUS_OLD_WH_IMPORT
  #define MODBUS_OLD_VAH_IMPORT
  #define MODBUS_OLD_VARH_IMPORT
  #define MODBUS_OLD_INTERRUPT_IMPORT
  
  
  #define MODBUS_DEMAND

****************************************************************************/


#ifdef MODEL_REX_1100
// MODBUS Data Here  
  #define MODBUS_VOL
  #define MODBUS_CUR
  #define MODBUS_FREQ
  #define MODBUS_CTPT_Ratio
 #endif

#ifdef MODEL_REX_1200
// MODBUS Data Here  
  #define MODBUS_VOL
  #define MODBUS_CUR
  #define MODBUS_CUR_NEU
  #define MODBUS_AVG_CUR
  #define MODBUS_ACTIVE_POWER
  #define MODBUS_APP_POWER
  #define MODBUS_FREQ
  #define MODBUS_CTPT_Ratio
#endif

#ifdef MODEL_REX_1300
// MODBUS Data Here  

  #define MODBUS_VOL
  #define MODBUS_CUR
  #define MODBUS_CUR_NEU
  #define MODBUS_AVG_CUR  
  #define MODBUS_FREQ
  #define MODBUS_APP_POWER
  #define MODBUS_POWER_FACTOR
  #define MODBUS_CTPT_Ratio
#endif

#ifdef MODEL_REX_1400
// MODBUS Data Here  
  #define MODBUS_VOL
  #define MODBUS_CUR
  #define MODBUS_CUR_NEU
  #define MODBUS_AVG_CUR
  #define MODBUS_ACTIVE_POWER
  #define MODBUS_APP_POWER
  #define MODBUS_FREQ
  #define MODBUS_CTPT_Ratio
#endif

#ifdef MODEL_REX_1500
// MODBUS Data Here  
  #define MODBUS_VOL
  #define MODBUS_CUR
  #define MODBUS_CUR_NEU
  #define MODBUS_AVG_CUR
  #define MODBUS_FREQ
  #define MODBUS_CUR_UNBAL
  #define MODBUS_VOL_UNBAL
  #define MODBUS_ACTIVE_POWER
  #define MODBUS_APP_POWER
  #define MODBUS_REACT_POWER
  #define MODBUS_POWER_FACTOR
  #define MODBUS_ANGLE
  #define MODBUS_CTPT_Ratio
 #endif

#ifdef MODEL_REX_1900
// MODBUS Data Here  
  #define MODBUS_ACTIVE_POWER
  #define MODBUS_APP_POWER
  #define MODBUS_REACT_POWER
  #define MODBUS_POWER_FACTOR
  #define MODBUS_CTPT_Ratio
 #endif

#ifdef MODEL_REX_2140
// MODBUS Data Here  
  #define MODBUS_VOL
  #define MODBUS_CUR
  #define MODBUS_FREQ
  #define MODBUS_WH_IMPORT
  #define MODBUS_VAH_IMPORT
  #define MODBUS_RH_IMPORT
  #define MODBUS_LH_IMPORT
  #define MODBUS_CTPT_Ratio
 #endif

#ifdef MODEL_REX_2330
// MODBUS Data Here  
  #define MODBUS_VOL
  #define MODBUS_CUR
  #define MODBUS_CUR_NEU
  #define MODBUS_AVG_CUR
  #define MODBUS_FREQ
  #define MODBUS_CUR_UNBAL
  #define MODBUS_VOL_UNBAL
  #define MODBUS_APP_POWER
  #define MODBUS_POWER_FACTOR
  #define MODBUS_ANGLE
  #define MODBUS_VARH_IMPORT
  #define MODBUS_CTPT_Ratio
#endif

#ifdef MODEL_REX_2440
// MODBUS Data Here  
  #define MODBUS_VOL
  #define MODBUS_CUR
  #define MODBUS_CUR_NEU
  #define MODBUS_AVG_CUR
  #define MODBUS_FREQ
  #define MODBUS_CUR_UNBAL
  #define MODBUS_VOL_UNBAL
  #define MODBUS_ACTIVE_POWER
  #define MODBUS_APP_POWER
  #define MODBUS_POWER_FACTOR
  #define MODBUS_ANGLE
  #define MODBUS_WH_IMPORT
  #define MODBUS_VAH_IMPORT
  #define MODBUS_RH_IMPORT
  #define MODBUS_LH_IMPORT
  #define MODBUS_INTERRUPT_IMPORT
  #define MODBUS_CTPT_Ratio
 #endif

#ifdef MODEL_REX_2411
// MODBUS Data Here  
  #define MODBUS_VOL
  #define MODBUS_CUR
  #define MODBUS_CUR_NEU
  #define MODBUS_AVG_CUR
  #define MODBUS_FREQ
  #define MODBUS_CUR_UNBAL
  #define MODBUS_VOL_UNBAL
  #define MODBUS_ACTIVE_POWER
  #define MODBUS_APP_POWER
  #define MODBUS_POWER_FACTOR
  #define MODBUS_ANGLE
  #define MODBUS_WH_IMPORT
  #define MODBUS_RH_IMPORT
  #define MODBUS_LH_IMPORT
  #define MODBUS_INTERRUPT_IMPORT
  #define MODBUS_OLD_WH_IMPORT
  #define MODBUS_OLD_RH_IMPORT
  #define MODBUS_OLD_LH_IMPORT
  #define MODBUS_OLD_INTERRUPT_IMPORT
  #define MODBUS_CTPT_Ratio
#endif

#ifdef MODEL_REX_2550
// MODBUS Data Here  
  #define MODBUS_VOL
  #define MODBUS_CUR
  #define MODBUS_CUR_NEU
  #define MODBUS_AVG_CUR
  #define MODBUS_FREQ
  #define MODBUS_CUR_UNBAL
  #define MODBUS_VOL_UNBAL
  #define MODBUS_ACTIVE_POWER
  #define MODBUS_APP_POWER
  #define MODBUS_REACT_POWER
  #define MODBUS_POWER_FACTOR
  #define MODBUS_ANGLE
  #define MODBUS_WH_IMPORT
  #define MODBUS_VA_IMPORT
  #define MODBUS_VAR_IMPORT
  #define MODBUS_RH_IMPORT
  #define MODBUS_LH_IMPORT
  #define MODBUS_VAH_IMPORT
  #define MODBUS_VARH_IMPORT
  #define MODBUS_INTERRUPT_IMPORT
  #define MODBUS_CTPT_Ratio
#endif

#ifdef MODEL_REX_2551
// MODBUS Data Here  
  #define MODBUS_VOL
  #define MODBUS_CUR
  #define MODBUS_CUR_NEU
  #define MODBUS_AVG_CUR
  #define MODBUS_FREQ
  #define MODBUS_CUR_UNBAL
  #define MODBUS_VOL_UNBAL
  #define MODBUS_ACTIVE_POWER
  #define MODBUS_APP_POWER
  #define MODBUS_REACT_POWER
  #define MODBUS_POWER_FACTOR
  #define MODBUS_ANGLE
  #define MODBUS_WH_IMPORT
  #define MODBUS_VA_IMPORT
  #define MODBUS_VAR_IMPORT
  #define MODBUS_RH_IMPORT
  #define MODBUS_LH_IMPORT
  #define MODBUS_VAH_IMPORT
  #define MODBUS_VARH_IMPORT
  #define MODBUS_INTERRUPT_IMPORT
  #define MODBUS_OLD_WH_IMPORT
  #define MODBUS_OLD_VAH_IMPORT
  #define MODBUS_OLD_VARH_IMPORT
  #define MODBUS_OLD_RH_IMPORT
  #define MODBUS_OLD_LH_IMPORT
  #define MODBUS_OLD_INTERRUPT_IMPORT
  #define MODBUS_CTPT_Ratio
#endif


