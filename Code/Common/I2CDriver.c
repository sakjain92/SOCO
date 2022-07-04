#include "stm32f37x.h"
#include "CommFlagDef.h"



void I2CRead(uint16_t DataLocation, uint8_t NoOfBytes, uint8_t DeviceAddress, uint8_t *DataArray );
void I2CWrite(uint16_t DataLocation,uint8_t NoOfBytes,uint8_t DeviceAddress,uint8_t *DataArray );
void CheckEpromFree(uint8_t DeviceAddress);



void I2CRead(uint16_t DataLocation, uint8_t NoOfBytes, uint8_t DeviceAddress, uint8_t *DataArray )
{
  
  uint16_t TempVariable;
  //TempVariable  = DataLocation & 0xFFFF0000;
  TempVariable  = DataLocation & 0xFFFF;
  I2C2->CR1 |= I2C_CR1_PE;                     //PE enable

  I2C2->CR2 = (uint8_t)DeviceAddress;
  I2C2->CR2 &=~I2C_CR2_RD_WRN;               //Direction
  I2C2->CR2 |=  I2C_CR2_START+0x20000;                //Generate Start and no of bytes to transmit(2)
  while(I2C2->CR2 & I2C_CR2_START);
  I2C2->TXDR = TempVariable >> 8;
  
  while(!(I2C2->ISR & I2C_ISR_TXE));
  I2C2->TXDR = (uint8_t)DataLocation;
  while(!(I2C2->ISR & I2C_ISR_TXE));
  
  I2C2->CR2 |= I2C_CR2_RD_WRN;               //Direction change
  I2C2->CR2 |= (uint32_t)(NoOfBytes * 65536) & 0xFF0000;
  I2C2->CR2 |= I2C_CR2_START;                //Generate Start
  while(I2C2->CR2 & I2C_CR2_START);
  for(TempVariable = 0; TempVariable < NoOfBytes; TempVariable++)
  {
    while(!(I2C2->ISR & I2C_ISR_RXNE));
    *(DataArray+TempVariable) = I2C2->RXDR; 
  }
  I2C2->CR2 |= I2C_CR2_STOP;
  //Stop
  TempVariable=0;
  while(!(I2C2->ISR & I2C_ISR_STOPF))
  {
    ++TempVariable;
    if (TempVariable > 10000) 
    {
      break;
    }
     
  }
  //Busy
  TempVariable=0;
  while(I2C2->ISR & I2C_ISR_BUSY)
  {
    ++TempVariable;
    if (TempVariable > 10000) 
    {
      break;
    }
  }
  I2C2->CR1 &=~I2C_CR1_PE;   
  
}


void I2CWrite(uint16_t DataLocation,uint8_t NoOfBytes,uint8_t DeviceAddress,uint8_t *DataArray )
{
  uint16_t TempVariable;
  uint32_t TempBytes;
  TempVariable  = DataLocation & 0xFFFF;
  TempBytes = (uint32_t)NoOfBytes+2 << 16;
  
  I2C2->CR1 |= I2C_CR1_PE;                     //PE enable

  I2C2->CR2 = (uint8_t)DeviceAddress;
  I2C2->CR2 &=~ I2C_CR2_RD_WRN;               //Direction
  I2C2->CR2 |= TempBytes;
  I2C2->CR2 |= I2C_CR2_START;                //Generate Start
  while(I2C2->CR2 & I2C_CR2_START);
  I2C2->TXDR = TempVariable >> 8;
  while(!(I2C2->ISR & I2C_ISR_TXE));
  I2C2->TXDR = (uint8_t)DataLocation;
  while(!(I2C2->ISR & I2C_ISR_TXE));
  for(TempVariable = 0; TempVariable < NoOfBytes; TempVariable++)
  {
    I2C2->TXDR = *(DataArray+TempVariable);
    while(!(I2C2->ISR & I2C_ISR_TXE));
  }
  I2C2->CR2 |= I2C_CR2_STOP;                  
  //Stop
  TempVariable=0;
  while(!(I2C2->ISR & I2C_ISR_STOPF))
  {
    ++TempVariable;
    if (TempVariable > 10000) 
    {
      break;
    }
     
  }
  //Busy
  TempVariable=0;
  while(I2C2->ISR & I2C_ISR_BUSY)
  {
    ++TempVariable;
    if (TempVariable > 10000) 
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

  do
  {
    I2C2->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF;
    I2C2->CR2 |= I2C_CR2_START;
    while(I2C2->CR2 & I2C_CR2_START);
    Temp++;
    
  }
  while(I2C2->ISR & I2C_ISR_NACKF);
  I2C2->CR1 &= ~I2C_CR1_PE;
}

  
void EepromWrite (uint16_t DataLocation,uint8_t NoOfBytes,uint8_t DeviceAddress,uint8_t *DataArray )

{
  I2CWrite(DataLocation,NoOfBytes,DeviceAddress,DataArray );
  CheckEpromFree(EXT_EEPROM);
}

  
