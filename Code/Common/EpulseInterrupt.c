#include "CommFlagDef.h"
#include "Struct.h"
#include "stm32f37x_flash.h"
#include <Math.h>
#include "extern_includes.h"



void Process1SecInterrupt(void)
{
    OneSecFlag |=ONE_SEC_FLAG_1SEC_OVER;
}




void ProcessFreqCapture(void)
{
  uint16_t FrequencyCount,PresentCaptureCount;
  PresentCaptureCount=TIM3->CCR4;
  if(CaptureSumCounter==0)PreviousCaptureCount=PresentCaptureCount;
  else
  {
    if(PresentCaptureCount<PreviousCaptureCount)
    {
      FrequencyCount=65536-(PreviousCaptureCount-PresentCaptureCount);
    }
    else FrequencyCount=PresentCaptureCount-PreviousCaptureCount;
    PreviousCaptureCount=PresentCaptureCount;
    
    CaptureValueSum +=FrequencyCount;
    if(CaptureSumCounter==4)
    {
      CaptureValueSav=(uint16_t)(CaptureValueSum>>2);
      CaptureSumCounter=0;
      CaptureValueSum=0;
      return;
    }
  }
  CaptureSumCounter++;
}

