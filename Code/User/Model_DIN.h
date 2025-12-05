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
// 1) We aren't measuring 5V (We have provision to measure 3V): This is needed for saving energy data
// We should measure 3.3V or 5V so that we can save data in EEPROM as soon as power goes away to avoid losing data
// 2) Check the resistor values for LEDs
// 3) Need LEDs for indicating whether we are displaying Mains or Solar Current/Voltages
// 4) Remove resistors: R69, R71, R72, R73, R74 from top pcb
// 5) Green LED is not working
// 6) Connector for JTAG & USB needs to be fixed
// 7) Check if R & B Led are inter-switched
// 8) Add labels for kind of relay output (Solar, Grid, R Phase of solar etc)
// 9) Change name of LEDs on sticker
// 10) In panel, all PFC driven  by 48V DC. We should make provision for adding make of Fuse and it's rating
// 11) In panel, K1, K2, K3 will be driven by grid voltages and not solar voltages
// 12) Can remove extra Relay (6th) from BOM (Can we really drive it)?
// 13) Add USB port and also code for DFU
// 14) LEDs and switches should be SMD. Check other components also. Try for SMD capacitors.
// 15) Remove RTC battery?
// 16) Fix power supply section layout (Tranformer and SMD components on corrct side of the PCB)
// 17) Switches direction on PCB is wrong
// 18) Remove R55 from Bottom pcb
// 19) M7 on bottom PCB connected to R130 is D28 but not present in BOM and not marked in schematic with name D28
// 20) Remove R55, R130, R129, R128 from Bottom PCB. D28 also.
// 21) Try to make sure no through hole component on top PCB. Move power supply to bottom PCB if space.
// 22) Make another mould enclouser with more spacing for future components.
// 23) Should add a LED for DG Running/SPD Healthy? Don't need info about SPD healthy but do require for DG Running/Load on Solar or Grid. Maybe remove COMM Led
// 24) With 37V Aux and all relays on, 5.3V is observed. So no need to change power supply with OMRON relays inplace (G6RN relays). We probably don't need High capacity relays but they have better dielectric strength. Have to check rated contact voltage for the relay (They are 250V currently for NO with max switching voltage of 250VAC. This is important. We need higher contact rating)
// 25) Need to add convert connector and FRC cable to 20pins from 16 pins J2 in top
// 26) Can we move op-amps to bottom pcb and also the power supply section so no high voltage goes to top PCB ever
// 27) Do we want to measure the 0.5 class accuracy across temperature range and different pieces. Might have to do calibration at two points for this (100V and 300V)
// 28) Change R118, R136, R117, R124, R122, R126 to 2.2k
// 29) Should change relay driving logic as in saturation, Ic=10*Ib. So to drive 100ma, need 10mA Ib, which is not possble from uC GPIO pin. Need two transistor pair (Current Vce is about 0.2)
// 30) Should check direction of mounting R/G/B LED
// 31) Check resistors for LEDs (R/G/B or other LEDs) to make sure about their life
// 32) Probably can read voltage from PFC via a serial resistor on uC pins
// 33) Probably need more protection to protect the opto-coupler. Maximum voltage it can withstand currently (max 50mA) = 1.5kV. Do we need to hav MELF resistors there?
// 34) Think of clean and dirty ground. Connect the Y Cap of the SMPS in bottom PCB, near the earth point
// 35) Instead of OMROM G6R Relay which has only max 250VAC contact rating, use Hongfa HF118F
// (50mA @ 5V, 440V Contact rating, Buy flux protected relay. Buy: hf118f0051zs1t. Might be drop
// in replacement for G6RN Relay of OMRON but have to check after ordering. Slight mechanical 
// difference). Good news is that our power supply seems sufficient to handle 5 relays.
// 36) Remove 1 extra relay to save PCB space. Remove . Also check with Jio if only having
// 48VDC and Solar input is enough since the NC contacts of relays ensure that Solar input
// (From K4) are always available when either Mains or Solar is available. Just that without
// having Mains input, we wouldn't be able protect contactors from under voltage on Mains when
// 48VDC is not available.
// 37) Add vishay wirewound resistors on power supply (10KV protected) and 320V MOV. Replace
// C14 on bottom PCB with 47uF/450V to make it properly surge protected.
// 38) Remove D3, D4 (not D5) from circuit to save space and ensure 48VDC is always connected
// in correct order. Also, can make all other inputs half wave rectifier if that save space
// if needed and can run all relays properly (But let's not risk half wave rectifier)
// 39) Check 5V on oscilloscope with 38V input / 160VAC and all relays on and see if there is
// bi ripple
// 40) Do we need to get EMI/EMC tests done also?
