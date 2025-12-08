struct Parameter
{
  uint8_t             VariableIndex;
  const uint8_t       *DesArrayTop;
  uint16_t            MinValue;
  uint16_t            MaxValue;
  uint8_t             StepValue;
  uint8_t             DecimalPosition; 
  uint8_t             ParaMeterLength;
  uint8_t             DisableType;
  uint16_t            DataWithDecUnit;         // 1= Data is followed by Dec and next by Unit 2=Dec 3=Unit
  uint16_t            DefaultValue;
  
};
