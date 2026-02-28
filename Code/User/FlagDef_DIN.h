#ifndef FLAGDEF_DIN_H
#define FLAGDEF_DIN_H

#include "stm32f37x.h"
#include <stdio.h>
#include "Parameter_DIN.h"
#include "Model_DIN.h"




#define RESET_WATCH_DOG   (IWDG->KR=0xAAAA)

/**************** Defined in icf file
 
define region CoeffDataLoc = mem:[from 0x0800FF00 to 0x0800FFFF];  
**************************************/
#define     NO_OF_SAMPLES                3200

#define     EEPROM_PAGE_LENGTH          64
#define     EXT_EEPROM                  0xA0
#define     MAX_NUM_PAGES               512

#define  MAX_DATA_SAVE_SIZE             512
#define  NUM_DATA_PAGES                 40
#define  DATA_SAVE_START_LOC            0
#define  DATA_SAVE_END_LOC              (MAX_DATA_SAVE_SIZE * NUM_DATA_PAGES - 1)
#define  PROGRAM_DATA_LOC1_START        (DATA_SAVE_END_LOC + 1)
#define  PROGRAM_DATA_LOC2_START        (PROGRAM_DATA_LOC1_START+256)
#define  OLD_DATA_LOC                   (PROGRAM_DATA_LOC2_START+256)

#define  POWER_DN_SAVE_PAGE            (OLD_DATA_LOC+MAX_DATA_SAVE_SIZE)

#define  PASSWORD_SAV_LOC              (POWER_DN_SAVE_PAGE+MAX_DATA_SAVE_SIZE)
#define  SCROLL_LOCK_LOC               (PASSWORD_SAV_LOC+EEPROM_PAGE_LENGTH)

#define  DATA_SAVE_DEBAR_TIME          5
#define  DATA_SAVE_DURATION           (30+DATA_SAVE_DEBAR_TIME) // External EEPROM Data Saved every 30 Sec

#define PORT_BIT_0       0X1
#define PORT_BIT_1       0X2
#define PORT_BIT_2       0X4
#define PORT_BIT_3       0X8
#define PORT_BIT_4       0X10
#define PORT_BIT_5       0X20
#define PORT_BIT_6       0X40
#define PORT_BIT_7       0X80
#define PORT_BIT_8       0X100
#define PORT_BIT_9       0X200
#define PORT_BIT_10      0X400
#define PORT_BIT_11      0X800
#define PORT_BIT_12      0X1000
#define PORT_BIT_13      0X2000
#define PORT_BIT_14      0X4000
#define PORT_BIT_15      0X8000

// Last 256 bytes left for Calibration Constants
// definations for keys/switches 
#define     KEY_NEXT         0X01
#define     KEY_DEC          0X02
#define     KEY_INC          0X04

#define     KEY_BACK        KEY_DEC

#define     KEY_EDIT         (KEY_INC+KEY_DEC)

#define     KEY_DIR_CAL      (KEY_DEC+KEY_NEXT)
#define     KEY_CT_CHK       (KEY_INC+KEY_NEXT)

// definations for InterruptFlag

#define  INT_CYCLE_OVER                 0x01
#define  INT_Y_NEW_FREQ_MEASURE         0x02
#define  INT_B_NEW_FREQ_MEASURE         0x04
#define  INT_R_PHASE_REV                0X08
#define  INT_Y_PHASE_REV                0X10
#define  INT_B_PHASE_REV                0X20
#define  PASSWORD_VERIFIED              0x40
#define  PASSWORD_FOR_VIEW              0x80
#define  PASSWORD_CHECK_ON              0x200
#define  INT_DATA_SAVING_EEPROM         0x400
#define  INT_POWER_OK                   0x800
#define  INT_R_SOLAR_PHASE_REV          0X1000
#define  INT_Y_SOLAR_PHASE_REV          0X2000
#define  INT_B_SOLAR_PHASE_REV          0X4000



#define MAX_PARAM_LIMIT  17
#define POWER_FAIL_SENSE_VALUE    1241



// Definations for Ports


#define     DIS_DATA_BIT        0X01

#define     SR_CLOCK_BIT        0X02

#define     OUTREG_CLOCK_BIT    0X04


#define     DISP_SR_ON          (GPIOA->BRR  = 0X04)
#define     DISP_SR_OFF         (GPIOA->BSRR = 0X04)

#define     DIS_DATA_OUT_LOW            (GPIOA->BRR  = DIS_DATA_BIT)
#define     DIS_DATA_OUT_HIGH           (GPIOA->BSRR = DIS_DATA_BIT)
#define     SHIFT_REGISTER_CLOCK_LOW    (GPIOA->BSRR = SR_CLOCK_BIT)
#define     SHIFT_REGISTER_CLOCK_HIGH   (GPIOA->BRR  = SR_CLOCK_BIT)
#define     OUTPUT_REGISTER_CLOCK_LOW   (GPIOF->BSRR = OUTREG_CLOCK_BIT)
#define     OUTPUT_REGISTER_CLOCK_HIGH  (GPIOF->BRR  = OUTREG_CLOCK_BIT)



#define     DISP_COM1_ENB       (GPIOF->BRR  = PORT_BIT_9)
#define     DISP_COM1_DIS       (GPIOF->BSRR = PORT_BIT_9)
#define     DISP_COM2_ENB       (GPIOB->BRR  = PORT_BIT_7)
#define     DISP_COM2_DIS       (GPIOB->BSRR = PORT_BIT_7)
#define     DISP_COM3_ENB       (GPIOB->BRR  = PORT_BIT_6)
#define     DISP_COM3_DIS       (GPIOB->BSRR = PORT_BIT_6)
#define     DISP_COM4_ENB       (GPIOC->BRR  = PORT_BIT_12)
#define     DISP_COM4_DIS       (GPIOC->BSRR = PORT_BIT_12)
#define     DISP_COM5_ENB       (GPIOA->BRR  = PORT_BIT_15)
#define     DISP_COM5_DIS       (GPIOA->BSRR = PORT_BIT_15)


#define     MAX_DIS_PARAMETER        18
#define     METER_INIT_VALUE         135

// According to IEC standard for class 1 meter:
//
// Voltage accuracy should be within 1% from 90% to 110% of nominal voltage
// (230V or 240V in this case)
// Current should start register from 0.4% of Ib (Let's say Ib = 10A) = 0.04A
// Current should have 1.5% accuracy between 5% of Ib to 10% of Ib (between 0.5A-1A)
// Above 10% Ib to Imax (100A in this case), the accuracy needs to be 1% (Between 1A-100A)
// Let's aim for 1% accuracy between 1-100A. Will aim for rest of the range later.
// An equivalent schenider model: https://download.schneider-electric.com/files?p_Doc_Ref=BRU19338&p_enDocType=Instruction+sheet
//

#define   VOLTAGE_COEFF            2.75479E-04f
#define   PH_VOLTAGE_COEFF         (VOLTAGE_COEFF)
#define   FUND_VOL_COEFF           3.4435E-04f

#define   CURRENT_COEFF            1.0115E-04f
#define   NEU_CURRENT_COEFF        (CURRENT_COEFF)

#define   FUND_CURRENT_COEFF       1.2644E-04f


#define   POWER_COEFF_3P4W         2.786537E-08f

#define  FUND_POWER_COEFF          4.3540E-08f

#define  FAN_CURRENT_COEFF         5.1790E-06f

#define  AMBIENT_TEMP_COEFF        2.929687E-05f

// Minimum voltage to maintain 0.5% accuracy is about 33VAC. Taking some margin
// UNDONE: Figure out the correct minimum value for Currents as per metering
// standard for 0.5 class metering.
// Currently using ADC (not SDADC) gives a ~0.08 current without any input attached
// UNDONE: Try and keep MIN_TOTAL_CUR_LIMIT as small as possible
// Investigate why when current is < MIN_TOTAL_CUR_LIMIT then high fluctuations
// are noticed (we start using Fundamental frequency based measurement only
// but that should be more stable but it turns out to be lesser stable. 
// Seems like a bug in implementation logic likely)
// Also, at lower current we are getting higher error in power factor
// (Need to investigate this. Is this cause of noise?)
//
#define   MIN_VOL_LIMIT            70.0f
#define   MIN_VOL_LIMIT_PH_PH      (MIN_VOL_LIMIT * 1.732f)
#define   MIN_TOTAL_CUR_LIMIT      0.8f
#define   MIN_NEU_CUR_LIMIT        0.1f
#define   MIN_CURRENT_LIMIT        0.1f

#define        T_20MS                                 64  // 20 msec delay
#define        T_5MS                                  10  // 20 msec delay
#define        SW_1SEC                                12 // 250 msec

#define        SW_OVF_LIMIT             10 
#define        SW_OVF_LIMIT2            20
#define        SW_OVF_LIMIT3            40
 
#define        SW_STEP_VALUE            5
#define        SW_STEP_VALUE2           15
#define        SW_STEP_VALUE3           30


// definations for sw_flag 

#define     EDIT_MODE_FLAG        01
#define     SW_NEW_VALUE_FLAG     02
#define     SW_MAX_FLAG           04


#define FF_MEAS_OVER            0x01
#define FF_HIGH_DETECTED        0x02



#define         MAX_ENERGY_SET_Wh       0
#define         MAX_ENERGY_SET_KWh      1
#define         MAX_ENERGY_SET_MWh      2
#define         MAX_ENERGY_SET_GWh      3



#define         CALIBRATE_OUT_1         1
#define         CALIBRATE_OUT_2         2
#define         CALIBRATE_OUT_3         3
#define         CALIBRATE_OUT_4         4
#define         CALIBRATE_OUT_5         5

#define         CALIBRATE_IN_START      6
#define         CALIBRATE_IN_1          7
#define         CALIBRATE_IN_2          8
#define         CALIBRATE_IN_3          9
#define         CALIBRATE_IN_4          10
#define         CALIBRATE_IN_5          11
#define         CALIBRATE_IN_6          12
#define         CALIBRATE_IN_7          13
#define         CALIBRATE_IN_8          14

#define         CALIBRATE_VOL_CUR_START 15
#define         CALIBRATE_DIS_H_VI      16
#define         CALIBRATE_H_VI          17
#define         CALIBRATE_DIS_H_PF      18
#define         CALIBRATE_H_PF          19

#define         CALIBRATE_DIS_M_VI      20
#define         CALIBRATE_M_VI          21
#define         CALIBRATE_DIS_M_PF      22
#define         CALIBRATE_M_PF          23

#define         CALIBRATE_DIS_L_VI      24
#define         CALIBRATE_L_VI          25
#define         CALIBRATE_DIS_L_PF      26
#define         CALIBRATE_L_PF          27

#define         CALIBRATE_END           28

// The ratio between CUR_HIGH_CAL_POINT/CUR_MID_CAL_POINT should be 5:1
// The ratio between CUR_MID_CAL_POINT/CUR_LOW_CAL_POINT should be 10:1
// UNDONE: Calibrating at 0.4 as below that in current hardware we are getting too much noise
// Note: The CUR_LOW_CAL_POINT determines the Least count for Phase Error Correction
// so keep it as small as possible
//
#define        CUR_HIGH_CAL_POINT                    20.0
#define        CUR_MID_CAL_POINT                     4.0
#define        CUR_LOW_CAL_POINT                     0.4
#define        NO_OF_CAL_ACCUMULATION_VI             4
#define        NO_OF_CAL_ACCUMULATION_POW            10
#define        CAL_ACC_DELAY                         8

#define        CAL_VOLTAGE_SETTING_HIGH              240.0f
#define        CAL_CURRENT_SETTING_HIGH              CUR_HIGH_CAL_POINT
#define        CAL_CURRENT_SETTING_MID               CUR_MID_CAL_POINT
#define        CAL_CURRENT_SETTING_LOW               CUR_LOW_CAL_POINT
#define        CURRENT_TOLRERANCE                    0.2f   // UNDONE: This was 0.1 earlier but in our board at lower end of calibration needed to increase error range
                                                            // We should use SDADC to get better accuracy
#define        VOLTAGE_TOLERANCE                     0.1f
#define        POWER_TOLERANCE                       0.4f 

#define        CAL_FAN_CUR_SETTING                   0.1f
#define        FAN_CUR_TOLERANCE                     0.2f

#define         CAL_UPF_POWER_SETTING_HIGH     ( NO_OF_CAL_ACCUMULATION_POW*CAL_VOLTAGE_SETTING_HIGH*CAL_CURRENT_SETTING_HIGH)
#define         CAL_PF_POWER_SETTING_HIGH      (CAL_UPF_POWER_SETTING_HIGH/2)

#define         CAL_UPF_POWER_SETTING_MID     ( NO_OF_CAL_ACCUMULATION_POW*CAL_VOLTAGE_SETTING_HIGH*CAL_CURRENT_SETTING_MID)
#define         CAL_PF_POWER_SETTING_MID      ( CAL_UPF_POWER_SETTING_MID/2)

#define         CAL_UPF_POWER_SETTING_LOW     ( NO_OF_CAL_ACCUMULATION_POW*CAL_VOLTAGE_SETTING_HIGH*CAL_CURRENT_SETTING_LOW)
#define         CAL_PF_POWER_SETTING_LOW      ( CAL_UPF_POWER_SETTING_LOW/2)

#define         VOLTAGE_HIGHER_LIMIT          (CAL_VOLTAGE_SETTING_HIGH*(1+VOLTAGE_TOLERANCE))
#define         VOLTAGE_LOWER_LIMIT           (CAL_VOLTAGE_SETTING_HIGH*(1-VOLTAGE_TOLERANCE))

#define         I_H_UPPER_LIMIT               (CAL_CURRENT_SETTING_HIGH*(1+CURRENT_TOLRERANCE))
#define         I_H_LOWER_LIMIT               (CAL_CURRENT_SETTING_HIGH*(1-CURRENT_TOLRERANCE))

#define         I_M_UPPER_LIMIT               (CAL_CURRENT_SETTING_MID*(1+CURRENT_TOLRERANCE))
#define         I_M_LOWER_LIMIT               (CAL_CURRENT_SETTING_MID*(1-CURRENT_TOLRERANCE))

#define         I_L_UPPER_LIMIT                (CAL_CURRENT_SETTING_LOW*(1+CURRENT_TOLRERANCE))
#define         I_L_LOWER_LIMIT               (CAL_CURRENT_SETTING_LOW*(1-CURRENT_TOLRERANCE))

#define         UPF_POWER_H_UPPER_LIMIT        (CAL_VOLTAGE_SETTING_HIGH*CAL_CURRENT_SETTING_HIGH*(1+POWER_TOLERANCE))
#define         UPF_POWER_H_LOWER_LIMIT        (CAL_VOLTAGE_SETTING_HIGH*CAL_CURRENT_SETTING_HIGH*(1-POWER_TOLERANCE))
#define         PF_POWER_H_UPPER_LIMIT         (UPF_POWER_H_UPPER_LIMIT*0.5)
#define         PF_POWER_H_LOWER_LIMIT         (UPF_POWER_H_LOWER_LIMIT *0.5)

#define         UPF_POWER_M_UPPER_LIMIT        (CAL_VOLTAGE_SETTING_HIGH*CAL_CURRENT_SETTING_MID*(1+POWER_TOLERANCE))
#define         UPF_POWER_M_LOWER_LIMIT        (CAL_VOLTAGE_SETTING_HIGH*CAL_CURRENT_SETTING_MID*(1-POWER_TOLERANCE))
#define         PF_POWER_M_UPPER_LIMIT         (UPF_POWER_M_UPPER_LIMIT*0.5)
#define         PF_POWER_M_LOWER_LIMIT         (UPF_POWER_M_LOWER_LIMIT *0.5)

#define         UPF_POWER_L_UPPER_LIMIT        (CAL_VOLTAGE_SETTING_HIGH*CAL_CURRENT_SETTING_LOW*(1+POWER_TOLERANCE))
#define         UPF_POWER_L_LOWER_LIMIT        (CAL_VOLTAGE_SETTING_HIGH*CAL_CURRENT_SETTING_LOW*(1-POWER_TOLERANCE))
#define         PF_POWER_L_UPPER_LIMIT         (UPF_POWER_L_UPPER_LIMIT*0.5)
#define         PF_POWER_L_LOWER_LIMIT         (UPF_POWER_L_LOWER_LIMIT *0.5)

#define         FAN_CUR_HIGHER_LIMIT           (CAL_FAN_CUR_SETTING*(1+FAN_CUR_TOLERANCE))
#define         FAN_CUR_LOWER_LIMIT            (CAL_FAN_CUR_SETTING*(1-FAN_CUR_TOLERANCE))

#define         PROTECTION_BIT_LOW             TO_BOOL(!(GPIOC->IDR & 0X200))
#define         PROTECTION_BIT_HIGH            TO_BOOL(GPIOC->IDR & 0X200)
//OneSecFlag 
#define         ONE_SEC_FLAG_1SEC_OVER      0x01



#define   WH_PULSE_VALUE  2e6
#define   ROW_TOP     0
//#define   ROW_MID     1
//#define   ROW_BOT     2


#define   DECIMAL_DIGIT_2    1
#define   DECIMAL_DIGIT_3    2
#define   DECIMAL_DIGIT_4    3
#define   DECIMAL_DIGIT_5    4
#define   DECIMAL_DIGIT_6    5



#define DIGIT_1    0
#define DIGIT_2    1
#define DIGIT_3    2
#define DIGIT_4    3
#define DIGIT_5    4
#define DIGIT_6    4

#define LED_OFF         0
#define LED_GRID_NONE   1
#define LED_GRID_R      2
#define LED_GRID_Y      3
#define LED_GRID_B      4
#define LED_GRID_TOTAL  5
#define LED_SOLAR_NONE  6
#define LED_SOLAR_R     7
#define LED_SOLAR_Y     8
#define LED_SOLAR_B     9
#define LED_SOLAR_TOTAL 10

#define SWITCH_OFF_LED2_R               GPIOF->BSRR = PORT_BIT_10
#define SWITCH_ON_LED2_R                GPIOF->BRR  = PORT_BIT_10

#define SWITCH_OFF_LED2_G               GPIOC->BSRR = PORT_BIT_13
#define SWITCH_ON_LED2_G                GPIOC->BRR  = PORT_BIT_13

#define SWITCH_OFF_LED2_B               GPIOE->BSRR = PORT_BIT_6 
#define SWITCH_ON_LED2_B                GPIOE->BRR  = PORT_BIT_6

#define SWITCH_OFF_LED_COMM             GPIOE->BSRR = PORT_BIT_5
#define SWITCH_ON_LED_COMM              GPIOE->BRR  = PORT_BIT_5

#define SWITCH_OFF_LED_GRID_DISPLAY     GPIOE->BSRR = PORT_BIT_4
#define SWITCH_ON_LED_GRID_DISPLAY      GPIOE->BRR = PORT_BIT_4

#define SWITCH_OFF_LED_SOLAR_DISPLAY    GPIOE->BSRR = PORT_BIT_3
#define SWITCH_ON_LED_SOLAR_DISPLAY     GPIOE->BRR = PORT_BIT_3

#define SWITCH_OFF_LED_GRID_HEALTHY     GPIOE->BSRR = PORT_BIT_2
#define SWITCH_ON_LED_GRID_HEALTHY      GPIOE->BRR = PORT_BIT_2
 
#define SWITCH_OFF_LED_SOLAR_HEALTHY    GPIOE->BSRR = PORT_BIT_1
#define SWITCH_ON_LED_SOLAR_HEALTHY     GPIOE->BRR = PORT_BIT_1

#define INPUT_KEY_INC                   (!(GPIOB->IDR & PORT_BIT_9))
#define INPUT_KEY_NEXT                  (!(GPIOB->IDR & PORT_BIT_8))
#define INPUT_KEY_DEC                   (!(GPIOF->IDR & PORT_BIT_4))

// All contactors (except "Load on Solar") are connected to the NC of the relays
// Relays are turned on by driving GPIO Pins high
//
#define TURN_RELAY1_ON                              GPIOD->BSRR = PORT_BIT_6
#define TURN_RELAY1_OFF                             GPIOD->BRR = PORT_BIT_6

#define TURN_RELAY2_ON                              GPIOD->BSRR = PORT_BIT_7
#define TURN_RELAY2_OFF                             GPIOD->BRR = PORT_BIT_7

#define TURN_RELAY3_ON                              GPIOD->BSRR = PORT_BIT_4
#define TURN_RELAY3_OFF                             GPIOD->BRR = PORT_BIT_4

#define TURN_RELAY4_ON                              GPIOE->BSRR = PORT_BIT_0
#define TURN_RELAY4_OFF                             GPIOE->BRR = PORT_BIT_0

#define TURN_RELAY5_ON                              GPIOD->BSRR = PORT_BIT_5
#define TURN_RELAY5_OFF                             GPIOD->BRR = PORT_BIT_5

#define TURN_RELAY6_ON                              GPIOD->BSRR = PORT_BIT_3
#define TURN_RELAY6_OFF                             GPIOD->BRR = PORT_BIT_3


#define SWITCH_OFF_CONTACTOR_R_PHASE_GRID_HEALTHY  TURN_RELAY1_ON
#define SWITCH_ON_CONTACTOR_R_PHASE_GRID_HEALTHY   TURN_RELAY1_OFF

#define SWITCH_OFF_CONTACTOR_Y_PHASE_GRID_HEALTHY  TURN_RELAY2_ON
#define SWITCH_ON_CONTACTOR_Y_PHASE_GRID_HEALTHY   TURN_RELAY2_OFF

#define SWITCH_OFF_CONTACTOR_B_PHASE_GRID_HEALTHY  TURN_RELAY3_ON
#define SWITCH_ON_CONTACTOR_B_PHASE_GRID_HEALTHY   TURN_RELAY3_OFF

#define SWITCH_OFF_CONTACTOR_LOAD_ON_SOLAR         TURN_RELAY4_OFF
#define SWITCH_ON_CONTACTOR_LOAD_ON_SOLAR          TURN_RELAY4_ON

#define SWITCH_OFF_CONTACTOR_LOAD_ON_GRID          TURN_RELAY5_ON
#define SWITCH_ON_CONTACTOR_LOAD_ON_GRID           TURN_RELAY5_OFF

#define SWITCH_OFF_FANS                            TURN_RELAY6_ON
#define SWITCH_ON_FANS                             TURN_RELAY6_OFF

// Inputs are Active Low
// Inputs from contactors are NO when they are open
// Input from SPD is NO when it's healthy
// Input from 48V is NC when it's healthy
// Input from DG is NO when it's running
//
#define INPUT_R_PHASE_GRID_HEALTHY_CONTACTOR_ON    TO_BOOL(!(GPIOC->IDR & PORT_BIT_6))
#define INPUT_Y_PHASE_GRID_HEALTHY_CONTACTOR_ON    TO_BOOL(!(GPIOC->IDR & PORT_BIT_7))
#define INPUT_B_PHASE_GRID_HEALTHY_CONTACTOR_ON    TO_BOOL(!(GPIOC->IDR & PORT_BIT_8))
#define INPUT_LOAD_ON_SOLAR_CONTACTOR_ON           TO_BOOL(!(GPIOA->IDR & PORT_BIT_8))
#define INPUT_LOAD_ON_GRID_CONTACTOR_ON            TO_BOOL(!(GPIOF->IDR & PORT_BIT_6))
#define INPUT_SOLAR_ISOLATOR_ON                    TO_BOOL(!(GPIOD->IDR & PORT_BIT_0))
#define INPUT_GRID_MCB_ON                          TO_BOOL(!(GPIOD->IDR & PORT_BIT_1))
#define INPUT_DG_RUNNING                           TO_BOOL(!(GPIOD->IDR & PORT_BIT_2))

#define PARA_ONLY_DATA  1
#define PARA_WITH_DEC   2
#define PARA_WITH_UNIT  3

#define DISPLAY_SCROLL_ON    0
#define DISPLAY_SCROLL_OFF   1

#define ENERGY_OVER_FLOW_LIMIT 999999.999999999 

#define FILT_600_COEFF_X1   0.18669433f
#define FILT_600_COEFF_X2   0.37338867f
#define FILT_600_COEFF_Y1   0.46293803f
#define FILT_600_COEFF_Y2   -0.20971536f


#define RS485Receive (GPIOB->BRR  =  PORT_BIT_5)
#define RS485Transmit (GPIOB->BSRR =  PORT_BIT_5)
#define RS485TransmitOn ( GPIOB->ODR & PORT_BIT_5)

#define BaudRate_1200   0
#define BaudRate_2400   1
#define BaudRate_4800   2
#define BaudRate_9600   3
#define BaudRate_19200  4

#define Parity_Odd      2
#define Parity_Even     1
#define Parity_None     0

#define Stopbit_one     0
#define Stopbit_two     1

#define   ADC_DC_PWR            0
#define   ADC_AC_PWR            1
#define   ADC_FAN_1             2
#define   ADC_FAN_2             3
#define   ADC_A_TEMP            4

#define    ADC_IR               0
#define    ADC_VR               1
#define    ADC_IY               2
#define    ADC_VY               3
#define    ADC_IB               4
#define    ADC_VB               5
#define    ADC_IR_SOLAR         6
#define    ADC_VR_SOLAR         7
#define    ADC_IY_SOLAR         8
#define    ADC_VY_SOLAR         9
#define    ADC_IB_SOLAR        10
#define    ADC_VB_SOLAR        11

#define    SDADC_VB_SOLAR       0
#define    SDADC_VY_SOLAR       1
#define    SDADC_VR_SOLAR       2
#define    SDADC_VY             3
#define    SDADC_IR             4
#define    SDADC_VR             5
#define    SDADC_VB             6
#define    SDADC_IB_SOLAR       7
#define    SDADC_IY_SOLAR       8
#define    SDADC_IR_SOLAR       9
#define    SDADC_IB            10
#define    SDADC_IY            11

#endif
