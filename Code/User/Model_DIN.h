#define MODEL_RELEASED

#define MODEL_COMM_PROCOM


//#define MODEL_DUAL_ENERGY

// The models are defined here. This is done for easy display configuration. 


//#define MODEL_REX_1100
//#define MODEL_REX_1200
//#define MODEL_REX_1300
//#define MODEL_REX_1400
//#define MODEL_REX_1500
//#define MODEL_REX_1900
//#define MODEL_REX_2140
//#define MODEL_REX_2330
//#define MODEL_REX_2440
//#define MODEL_REX_2411
//#define MODEL_REX_2550
#define MODEL_REX_2551


#ifdef MODEL_COMM_PROCOM
     #define MODBUS_MAP_PROCOM
     #define MODEL_COMM_SETTING
     #define MODEL_RS485
     #define MODBUS_EDIT_PARA
#endif


#if ((defined MODEL_REX_1100) || (defined MODEL_REX_1200) || (defined MODEL_REX_1300) || (defined MODEL_REX_1400) || (defined MODEL_REX_1500) ||\
    (defined MODEL_REX_1900) || (defined MODEL_REX_2140) || (defined MODEL_REX_2330) || (defined MODEL_REX_2440) || (defined MODEL_REX_2550) )                                                                                                          // import only model
    #define MODEL_DATA_SAVE  
    #define MODEL_IMPORT_ONLY
#endif

#if ((defined MODEL_REX_2411) || (defined MODEL_REX_2551))                     // import only model
    #define MODEL_DATA_SAVE  
    #define MODEL_IMPORT_ONLY
    #define MODEL_DIS_OLD_DATA   
#endif

// UNDONE:
// 1) We aren't measuring 5V (We have provision to measure 3V)
// We should measure 3.3V or 5V so that we can save data in EEPROM as soon as power goes away to avoid losing data
// 2) Check the resistor values for LEDs
// 3) Need LEDs for indicating whether we are displaying Mains or Solar Current/Voltages
// 4) Forgot about protection bit
