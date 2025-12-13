#define MODEL_RELEASED

// 3 digit version number
// Shown on display as X.YY
//
#define VERSION_NO  201

#define MODEL_COMM_PROCOM
#define MODEL_DATA_SAVE  
#define MODEL_IMPORT_ONLY
//#define MODEL_DIS_OLD_DATA   

#ifdef MODEL_COMM_PROCOM
    #define MODBUS_MAP_PROCOM
    #define MODEL_RS485
    #define MODBUS_EDIT_PARA
#endif

// UNDONE:
// 1) We aren't measuring 5V (We have provision to measure 3V): This is needed for saving energy data
// We should measure 3.3V or 5V so that we can save data in EEPROM as soon as power goes away to avoid losing data
// 2) (Partially Done)Check the resistor values for LEDs
// 3) Need LEDs for indicating whether we are displaying Mains or Solar Current/Voltages. Need more LEDs for other status like contactors input/outputs etc. Check if we can show these status on display.
// 4) Remove resistors: R69, R71, R72, R73, R74 from top pcb
// 5) (Done: No Issue)Green LED is not working
// 6) Connector for JTAG & USB needs to be fixed
// 7) (Done: No Issue)Check if R & B Led are inter-switched
// 8) Add labels for kind of relay output (Solar, Grid, R Phase of solar etc)
// 9) Change name of LEDs on sticker
// 10) In panel, all PFC driven  by 48V DC. We should make provision for adding make of Fuse and it's rating
// 11) (Done)In panel, K1, K2, K3 will be driven by grid voltages and not solar voltages. See v1.5 drawing
// 12) Can remove extra Relay (6th) from BOM (Can we really drive it)?
// 13) Add USB port and also code for DFU. USB port cutout needed
// 14) LEDs and switches should be SMD. Check other components also. Try for SMD capacitors.
// 15) Remove RTC battery? Currently not used in code
// 16) Fix power supply section layout (Tranformer and SMD components on corrct side of the PCB)
// 17) Switches direction on PCB is wrong
// 18) Remove R55 from Bottom pcb (But keep it there for future use?)
// 19) M7 on bottom PCB connected to R130 is D28 but not present in BOM and not marked in schematic with name D28
// 20) Remove R55, R130, R129, R128 from Bottom PCB. D28 also.
// 21) Try to make sure no through hole component on top PCB. Move power supply to bottom PCB if space (so no high voltages going to top PCB. Remove relay if that gives space. More relay terminals accordingly).
// 22) Make another mould enclouser with more spacing for future components.
// 23) Should add a LED for DG Running/SPD Healthy? Don't need info about SPD healthy but do require for DG Running/Load on Solar or Grid. Maybe remove COMM Led
// 24) With 37V Aux and all relays on, 5.3V is observed. So no need to change power supply with OMRON relays inplace (G6RN relays). We probably don't need High capacity relays but they have better dielectric strength. Have to check rated contact voltage for the relay (They are 250V currently for NO with max switching voltage of 250VAC. This is important. We need higher contact rating)
// 25) Need to add convert connector and FRC cable to 20pins from 16 pins J2 in top
// 26) Can we move op-amps to bottom pcb and also the power supply section so no high voltage goes to top PCB ever
// 27) Do we want to measure the 0.5 class accuracy across temperature range and different pieces. Might have to do calibration at two points for this (100V and 300V). Get lab tested for meter accuracy and protction.
// 28) Change R118, R136, R117, R124, R122, R126 to 2.2k. Check BOM of Ace/COP/REX/IPR to see difference in BOM vs Schematic as we have taken schematics as references
// 29) Should change relay driving logic as in saturation, Ic=10*Ib. So to drive 100ma, need 10mA Ib, which is not possble from uC GPIO pin. Need two transistor pair (Current Vce is about 0.2)
// 30) Should check direction of mounting R/G/B LED (It was solder in wrong direction. Is this possible in production also)
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
// 41) Add cuts below optocoupler. Also do we need 10kV isolated opto-couplers?
// 42) Should we have BAV99 protection in current section like in COP?
// 43) X-caps on voltage measurement/current section for dv/dt? Isolation transformer?
// 44) Have to go through whole code and check at metrology. There is a bug in digital filter at 600Hz in Interrupt.c. Similarly have to check other things. Test the whole code again
// 45) The capacitor on opto-coupler to be changed as re reduced resistor values so increase capacitor values (Circuit was copied from elsewhere)
// 46) Use one of the digital inputs to check for presence of 48V. Also, export that over modbus. The resistor values (30K + 4.7k) on optocoupler indicates ~1mA current at 40V input. We can't go below this (opto-coupler datasheets starts at 1mA). But we can add TVS in parallel to the capacitor along with cut beneath the opto-coupler
// 47) (IMPORTANT)We should move to using SDADC if possible as it has 16 bits and also it has programmable gain so we can keep the actual input voltage low coming on the pin low. Use single-ended measurements (We can use maximum of 11 double ended SDADC which leaves us with 1 sample.. Maybe measurement of B phase of solar for this can be done via ADC?)
// 48) Four quadrant net metering required
// 49) Get panel and controller tested in labs
// 50) Add cuts in the terminal pins where high voltage is coming. Probably should still use SDADC in single ended mode for current as it needs more accuracy at lower ranges
// 51) Should do double voltage calibration for better accuracy (240 & 110?). Also extrapolate the linear calibration above 240V and below 110V. Same for current (We cap to using the higher end of calibration instead of extrapolating)
// 52) For all ADC inputs or SDADC input, put 10k in series and a 10nF capacitor at the pin (Check works at higher input frequency still)
// 53) Check that we aren't showing Energy & Old Energy in decimals when we actually can show it in decimal. Check what other data we aren't showing in decimal. Also do we need to show Voltage & Current with many decimals?
// 54) Check what should be an appropriate minimum starting current
// 55) For whole current, how do I calibrate at 100A?
// 56) We should make it possible to see state of outputs/inputs on the controller otherwise will be difficult to debug issues in the field (contactor latching etc)
// 57) Since we use NC or NO of a relay, do we want to just have one output per relay on board (less chances of mistake when making the panel)
// 58) For four quarant metering, we need to fix P1/P2 & S1/S2 of CT with CT polarity correction. Possibly have to get it tested on panel side also via a WebUSB based RS485 testing software
// 59) Remove label of Indotech from external CT before giving panels to Jio
// 60) Connectors are very near to screws on the IPR backside PCB. Can maybe only have 2 terminals per relay
// 61) 7 segment LED display needs to be in centre
// 62) I don't think we are measuring per-phase energy (KWh, KVAh, KVARh). Jio requires this.
// 63) Retest full code and accuracy and also that at no current/voltage applied, all parameters are 0 (noise ignored)
// 64) Check spacing for wirewond resistors on PCB
// 65) In self test, we should check power supply from all 3 inputs
// 66) Removed C16 (Y-Cap) connecting primary & secondary of SMPS on top PCB. Without removing it, power supply was impacting voltage measurement when the power supply and measurement section was same.
// But will it cause EMI/EMC issue? Should we use a lower value Y cap there (higher impedence) along with higher voltage rating (more than 2.5kV)?
// 67) The "t" in font list is wrong I think 
// 68) Fix the fact that upon calibration error, the error doesn't show up on screen
// 69) Check fluctuations in voltage/current measurement when DC/AC power supply is used
// 70) Improve calibration of current/voltages using SDADC (single ended measurement)
// 71) Add 100nF capacitor near uC pins on all ADC measurements (10nf + 10k series = 1.5khz cutoff with 0.05% impact at 50%)
// 72) Check that meter can be calibrated after it's already calibrated
// 73) Add LEDs in self-test also
// 74) Check accuracy (amplitude & phase) of external CTs across wide range (100A - 0.01A). For class 0.5 meter, check which is minimum startup current as per standards
// 75) Replace R48, R52, R56, R47, R51, R55 with 6.2k (We don't have 8.2k). Replace R40, R48, R23, R32, R94, R101, R108, R115, R131, R132, R133, R134 with 10ohm (Later when we use SDADC, we can reduce to 5ohm to keep burden on external CT low and also to generate low voltages across shunt for better protection). Need extra protection on current circuit (reverse engineer the Siemens/L&T meter).
// ALso check if long wires of external CT causes any noise pickup
// 76) Check replacing all series resistors (Voltage/Current) to 10K for better protection of uC and check if it still works with good accuracy
// 77) Have to check fluctuation level on voltage/current and error after moving to SDADC (Currently the error is not acceptable)
// 78) Change the LEDs sticker on SOCO as the red filter on LEDs makes the green and blue LED not pass through the filter
// 79) When error on calibration, it doesn't show on display and also difficult to debug
// 80) When controller starts, all contactors turn on/off initially. Fixt this.
// 81) Should add testing code over RS485 to check electrical interconnects & SPD connections in panel which might not be checked by operator.
// Create a full testing code over RS485 for the panel. Some things have to be tested visually like coil surge suppressor, mechanical interlock.
// 82) If we use shorting link in Panel, have to make sure that there is proper insulation between them.
// 83) In panel, joints should come preadded in the mouting plate from the panel supplier. Also, sloped roof might be cheaper.
// 84) Check with panel builder about the ideal dimensions of the panel to reduce wastages
// 85) Need EMS wiring details from Jio
// 86) There should be a sticker in the panel near the control wiring coming from outside.
// 87) Salt spray & Heat test needed for the panel & Vibration for transportation.
// 88) CTs might need to be aded before MCBs as not much space in the panel otherwise. Can this cause any issue in case of surge (High current flowing into SPD)?
// 89) Check which op-amp to use with low offset & drift for class 1 accuracy
// 90) Reset the controller when any setting is changed 
// 91) Add RS485 test for checking electrical interlocking
// 92) Add extra 10k resistor on all ADC inputs + Digital Inputs + Digital outputs.
// 93) Check stray current coming on mains current (~100mA) when solar voltage is applied
// 94) Check SOCO sticker & switches alignment and check if it fits 
// 95) Add 10nF capacitor near: all current & voltage measurement section + 10k series resistor after that + on all digital inputs (PFCs)
// 96) We can use SDADC in single-ended mode to read many voltages (probably can read all voltages/currents in single ended mode but have to check the timing)
// 97) Add separate Vref for currents also
// 98) Add TVS near input of CTs. Also, make sure CTs can never be open. Also, all filtering of inputs should be wrt to Chassis right and not MCU ground? Also, add ferrite beads with surge protection on current and voltage protection
// 99) Can modify the product to just run on 48VDC (Reduce MOV voltage and protect the circuit much better)
// 100) We need to protect the power supply for Common mode surge (Route surge to earth/chassis)
// 101) Check MOV in RS485 section is 175V and not 305V
// 102) Can change MOV in DC 48V power supply to be 175V MOV (In ECON, also the BTS from 48V can be changed to 175V MOV instead of 320V MOV)
// 103) For better MCU pin protection in current section, if 10K input impedence is not possible, can use a 1K ferrite bead for noise suppression
// 98) We need to change R59 & R60 to be 6.2k
// The equation of op-amp is as follows:
// Vout = ((Rp + Rf)*(Rn*Vb + Rb*Vn)/(Rn + Rb) - Vp*Rf)/Rp (Note the output is inverted voltage so 180 phase shift)
// Where Rp = Resistor on Phase input = 3 * 1Mohm
// Rn = Resistor on Neutral input = 3 * 1 Mohm
// Rb = Resistor on Bias voltage = 6.2k 
// Rf = Feedback resistor = 6.2k
// Vp = Phase voltage wrt to ground
// Vn = Neutral voltage wrt to ground
// Vb = Bias voltage = 1.35V
// For equation to make sense, it's important that Rn = Rp && Rf = Rb. But note that for this we also need to factor in tolerance of resistors and TCR if we want no fluctuation.
// (I don't think there is any other way out to match the accuracy). We probably need 0.25% or 0.1% resistors for 0.5% class here for both MELF and 6.2k resistors.
//
//
// NOTES:
// 1) For Apparent power, we are using RMS (IEC 60038) instead of (IEC 61000-4-7) where we just measure first harmonics as we need to match energy with utility meter (and SMPS will have noise)
// 2) We tested surge via primary and connected 100:5A CT externally (like in ECON) to ECON current section and didn't see any failure in ECON. Does this indicate current section is directly
// immune to surge (except for conductive surge)
// 3) Note that this code is specialized for Jio. It doesn't have all the data calculation on Solar side because it didn't needed to be measured for Jio.
//
//
// PANEL:
// 1) Color code external CTs and without sticker
// 2) Calibration of polarity of CTs and recheck via self test (RS485) of polarity of CTs in Panel division
// 3) Check rubber gasket type to be used for IP65 protection (THere is a cut in the gasket). Get salt spray for 1000 hours for the panel
// 4) Maintiain spacing around wiring and contactor for heat dissipiation. Get a temperature test done of panel to see maximum temp on contactor (to be below 10)
// 5) Have to figure out spacing of CTs in panel as after MCB there is not much space left for CTs. Can we put CTs before MCBs (have to think about surge current). Also
// currently in panel CTs are being added after voltage measurement due to thee points from where we can measure voltage in panel is only at MCB not at contactor.
// 6) Thermocol packing specific to panel size would be better. Also, we should make the panel outer as close to rectangle as possible to make packing easier. Packing box should be exact fit. The mounting brackets should be screwed to make packing easier. Same for extra canopy? Have to check IP65 for any extra screws
// 7) Need to make a proper mechanical drawing of the encloure with all the mechanical details and find a good vendor for panel (Current panel manufacturer isn't great at quality)
// 8) Take Mains voltage measurement from SPD terminals and power supply for controller also.
// 9) We can potentially remove 2 MCB from panel to save cost. Also, do they need to be 6A or can they be lower rating.
// 10) We can connect SPD remote indication & door lock indication to EMS box instead of our controller (saves 1 input + 1 output relay). Also, use 24VDC from output of EMS for running all PFCs instead of 48V as the 24V will be isolated and safer
// 11) Is Panel door also need to be Earth? No components on it.
// 12) Have to check if 25mm armoured cable can bend to go to terminal in panel (Get 25mm alumnium for testing)
// 13) We also use ring type thimble. Should be okay but Jio specified only pin & boot type. Maybe that's for control wiring.
//
