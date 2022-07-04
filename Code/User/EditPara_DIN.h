#ifndef EDITPARA_DIN_H
#define EDITPARA_DIN_H

#include "Model_DIN.h"
#include "Parameter_DIN.h"
#include "Struct_edit.h"

/* Local Functions Declarations */
void EditResetPassword(uint8_t KeyPressed);
void EditSystemParameter(uint8_t KeyPressed);
void ViewSystemParameter(uint8_t KeyPressed);
void ViewOldData(uint8_t KeyPressed);

/*  Local variables Declarations */
const uint8_t DisKva3D[]="  3D";
const uint8_t DisKvaAirth[]="ARTH";
const uint8_t *DisKvaType[]={DisKva3D,DisKvaAirth};

const uint8_t   DisSysConfig3P4W[]="3P4W";
const uint8_t   DisSysConfig1P[]=  "  1P";
const uint8_t   DisSysConfig3P3W[]="3P3W";
const uint8_t *DisSysConfig[]={DisSysConfig3P4W,DisSysConfig1P,DisSysConfig3P3W};

const uint8_t   DisParityNone[]="NONE";
const uint8_t   DisParityEven[]="EVEN";
const uint8_t   DisParityOdd[]= " ODD";
const uint8_t   *DisParity[]={DisParityNone,DisParityEven,DisParityOdd};

const uint8_t   DisBaudRate1200[]="1200";
const uint8_t   DisBaudRate2400[]="2400";
const uint8_t   DisBaudRate4800[]="4800";
const uint8_t   DisBaudRate9600[]="9600";
const uint8_t   DisBaudRate19200[]="9200";// 1 done seperately
const uint8_t   *DisBaudRate[]={DisBaudRate1200,DisBaudRate2400,DisBaudRate4800,DisBaudRate9600,DisBaudRate19200};

const uint8_t   DisStopBit1[]="1";
const uint8_t   DisStopBit2[]="2";
const uint8_t   *DisStopBits[]={DisStopBit1,DisStopBit2};


const uint8_t   DisEneType1[]="RESO";
const uint8_t   DisEneType2[]="COUNT";
const uint8_t   *DisEneType[]={DisEneType1,DisEneType2};

const uint8_t   DisLittleEndian[]=" LIT";
const uint8_t   DisBigEndian[]=" BIG";
const uint8_t   *DisEndTyp[]={DisBigEndian,DisLittleEndian};


const uint8_t DIS_ENTER[]="ENTR";
const uint8_t DIS_CUR_PASS[]="C PASS";
const uint8_t DIS_PASS[]="PASS";
const uint8_t DIS_OK[]="  OK";
const uint8_t DIS_NOK[]=" NOK";
const uint8_t DIS_EDIT[]="EDIT";
const uint8_t DIS_VIEW[]="VIEW";
//const uint8_t DIS_TRIP[]="TRIP";
const uint8_t DIS_OLD[]=" OLD";
const uint8_t DIS_RESET[]=" RST";
const uint8_t DIS_SEL_RESO[]="RESO";
const uint8_t DIS_SEL_ENE[]=" ENE";
const uint8_t DIS_PASSWORD[]="PASS";
const uint8_t DIS_UPDT[]="UPDT";
const uint8_t DIS_REJ[]="REJ";
const uint8_t DIS_SET_NEW[]="S NEW";
const uint8_t DIS_BLANK[]="";
const uint8_t DIS_SEL_STRT[]="STRT";
const uint8_t DIS_SEL_CURR[]="  MA";
const uint8_t DIS_SEL_PT_PRIMARY[]="PTPR";
const uint8_t DIS_SEL_VALUE[]=" VAL";
const uint8_t DIS_SEL_DEC[]=" DEC";
const uint8_t DIS_SEL_UNIT[]="UNIT";
const uint8_t DIS_SEL_PT_SECONDARY[]="PTSR";
const uint8_t DIS_SEL_CT_PRIMARY[]="CTPR";
const uint8_t DIS_SEL_CT_SECONDARY[]="CTSR";
const uint8_t DIS_SEL_KVA[]=" KVA";
const uint8_t DIS_SEL_END[]="ENDI";
const uint8_t DIS_SEL_TYPE[]="TYPE";
const uint8_t DIS_SEL_SYSTEM[]=" SYS";
const uint8_t DIS_SEL_CONF[]="CONF";
const uint8_t DIS_SEL_DEVICE[]=" DEV";
const uint8_t DIS_SEL_ID[]="  ID";
const uint8_t DIS_SEL_BAUD[]="BAUD";
const uint8_t DIS_SEL_RATE[]="RATE";
const uint8_t DIS_SEL_COM[]="COMM";
const uint8_t DIS_SEL_PAR[]="PARI";
const uint8_t DIS_SEL_STOP[]="STOP";
const uint8_t DIS_SEL_BIT[]=" BIT";
const uint8_t DIS_SEL_MEAS[]="MEAS";
const uint8_t DIS_SEL_UPDT[]="UPDT";
const uint8_t DIS_CHK[]="CHK";
const uint8_t DIS_CT_PT[]="E CTPT";
const uint8_t DIS_CT[]="CT";
const uint8_t DIS_PT[]="PT";
const uint8_t DIS_SEL_DISP[]="DISP";

struct EditStart
{
  uint8_t  *FirstLine;
  uint8_t   StartIndex;
  uint8_t    EndIndex;
  void      (* Function)(uint8_t KeyPressed);
  uint8_t    NoFirstLineDisplay;

};

const struct EditStart StartSelectingBlock[12]=
{
  {(uint8_t*)0,                    0,   0,       0,                       0},// Should always be first Line.
#ifndef MODEL_COMM_SETTING   
  {(uint8_t*)DIS_EDIT,             0,   12,       EditSystemParameter,    1},
#else
  {(uint8_t*)DIS_EDIT,             0,   17,       EditSystemParameter,    1},
#endif
  {(uint8_t*)DIS_VIEW,             0,   17,       ViewSystemParameter,    0},
#ifdef MODEL_DIS_OLD_DATA
  {(uint8_t*)DIS_OLD,             0,   16,       ViewOldData,            0},
#endif
  {(uint8_t*)DIS_PASSWORD,        255,  0,        EditResetPassword,      1},
  {(uint8_t*)DIS_BLANK,            0,    0,       0,                       0}// Always the last line
};







// DecimalPosition upto 10 ts for decimal position after that 
// to select discreet selection , like ON/OFF etc.


const struct Parameter EditParameters[MAX_PARAM_LIMIT]=
  {
    {    
    
         PARA_PT_PRIMARY,                          // VariableIndex
         DIS_SEL_PT_PRIMARY,                       //*DesArrayTop
         DIS_SEL_VALUE,                            //*DesArrayMid
         50,                                        //MinValue
         999,                                      // Max value
         1,                                         //StepValue
         0,                                        //DecimalPosition
         3,                                       //ParaMeterLength
         0,                                       //DisableType
         PARA_ONLY_DATA,                          //DataWithDecUnit
         415,                                     //Default Value
    },
    
    {    
    
         PARA_PT_PRIMARY_DEC,                       // VariableIndex
         DIS_SEL_DEC,                             //*DesArrayTop
         DIS_SEL_DEC,                            //*DesArrayMid
         0,                                         //MinValue
         2,                                        // Max value
         1,                                         //StepValue
         0,                                       //DecimalPosition
         3,                                       //ParaMeterLength
         0,                                       //DisableType
         PARA_WITH_DEC,                          //DataWithDecUnit
         0,                                      //Default Value
    },
    
    
    {    
    
         PARA_PT_PRI_UNIT,                         // VariableIndex
         DIS_SEL_UNIT,                       //*DesArrayTop
         DIS_SEL_UNIT,                            //*DesArrayMid
         0,                                        //MinValue
         1,                                         // Max value
         1,                                       //StepValue
         0,                                      //DecimalPosition
         3,                                      //ParaMeterLength
         0,                                       //DisableType
         PARA_WITH_UNIT,                         //DataWithDecUnit
         0,                                      //Default Value
    },
    
    {    
    
         PARA_PT_SECONDARY,                          // VariableIndex
         DIS_SEL_PT_SECONDARY,                       //*DesArrayTop
         DIS_SEL_VALUE,                            //*DesArrayMid
         50,                                        //MinValue
         999,                                      // Max value
         1,                                       //StepValue
         0,                                       //DecimalPosition
         3,                                      //ParaMeterLength
         0,                                       //DisableType
         0,                                       //DataWithDecUnit
         415,                                     //Default Value
    },
    
    {    
    
         PARA_CT_PRIMARY,                          // VariableIndex
         DIS_SEL_CT_PRIMARY,                       //*DesArrayTop
         DIS_SEL_VALUE,                            //*DesArrayMid
         1,                                        //MinValue
         999,                                      // Max value
         1,                                         //StepValue
         0,                                        //DecimalPosition
         3,                                       //ParaMeterLength
         0,                                       //DisableType
         PARA_ONLY_DATA,                         //DataWithDecUnit
         5,                                      //Default Value
    },
    
    {    
    
         PARA_CT_PRIMARY_DEC,                       // VariableIndex
         DIS_SEL_DEC,                       //*DesArrayTop
         DIS_SEL_DEC,                            //*DesArrayMid
         0,                                         //MinValue
         2,                                        // Max value
         1,                                         //StepValue
         0,                                       //DecimalPosition
         3,                                       //ParaMeterLength
         0,                                       //DisableType
         PARA_WITH_DEC,                         //DataWithDecUnit
         0,                                      //Default Value
    },
    
    
    {    
    
         PARA_CT_PRI_UNIT,                         // VariableIndex
         DIS_SEL_UNIT,                       //*DesArrayTop
         DIS_SEL_UNIT,                            //*DesArrayMid
         0,                                        //MinValue
         1,                                         // Max value
         1,                                       //StepValue
         0,                                      //DecimalPosition
         3,                                      //ParaMeterLength
         0,                                       //DisableType
         PARA_WITH_UNIT,                         //DataWithDecUnit
         0,                                      //Default Value
    },
    
    {    
    
         PARA_CT_SECONDARY,                          // VariableIndex
         DIS_SEL_CT_SECONDARY,                       //*DesArrayTop
         DIS_SEL_VALUE,                            //*DesArrayMid
         1,                                        //MinValue
         10,                                      // Max value
         1,                                       //StepValue
         0,                                       //DecimalPosition
         2,                                      //ParaMeterLength
         0,                                       //DisableType
         0,                                       //DataWithDecUnit
         5,                                      //Default Value
    },
    {    
    
         PARA_SYSTEM_CONFIG,                          // VariableIndex
         DIS_SEL_SYSTEM,                          //*DesArrayTop
         DIS_SEL_CONF,                            //*DesArrayMid
         0,                                        //MinValue
         2,                                      // Max value
         1,                                       //StepValue
         EDIT_DEC_SYSTEM_CONFIG,                   //DecimalPosition
         16,                                      //ParaMeterLength
         0,                                      //DisableType
         0,                                       //DataWithDecUnit
         0,                                      //Default Value
    },
    {    
    
         PARA_KVA_TYPE,                          // VariableIndex
         DIS_SEL_KVA,                            //*DesArrayTop
         DIS_SEL_TYPE,                            //*DesArrayMid
         0,                                        //MinValue
         1,                                      // Max value
         1,                                       //StepValue
         EDIT_DEC_KVA_TYPE,                     //DecimalPosition
         16,                                      //ParaMeterLength
         0,                                       //DisableType
         0,                                       //DataWithDecUnit
         0,                                      //Default Value
    },
    
    
    
    
     {    
    
         PARA_STARTING_CURRENT,                          // VariableIndex
         DIS_SEL_STRT,                            //*DesArrayTop
         DIS_SEL_CURR,                            //*DesArrayMid
         1,                                        //MinValue
         200,                                      // Max value
         1,                                       //StepValue
         0,                                    //DecimalPosition
         3,                                      //ParaMeterLength
         0,                                      //DisableType
         0,                                       //DataWithDecUnit
         5,                                      //Default Value
    },
    {    
    
         PARA_DISP_RESO,                          // VariableIndex
         DIS_SEL_DISP,                            //*DesArrayTop
         DIS_SEL_RESO,                            //*DesArrayMid
         2,                                        //MinValue
         3,                                      // Max value
         1,                                       //StepValue
         0,                                    //DecimalPosition
         3,                                      //ParaMeterLength
         EDITDIS_SKIP_ALWAYS,                    //DisableType
         0,                                       //DataWithDecUnit
         3,                                      //Default Value
    },
    {    
    
         PARA_ENE_TYPE,                          // VariableIndex
         DIS_SEL_ENE,                            //*DesArrayTop
         DIS_SEL_TYPE,                            //*DesArrayMid
         0,                                        //MinValue
         1,                                      // Max value
         1,                                       //StepValue
         EDIT_DEC_ENERGY_TYPE,                    //DecimalPosition
         16,                                      //ParaMeterLength
         EDITDIS_SKIP_ALWAYS,                    //DisableType
         0,                                       //DataWithDecUnit
         0,                                      //Default Value
    },
    {    
    
         PARA_DEVICE_ID,                          // VariableIndex
         DIS_SEL_DEVICE,                         //*DesArrayTop
         DIS_SEL_ID,                              //*DesArrayMid
         1,                                        //MinValue
         247,                                      // Max value
         1,                                       //StepValue
         0,                                        //DecimalPosition
         3,                                      //ParaMeterLength
  #ifdef MODEL_COMM_SETTING      
         0,                                      //DisableType
  #else 
         EDITDIS_SKIP_ALWAYS,                     //DisableType
  #endif       
         0,                                       //DataWithDecUnit
         1,                                      //Default Value
    },
    
    {    
    
         PARA_BAUD_RATE,                          // VariableIndex
         DIS_SEL_BAUD,                            //*DesArrayTop
         DIS_SEL_RATE,                            //*DesArrayMid
         0,                                        //MinValue
         4,                                      // Max value
         1,                                       //StepValue
         EDIT_DEC_BAUD_RATE,                    //DecimalPosition
         5,                                      //ParaMeterLength
  #ifdef MODEL_COMM_SETTING      
         0,                                      //DisableType
  #else 
         EDITDIS_SKIP_ALWAYS,                     //DisableType
  #endif
         0,                                       //DataWithDecUnit
         3,                                      //Default Value
    },
    
    {    
    
         PARA_PARITY,                          // VariableIndex
         DIS_SEL_COM,                          //*DesArrayTop
         DIS_SEL_PAR,                          //*DesArrayMID
         0,                                        //MinValue
         2,                                      // Max value
         1,                                       //StepValue
         EDIT_DEC_PARITY,                    //DecimalPosition
         16,                                      //ParaMeterLength
  #ifdef MODEL_COMM_SETTING      
         0,                                      //DisableType
  #else 
         EDITDIS_SKIP_ALWAYS,                     //DisableType
  #endif
         0,                                       //DataWithDecUnit
         0,                                      //Default Value

    },
    
     {    
    
         PARA_STOP_BIT,                          // VariableIndex
         DIS_SEL_STOP,                          //*DesArrayTop
         DIS_SEL_BIT,                          //*DesArrayMID
         0,                                        //MinValue
         1,                                      // Max value
         1,                                       //StepValue
         EDIT_DEC_STOP_BIT,                    //DecimalPosition
         16,                                      //ParaMeterLength
  #ifdef MODEL_COMM_SETTING      
         0,                                      //DisableType
  #else 
         EDITDIS_SKIP_ALWAYS,                     //DisableType
  #endif
         0,                                       //DataWithDecUnit
         0,                                      //Default Value
    },
    
    {    
  
       PARA_ENDIAN,                          // VariableIndex
       DIS_SEL_END,                          //*DesArrayTop
       DIS_SEL_TYPE,                          //*DesArrayMID
       0,                                        //MinValue
       1,                                      // Max value
       1,                                       //StepValue
       EDIT_ENDIAN_TYPE,                      //DecimalPosition
       16,                                      //ParaMeterLength    
  #ifdef MODEL_COMM_SETTING      
        0,                                      //DisableType
  #else 
        EDITDIS_SKIP_ALWAYS,                     //DisableType
  #endif
       0,                                       //DataWithDecUnit
       0,                                      //Default Value
  },
   
  };


#endif
