#define MODEL_RELEASED

// 3 digit version number
// Shown on display as X.YY
//
#define VERSION_NO  207

#define MODEL_COMM_PROCOM
#define MODEL_DATA_SAVE  
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
// 22) Make another mould enclouser with more spacing for future components. Preferablly just increase depth of IPR5 enclousre
// 23) Should add a LED for DG Running/SPD Healthy? Don't need info about SPD healthy but do require for DG Running/Load on Solar or Grid. Maybe remove COMM Led
// 24) With 37V Aux and all relays on, 5.3V is observed. So no need to change power supply with OMRON relays inplace (G6RN relays). We probably don't need High capacity relays but they have better dielectric strength. Have to check rated contact voltage for the relay (They are 250V currently for NO with max switching voltage of 250VAC. This is important. We need higher contact rating)
// 25) Need to add convert connector and FRC cable to 20pins from 16 pins J2 in top
// 26) Can we move op-amps to bottom pcb and also the power supply section so no high voltage goes to top PCB ever
// 27) Do we want to measure the 0.5 class accuracy across temperature range and different pieces. Might have to do calibration at two points for this (100V and 300V). Get lab tested for meter accuracy and protction.
// 28) Change R118, R116, R117, R124, R122, R123 to 2.2k. Check BOM of Ace/COP/REX/IPR to see difference in BOM vs Schematic as we have taken schematics as references
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
// 48VDC is not available (Have to add software logic for this).
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
// Also since we have programmable gain, in lower current measurements, we can use higher gain to get better accuracy
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
// 76) Check replacing all series resistors (Voltage/Current) to 10K for better protection of uC and check if it still works with good accuracy (Not working with current section. Probably need to buffer it with Op-amp to add 10k). We have to check if a better op-amp is required for lesser Common mode noise & offset error/bias error etc
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
// 99) Can modify the product to just run on 48VDC (Reduce MOV voltage and protect the circuit much better): No, Jio want 240V power supply also
// 100) We need to protect the power supply for Common mode surge (Route surge to earth/chassis)
// 101) Check MOV in RS485 section is 175V and not 305V (Also if RS485 is completely internal in the panel, we can reduce it's protection. Is RS485 just for connecting SOCO to EMS?)
// 102) Can change MOV in DC 48V power supply to be 175V (or 60V is better) MOV (In ECON, also the BTS from 48V can be changed to 175V MOV instead of 320V MOV)
// 103) For better MCU pin protection in current section, if 10K input impedence is not possible, can use a 1K ferrite bead for noise suppression
// 104) Should add some provision over RS485 to reset energy and/or display (special password for our testing team?)
// 105) Two power wires coming from bottom PCB to top PCB should have a diode to avoid mistake in production where the wires are reversed
// 106) In self-test, improve it (Delay between input testing & output testing is too much and long press in self-test shouldn't change Scroll on/off)
// 107) Check the calculation in this codebase v/s in Ace code base. There are differences. Also ask Chatgpt to proof read code (atleast the calculation logic) as code in AcePlus seems to have bugs
// 108) Maybe we should make CtPt ratio as 2500/5 as lot of the codebase assumes a current flowing into the meter as 5A and then the CT ratio multipled. Check constants like (0.1) being used in metrology.
// 109) Energies can overflow from display. We should add provision for a) Reseting energy over modbus b) Showing old data over modbus or start showing energies in MWh also (I think they shouldn't overflow storage & data sent over modbus). Also, on modbus, data is being transmitted as float so accuracy is limited at 2^24 and maximum value is limited to 2^32. We should fix this (Send data as double over modbus or send as divided by 1000)
// 110) Test modbus as Jio will use modbus
// 111) Consider adding PT outside controller in panel for reducing risk in voltage measurement
// 112) Ask for shielded cable for external CTs? Also check the proper length (not to long) for external CT. Also we already use another external CT in Whole Current Meter. Can we use the same here?
// 111) In self-test we should also test all digits/leds of the display
// 112) Check if the power supply circuit is properly designed for low voltage
// 113) Check delay between DG turning off from on and load switching to solar
// 114) The terminals of the controller are on the back side of the controller. Makes it hard to understand the wiring. Maybe the L-Channel should have a hinge or change the body to din-rail type
// (We have extra space now so can try for a single flat din-rail mounted PCB)
// 115) Need RS485 based K5 tripping (Where if EMS box wants, it can trip K5 even if Mains is healthy (K4 can be turned on in this case if solar available)).
// This needs to be time based. So if the command doesn't repeat again and again every 5 minutes, we turn on K5 again.
// Jio wants this so as to allow battery to discharge so that Mains is not used to charge the battery but the battery gets charged when the solar becomes available
// The whole purpose of AC based Solar invertors is to export solar to grid as solar is high on afternoon but telecom load is highest at night
// 116) Need USB for firmware upgrade as Jio will keep asking for changes. Also firmware upgrade over RS485 is also important to them.
// 117) Need to send alarms to RS485 when the contactors inputs don't match our controlling of the logic. Do we need to flash it on screen also? Also, can check contactor input against metering data to see if the contactor positions matches our expectations.
// 118) Current measurement accuracy needs to be improved to make neutral current measurement accurate
// 119) When SPD is healthy, is the output contact NC?
// 120) Read the whole code base again. There are many hardcoded stuff here that can potentially cause issues
// 121) The controller's wiring diagram is a bit off. The pins mentioned on the sticker don't exactly align with the terminals
// 122) It's possible currently for current to flow from 
// 123) We should do proper bound error checks on all requests coming over Modbus
// 124) IMP: The power supply section is a bit wrong. The controller can be powered on via current leakage from Neutral of grid to Neutral of solar or from Neutral of grid to 48-ive of battery etc.
// Also, Check https://www.power.com/node/463601 where they are saying that they don't recommend the power supply to run on lower than 70V
// 125) Check bug in modbuses if any. We had a bug that was caught after 10 mins of polling on modbus
// 126) Remove support for little endian maybe since we don't support write holding registers in little endian (or add support for it)
// 127) Check response type of Modbus Write Queries
// 128) Do we need to show details of faults of the contactor on display
// 129) Jio wants feedback of MCBs also via digital inputs. Should we do it or keep it in EMS scope along with Canopy door open logic? How to take feedback of MCBs also? Total DI: 5 from contactors, 2 from MCBs/Isolators, & DG Running. EMS to take digital inputs for canopy open and SPD to be taken by EMS (They can also take MCB/Isolator inputs)
// 130) We should send firmware version over Modbus
// 131) Make a manual/datasheet (Similar to Ace/REX)
// 132) IMPORTANT: In 230V power supply, our IC currently is max 700V drain mosfet. This needs to handle clamping voltage of MOV + inductor voltage. Should we try for 1500V?
// Servo has a high voltage power supply available. Also, common mode surge noise has to be considered in power supply (230V and 48V. Note 48V is positively earthed). One solution is
// using StackFET configuration via using external mosfet, like that used in Servo PSU
// 133) Check that 200mm * 250mm pcb will be able to be manufactured in the SMD room properly and in our shop floor
// 134) We should measure 5V from 48V to check if 48V is available or not
// (We should have Cuts below MOVs on PCB for isolation)
// 135) For differential op-amp circuit, check: a) Phase shift due to inverting (check power direction) b) Accuracy of resistors to match to make it class 0.5
// Don't use LM324 as it has high offset error. Probably should use direct SDADC connections for current
// Also, if using op-amps, have to check the maximum and minimum voltages that can be measured (due to maximum common mode voltage and maximum differential voltage
// ratinng of op-amp. Maximum common mode voltage of LM324 can only be 3.3-2V = 1.3V but full scale differential voltage is allowed.
// Also note that LM324 can at minimum reach to 20mV to 20mV needs to be clipped off the range of input)
// 136) For 0.5% class, we should use resistors with better temp drift and maybe 0.1% accuracy or 0.5% accuracy resistors?
// 137) For voltage measurement inputs in op-amp, consider adding guard rails at bias voltage around input pins
// 138) Conformal coating might be required for creepage/clearance. Do we need them if we got cuts?
// 139) For voltage measurement, LM324 is good enough as we have to support only maybe 50V-300V maximum. But consider calibrating at 100V, 200V, 300V. Keep decouping caps on bias voltage output from an op=amp
// 140) Need a proper circuit for protection at the current input
// 141) Add some error in coefficients to force calibration
// 142) If we stick with 3 layer PCB, we will have to improve the connection between PCBs properly
// 143) If we want to use SDADC for voltages also for better accuracy, we will have to change the signals of SDADC (We can only use SDADC Positive signals in single ended and we want
// to keep the Voltages measurement just after current measurement for better phase accuracy). Also for currents, we can use differential measurement to remove noise coming from Vref/2 (keep the two traces as close to each other as possible). Review voltage and current section measurement circuit and layout properly
// Also keep capacitor near uC pins (1K + 10nF?)
// Keep current and voltage measurement sections away from each other (Keep power supply section also away from both of these)
// 144) Check that using op-amp is safe in voltage section (High voltages)
// 145) We should add conformal coating where there isn't enough spacing. If spacing is enough with cuts, do we need conformal coating?
// 146) In v1.3, the 12V output port isn't protected against surge. Need to protect it.
// 147)Use 60V MOV 20mm in DI and 48V power supply. What MOV to use for common mode? (Note that transformer only give 1KV isolation. Do we need to increase this or rely in common mode noise suppression instead)
>>>>>>> Stashed changes
//
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
// We should get type test for metering standards also
//
// 200) How to protect DI and current measurement section from common mode surges?
// 201) Add frequency measurement of solar & grid in hardware else do software zero voltage crossing detection
// 202) Need to measure differential voltage/current using SDADC (keep the two wires as close to each other as possible in input and near microcontroller). Need to reduce power factor error
// and current fluctuations at low currents. Keep current & voltage & power sections away from each other.
// 203) Check if op-amp can survive surges in voltage section or should we just use SDADC in voltage section also without op-amp? For current section, LM324 doesn't make sense. Use SDADC directly.
// 204) Use 60V MOV in DI & 48V power supply
// 205) Use stackFET in power supplly for protection upto 1500V (Clamping voltage of 320V MOV can go upto 900V)
// 206) Do we need to add 100ohm resistor in Neutral or 48V- in power supply also
// 207) In current section & voltage section, do we need a differential capacitor (between both input lines?)
// 208) Check maximum and minimum current measurable. Maximum current is impacted by BAV99 maximum.
// 209) One of the space relays will be used for controlling FAN (when temperature rises above 45C or 50C we will start Fan. NC of relay to be used).
// This is because we want to limit the life of the Fan.
// 210) BCH will provide load bearing certification for their mounting brackets.
// 211) We should get the panel checked by BCH. Also ask their advice on packagain/transportation of it.
// 212) We need to check if the AC fans can handle the 300V at remote sites + surges (Do we want to give fan failure protection also like in TMTL?). Also, SOCO should have temperature
// measurement circuit (Do we use STM32F373 temp measurement or external IC like in IPR5)
// 213) We should add under voltagr and over voltagr protection of contactors also (If there is excessive voltages or under voltages, switch off the contactors)
// 214) Should we remove op-amp in voltage section also?
// 215) We should do insulation test or dielectric test to check that the body of SOCO is not connected to PCB anyhow
// 216) Creepage/Clearance around high voltage inputs: Make sure no low voltage signal on PCB layout comes anywhere near any high voltage input
// 217) In AC power supply, do we use 2 450V Capacitor in series with bleeder resistor for input protection
// 218) Get AC power supply reviewed by someone in Electricity Meter (They make according to standard). Compare with Servo PSU also
// 219) Should we use 510VAC MOVs or 320VAC MOVs on AC line as what if two phases get short externally or netureal get short to phase?
// 220) Check if after adding 100ohm * 2 resistors, all 6 relays can still work
// 221) Do we need EMI/EMC on power supply?
// 222) Check that if only one phase voltage is added, it's shown accurately even though the calibration happens in 3 phase. Same, check when 0 phase angle between R/Y/B voltage & current, is there any issue (This will happen on solar side)
// 223) Check the voltage section is safe from surges (use of op-amp is correct)
// 224) Make sure the PCB is not touching the body from edges (if it's touching then bare part should only be touching of PCB, no tracks)
// 225) Make sure the contact points of body with PCB is not affected by powder coating
// 226) Make sure that non-powder coating area doesn't rust in body
// 227) Make sure that even if screws get rusted, no track is impacted
// 228) Check if supports needed in between PCB
// 229) Should we add RC filter on digitial input section also near the MCU pin (incase the external input is oscillating)? Might not be needed as we already have RC filter before optocoupler
// 230) Remove single ended current measuring circuit & corresponding all zero ohm resistors. Hopefully this will make layout easier. Make sure voltage pins on MCU and current pins on MCU are away
// from each other if possible
//
// NOTES:
// 1) For Apparent power, we are using RMS (IEC 60038) instead of (IEC 61000-4-7) where we just measure first harmonics as we need to match energy with utility meter (and SMPS will have noise)
// 2) We tested surge via primary and connected 100:5A CT externally (like in ECON) to ECON current section and didn't see any failure in ECON. Does this indicate current section is directly
// immune to surge (except for conductive surge)
// 3) Note that this code is specialized for Jio. It doesn't have all the data calculation on Solar side because it didn't needed to be measured for Jio.
//
//
// PANEL:
// 1) Color code external CTs and without sticker (There should be sticker of R/Y/B Grid/Solar P1/P2 on the CT also)
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
// 12) Have to check if 25mm armoured cable can bend to go to terminal in panel (Get 25mm alumnium for testing). Can check with panel already installed at site.
// 13) We also use ring type thimble. Should be okay but Jio specified only pin & boot type. Maybe that's for control wiring.
// 14) Canopy might need to be extended on all sides to protect from rain as per Jio's requirements (About 40mm air gap required between canopy and panel top)
// 15) Wooden packing is essential for avoiding transportation as high vibration
// 16) EP Sheet required inside the panel to maintain tight contact between the panel door and contactors (Check other components also) during transportation
// 17) Mounting screws for mounting plate needs to be screwed through the back sheet and not welded for more strength. 6 screws is a good idea due to high weight of the mounting plate.
// Vibration testing is required
// 18) Check if enough space for bending of armoured cable
// 19) For panel heat test, need to bring our own 100A * 240V source and load.
// 20) Check a weird phenomenon we saw: Solar & Grid Power Supply MCB to controller was off as well as 48VDC MCB. Solar voltage 3 phase was present and no voltage on grid. The controller was still flickering. Why? Is there any capacitance causing current flow path? (This was an issue in SOCO power supply design)
// 21) Change drawing of the Panel to indicate SPD fb to come from NC
// 22) Have to check agreement on pricing from contactor manufacturers/panel enclousre manufacturer as to frequency of change in pricing based on raw material escalation and appropriately specify the
// same T&C to jio.
// 23) Have to check filter used in panel as non-standard filter can make the panel non-IP55. Use an off the shelf plastic filters/louver as per BCH recommendation (Can buy separately as BCH will charge an overhead). Can buy fan also separately to reduce cost (But if it's under BCH, warranty dealing will be simpler)
// 24) Panel if kept in outdoor facing solar directly will cause heat-up. Need to put it in shade. Also assuming pole mounted and not wall mounted so heat transfer can happen from backside also. Gland plate will not be considered for heat transfer as it will have glands cutouts. Need to check current flowing through wires as the current flowing in wires can be major contributor for heat. Also need to check the upper temperature range for all components.
// 25) Panel canopy can be extended on sides also in BCH as Jio wanted it
// 26) BCH is saying that their wall mounted brackets are only 3mm and can handle 60-70kg. They don't have a type test report for it. But they can go for 4mm brackets (it will be expensive)
// 27) The mounting plate is attached to back plate via screws that are welded on the panel enclosure. Make sure they can handle 30-40kg of components especially when vibrating during transportation
// 28) Panel door open switch required in panel
// 29) Ask BCH to have a look at our panel and suggests improvements based on their experience to make a good quality product. Maybe they can help with Fire Engine also.
// 30) Instead of a padlock provision, we will provide option of key lock to Jio if required (Maybe pad lock provision is required by Jio?)
// 31) Canopy will cover all 4 sides with minimum of 50mm air gap between canopy and panel top
// 32) Canopy color will be white (RAL-9003) to reduce temperature. BCH to confirm there will be no discoloration and no dust coloration over period of time (Dust will always settle anyways)
// 33) Door limit switch will be there
// 34) Possible optimizations: 1) Use 1.6mm Gland Plate instead of 2mm gland plate (no need to add a new gland plate) 2) Reduce depth from 220mm (have to do heat calculations again. But I think the filter size if big enough that reduction is not possible) 3) Reduce Width from 700mm to 665mm (Mounting plate will be used less. BCH is saying they are buying custom sheet so not much effect)
// 35) Why SOCO is on right side in panel? Shouldn't it be on left side and EMS box on right side? This will reduce control wiring (But does having SOCO away from contactors make it more resilient?:)
// 36) For packing of panel, we should use wooden packing with plastic cover to protect from dust/water. Also the wooden packing to be such that no movement of panel possible
// 37) Check earthing stud for SOCO in panel (should be nearby inside)
// 38) Canopy can be changed to reduced cost if possible (doesn't need to extend at back)
// 39) Have to check Pad lock hole and also ask Kanoda to change the mechanical mounting structure
// 40) BOM has been changed: 50A 3 pole MCB is being used, 1 sq mm wire is being used for control wiring. 35mmsq glands are being used.
//
// Panel ventilation:
// 1) IEC 60890 seems to state that ventilation cutouts with filters impede air circulation so much that it's equivalent to no cutouts
// 2) If Fan is to be put, it should be put on bottom sucking air in
// 3) Air outlet cutout should be 1.1x of air inlet cutout (Or treat the cutouts as 0.9x)
// 4) Canopy is important otherwise temperature on top of the panel will be very high, just where SOCO will be placed
// 5) Ambient temperature inside panel shouldn't cross 65 for wires
// 6) Effect of temperature also should be considered in SOCO design
// 7) We might have to use a Fan to decrease temperature
// 8) Standard says best option is fan attached at bottom flowing air into the panel from bottom and exit from the top. During panel construction, we will have to properly ensure poper direction of fans
//
// Future scope:
// 1) Give relay card channel to Jio with OMRON for v1.5 of the Panel
// 2) ACDB controller can be given to Jio for residential project also. They require it, but economical
//
