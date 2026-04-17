
#include "stm32f37x.h"
#include <stdio.h>
#include "Struct.h"

#pragma section = "FLASH_COEFF_SECTION"

#ifdef SEED_CALIBRATION_DEFAULTS

// Factory/release build: burn unit-gain defaults into the calibration page
// so a never-calibrated board still boots. Overwritten at the end of
// Cal Mode with the actual measured coefficients.
//
const struct PH_COEFF_STRUCT CalibrationCoeff @ "FLASH_COEFF_SECTION"=
{
  1.0f,  // Vol 240V
  1.0f,
  1.0f,

  1.0f,  // R
  1.0f,
  1.0f,

  1.0f,  // Y
  1.0f,
  1.0f,

  1.0f,  // b
  1.0f,
  1.0f,

  0.0f,
  0.0f,
  0.0f,

  0.0f,
  0.0f,
  0.0f,

  0.0f,
  0.0f,
  0.0f,

  1.0f,  // Vol 240V
  1.0f,
  1.0f,

  1.0f,  // R
  1.0f,
  1.0f,

  1.0f,  // Y
  1.0f,
  1.0f,

  1.0f,  // b
  1.0f,
  1.0f,

  0.0f,
  0.0f,
  0.0f,

  0.0f,
  0.0f,
  0.0f,

  0.0f,
  0.0f,
  0.0f,

  1.0f,
  1.0f,

  // Unused
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

  0,
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




 
     
