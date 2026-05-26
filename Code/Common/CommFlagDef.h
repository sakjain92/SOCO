#include <stddef.h>
#include <intrinsics.h>
// Modbus 0x15 (Write File Record) with 200-byte payload = 212 bytes on the wire
#define MAX_BYTE_TO_RECIEVE  220

#include "FlagDef_DIN.h"
#include "DIN_MOD_MODEL.h" 
#include "Model_DIN.h"

