/*
******************************************************************************
  * @file    comm.h
  * @author  MRM R&D Team
  * @version V1.0.0
  * @date    07-04-2022
  * @brief   This file contains all the functions prototypes and Variables 
             for the meter communication
******************************************************************************
*/

#ifndef COMM_H
#define COMM_H

#include "CommFlagDef.h"
#include <Math.h>
#include "Struct.h"
#include "Struct_edit.h"
#include "extern_includes.h"
#ifdef MODBUS_MAP_PROCOM
    #include "ModBus.h"
#endif

#include "stm32f37x_flash.h"

/* Private Functions Declarations */

uint16_t ModBusCRCCalculation(uint8_t * str,uint8_t length);
void SendData_UART(uint8_t Add, uint8_t Func,  uint8_t Send_Length);
void CalPF(float Error, float * CalGainBufferPointer,float * CalBetaBufferPointer);
void DirectCalibration(void);
void ModbusUpdateParameter(uint16_t Address,uint16_t NoOfBytes);
void CheckPasswordEdit(uint16_t Address,uint16_t NoOfBytes);

/* Private Variables Declarations */

uint8_t Add_Received=0xFF,Fun_Received,CRC_Received_Low,CRC_Received_High;
float CalVolR,CalVolY,CalVolB,CalCurR,CalCurY,CalCurB;
float CalVolRSolar,CalVolYSolar,CalVolBSolar,CalCurRSolar,CalCurYSolar,CalCurBSolar;
float CalPowR,CalPowY,CalPowB;
float CalPowRSolar,CalPowYSolar,CalPowBSolar;
float CalFan1Current, CalFan2Current;
struct Field Mod_TransmitFrame;
uint8_t NoOfBytes_Low,NoOfBytes_High,Start_Add_Low,Start_Add_High;
uint8_t AvailableByte, ArrayIndex, DataLengthRegister,DataArrLoc;
uint8_t Data_Received[MAX_BYTE_TO_RECIEVE];
uint16_t CRC_Received, CRC_Calculated,ByteCount,Start_Add;
uint16_t Temp_Start_Add=0;


#endif
