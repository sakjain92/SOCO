
#define SCREEN_VOL_LL_RY                0
#define SCREEN_VOL_LL_YB                1
#define SCREEN_VOL_LL_BR                2
#define SCREEN_VOL_LN_R                 3
#define SCREEN_VOL_LN_Y                 4
#define SCREEN_VOL_LN_B                 5
#define SCREEN_CUR_R                    6
#define SCREEN_CUR_Y                    7
#define SCREEN_CUR_B                    8
#define SCREEN_NEU_CURR                 9
#define SCREEN_AVG_CUR                  10
#define SCREEN_FREQ                     11
#define SCREEN_VOL_UNBAL                12
#define SCREEN_CUR_UNBAL                13
#define SCREEN_W_R                      14
#define SCREEN_W_Y                      15
#define SCREEN_W_B                      16
#define SCREEN_SUM_W_3P                 17
#define SCREEN_VA_R                     18
#define SCREEN_VA_Y                     19
#define SCREEN_VA_B                     20
#define SCREEN_SUM_VA_3P                21
#define SCREEN_VAR_R                    22
#define SCREEN_VAR_Y                    23
#define SCREEN_VAR_B                    24
#define SCREEN_SUM_VAR_3P               25
#define SCREEN_PF_R                     26
#define SCREEN_PF_Y                     27
#define SCREEN_PF_B                     28
#define SCREEN_SUM_PF_3P                29

//IMPORT
#define SCREEN_IMP_ENE_W_STR            30
#define SCREEN_IMP_ENE_W_VAL            31
#define SCREEN_IMP_ENE_VA_STR           32
#define SCREEN_IMP_ENE_VA_VAL           33
#define SCREEN_IMP_ENE_VAR_POS_STR      34
#define SCREEN_IMP_ENE_VAR_POS_VAL      35
#define SCREEN_IMP_ENE_VAR_NEG_STR      36
#define SCREEN_IMP_ENE_VAR_NEG_VAL      37
#define SCREEN_IMP_RHr_STR              38
#define SCREEN_IMP_RHr_VAL              39
#define SCREEN_IMP_LHr_STR              40
#define SCREEN_IMP_LHr_VAL              41

// OLD IMPORT
#define SCREEN_IMP_OLD_ENE_W_STR        42
#define SCREEN_IMP_OLD_ENE_W_VAL        43
#define SCREEN_IMP_OLD_ENE_VA_STR       44
#define SCREEN_IMP_OLD_ENE_VA_VAL       45
#define SCREEN_IMP_OLD_ENE_VAR_POS_STR  46
#define SCREEN_IMP_OLD_ENE_VAR_POS_VAL  47
#define SCREEN_IMP_OLD_ENE_VAR_NEG_STR  48
#define SCREEN_IMP_OLD_ENE_VAR_NEG_VAL  49
#define SCREEN_IMP_OLD_RHr_STR          50
#define SCREEN_IMP_OLD_RHr_VAL          51
#define SCREEN_IMP_OLD_LHr_STR          52
#define SCREEN_IMP_OLD_LHr_VAL          53

//ANGLE
#define SCREEN_ANGLE_R                  54
#define SCREEN_ANGLE_Y                  55
#define SCREEN_ANGLE_B                  56

// INTERRUPTS
#define SCREEN_INT_STR                  57
#define SCREEN_INT_VAL                  58
#define SCREEN_OLD_INT_STR              59
#define SCREEN_OLD_INT_VAL              60

// Solar Voltage
#define SCREEN_VOL_LN_R_SOLAR           61
#define SCREEN_VOL_LN_Y_SOLAR           62
#define SCREEN_VOL_LN_B_SOLAR           63

// Solar Current
#define SCREEN_CUR_R_SOLAR              64
#define SCREEN_CUR_Y_SOLAR              65
#define SCREEN_CUR_B_SOLAR              66

// Solar Power
#define SCREEN_W_R_SOLAR                67
#define SCREEN_W_Y_SOLAR                68
#define SCREEN_W_B_SOLAR                69
#define SCREEN_SUM_W_3P_SOLAR           70
#define SCREEN_VA_R_SOLAR               71
#define SCREEN_VA_Y_SOLAR               72
#define SCREEN_VA_B_SOLAR               73
#define SCREEN_SUM_VA_3P_SOLAR          74
#define SCREEN_VAR_R_SOLAR              75
#define SCREEN_VAR_Y_SOLAR              76
#define SCREEN_VAR_B_SOLAR              77
#define SCREEN_SUM_VAR_3P_SOLAR         78

// Solar PF
#define SCREEN_PF_R_SOLAR               79
#define SCREEN_PF_Y_SOLAR               80
#define SCREEN_PF_B_SOLAR               81
#define SCREEN_SUM_PF_3P_SOLAR          82

// Solar Energy
#define SCREEN_IMP_ENE_W_VAL_SOLAR       83
#define SCREEN_IMP_ENE_VA_VAL_SOLAR      84
#define SCREEN_IMP_ENE_VAR_POS_VAL_SOLAR 85
#define SCREEN_IMP_ENE_VAR_NEG_VAL_SOLAR 86

// Export Energy
#define SCREEN_EXP_ENE_W_STR            87
#define SCREEN_EXP_ENE_W_VAL            88
#define SCREEN_EXP_ENE_VA_STR           89
#define SCREEN_EXP_ENE_VA_VAL           90
#define SCREEN_EXP_ENE_VAR_POS_STR      91
#define SCREEN_EXP_ENE_VAR_POS_VAL      92
#define SCREEN_EXP_ENE_VAR_NEG_STR      93
#define SCREEN_EXP_ENE_VAR_NEG_VAL      94

// Solar Export Energy
#define SCREEN_EXP_ENE_W_VAL_SOLAR      95
#define SCREEN_EXP_ENE_VA_VAL_SOLAR     96
#define SCREEN_EXP_ENE_VAR_POS_VAL_SOLAR 97
#define SCREEN_EXP_ENE_VAR_NEG_VAL_SOLAR 98


  const uint8_t SCREEN_SEQ_3P_MAIN[]=  
  {
   SCREEN_VOL_LN_R,             
   SCREEN_VOL_LN_Y,             
   SCREEN_VOL_LN_B, 
   SCREEN_VOL_LN_R_SOLAR,
   SCREEN_VOL_LN_B_SOLAR,
   SCREEN_VOL_LN_Y_SOLAR,
   SCREEN_CUR_R,                
   SCREEN_CUR_Y,                
   SCREEN_CUR_B,    
   SCREEN_CUR_R_SOLAR,                
   SCREEN_CUR_Y_SOLAR,                
   SCREEN_CUR_B_SOLAR,
   SCREEN_W_R,                  
   SCREEN_W_Y,                  
   SCREEN_W_B,                  
   SCREEN_SUM_W_3P,             
   SCREEN_W_R_SOLAR,                  
   SCREEN_W_Y_SOLAR,                  
   SCREEN_W_B_SOLAR,                  
   SCREEN_SUM_W_3P_SOLAR,             
   SCREEN_VA_R,                 
   SCREEN_VA_Y,                 
   SCREEN_VA_B,                 
   SCREEN_SUM_VA_3P,    
   SCREEN_VA_R_SOLAR,                 
   SCREEN_VA_Y_SOLAR,                 
   SCREEN_VA_B_SOLAR,                 
   SCREEN_SUM_VA_3P_SOLAR,     
   SCREEN_VAR_R,                
   SCREEN_VAR_Y,                
   SCREEN_VAR_B,                
   SCREEN_SUM_VAR_3P,           
   SCREEN_VAR_R_SOLAR,                
   SCREEN_VAR_Y_SOLAR,                
   SCREEN_VAR_B_SOLAR,                
   SCREEN_SUM_VAR_3P_SOLAR,           
   SCREEN_PF_R,                 
   SCREEN_PF_Y,                 
   SCREEN_PF_B,                 
   SCREEN_SUM_PF_3P,            
   SCREEN_PF_R_SOLAR,                 
   SCREEN_PF_Y_SOLAR,                 
   SCREEN_PF_B_SOLAR,                 
   SCREEN_SUM_PF_3P_SOLAR,            
   SCREEN_IMP_ENE_W_STR,        
   SCREEN_IMP_ENE_W_VAL,        
   SCREEN_IMP_ENE_W_VAL_SOLAR,        
   SCREEN_IMP_ENE_VA_STR,       
   SCREEN_IMP_ENE_VA_VAL,       
   SCREEN_IMP_ENE_VA_VAL_SOLAR,       
   SCREEN_IMP_ENE_VAR_POS_STR,  
   SCREEN_IMP_ENE_VAR_POS_VAL,  
   SCREEN_IMP_ENE_VAR_POS_VAL_SOLAR,  
   SCREEN_IMP_ENE_VAR_NEG_STR,  
   SCREEN_IMP_ENE_VAR_NEG_VAL,  
   SCREEN_IMP_ENE_VAR_NEG_VAL_SOLAR,
   SCREEN_EXP_ENE_W_STR,
   SCREEN_EXP_ENE_W_VAL,
   SCREEN_EXP_ENE_W_VAL_SOLAR,
   SCREEN_EXP_ENE_VA_STR,
   SCREEN_EXP_ENE_VA_VAL,
   SCREEN_EXP_ENE_VA_VAL_SOLAR,
   SCREEN_EXP_ENE_VAR_POS_STR,
   SCREEN_EXP_ENE_VAR_POS_VAL,
   SCREEN_EXP_ENE_VAR_POS_VAL_SOLAR,
   SCREEN_EXP_ENE_VAR_NEG_STR,
   SCREEN_EXP_ENE_VAR_NEG_VAL,
   SCREEN_EXP_ENE_VAR_NEG_VAL_SOLAR,
   0XFF,
  };
  const uint8_t SCREEN_SEQ_OLD_DATA[]=
  {
   SCREEN_IMP_OLD_ENE_W_STR,    
   SCREEN_IMP_OLD_ENE_W_VAL,    
   SCREEN_IMP_OLD_ENE_VA_STR,   
   SCREEN_IMP_OLD_ENE_VA_VAL,   
   SCREEN_IMP_OLD_ENE_VAR_POS_STR,
   SCREEN_IMP_OLD_ENE_VAR_POS_VAL,
   SCREEN_IMP_OLD_ENE_VAR_NEG_STR,
   SCREEN_IMP_OLD_ENE_VAR_NEG_VAL,
   0XFF,
  };

COMPILE_ASSERT(ARRAY_SIZE(SCREEN_SEQ_3P_MAIN) <= ARRAY_SIZE(DisplayParameterBuffer));
COMPILE_ASSERT(ARRAY_SIZE(SCREEN_SEQ_OLD_DATA) <= ARRAY_SIZE(DisplayParameterBuffer));
