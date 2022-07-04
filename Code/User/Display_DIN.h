
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



#ifdef MODEL_REX_1100
  const uint8_t SCREEN_SEQ_1P_MAIN[]=  
  {
    SCREEN_VOL_LN_R,            //3
    SCREEN_CUR_R,               //6
    SCREEN_FREQ,                //11 
    0XFF,
  };
  const uint8_t SCREEN_SEQ_3P_MAIN[]=  
  {
   SCREEN_VOL_LL_RY,            //0
   SCREEN_VOL_LL_YB,            //1
   SCREEN_VOL_LL_BR,            //2
   SCREEN_VOL_LN_R,             //3
   SCREEN_VOL_LN_Y,             //4
   SCREEN_VOL_LN_B,             //5
   SCREEN_CUR_R,                //6
   SCREEN_CUR_Y,                //7
   SCREEN_CUR_B,                //8
   SCREEN_FREQ,                 //11
   0XFF,
  };
  const uint8_t SCREEN_SEQ_OLD_DATA[]=
  {
   0XFF,
  };
#endif
  
#ifdef MODEL_REX_1200
  const uint8_t SCREEN_SEQ_1P_MAIN[]=  
  {
    
   SCREEN_VOL_LN_R,             //3
   SCREEN_CUR_R,                //6
   SCREEN_FREQ,                 //11
   SCREEN_W_R,                  //14
   SCREEN_VA_R,                 //18
   0XFF,
  };
  const uint8_t SCREEN_SEQ_3P_MAIN[]=  
  {
   SCREEN_VOL_LL_RY,            //0
   SCREEN_VOL_LL_YB,            //1
   SCREEN_VOL_LL_BR,            //2
   SCREEN_VOL_LN_R,             //3
   SCREEN_VOL_LN_Y,             //4
   SCREEN_VOL_LN_B,             //5
   SCREEN_CUR_R,                //6
   SCREEN_CUR_Y,                //7
   SCREEN_CUR_B,                //8
   SCREEN_NEU_CURR,             //9
   SCREEN_AVG_CUR,              //10
   SCREEN_FREQ,                 //11
   SCREEN_W_R,                  //14
   SCREEN_W_Y,                  //15
   SCREEN_W_B,                  //16
   SCREEN_SUM_W_3P,             //17
   SCREEN_VA_R,                 //18
   SCREEN_VA_Y,                 //19
   SCREEN_VA_B,                 //20
   SCREEN_SUM_VA_3P,            //21
   0XFF,
  };
  const uint8_t SCREEN_SEQ_OLD_DATA[]=
  {
   0XFF,
  };
  
 
#endif
  
#ifdef MODEL_REX_1300
  const uint8_t SCREEN_SEQ_1P_MAIN[]=  
  {
   SCREEN_VOL_LN_R,             //3
   SCREEN_CUR_R,                //6
   SCREEN_FREQ,                 //11
   SCREEN_VA_R,                 //18
   SCREEN_PF_R,                 //26
   0XFF,
  };
  const uint8_t SCREEN_SEQ_3P_MAIN[]=  
  {
    
   SCREEN_VOL_LL_RY,            //0
   SCREEN_VOL_LL_YB,            //1
   SCREEN_VOL_LL_BR,            //2
   SCREEN_VOL_LN_R,             //3
   SCREEN_VOL_LN_Y,             //4
   SCREEN_VOL_LN_B,             //5
   SCREEN_CUR_R,                //6
   SCREEN_CUR_Y,                //7
   SCREEN_CUR_B,                //8
   SCREEN_NEU_CURR,             //9
   SCREEN_AVG_CUR,              //10
   SCREEN_FREQ,                 //11
   SCREEN_VA_R,                 //18
   SCREEN_VA_Y,                 //19
   SCREEN_VA_B,                 //20
   SCREEN_SUM_VA_3P,            //21
   SCREEN_PF_R,                 //26
   SCREEN_PF_Y,                 //27
   SCREEN_PF_B,                 //28
   SCREEN_SUM_PF_3P,            //29
   0XFF,
  };
  const uint8_t SCREEN_SEQ_OLD_DATA[]=
  {
   0XFF,
  };
   
#endif  
  
  
  
#ifdef MODEL_REX_1400
  const uint8_t SCREEN_SEQ_1P_MAIN[]=  
  {
   SCREEN_VOL_LN_R,             //3
   SCREEN_CUR_R,                //6
   SCREEN_FREQ,                 //11
   SCREEN_W_R,                  //14
   SCREEN_VA_R,                 //18
   SCREEN_PF_R,                 //26
   0XFF,
  };
  const uint8_t SCREEN_SEQ_3P_MAIN[]=  
  {
   SCREEN_VOL_LL_RY,            //0
   SCREEN_VOL_LL_YB,            //1
   SCREEN_VOL_LL_BR,            //2
   SCREEN_VOL_LN_R,             //3
   SCREEN_VOL_LN_Y,             //4
   SCREEN_VOL_LN_B,             //5
   SCREEN_CUR_R,                //6
   SCREEN_CUR_Y,                //7
   SCREEN_CUR_B,                //8
   SCREEN_NEU_CURR,             //9
   SCREEN_AVG_CUR,              //10
   SCREEN_FREQ,                 //11
   SCREEN_W_R,                  //14
   SCREEN_W_Y,                  //15
   SCREEN_W_B,                  //16
   SCREEN_SUM_W_3P,             //17
   SCREEN_VA_R,                 //18
   SCREEN_VA_Y,                 //19
   SCREEN_VA_B,                 //20
   SCREEN_SUM_VA_3P,            //21
   SCREEN_PF_R,                 //26
   SCREEN_PF_Y,                 //27
   SCREEN_PF_B,                 //28
   SCREEN_SUM_PF_3P,            //29
   0XFF,
  };
  const uint8_t SCREEN_SEQ_OLD_DATA[]=
  {
   0XFF,
  };
#endif
  
#ifdef MODEL_REX_1500
  const uint8_t SCREEN_SEQ_1P_MAIN[]=  
  {
    
   SCREEN_VOL_LN_R,             //3
   SCREEN_CUR_R,                //6
   SCREEN_FREQ,                 //11
   SCREEN_W_R,                  //14
   SCREEN_VA_R,                 //18
   SCREEN_VAR_R,                //22
   SCREEN_PF_R,                 //26
   SCREEN_ANGLE_R,              //54
   0XFF,
  };
  const uint8_t SCREEN_SEQ_3P_MAIN[]=  
  {
   SCREEN_VOL_LL_RY,            //0
   SCREEN_VOL_LL_YB,            //1
   SCREEN_VOL_LL_BR,            //2
   SCREEN_VOL_LN_R,             //3
   SCREEN_VOL_LN_Y,             //4
   SCREEN_VOL_LN_B,             //5
   SCREEN_CUR_R,                //6
   SCREEN_CUR_Y,                //7
   SCREEN_CUR_B,                //8
   SCREEN_NEU_CURR,             //9
   SCREEN_AVG_CUR,              //10
   SCREEN_FREQ,                 //11
   SCREEN_VOL_UNBAL,            //12
   SCREEN_CUR_UNBAL,            //13
   SCREEN_W_R,                  //14
   SCREEN_W_Y,                  //15
   SCREEN_W_B,                  //16
   SCREEN_SUM_W_3P,             //17
   SCREEN_VA_R,                 //18
   SCREEN_VA_Y,                 //19
   SCREEN_VA_B,                 //20
   SCREEN_SUM_VA_3P,            //21
   SCREEN_VAR_R,                //22
   SCREEN_VAR_Y,                //23
   SCREEN_VAR_B,                //24
   SCREEN_SUM_VAR_3P,           //25
   SCREEN_PF_R,                 //26
   SCREEN_PF_Y,                 //27
   SCREEN_PF_B,                 //28
   SCREEN_SUM_PF_3P,            //29
   SCREEN_ANGLE_R,              //54
   SCREEN_ANGLE_Y,              //55
   SCREEN_ANGLE_B,              //56
   0XFF,
  };
  const uint8_t SCREEN_SEQ_OLD_DATA[]=
  {
   0XFF,
  };
  
#endif
  
#ifdef MODEL_REX_1900
  const uint8_t SCREEN_SEQ_1P_MAIN[]=  
  {
   SCREEN_W_R,                  //14
   SCREEN_VA_R,                 //18
   SCREEN_VAR_R,                //22
   SCREEN_PF_R,                 //26
   0XFF,
  };
  const uint8_t SCREEN_SEQ_3P_MAIN[]=  
  {
   SCREEN_W_R,                  //14
   SCREEN_W_Y,                  //15
   SCREEN_W_B,                  //16
   SCREEN_SUM_W_3P,             //17
   SCREEN_VA_R,                 //18
   SCREEN_VA_Y,                 //19
   SCREEN_VA_B,                 //20
   SCREEN_SUM_VA_3P,            //21
   SCREEN_VAR_R,                //22
   SCREEN_VAR_Y,                //23
   SCREEN_VAR_B,                //24
   SCREEN_SUM_VAR_3P,           //25
   SCREEN_PF_R,                 //26
   SCREEN_PF_Y,                 //27
   SCREEN_PF_B,                 //28
   SCREEN_SUM_PF_3P,            //29
   0XFF,
  };
  const uint8_t SCREEN_SEQ_OLD_DATA[]=
  {
   0XFF,
  };
  
#endif
  
#ifdef MODEL_REX_2140
  const uint8_t SCREEN_SEQ_1P_MAIN[]=  
  {
    
   SCREEN_VOL_LN_R,             //3
   SCREEN_CUR_R,                //6
   SCREEN_FREQ,                 //11
   SCREEN_IMP_ENE_W_STR,        //30
   SCREEN_IMP_ENE_W_VAL,        //31
   SCREEN_IMP_ENE_VA_STR,       //32
   SCREEN_IMP_ENE_VA_VAL,       //33
   SCREEN_IMP_RHr_STR,          //38
   SCREEN_IMP_RHr_VAL,          //39
   SCREEN_IMP_LHr_STR,          //40
   SCREEN_IMP_LHr_VAL,          //41
   0XFF,
  };
  const uint8_t SCREEN_SEQ_3P_MAIN[]=  
  {
   SCREEN_VOL_LL_RY,            //0
   SCREEN_VOL_LL_YB,            //1
   SCREEN_VOL_LL_BR,            //2
   SCREEN_VOL_LN_R,             //3
   SCREEN_VOL_LN_Y,             //4
   SCREEN_VOL_LN_B,             //5
   SCREEN_CUR_R,                //6
   SCREEN_CUR_Y,                //7
   SCREEN_CUR_B,                //8
   SCREEN_FREQ,                 //11
   SCREEN_IMP_ENE_W_STR,        //30
   SCREEN_IMP_ENE_W_VAL,        //31
   SCREEN_IMP_ENE_VA_STR,       //32
   SCREEN_IMP_ENE_VA_VAL,       //33
   SCREEN_IMP_RHr_STR,          //38
   SCREEN_IMP_RHr_VAL,          //39
   SCREEN_IMP_LHr_STR,          //40
   SCREEN_IMP_LHr_VAL,          //41
   0XFF,
  };
  const uint8_t SCREEN_SEQ_OLD_DATA[]=
  {
   0XFF,
  };
  
#endif

#ifdef MODEL_REX_2330
  const uint8_t SCREEN_SEQ_1P_MAIN[]=  
  {
   SCREEN_VOL_LN_R,             //3
   SCREEN_CUR_R,                //6
   SCREEN_FREQ,                 //11
   SCREEN_VA_R,                 //18
   SCREEN_PF_R,                 //26
   SCREEN_IMP_ENE_VAR_POS_STR,  //34
   SCREEN_IMP_ENE_VAR_POS_VAL,  //35
   SCREEN_IMP_ENE_VAR_NEG_STR,  //36
   SCREEN_IMP_ENE_VAR_NEG_VAL,  //37
   SCREEN_ANGLE_R,              //54
   0XFF,
  };
  const uint8_t SCREEN_SEQ_3P_MAIN[]=  
  {
   SCREEN_VOL_LL_RY,            //0
   SCREEN_VOL_LL_YB,            //1
   SCREEN_VOL_LL_BR,            //2
   SCREEN_VOL_LN_R,             //3
   SCREEN_VOL_LN_Y,             //4
   SCREEN_VOL_LN_B,             //5
   SCREEN_CUR_R,                //6
   SCREEN_CUR_Y,                //7
   SCREEN_CUR_B,                //8
   SCREEN_NEU_CURR,             //9
   SCREEN_AVG_CUR,              //10
   SCREEN_FREQ,                 //11
   SCREEN_VOL_UNBAL,            //12
   SCREEN_CUR_UNBAL,            //13
   SCREEN_VA_R,                 //18
   SCREEN_VA_Y,                 //19
   SCREEN_VA_B,                 //20
   SCREEN_SUM_VA_3P,            //21
   SCREEN_PF_R,                 //26
   SCREEN_PF_Y,                 //27
   SCREEN_PF_B,                 //28
   SCREEN_SUM_PF_3P,            //29
   SCREEN_IMP_ENE_VAR_POS_STR,  //34
   SCREEN_IMP_ENE_VAR_POS_VAL,  //35
   SCREEN_IMP_ENE_VAR_NEG_STR,  //36
   SCREEN_IMP_ENE_VAR_NEG_VAL,  //37
   SCREEN_ANGLE_R,              //54
   SCREEN_ANGLE_Y,              //55
   SCREEN_ANGLE_B,              //56
   0XFF,
  };
  const uint8_t SCREEN_SEQ_OLD_DATA[]=
  {
   0XFF,
  };
  
#endif
  
#ifdef MODEL_REX_2440
  const uint8_t SCREEN_SEQ_1P_MAIN[]=  
  {
   SCREEN_VOL_LN_R,             //3
   SCREEN_CUR_R,                //6
   SCREEN_FREQ,                 //11
   SCREEN_W_R,                  //14
   SCREEN_VA_R,                 //18
   SCREEN_PF_R,                 //26
   SCREEN_IMP_ENE_W_STR,        //30
   SCREEN_IMP_ENE_W_VAL,        //31
   SCREEN_IMP_ENE_VA_STR,       //32
   SCREEN_IMP_ENE_VA_VAL,       //33
   SCREEN_IMP_RHr_STR,          //38
   SCREEN_IMP_RHr_VAL,          //39
   SCREEN_IMP_LHr_STR,          //40
   SCREEN_IMP_LHr_VAL,          //41
   SCREEN_ANGLE_R,              //54
   SCREEN_INT_STR,              //57
   SCREEN_INT_VAL,              //58
   0XFF,
  };
  const uint8_t SCREEN_SEQ_3P_MAIN[]=  
  {
   SCREEN_VOL_LL_RY,            //0
   SCREEN_VOL_LL_YB,            //1
   SCREEN_VOL_LL_BR,            //2
   SCREEN_VOL_LN_R,             //3
   SCREEN_VOL_LN_Y,             //4
   SCREEN_VOL_LN_B,             //5
   SCREEN_CUR_R,                //6
   SCREEN_CUR_Y,                //7
   SCREEN_CUR_B,                //8
   SCREEN_NEU_CURR,             //9
   SCREEN_AVG_CUR,              //10
   SCREEN_FREQ,                 //11
   SCREEN_VOL_UNBAL,            //12
   SCREEN_CUR_UNBAL,            //13
   SCREEN_W_R,                  //14
   SCREEN_W_Y,                  //15
   SCREEN_W_B,                  //16
   SCREEN_SUM_W_3P,             //17
   SCREEN_VA_R,                 //18
   SCREEN_VA_Y,                 //19
   SCREEN_VA_B,                 //20
   SCREEN_SUM_VA_3P,            //21
   SCREEN_PF_R,                 //26
   SCREEN_PF_Y,                 //27
   SCREEN_PF_B,                 //28
   SCREEN_SUM_PF_3P,            //29
   SCREEN_IMP_ENE_W_STR,        //30
   SCREEN_IMP_ENE_W_VAL,        //31
   SCREEN_IMP_ENE_VA_STR,       //32
   SCREEN_IMP_ENE_VA_VAL,       //33
   SCREEN_IMP_RHr_STR,          //38
   SCREEN_IMP_RHr_VAL,          //39
   SCREEN_IMP_LHr_STR,          //40
   SCREEN_IMP_LHr_VAL,          //41
   SCREEN_ANGLE_R,              //54
   SCREEN_ANGLE_Y,              //55
   SCREEN_ANGLE_B,              //56
   SCREEN_INT_STR,              //57
   SCREEN_INT_VAL,              //58
   0XFF,
  };
  const uint8_t SCREEN_SEQ_OLD_DATA[]=
  {
    0XFF,
  };
#endif  

#ifdef MODEL_REX_2411
  const uint8_t SCREEN_SEQ_1P_MAIN[]=  
  {
   SCREEN_VOL_LN_R,             //3
   SCREEN_CUR_R,                //6
   SCREEN_FREQ,                 //11
   SCREEN_W_R,                  //14
   SCREEN_VA_R,                 //18
   SCREEN_PF_R,                 //26
   SCREEN_IMP_ENE_W_STR,        //30
   SCREEN_IMP_ENE_W_VAL,        //31
   SCREEN_IMP_RHr_STR,          //38
   SCREEN_IMP_RHr_VAL,          //39
   SCREEN_IMP_LHr_STR,          //40
   SCREEN_IMP_LHr_VAL,          //41
   SCREEN_ANGLE_R,              //54
   SCREEN_INT_STR,              //57
   SCREEN_INT_VAL,              //58
   0XFF,
  };
  const uint8_t SCREEN_SEQ_3P_MAIN[]=  
  {
   
   SCREEN_VOL_LL_RY,            //0
   SCREEN_VOL_LL_YB,            //1
   SCREEN_VOL_LL_BR,            //2
   SCREEN_VOL_LN_R,             //3
   SCREEN_VOL_LN_Y,             //4
   SCREEN_VOL_LN_B,             //5
   SCREEN_CUR_R,                //6
   SCREEN_CUR_Y,                //7
   SCREEN_CUR_B,                //8
   SCREEN_NEU_CURR,             //9
   SCREEN_AVG_CUR,              //10
   SCREEN_FREQ,                 //11
   SCREEN_VOL_UNBAL,            //12
   SCREEN_CUR_UNBAL,            //13
   SCREEN_W_R,                  //14
   SCREEN_W_Y,                  //15
   SCREEN_W_B,                  //16
   SCREEN_SUM_W_3P,             //17
   SCREEN_VA_R,                 //18
   SCREEN_VA_Y,                 //19
   SCREEN_VA_B,                 //20
   SCREEN_SUM_VA_3P,            //21
   SCREEN_PF_R,                 //26
   SCREEN_PF_Y,                 //27
   SCREEN_PF_B,                 //28
   SCREEN_SUM_PF_3P,            //29
   SCREEN_IMP_ENE_W_STR,        //30
   SCREEN_IMP_ENE_W_VAL,        //31
   SCREEN_IMP_RHr_STR,          //38
   SCREEN_IMP_RHr_VAL,          //39
   SCREEN_IMP_LHr_STR,          //40
   SCREEN_IMP_LHr_VAL,          //41
   SCREEN_ANGLE_R,              //54
   SCREEN_ANGLE_Y,              //55
   SCREEN_ANGLE_B,              //56
   SCREEN_INT_STR,              //57
   SCREEN_INT_VAL,              //58
   
   0XFF,
  };
  const uint8_t SCREEN_SEQ_OLD_DATA[]=
  {
   SCREEN_IMP_OLD_ENE_W_STR,    //42
   SCREEN_IMP_OLD_ENE_W_VAL,    //43
   SCREEN_IMP_OLD_RHr_STR,      //50
   SCREEN_IMP_OLD_RHr_VAL,      //51
   SCREEN_IMP_OLD_LHr_STR,      //52
   SCREEN_IMP_OLD_LHr_VAL,      //53
   SCREEN_OLD_INT_STR,          //59
   SCREEN_OLD_INT_VAL,          //60
    0XFF,
  };
  
 
#endif  

#ifdef MODEL_REX_2550
  const uint8_t SCREEN_SEQ_1P_MAIN[]=  
  {
   SCREEN_VOL_LN_R,             //3
   SCREEN_CUR_R,                //6
   SCREEN_FREQ,                 //11
   SCREEN_W_R,                  //14
   SCREEN_VA_R,                 //18
   SCREEN_VAR_R,                //22
   SCREEN_PF_R,                 //26
   SCREEN_IMP_ENE_W_STR,        //30
   SCREEN_IMP_ENE_W_VAL,        //31
   SCREEN_IMP_ENE_VA_STR,       //32
   SCREEN_IMP_ENE_VA_VAL,       //33
   SCREEN_IMP_ENE_VAR_POS_STR,  //34
   SCREEN_IMP_ENE_VAR_POS_VAL,  //35
   SCREEN_IMP_ENE_VAR_NEG_STR,  //36
   SCREEN_IMP_ENE_VAR_NEG_VAL,  //37
   SCREEN_IMP_RHr_STR,          //38
   SCREEN_IMP_RHr_VAL,          //39
   SCREEN_IMP_LHr_STR,          //40
   SCREEN_IMP_LHr_VAL,          //41
   SCREEN_ANGLE_R,              //54
   SCREEN_INT_STR,              //57
   SCREEN_INT_VAL,              //58
   0XFF,
  };
  const uint8_t SCREEN_SEQ_3P_MAIN[]=  
  {
   
   SCREEN_VOL_LL_RY,            //0
   SCREEN_VOL_LL_YB,            //1
   SCREEN_VOL_LL_BR,            //2
   SCREEN_VOL_LN_R,             //3
   SCREEN_VOL_LN_Y,             //4
   SCREEN_VOL_LN_B,             //5
   SCREEN_CUR_R,                //6
   SCREEN_CUR_Y,                //7
   SCREEN_CUR_B,                //8
   SCREEN_NEU_CURR,             //9
   SCREEN_AVG_CUR,              //10
   SCREEN_FREQ,                 //11
   SCREEN_VOL_UNBAL,            //12
   SCREEN_CUR_UNBAL,            //13
   SCREEN_W_R,                  //14
   SCREEN_W_Y,                  //15
   SCREEN_W_B,                  //16
   SCREEN_SUM_W_3P,             //17
   SCREEN_VA_R,                 //18
   SCREEN_VA_Y,                 //19
   SCREEN_VA_B,                 //20
   SCREEN_SUM_VA_3P,            //21
   SCREEN_VAR_R,                //22
   SCREEN_VAR_Y,                //23
   SCREEN_VAR_B,                //24
   SCREEN_SUM_VAR_3P,           //25
   SCREEN_PF_R,                 //26
   SCREEN_PF_Y,                 //27
   SCREEN_PF_B,                 //28
   SCREEN_SUM_PF_3P,            //29
   SCREEN_IMP_ENE_W_STR,        //30
   SCREEN_IMP_ENE_W_VAL,        //31
   SCREEN_IMP_ENE_VA_STR,       //32
   SCREEN_IMP_ENE_VA_VAL,       //33
   SCREEN_IMP_ENE_VAR_POS_STR,  //34
   SCREEN_IMP_ENE_VAR_POS_VAL,  //35
   SCREEN_IMP_ENE_VAR_NEG_STR,  //36
   SCREEN_IMP_ENE_VAR_NEG_VAL,  //37
   SCREEN_IMP_RHr_STR,          //38
   SCREEN_IMP_RHr_VAL,          //39
   SCREEN_IMP_LHr_STR,          //40
   SCREEN_IMP_LHr_VAL,          //41
   SCREEN_ANGLE_R,              //54
   SCREEN_ANGLE_Y,              //55
   SCREEN_ANGLE_B,              //56
   SCREEN_INT_STR,              //57
   SCREEN_INT_VAL,              //58
   
   0XFF,
  };
  const uint8_t SCREEN_SEQ_OLD_DATA[]=
  {
   0XFF,
  };
  
 
#endif  

#ifdef MODEL_REX_2551
  const uint8_t SCREEN_SEQ_1P_MAIN[]=  
  {
   SCREEN_VOL_LN_R,             //3
   SCREEN_CUR_R,                //6
   SCREEN_FREQ,                 //11
   SCREEN_W_R,                  //14
   SCREEN_VA_R,                 //18
   SCREEN_VAR_R,                //22
   SCREEN_PF_R,                 //26
   SCREEN_IMP_ENE_W_STR,        //30
   SCREEN_IMP_ENE_W_VAL,        //31
   SCREEN_IMP_ENE_VA_STR,       //32
   SCREEN_IMP_ENE_VA_VAL,       //33
   SCREEN_IMP_ENE_VAR_POS_STR,  //34
   SCREEN_IMP_ENE_VAR_POS_VAL,  //35
   SCREEN_IMP_ENE_VAR_NEG_STR,  //36
   SCREEN_IMP_ENE_VAR_NEG_VAL,  //37
   SCREEN_IMP_RHr_STR,          //38
   SCREEN_IMP_RHr_VAL,          //39
   SCREEN_IMP_LHr_STR,          //40
   SCREEN_IMP_LHr_VAL,          //41
   SCREEN_ANGLE_R,              //54
   SCREEN_INT_STR,              //57
   SCREEN_INT_VAL,              //58
   0XFF,
  };
  const uint8_t SCREEN_SEQ_3P_MAIN[]=  
  {
   SCREEN_VOL_LL_RY,            //0
   SCREEN_VOL_LL_YB,            //1
   SCREEN_VOL_LL_BR,            //2
   SCREEN_VOL_LN_R,             //3
   SCREEN_VOL_LN_Y,             //4
   SCREEN_VOL_LN_B,             //5
   SCREEN_CUR_R,                //6
   SCREEN_CUR_Y,                //7
   SCREEN_CUR_B,                //8
   SCREEN_NEU_CURR,             //9
   SCREEN_AVG_CUR,              //10
   SCREEN_FREQ,                 //11
   SCREEN_VOL_UNBAL,            //12
   SCREEN_CUR_UNBAL,            //13
   SCREEN_W_R,                  //14
   SCREEN_W_Y,                  //15
   SCREEN_W_B,                  //16
   SCREEN_SUM_W_3P,             //17
   SCREEN_VA_R,                 //18
   SCREEN_VA_Y,                 //19
   SCREEN_VA_B,                 //20
   SCREEN_SUM_VA_3P,            //21
   SCREEN_VAR_R,                //22
   SCREEN_VAR_Y,                //23
   SCREEN_VAR_B,                //24
   SCREEN_SUM_VAR_3P,           //25
   SCREEN_PF_R,                 //26
   SCREEN_PF_Y,                 //27
   SCREEN_PF_B,                 //28
   SCREEN_SUM_PF_3P,            //29
   SCREEN_IMP_ENE_W_STR,        //30
   SCREEN_IMP_ENE_W_VAL,        //31
   SCREEN_IMP_ENE_VA_STR,       //32
   SCREEN_IMP_ENE_VA_VAL,       //33
   SCREEN_IMP_ENE_VAR_POS_STR,  //34
   SCREEN_IMP_ENE_VAR_POS_VAL,  //35
   SCREEN_IMP_ENE_VAR_NEG_STR,  //36
   SCREEN_IMP_ENE_VAR_NEG_VAL,  //37
   SCREEN_IMP_RHr_STR,          //38
   SCREEN_IMP_RHr_VAL,          //39
   SCREEN_IMP_LHr_STR,          //40
   SCREEN_IMP_LHr_VAL,          //41
   SCREEN_ANGLE_R,              //54
   SCREEN_ANGLE_Y,              //55
   SCREEN_ANGLE_B,              //56
   SCREEN_INT_STR,              //57
   SCREEN_INT_VAL,              //58
   0XFF,
  };
  const uint8_t SCREEN_SEQ_OLD_DATA[]=
  {
   SCREEN_IMP_OLD_ENE_W_STR,    //42
   SCREEN_IMP_OLD_ENE_W_VAL,    //43
   SCREEN_IMP_OLD_ENE_VA_STR,   //44
   SCREEN_IMP_OLD_ENE_VA_VAL,   //45
   SCREEN_IMP_OLD_ENE_VAR_POS_STR,//46
   SCREEN_IMP_OLD_ENE_VAR_POS_VAL,//47
   SCREEN_IMP_OLD_ENE_VAR_NEG_STR,//48
   SCREEN_IMP_OLD_ENE_VAR_NEG_VAL,//49
   SCREEN_IMP_OLD_RHr_STR,      //50
   SCREEN_IMP_OLD_RHr_VAL,      //51
   SCREEN_IMP_OLD_LHr_STR,      //52
   SCREEN_IMP_OLD_LHr_VAL,      //53
   SCREEN_OLD_INT_STR,          //59
   SCREEN_OLD_INT_VAL,          //60
    0XFF,
  };
  
 
#endif  
  
  
    