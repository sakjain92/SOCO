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
#define     EEPROM_PAGES_FOR_DATA_SAVE  100

#define  DATA_SAVE_START_LOC            0
#define  DATA_SAVE_END_LOC              5119     // 40 pages of 128 bytes
#define  PROGRAM_DATA_LOC1_START        (EEPROM_PAGE_LENGTH*EEPROM_PAGES_FOR_DATA_SAVE)
#define  PROGRAM_DATA_LOC2_START        (PROGRAM_DATA_LOC1_START+256)
#define  OLD_DATA_LOC                   (PROGRAM_DATA_LOC2_START+256)

#define  POWER_DN_SAVE_PAGE            (OLD_DATA_LOC+256)

#define  PASSWORD_SAV_LOC              (POWER_DN_SAVE_PAGE+256)
#define  SCROLL_LOCK_LOC               (PASSWORD_SAV_LOC+16)

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

#define     KEY_EDIT         (KEY_NEXT+KEY_DEC)

#define     KEY_DIR_CAL      (KEY_DEC+KEY_INC)
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



#define MAX_PARAM_LIMIT  19
#define POWER_FAIL_SENSE_VALUE    2200



// Definations for Ports


#define     DIS_DATA_BIT        0X01

#define     SR_CLOCK_BIT        0X02

#define     OUTREG_CLOCK_BIT    0X08


#define     DISP_SR_ON          (GPIOA->BRR  = 0X04)
#define     DISP_SR_OFF         (GPIOA->BSRR = 0X04)

#define     DIS_DATA_OUT_LOW            (GPIOA->BRR  = DIS_DATA_BIT)
#define     DIS_DATA_OUT_HIGH           (GPIOA->BSRR = DIS_DATA_BIT)
#define     SHIFT_REGISTER_CLOCK_LOW    (GPIOA->BSRR = SR_CLOCK_BIT)
#define     SHIFT_REGISTER_CLOCK_HIGH   (GPIOA->BRR  = SR_CLOCK_BIT)
#define     OUTPUT_REGISTER_CLOCK_LOW   (GPIOA->BSRR = OUTREG_CLOCK_BIT)
#define     OUTPUT_REGISTER_CLOCK_HIGH  (GPIOA->BRR  = OUTREG_CLOCK_BIT)



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


// Burden 20E
#define   VOLTAGE_COEFF            2.33090E-03f
#define   PH_VOLTAGE_COEFF         (VOLTAGE_COEFF)
#define   FUND_VOL_COEFF           2.9136E-03f

#define   CURRENT_COEFF            6.7974E-06f
#define   NEU_CURRENT_COEFF        (CURRENT_COEFF)

#define   FUND_CURRENT_COEFF       8.4968E-06f


#define   POWER_COEFF_3P4W         1.5844E-08f

#define  FUND_POWER_COEFF          2.4757E-08f

#define   MIN_VOL_LIMIT            10.0f
#define   MIN_VOL_LIMIT_PH_PH      17.0f
#define   MIN_TOTAL_CUR_LIMIT      0.04f
#define   MIN_NEU_CUR_LIMIT        0.005f


#define        FREQ_COEFF                 8800000              

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





#define         CALIBRATE_DIS_H_VI      1
#define         CALIBRATE_H_VI          2
#define         CALIBRATE_DIS_H_PF      3
#define         CALIBRATE_H_PF          4

#define         CALIBRATE_DIS_M_VI      5
#define         CALIBRATE_M_VI          6
#define         CALIBRATE_DIS_M_PF      7
#define         CALIBRATE_M_PF          8

#define         CALIBRATE_DIS_L_VI      9
#define         CALIBRATE_L_VI          10
#define         CALIBRATE_DIS_L_PF      11
#define         CALIBRATE_L_PF          12








#define        CUR_MID_CAL_POINT                     1.0
#define        CUR_LOW_CAL_POINT                     0.1
#define        NO_OF_CAL_ACCUMULATION_VI                4
#define        NO_OF_CAL_ACCUMULATION_POW            10
#define        CAL_ACC_DELAY                         8

#define        CAL_VOLTAGE_SETTING_HIGH              240.0f
#define        CAL_CURRENT_SETTING_HIGH              5.0f
#define        CAL_CURRENT_SETTING_MID               1.0f
#define        CAL_CURRENT_SETTING_LOW               0.1f
#define        CURRENT_TOLRERANCE                    0.2f
#define        VOLTAGE_TOLERANCE                     0.1f
#define        POWER_TOLERANCE                       0.4f


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

#define         PROTECTION_BIT_LOW           (!(GPIOC->IDR & 0X200))
#define         PROTECTION_BIT_HIGH           (GPIOC->IDR & 0X200)
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

#define LED_OFF    0
#define LED_R      1
#define LED_Y      2
#define LED_B      3

#define SWITCH_OFF_LED_COMM             GPIOB->BSRR = PORT_BIT_8
#define SWITCH_ON_LED_COMM              GPIOB->BRR  = PORT_BIT_8

#define SWITCH_OFF_LED2_R               GPIOD->BSRR = PORT_BIT_0 
#define SWITCH_ON_LED2_R                GPIOD->BRR  = PORT_BIT_0

#define SWITCH_OFF_LED2_G               GPIOD->BSRR = PORT_BIT_1 
#define SWITCH_ON_LED2_G                GPIOD->BRR  = PORT_BIT_1

#define SWITCH_OFF_LED2_B               GPIOD->BSRR = PORT_BIT_2 
#define SWITCH_ON_LED2_B                GPIOD->BRR  = PORT_BIT_2

#define SWITCH_OFF_LED_GRID_DISPLAY     GPIOD->BSRR = PORT_BIT_3
#define SWITCH_ON_LED_GRID_DISPLAY      GPIOD->BRR = PORT_BIT_3

#define SWITCH_OFF_LED_SOLAR_DISPLAY    GPIOD->BSRR = PORT_BIT_4
#define SWITCH_ON_LED_SOLAR_DISPLAY     GPIOD->BRR = PORT_BIT_4

#define SWITCH_OFF_LED_GRID_HEALTHY     GPIOD->BSRR = PORT_BIT_5
#define SWITCH_ON_LED_GRID_HEALTHY      GPIOD->BRR = PORT_BIT_5

#define SWITCH_OFF_LED_SOLAR_HEALTHY         GPIOB->BSRR = PORT_BIT_9
#define SWITCH_ON_LED_SOLAR_HEALTHY     GPIOB->BRR = PORT_BIT_9



#define SWITCH_OFF_RELAY_R_PHASE_GRID_HEALTHY  GPIOD->BRR = PORT_BIT_8
#define SWITCH_ON_RELAY_R_PHASE_GRID_HEALTHY   GPIOD->BSRR = PORT_BIT_8

#define SWITCH_OFF_RELAY_Y_PHASE_GRID_HEALTHY  GPIOD->BRR = PORT_BIT_9
#define SWITCH_ON_RELAY_Y_PHASE_GRID_HEALTHY   GPIOD->BSRR = PORT_BIT_9

#define SWITCH_OFF_RELAY_B_PHASE_GRID_HEALTHY  GPIOE->BRR = PORT_BIT_8
#define SWITCH_ON_RELAY_B_PHASE_GRID_HEALTHY   GPIOE->BSRR = PORT_BIT_8

#define SWITCH_OFF_RELAY_LOAD_ON_SOLAR         GPIOE->BRR = PORT_BIT_9
#define SWITCH_ON_RELAY_LOAD_ON_SOLAR          GPIOE->BSRR = PORT_BIT_9

#define SWITCH_OFF_RELAY_LOAD_ON_GRID          GPIOD->BRR = PORT_BIT_10
#define SWITCH_ON_RELAY_LOAD_ON_GRID           GPIOD->BSRR = PORT_BIT_10

#define INPUT_R_PHASE_GRID_HEALTHY             (!(GPIOE->IDR & PORT_BIT_0)
#define INPUT_Y_PHASE_GRID_HEALTHY             (!(GPIOE->IDR & PORT_BIT_1)
#define INPUT_B_PHASE_GRID_HEALTHY             (!(GPIOE->IDR & PORT_BIT_2)
#define INPUT_LOAD_ON_SOLAR                    (!(GPIOE->IDR & PORT_BIT_3)
#define INPUT_LOAD_ON_GRID                     (!(GPIOE->IDR & PORT_BIT_4)
#define INPUT_SPD_HEALTHY                      (GPIOE->IDR & PORT_BIT_6)
#define INPUT_DG_RUNNING                       (GPIOE->IDR & PORT_BIT_4)

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


#define RS485Receive (GPIOA->BRR  =  PORT_BIT_15)
#define RS485Transmit (GPIOA->BSRR =  PORT_BIT_15)

#define RS485TransmitOn ( GPIOA->ODR & PORT_BIT_15)

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
#define    ADC_IR1              0
#define    ADC_VR1              1
#define    ADC_IY1              2
#define    ADC_VY1              3
#define    ADC_IB1              4
#define    ADC_VB1              5

#endif
