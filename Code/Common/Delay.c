
#include "stm32f37x.h"
#include <stdio.h>
#include <intrinsics.h>

void Delay1Msec12Mhz( uint32_t Delay);
void Delay10Usec12Mhz(uint32_t Delay);
void Delay1Usec12Mhz(void)  ; 


#pragma optimize=none
void Delay1Msec12Mhz( uint32_t Delay)
{
 Delay=Delay*857;
  while(Delay>0)
  {
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    Delay--;
  }
}

#pragma optimize=none
void Delay10Usec12Mhz(uint32_t Delay)  // At 1 its 2 Usec and above 50Usec its accurate
{ 
   while(Delay)
   {
     Delay--;
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();

   }
}
#pragma optimize=none
void Delay1Usec12Mhz(void)  
{ 

    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
}





