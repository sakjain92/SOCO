#define  DISPLAY_PAGE1  0xB0
#define  DISPLAY_PAGE2  0xB1
#define  DISPLAY_PAGE3  0xB2
#define  DISPLAY_PAGE4  0xB3
#define  DISPLAY_PAGE5  0xB4
#define  DISPLAY_PAGE6  0xB5
#define  DISPLAY_PAGE7  0xB6
#define  DISPLAY_PAGE8  0xB7


#define     PARA_PT_PRIMARY             0
#define     PARA_PT_PRIMARY_DEC         1
#define     PARA_PT_PRI_UNIT            2
#define     PARA_PT_SECONDARY           3
#define     PARA_CT_PRIMARY             4
#define     PARA_CT_PRIMARY_DEC         5
#define     PARA_CT_PRI_UNIT            6
#define     PARA_CT_SECONDARY           7
#define     PARA_SYSTEM_CONFIG          8
#define     PARA_KVA_TYPE               9
#define     PARA_STARTING_CURRENT      10 
#define     PARA_DISP_RESO             11
#define     PARA_ENE_TYPE              12
#define     PARA_DEVICE_ID             13
#define     PARA_BAUD_RATE             14
#define     PARA_PARITY                15
#define     PARA_STOP_BIT              16
#define     PARA_ENDIAN                17







#define     SYSTEM_CONFIG_3P4W         0
#define     SYSTEM_CONFIG_1P           1
#define     SYSTEM_CONFIG_3P3W         2



/*************************************************************************
 if decimal is >10 , its points to array of strings to selct from
Than the value indicates that the paramter value has to be displayed in a
string as defined below
These all shall be array of strings. The array shall end with a null string
************************************************************************/

#define EDIT_DEC_ENERGY_TYPE          11
#define EDIT_DEC_KVA_TYPE             12
#define EDIT_DEC_SYSTEM_CONFIG        13
#define EDIT_DEC_BAUD_RATE            14
#define EDIT_DEC_PARITY               15
#define EDIT_DEC_STOP_BIT             16
#define EDIT_ENDIAN_TYPE              17






/***********************************************************************
DisableType

***********************************************************************/
#define EDITDIS_NEVER_DISABLED                  0
#define EDITDIS_SKIP_ALWAYS                   0XFF

