
#include "stm32f37x.h"
#include <stdio.h>
#include "Struct.h"

#pragma section = "FLASH_COEFF_SECTION"

#ifdef SEED_CALIBRATION_DEFAULTS

// Factory/release build: burn unit-gain defaults into the calibration page
// so a never-calibrated board still boots. Overwritten at the end of
// Cal Mode with the actual measured coefficients.
//
// Field order MUST match struct PH_COEFF_STRUCT in Struct.h. Each I gain
// and I phase-error block has four entries (XHIGH, HIGH, MID, LOW).
//
const struct PH_COEFF_STRUCT CalibrationCoeff @ "FLASH_COEFF_SECTION"=
{
  1.0f,  // Vol 240V (R, Y, B)
  1.0f,
  1.0f,

  1.0f,  // IR (XHIGH, HIGH, MID, LOW)
  1.0f,
  1.0f,
  1.0f,

  1.0f,  // IY
  1.0f,
  1.0f,
  1.0f,

  1.0f,  // IB
  1.0f,
  1.0f,
  1.0f,

  0.0f,  // IR phase error (XHIGH, HIGH, MID, LOW)
  0.0f,
  0.0f,
  0.0f,

  0.0f,  // IY phase error
  0.0f,
  0.0f,
  0.0f,

  0.0f,  // IB phase error
  0.0f,
  0.0f,
  0.0f,

  1.0f,  // Solar Vol 240V (R, Y, B)
  1.0f,
  1.0f,

  1.0f,  // IR Solar (XHIGH, HIGH, MID, LOW)
  1.0f,
  1.0f,
  1.0f,

  1.0f,  // IY Solar
  1.0f,
  1.0f,
  1.0f,

  1.0f,  // IB Solar
  1.0f,
  1.0f,
  1.0f,

  0.0f,  // IR Solar phase error (XHIGH, HIGH, MID, LOW)
  0.0f,
  0.0f,
  0.0f,

  0.0f,  // IY Solar phase error
  0.0f,
  0.0f,
  0.0f,

  0.0f,  // IB Solar phase error
  0.0f,
  0.0f,
  0.0f,

  1.0f,  // FAN1, FAN2
  1.0f,

  // unused[12]
  //
  0.0f,
  0.0f,
  0.0f,
  0.0f,
  0.0f,
  0.0f,
  0.0f,
  0.0f,
  0.0f,
  0.0f,
  0.0f,
  0.0f,

  0,     // INIT_DATA1, INIT_DATA2
  0
};

#else

// Development build: reserve the calibration page address but emit no
// initialization bytes. Reflashing over SWD therefore leaves the existing
// calibration page untouched, preserving any prior field calibration.
//
// A fresh, never-calibrated board must be flashed at least once with
// SEED_CALIBRATION_DEFAULTS defined to seed the defaults; after that it can be
// recalibrated and then reflashed freely in dev mode.
//
__no_init const struct PH_COEFF_STRUCT CalibrationCoeff @ "FLASH_COEFF_SECTION";

#endif




 
     
