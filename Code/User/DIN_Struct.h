#include "Struct.h"
#include "CommFlagDef.h"
#include "extern_includes.h"

//extern struct INST_PARA      InstantPara;
extern struct STORE StorageBuffer;








#define DATA_TYPE_VARIABLE   0
#define DATA_TYPE_STRING     1

/*********************************************************************
                  Decimal Place

In ICON: Decimal is used for No. Of columns

Variable:Decimal Place has multiple meaning in case of variable
Upto 10 its just the decimal place

11. Current Variable decimal place
12. KW/KWT/KVA/KVAr



**********************************************************************/




#define DIS_VAL_VOLTAGE             11
#define DIS_VAL_CURRENT             12
#define DIS_VAL_POWER               13
#define DIS_VAL_POW_VA              14
#define DIS_VAL_POW_VAR             15
#define DIS_RUN_HR_IMP              16
#define DIS_LH_IMP                  17
#define DIS_W_IMP                   18
#define DIS_VA_IMP                  19
#define DIS_VAR_POS_IMP             20
#define DIS_VAR_NEG_IMP             21

#define DIS_RUN_HR_IMP_OLD          22
#define DIS_LH_IMP_OLD              23
#define DIS_W_IMP_OLD               24
#define DIS_VA_IMP_OLD              25
#define DIS_VAR_POS_IMP_OLD         26
#define DIS_VAR_NEG_IMP_OLD         27

#define DIS_VAL_PF                  28
#define DIS_VAL_ANGLE               29
#define DIS_INT_IMP                 30
#define DIS_INT_IMP_OLD             31
#define DIS_VAL_FREQ                32
#define DIS_VAL_UNB                 33

#define DIS_SOLAR_W_IMP             34
#define DIS_SOLAR_VA_IMP            35
#define DIS_SOLAR_VAR_POS_IMP       36
#define DIS_SOLAR_VAR_NEG_IMP       37

#define DIS_W_EXP                   38
#define DIS_VA_EXP                  39
#define DIS_VAR_POS_EXP             40
#define DIS_VAR_NEG_EXP             41

#define DIS_SOLAR_W_EXP             42
#define DIS_SOLAR_VA_EXP            43
#define DIS_SOLAR_VAR_POS_EXP       44
#define DIS_SOLAR_VAR_NEG_EXP       45

#define DIS_VAL_FAN_CURRENT         46         

const uint8_t StrRY[]="RY";
const uint8_t StrYB[]="YB";
const uint8_t StrBR[]="BR";
const uint8_t StrRV[]="RV";
const uint8_t StrYV[]="YV";
const uint8_t StrBV[]="BV";
const uint8_t StrRA[]="RA";
const uint8_t StrYA[]="YA";
const uint8_t StrBA[]="BA";
const uint8_t StrNA[]="NA";
const uint8_t StrAA[]="AA";
const uint8_t StrHZ[]="HZ";
const uint8_t StrV[]=" V";
const uint8_t StrA[]=" A";

const uint8_t StrRW[]="RW";
const uint8_t StrYW[]="YW";
const uint8_t StrBW[]="BW";
const uint8_t StrW[]=" W";
const uint8_t StrTW[]="TW";
const uint8_t StrVA[]="VA";
const uint8_t StrVR[]="VR";
const uint8_t StrPF[]="PF";





const uint8_t DIS_HIGHCALPF[]="C HPF";
const uint8_t DIS_MIDCALPF[]="C MPF";
const uint8_t DIS_LOWCALPF[]="C LPF";
const uint8_t DIS_CALHIGHVI[]="C HVI";
const uint8_t DIS_CALMIDVI[]="C MVI";
const uint8_t DIS_CALLOWVI[]="C LVI";
const uint8_t DIS_UPF[]=" UPF";
const uint8_t DIS_PF[] ="  PF";
const uint8_t DIS_PRES[]="PRES";
const uint8_t DIS_NEXT[]="NEXT";
const uint8_t DIS_SCRLON[]="SCRON";
const uint8_t DIS_SCRLOFF[]="SCROFF";
const uint8_t DIS_SET[]=" SET";
const uint8_t DIS_SETHIGHPF[]="S HPF";
const uint8_t DIS_SETMIDPF[]="S MPF";
const uint8_t DIS_SETLOWPF[]="S LPF";
const uint8_t DIS_LOW[]=" LOW";
const uint8_t DIS_VI[]="VI";

const uint8_t StrAN[]="PA";

const uint8_t StrSUM_VA[]="  VA";
const uint8_t StrSUM_PF[]="  PF";
const uint8_t StrUV[]="UV";
const uint8_t StrUA[]="UA";
const uint8_t StrSUM_VAR[]=" VAR";


#ifdef MODEL_IMPORT_ONLY
const uint8_t StrImpWh[]=  "    Wh";
const uint8_t StrImpVah[]= "   VAh";
const uint8_t StrImpVarh[]="  VARh";
const uint8_t StrImpLdHr[]="  LdHr";
const uint8_t StrImpRnHr[]="  RnHr";
#else
const uint8_t StrImpWh[]=  "F   Wh";
const uint8_t StrImpVah[]= "F  VAh";
const uint8_t StrImpVarh[]="F VARh";
const uint8_t StrImpLdHr[]="F LdHr";
const uint8_t StrImpRnHr[]="F  RnHr";
#endif

const uint8_t StrExpWh[]=  "R   Wh";  
const uint8_t StrExpVah[]= "R  VAh";
const uint8_t StrExpVarh[]="R VARh";
const uint8_t StrExpRnHr[]="R RnHr";
const uint8_t StrExpLdHr[]="R LdHr";

#ifdef MODEL_IMPORT_ONLY
const uint8_t StrOldImpWh[]= "O   Wh";
const uint8_t StrOldImpVah[]="O  VAh";
const uint8_t StrOldImpVarh[]="O VARh";
const uint8_t StrOldImpRnHr[]="O RnHr";
const uint8_t StrOldImpLdHr[]="O LdHr";
#else
const uint8_t StrOldImpWh[]="OF  Wh";
const uint8_t StrOldImpVah[]="OF VAh";
const uint8_t StrOldImpVarh[]="OFVARh";
const uint8_t StrOldImpRnHr[]="OFRnHr";
const uint8_t StrOldImpLdHr[]="OFLdHr";
#endif

const uint8_t StrFan1Current[]="FAN  1";
const uint8_t StrFan2Current[]="FAN  2";

const uint8_t StrInterr[]="  INTR";
const uint8_t StrOldInterr[]="O INTR";


const uint8_t StrBlank[]="";
const uint8_t StrDash[]="_";
const uint8_t DIS_CHECK_IN[]="CHK IN";
const uint8_t DIS_CAL_DONE[]="CAL OK";
const uint8_t DIS_IN[]="IN";
const uint8_t DIS_PUT[]="PUT";
const uint8_t DIS_PRO[]="PRO";
const uint8_t DIS_COM[]="COM";
const uint8_t DIS_SOCO[]="S0C0";
const uint8_t DIS_INV_R[]="CT R";
const uint8_t DIS_INV_Y[]="CT Y";
const uint8_t DIS_INV_B[]="CT B";

struct object
{
  uint8_t DataType;
  float  *Variable;
  uint8_t NumOfDigits;
  uint8_t DecPos;                   // First DFigit on Left(Digit1) Dec ia 1 and so on till 5      
  uint8_t DigitAdd;                    // Starting from 2-6. The digit 1 is combined with 2 and can't be seperated
  const uint8_t *IconStringArray;
  uint8_t LedType;
};


const struct object Screen[][3]=
{
// SCREEN 0: Line to Line Voltage 3P RY
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrRY,         LED_GRID_NONE   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.VolRY,     4,      DIS_VAL_VOLTAGE,        DIGIT_3,         0,             LED_GRID_NONE   },
      { 0xff,                   0,                      0,     0,                       DIGIT_3,         0,             0               }
  },
  // SCREEN 1: Line to Line Voltage 3P YB
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrYB,         LED_GRID_NONE   }, 
      { DATA_TYPE_VARIABLE,     &InstantPara.VolYB,     4,      DIS_VAL_VOLTAGE,        DIGIT_3,         0,             LED_GRID_NONE   },
      { 0xff,                   0,                      0,     0,                       DIGIT_3,         0,             0               }
  },
  // SCREEN 2: Line to Line Voltage 3P BR
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrBR,         LED_GRID_NONE   },
      { DATA_TYPE_VARIABLE,     &InstantPara.VolBR,     4,      DIS_VAL_VOLTAGE,        DIGIT_3,         0,             LED_GRID_NONE   },
      { 0xff,                   0,                      0,     0,                       DIGIT_3,         0,             0               }
  },
 
  // SCREEN 3:  Line to Neutral Voltage 3P R
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrV,          LED_GRID_R      },   
      { DATA_TYPE_VARIABLE,     &InstantPara.VolR,      4,      DIS_VAL_VOLTAGE,        DIGIT_3,         0,             LED_GRID_R      },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             0               }
  },
  // SCREEN 4:  Line to Neutral Voltage 3P Y
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrV,          LED_GRID_Y      },   
      { DATA_TYPE_VARIABLE,     &InstantPara.VolY,      4,      DIS_VAL_VOLTAGE,        DIGIT_3,         0,             LED_GRID_Y      },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             0,              }
  },
  // SCREEN 5:  Line to Neutral Voltage 3P B
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrV,          LED_GRID_B      },   
      { DATA_TYPE_VARIABLE,     &InstantPara.VolB,      4,      DIS_VAL_VOLTAGE,        DIGIT_3,         0,             LED_GRID_B      },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             0               }
  },
  
  // SCREEN 6: Current 3P R
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrA,          LED_GRID_R      },   
      { DATA_TYPE_VARIABLE,     &InstantPara.CurrentR,  4,      DIS_VAL_CURRENT,        DIGIT_3,         0,             LED_GRID_R      },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             0,              }
  },
  // SCREEN 7: Current 3P Y
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrA,          LED_GRID_Y      },   
      { DATA_TYPE_VARIABLE,     &InstantPara.CurrentY,  4,      DIS_VAL_CURRENT,        DIGIT_3,         0,             LED_GRID_Y      },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             0,              }
  },
  // SCREEN 8: Current 3P B
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrA,          LED_GRID_B      },   
      { DATA_TYPE_VARIABLE,     &InstantPara.CurrentB,  4,      DIS_VAL_CURRENT,        DIGIT_3,         0,             LED_GRID_B      },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             LED_GRID_B      }
  },
 // SCREEN 9: Neutral Current 3P
 {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrNA,         LED_GRID_NONE   }, 
      { DATA_TYPE_VARIABLE,     &InstantPara.CurrentN,  4,      DIS_VAL_CURRENT,        DIGIT_3,         0,             LED_GRID_NONE   },
      { 0xff,                   0,                      0,     0,                       DIGIT_3,         0,             0 }
  },
  // SCREEN 10: Average Current 3P
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrAA,         LED_GRID_NONE }, 
      { DATA_TYPE_VARIABLE,     &InstantPara.AvgCurr,   4,      DIS_VAL_CURRENT,        DIGIT_3,         0,             LED_GRID_NONE },
      { 0xff,                   0,                      0,     0,                       DIGIT_3,         0,             LED_OFF }
  },
 // SCREEN 11: frequency
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrHZ,         LED_GRID_NONE }, 
      { DATA_TYPE_VARIABLE,     &InstantPara.Frequency, 4,      DIS_VAL_FREQ,           DIGIT_3,         0,             LED_GRID_NONE },
      { 0xff,                   0,                      0,      0 ,                     DIGIT_3,         0,             LED_OFF }
  },
  // SCREEN 12: Vol Unbalance
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrUV,         LED_GRID_NONE }, 
      { DATA_TYPE_VARIABLE,     &InstantPara.VolUnb,    3,      DIS_VAL_UNB,            DIGIT_3,         0,             LED_GRID_NONE },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             0       }
  },
  // SCREEN 13: Current Unbalance
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrUA,         LED_GRID_NONE },   
      { DATA_TYPE_VARIABLE,     &InstantPara.CurUnb,    3,      DIS_VAL_UNB,            DIGIT_3,         0,             LED_GRID_NONE },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             0       }
  },
  // SCREEN 14: POWER KW 3P R
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrW,         LED_GRID_R   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.TotalPowerR,  4,      DIS_VAL_POWER,        DIGIT_3,         0,            LED_GRID_R   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 15: POWER KW 3P Y
   {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrW,         LED_GRID_Y   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.TotalPowerY,  4,      DIS_VAL_POWER,        DIGIT_3,         0,            LED_GRID_Y   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 16: POWER KW 3P B
   {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrW,         LED_GRID_B   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.TotalPowerB,  4,      DIS_VAL_POWER,        DIGIT_3,         0,            LED_GRID_B   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 17: Sum Power  
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrTW,        LED_GRID_TOTAL },   
      { DATA_TYPE_VARIABLE,     &InstantPara.SumTotalPower,4,      DIS_VAL_POWER,        DIGIT_3,         0,            LED_GRID_TOTAL },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0 }
  },
  // SCREEN 18: kVA 3P R
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVA,        LED_GRID_R   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.AppPowerR,    4,      DIS_VAL_POW_VA,       DIGIT_3,         0,            LED_GRID_R   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 19: kVA 3P Y
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVA,        LED_GRID_Y   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.AppPowerY,    4,      DIS_VAL_POW_VA,       DIGIT_3,         0,            LED_GRID_Y   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 20: kVA 3P B
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVA,        LED_GRID_B   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.AppPowerB,    4,      DIS_VAL_POW_VA,       DIGIT_3,         0,            LED_GRID_B   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
   // SCREEN 21: Sum VA  
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVA,        LED_GRID_TOTAL },   
      { DATA_TYPE_VARIABLE,     &InstantPara.TotalAppPower,4,      DIS_VAL_POW_VA,       DIGIT_3,         0,            LED_GRID_TOTAL },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            LED_GRID_TOTAL }
  },
  // SCREEN 22: kVAr 3P R
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVR,        LED_GRID_R   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.ReactPowerR,  4,      DIS_VAL_POW_VAR,      DIGIT_3,         0,            LED_GRID_R   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
 
  },
  // SCREEN 23: kVAr 3P Y
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVR,        LED_GRID_Y   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.ReactPowerY,  4,      DIS_VAL_POW_VAR,      DIGIT_3,         0,            LED_GRID_Y   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 24: kVAr 3P B
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVR,        LED_GRID_B   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.ReactPowerB,  4,      DIS_VAL_POW_VAR,      DIGIT_3,         0,            LED_GRID_B   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 25: Sum VAR 
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVR,        LED_GRID_TOTAL },   
      { DATA_TYPE_VARIABLE,     &InstantPara.TotalReactPower,4,    DIS_VAL_POW_VAR,      DIGIT_3,         0,            LED_GRID_TOTAL },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            LED_GRID_TOTAL }
  },
  // SCREEN 26: Power factor 3P R
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrPF,        LED_GRID_R   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.PowerFactorR, 4,      DIS_VAL_PF,           DIGIT_3,         0,            LED_GRID_R   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 27: Power factor 3P Y
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrPF,        LED_GRID_Y   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.PowerFactorY, 4,      DIS_VAL_PF,           DIGIT_3,         0,            LED_GRID_Y   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 28: Power factor 3P B
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrPF,        LED_GRID_B   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.PowerFactorB, 4,      DIS_VAL_PF,           DIGIT_3,         0,            LED_GRID_B   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 29: Sum PF 
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrPF,        LED_GRID_TOTAL },   
      { DATA_TYPE_VARIABLE,     &InstantPara.TotalPowerFactor,4,   DIS_VAL_PF,           DIGIT_3,         0,            LED_GRID_TOTAL },
      { 0xff,                   0,                         0,      0,                    DIGIT_3,         0,            LED_GRID_TOTAL }
  },
  // SCREEN 30: Kwh  Import Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,       0,                   DIGIT_6,         StrImpWh,     LED_OFF },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,         0,            LED_OFF }
  },
  // SCREEN 31: Kwh  Import Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,       DIS_W_IMP,           DIGIT_1,          0,           LED_GRID_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_GRID_TOTAL }
  },
  // SCREEN 32: KVah  Import Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,        0,                  DIGIT_6,          StrImpVah,   LED_OFF },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 33: KVah  Import Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,       DIS_VA_IMP,          DIGIT_1,          0,           LED_GRID_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_GRID_TOTAL }
  },
  // SCREEN 34: Reactive Pos  Import Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,      0,                    DIGIT_6,          StrImpVarh,  LED_OFF },
    {  0xff,                   0,                          0,      0,                    DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 35: Reactive Pos  Import Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,      DIS_VAR_POS_IMP,      DIGIT_1,          0,           LED_GRID_TOTAL },
    {  0xff,                   0,                          0,      0,                    DIGIT_2,          0,           LED_GRID_TOTAL }
  },
  // SCREEN 36: Reactive Neg  Import Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,       0,                   DIGIT_6,          StrImpVarh,  LED_OFF },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 37: Reactive Neg  Import Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,       DIS_VAR_NEG_IMP,     DIGIT_1,          0,           LED_GRID_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_GRID_TOTAL }
  },
   // SCREEN 38: Run Hour Import Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,      0,                    DIGIT_6,          StrImpRnHr,  LED_OFF },
    {  0xff,                   0,                          0,      0,                    DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 39: Run Hour Import Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,      DIS_RUN_HR_IMP,       DIGIT_1,          0,           LED_OFF },
    {  0xff,                   0,                          0,      0,                    DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 40: Load Hour   Import Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,       0,                   DIGIT_6,          StrImpLdHr,  LED_OFF },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 41: Load Hour   Import Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,       DIS_LH_IMP,          DIGIT_1,          0,           LED_GRID_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_GRID_TOTAL }
  },
  // SCREEN 42: Kwh  Import Old Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,       0,                   DIGIT_6,          StrOldImpWh, LED_OFF },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 43: Kwh  Import Old Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,       DIS_W_IMP_OLD,       DIGIT_1,          0,           LED_GRID_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_GRID_TOTAL }
  },
  // SCREEN 44: KVah  Import Old Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,       0,                   DIGIT_6,          StrOldImpVah, LED_OFF },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 45: KVah  Import Old Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,       DIS_VA_IMP_OLD,      DIGIT_1,          0,           LED_GRID_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_GRID_TOTAL }
  },
  // SCREEN 46: Reactive Pos  Import Old Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,      0,                    DIGIT_6,          StrOldImpVarh, LED_OFF },
    {  0xff,                   0,                          0,      0,                    DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 47: Reactive Pos  Import Old Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,      DIS_VAR_POS_IMP_OLD,  DIGIT_1,          0,           LED_GRID_TOTAL },
    {  0xff,                   0,                          0,      0,                    DIGIT_2,          0,           LED_GRID_TOTAL }
  },
  // SCREEN 48: Reactive Neg  Import Old Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,       0,                   DIGIT_6,          StrOldImpVarh, LED_OFF },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 49: Reactive Neg  Import Old Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,       DIS_VAR_NEG_IMP_OLD, DIGIT_1,          0,           LED_GRID_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_GRID_TOTAL }
  },
   // SCREEN 50: Run Hour Import Old Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,      0,                    DIGIT_6,          StrOldImpRnHr, LED_OFF },
    {  0xff,                   0,                          0,      0,                    DIGIT_2,          0,           LED_OFF }
  },
   // SCREEN 51: Run Hour Import Old Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,      DIS_RUN_HR_IMP_OLD,   DIGIT_1,          0,           LED_OFF },
    {  0xff,                   0,                          0,      0,                    DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 52: Load Hour   Import Old Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,       0,                   DIGIT_6,          StrOldImpLdHr, LED_OFF },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 53: Load Hour   Import Old Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,       DIS_LH_IMP_OLD,      DIGIT_1,          0,           LED_GRID_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_GRID_TOTAL }
  },
  // SCREEN 54:Angle 3Phase R
  
   { 
      {DATA_TYPE_STRING,       0,                          0,      0,                    DIGIT_2,         StrAN,        LED_GRID_R   }, 
      { DATA_TYPE_VARIABLE,     &InstantPara.AngleRPhase,  4,      DIS_VAL_ANGLE,        DIGIT_3,         0,            LED_GRID_R   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
   },
   // SCREEN 55:Angle 3Phase Y
  
   { 
      {DATA_TYPE_STRING,       0,                          0,      0,                    DIGIT_2,         StrAN,        LED_GRID_Y   },  
      { DATA_TYPE_VARIABLE,     &InstantPara.AngleYPhase,  4,      DIS_VAL_ANGLE,        DIGIT_3,         0,            LED_GRID_Y   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
   },
   // SCREEN 56:Angle 3Phase B
  
   { 
      {DATA_TYPE_STRING,       0,                          0,      0,                    DIGIT_2,         StrAN,        LED_GRID_B   }, 
      { DATA_TYPE_VARIABLE,     &InstantPara.AngleBPhase,  4,      DIS_VAL_ANGLE,        DIGIT_3,         0,            LED_GRID_B   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
   },
   
   // SCREEN 57: Interruption Str
   
    { 
        {  DATA_TYPE_STRING,       0,                          0,       0,               DIGIT_6,          StrInterr,   LED_OFF  },
        {  0xff,                   0,                          0,       0,               DIGIT_2,          0,            0       }
    },
    // SCREEN 58: Interruption Val
   
    { 
        {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,       DIS_INT_IMP,     DIGIT_1,          0,           LED_OFF },
        {  0xff,                   0,                          0,       0,               DIGIT_2,          0,           LED_OFF }
    },
   // SCREEN 59:  Old Interruption Str
     { 
        {  DATA_TYPE_STRING,       0,                          0,       0,               DIGIT_6,          StrOldInterr, LED_OFF },
        {  0xff,                   0,                          0,       0,               DIGIT_2,          0,           LED_OFF }
      },
    // SCREEN 60:  Old Interruption Val
    { 
        {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,       DIS_INT_IMP_OLD, DIGIT_1,          0,           LED_OFF },
        {  0xff,                   0,                          0,       0,               DIGIT_2,          0,           LED_OFF }
    },
  
    // SCREEN 61:  Line to Neutral Voltage 3P R Solar
    {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrV,          LED_SOLAR_R     },   
      { DATA_TYPE_VARIABLE,     &InstantPara.VolRSolar, 4,      DIS_VAL_VOLTAGE,        DIGIT_3,         0,             LED_SOLAR_R     },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             0               }
    },
    // SCREEN 62:  Line to Neutral Voltage 3P Y Solar
    {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrV,          LED_SOLAR_Y     },   
      { DATA_TYPE_VARIABLE,     &InstantPara.VolYSolar, 4,      DIS_VAL_VOLTAGE,        DIGIT_3,         0,             LED_SOLAR_Y     },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             0,              }
    },
    // SCREEN 63:  Line to Neutral Voltage 3P B Solar
    {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrV,          LED_SOLAR_B     },   
      { DATA_TYPE_VARIABLE,     &InstantPara.VolBSolar, 4,      DIS_VAL_VOLTAGE,        DIGIT_3,         0,             LED_SOLAR_B     },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             0               }
    },
    // SCREEN 64: Current 3P R Solar
    {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrA,          LED_SOLAR_R      },   
      { DATA_TYPE_VARIABLE,     &InstantPara.CurrentRSolar,  4, DIS_VAL_CURRENT,        DIGIT_3,         0,             LED_SOLAR_R      },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             0,              }
    },
    // SCREEN 65: Current 3P Y Solar
    {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrA,          LED_SOLAR_Y      },   
      { DATA_TYPE_VARIABLE,     &InstantPara.CurrentYSolar,  4, DIS_VAL_CURRENT,        DIGIT_3,         0,             LED_SOLAR_Y      },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             0,              }
    },
    // SCREEN 66: Current 3P B Solar
    {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrA,          LED_SOLAR_B      },   
      { DATA_TYPE_VARIABLE,     &InstantPara.CurrentBSolar,  4, DIS_VAL_CURRENT,        DIGIT_3,         0,             LED_SOLAR_B      },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             LED_SOLAR_B      }
    },
      // SCREEN 67: POWER KW 3P R Solar
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrW,         LED_SOLAR_R   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.TotalPowerRSolar,  4, DIS_VAL_POWER,        DIGIT_3,         0,            LED_SOLAR_R   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 68: POWER KW 3P Y Solar
   {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrW,         LED_SOLAR_Y   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.TotalPowerYSolar,  4, DIS_VAL_POWER,        DIGIT_3,         0,            LED_SOLAR_Y   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 69: POWER KW 3P B Solar
   {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrW,         LED_SOLAR_B   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.TotalPowerBSolar,  4, DIS_VAL_POWER,        DIGIT_3,         0,            LED_SOLAR_B   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 70: Sum Power  Solar
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrTW,        LED_SOLAR_TOTAL },   
      { DATA_TYPE_VARIABLE,     &InstantPara.SumTotalPowerSolar,4, DIS_VAL_POWER,        DIGIT_3,         0,            LED_SOLAR_TOTAL },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0 }
  },
  // SCREEN 71: kVA 3P R Solar
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVA,        LED_SOLAR_R   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.AppPowerRSolar,    4, DIS_VAL_POW_VA,       DIGIT_3,         0,            LED_SOLAR_R   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 72: kVA 3P Y Solar
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVA,        LED_SOLAR_Y   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.AppPowerYSolar,    4, DIS_VAL_POW_VA,       DIGIT_3,         0,            LED_SOLAR_Y   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 73: kVA 3P B Solar
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVA,        LED_SOLAR_B   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.AppPowerBSolar,    4, DIS_VAL_POW_VA,       DIGIT_3,         0,            LED_SOLAR_B   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
   // SCREEN 74: Sum VA  SOlar
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVA,        LED_SOLAR_TOTAL },   
      { DATA_TYPE_VARIABLE,     &InstantPara.TotalAppPowerSolar,4, DIS_VAL_POW_VA,       DIGIT_3,         0,            LED_SOLAR_TOTAL },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            LED_SOLAR_TOTAL }
  },
  // SCREEN 75: kVAr 3P R Solar
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVR,        LED_SOLAR_R   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.ReactPowerRSolar,  4, DIS_VAL_POW_VAR,      DIGIT_3,         0,            LED_SOLAR_R   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
 
  },
  // SCREEN 76: kVAr 3P Y SOlar
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVR,        LED_SOLAR_Y   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.ReactPowerYSolar,  4, DIS_VAL_POW_VAR,      DIGIT_3,         0,            LED_SOLAR_Y   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 77: kVAr 3P B Solar
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVR,        LED_SOLAR_B   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.ReactPowerBSolar,  4, DIS_VAL_POW_VAR,      DIGIT_3,         0,            LED_SOLAR_B   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 78: Sum VAR Solar 
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrVR,        LED_SOLAR_TOTAL },   
      { DATA_TYPE_VARIABLE,     &InstantPara.TotalReactPowerSolar,4,    DIS_VAL_POW_VAR, DIGIT_3,         0,            LED_SOLAR_TOTAL },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            LED_SOLAR_TOTAL }
  },
  // SCREEN 79: Power factor 3P R Solar
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrPF,        LED_SOLAR_R   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.PowerFactorRSolar, 4, DIS_VAL_PF,           DIGIT_3,         0,            LED_SOLAR_R   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 80: Power factor 3P Y Solar
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrPF,        LED_SOLAR_Y   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.PowerFactorYSolar, 4, DIS_VAL_PF,           DIGIT_3,         0,            LED_SOLAR_Y   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 81: Power factor 3P B Solar
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrPF,        LED_SOLAR_B   },   
      { DATA_TYPE_VARIABLE,     &InstantPara.PowerFactorBSolar, 4, DIS_VAL_PF,           DIGIT_3,         0,            LED_SOLAR_B   },
      { 0xff,                   0,                         0,     0,                     DIGIT_3,         0,            0       }
  },
  // SCREEN 82: Sum PF 
  {
      { DATA_TYPE_STRING,       0,                         0,      0,                    DIGIT_2,         StrPF,        LED_SOLAR_TOTAL },   
      { DATA_TYPE_VARIABLE,     &InstantPara.TotalPowerFactorSolar,4,DIS_VAL_PF,         DIGIT_3,         0,            LED_SOLAR_TOTAL },
      { 0xff,                   0,                         0,      0,                    DIGIT_3,         0,            LED_SOLAR_TOTAL }
  },

  // SCREEN 83: Kwh  Import Val Solar
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolRSolar,     9,       DIS_SOLAR_W_IMP,     DIGIT_1,          0,           LED_SOLAR_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_SOLAR_TOTAL }
  },
  // SCREEN 84: KVah  Import Val Solar
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolRSolar,     9,       DIS_SOLAR_VA_IMP,    DIGIT_1,          0,           LED_SOLAR_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_SOLAR_TOTAL }
  },
  // SCREEN 85: Reactive Pos  Import Val Solar
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolRSolar,     9,      DIS_SOLAR_VAR_POS_IMP, DIGIT_1,          0,           LED_SOLAR_TOTAL },
    {  0xff,                   0,                          0,      0,                    DIGIT_2,          0,           LED_SOLAR_TOTAL }
  },
  // SCREEN 86: Reactive Neg  Import Val Solar
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolRSolar,     9,       DIS_SOLAR_VAR_NEG_IMP,     DIGIT_1,          0,           LED_SOLAR_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_SOLAR_TOTAL }
  },


  // SCREEN 87: Kwh  Export Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,       0,                   DIGIT_6,         StrExpWh,     LED_OFF },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,         0,            LED_OFF }
  },
  // SCREEN 88: Kwh  Export Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,       DIS_W_EXP,           DIGIT_1,          0,           LED_GRID_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_GRID_TOTAL }
  },
  // SCREEN 89: KVah  Export Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,        0,                  DIGIT_6,          StrExpVah,   LED_OFF },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 90: KVah  Export Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,       DIS_VA_EXP,          DIGIT_1,          0,           LED_GRID_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_GRID_TOTAL }
  },
  // SCREEN 91: Reactive Pos  Export Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,      0,                    DIGIT_6,          StrExpVarh,  LED_OFF },
    {  0xff,                   0,                          0,      0,                    DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 92: Reactive Pos  Export Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,      DIS_VAR_POS_EXP,      DIGIT_1,          0,           LED_GRID_TOTAL },
    {  0xff,                   0,                          0,      0,                    DIGIT_2,          0,           LED_GRID_TOTAL }
  },
  // SCREEN 93: Reactive Neg  Export Str
  { 
    {  DATA_TYPE_STRING,       0,                          0,       0,                   DIGIT_6,          StrExpVarh,  LED_OFF },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_OFF }
  },
  // SCREEN 94: Reactive Neg  Export Val
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolR,          9,       DIS_VAR_NEG_EXP,     DIGIT_1,          0,           LED_GRID_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_GRID_TOTAL }
  },

  // SCREEN 95: Kwh  Export Val Solar
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolRSolar,     9,       DIS_SOLAR_W_EXP,     DIGIT_1,          0,           LED_SOLAR_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_SOLAR_TOTAL }
  },
  // SCREEN 96: KVah  Export Val Solar
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolRSolar,     9,       DIS_SOLAR_VA_EXP,    DIGIT_1,          0,           LED_SOLAR_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_SOLAR_TOTAL }
  },
  // SCREEN 97: Reactive Pos  Export Val Solar
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolRSolar,     9,      DIS_SOLAR_VAR_POS_EXP,DIGIT_1,          0,           LED_SOLAR_TOTAL },
    {  0xff,                   0,                          0,      0,                    DIGIT_2,          0,           LED_SOLAR_TOTAL }
  },
  // SCREEN 98: Reactive Neg  Export Val Solar
  { 
    {  DATA_TYPE_VARIABLE,     &InstantPara.VolRSolar,     9,       DIS_SOLAR_VAR_NEG_EXP,DIGIT_1,          0,          LED_SOLAR_TOTAL },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,          0,           LED_SOLAR_TOTAL }
  },

  // SCREEN 99: FAN 1 Display
  { 
    {  DATA_TYPE_STRING,       0,                          0,       0,                   DIGIT_6,         StrFan1Current,LED_OFF        },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,         0,             LED_OFF        }
  },
  // SCREEN 100: FAN 1 Current
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrA,          LED_OFF         },   
      { DATA_TYPE_VARIABLE,     &InstantPara.Fan1Current,4,     DIS_VAL_FAN_CURRENT,    DIGIT_3,         0,             LED_OFF         },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             0,              }
  },

  // SCREEN 101: FAN 2 Display
  { 
    {  DATA_TYPE_STRING,       0,                          0,       0,                   DIGIT_6,         StrFan2Current,LED_OFF        },
    {  0xff,                   0,                          0,       0,                   DIGIT_2,         0,             LED_OFF        }
  },
  // SCREEN 102: FAN 2 Current
  {
      { DATA_TYPE_STRING,       0,                      0,      0,                      DIGIT_2,         StrA,          LED_OFF         },   
      { DATA_TYPE_VARIABLE,     &InstantPara.Fan2Current,4,     DIS_VAL_FAN_CURRENT,    DIGIT_3,         0,             LED_OFF         },
      { 0xff,                   0,                      0,      0,                      DIGIT_3,         0,             0,              }
  },

};

