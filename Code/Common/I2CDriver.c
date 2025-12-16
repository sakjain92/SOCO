#include "stm32f37x.h"
#include "CommFlagDef.h"



void I2CRead(uint16_t DataLocation, uint8_t NoOfBytes, uint8_t DeviceAddress, uint8_t *DataArray );
void I2CWrite(uint16_t DataLocation,uint8_t NoOfBytes,uint8_t DeviceAddress,uint8_t *DataArray );
void CheckEpromFree(uint8_t DeviceAddress);


// UNDONE: Unconditional while loops should be avoided in this codebase everywhere
//
void I2CRead(uint16_t DataLocation, uint8_t NoOfBytes, uint8_t DeviceAddress, uint8_t *DataArray )
{
  
  uint16_t TempVariable;
  uint32_t counter = 0;
  //TempVariable  = DataLocation & 0xFFFF0000;
  TempVariable  = DataLocation & 0xFFFF;
  I2C2->CR1 |= I2C_CR1_PE;                     //PE enable

  I2C2->CR2 = (uint8_t)DeviceAddress;
  I2C2->CR2 &=~I2C_CR2_RD_WRN;               //Direction
  I2C2->CR2 |=  I2C_CR2_START+0x20000;                //Generate Start and no of bytes to transmit(2)
  counter = 0;
  while(I2C2->CR2 & I2C_CR2_START)
  {
    counter++;
    if (counter > 100000)
    {
        break;
    }
  }

  I2C2->TXDR = TempVariable >> 8;
  
  counter = 0;
  while(!(I2C2->ISR & I2C_ISR_TXE))
  {
    counter++;
    if (counter > 100000)
    {
        break;
    }
  }

  I2C2->TXDR = (uint8_t)DataLocation;
  counter = 0;
  while(!(I2C2->ISR & I2C_ISR_TXE))
  {
    counter++;
    if (counter > 100000)
    {
        break;
    }
  }

  I2C2->CR2 |= I2C_CR2_RD_WRN;               //Direction change
  I2C2->CR2 |= (uint32_t)(NoOfBytes * 65536) & 0xFF0000;
  I2C2->CR2 |= I2C_CR2_START;                //Generate Start
  counter = 0;
  while(I2C2->CR2 & I2C_CR2_START)
  {
    counter++;
    if (counter > 100000)
    {
        break;
    }
  }

  for(TempVariable = 0; TempVariable < NoOfBytes; TempVariable++)
  {
    counter = 0;
    while(!(I2C2->ISR & I2C_ISR_RXNE))
    {
      counter++;
      if (counter > 100000)
      {
          break;
      }
    }

    *(DataArray+TempVariable) = I2C2->RXDR; 
  }
  I2C2->CR2 |= I2C_CR2_STOP;
  //Stop
  counter = 0;
  while(!(I2C2->ISR & I2C_ISR_STOPF))
  {
    counter++;
    if (counter > 100000)
    {
      break;
    }
  }
  //Busy
  counter = 0;
  while(I2C2->ISR & I2C_ISR_BUSY)
  {
    counter++;
    if (counter > 100000)
    {
      break;
    }
  }

  I2C2->CR1 &=~I2C_CR1_PE;   
}

void EepromRead(uint16_t DataLocation, uint16_t NoOfBytes, uint8_t DeviceAddress, uint8_t *DataArray)
{
  while (NoOfBytes)
  {
    uint8_t toRead = NoOfBytes > 64 ? 64 : NoOfBytes;
    I2CRead(DataLocation,toRead,DeviceAddress,DataArray);
    DataLocation += toRead;
    NoOfBytes -= toRead;
    DataArray += toRead;
  }
}


void I2CWrite(uint16_t DataLocation,uint8_t NoOfBytes,uint8_t DeviceAddress,uint8_t *DataArray )
{
  uint16_t TempVariable;
  uint32_t TempBytes;
  uint32_t counter = 0;
  TempVariable  = DataLocation & 0xFFFF;
  TempBytes = (uint32_t)NoOfBytes+2 << 16;
  
  I2C2->CR1 |= I2C_CR1_PE;                     //PE enable

  I2C2->CR2 = (uint8_t)DeviceAddress;
  I2C2->CR2 &=~ I2C_CR2_RD_WRN;               //Direction
  I2C2->CR2 |= TempBytes;
  I2C2->CR2 |= I2C_CR2_START;                //Generate Start
  counter = 0;
  while(I2C2->CR2 & I2C_CR2_START)
  {
    counter++;
    if (counter > 100000)
    {
      break;
    }
  }

  I2C2->TXDR = TempVariable >> 8;
  counter = 0;
  while(!(I2C2->ISR & I2C_ISR_TXE))
  {
    counter++;
    if (counter > 100000)
    {
      break;
    }
  }

  I2C2->TXDR = (uint8_t)DataLocation;
  counter = 0;
  while(!(I2C2->ISR & I2C_ISR_TXE))
  {
    counter++;
    if (counter > 100000)
    {
      break;
    }
  }

  for(TempVariable = 0; TempVariable < NoOfBytes; TempVariable++)
  {
    I2C2->TXDR = *(DataArray+TempVariable);
    counter = 0;
    while(!(I2C2->ISR & I2C_ISR_TXE))
    {
      counter++;
      if (counter > 100000)
      {
        break;
      }
    }
  }
  I2C2->CR2 |= I2C_CR2_STOP;                  
  //Stop
  counter = 0;
  while(!(I2C2->ISR & I2C_ISR_STOPF))
  {
    counter++;
    if (counter > 100000)
    {
      break;
    }
  }

  //Busy
  counter = 0;
  while(I2C2->ISR & I2C_ISR_BUSY)
  {
    counter++;
    if (counter > 100000)
    {
      break;
    }
  }
  I2C2->CR1 &=~I2C_CR1_PE;
}

void CheckEpromFree(uint8_t DeviceAddress)
{
  uint32_t Temp=0;
  I2C2->CR2=DeviceAddress+0x02000000;
  I2C2->CR1 |= I2C_CR1_PE;
  uint32_t counter = 0;

  do
  {
    I2C2->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF;
    I2C2->CR2 |= I2C_CR2_START;
    counter = 0;
    while(I2C2->CR2 & I2C_CR2_START)
    {
        counter++;
        if (counter > 100000)
        {
            break;
        }
    }

    Temp++;
    if (Temp > 100000)
    {
        break;
    }
  }
  while(I2C2->ISR & I2C_ISR_NACKF);
  I2C2->CR1 &= ~I2C_CR1_PE;
}

  
void EepromWrite(uint16_t DataLocation,uint16_t NoOfBytes,uint8_t DeviceAddress,uint8_t *DataArray )
{
  while (NoOfBytes)
  {
    uint8_t toWrite = NoOfBytes > 64 ? 64 : NoOfBytes;
    I2CWrite(DataLocation,toWrite,DeviceAddress,DataArray);
    CheckEpromFree(EXT_EEPROM);
    DataLocation += toWrite;
    NoOfBytes -= toWrite;
    DataArray += toWrite;
  }
}

  
