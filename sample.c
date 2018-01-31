#include "sample.h"
#include "stm8s.h"
#include "stdio.h"

#define SAMPLE_NUMS 32
#define SAMPLE_SHIFT 5

typedef struct PWMStat{
  uint8_t InsertPos;
  bool isFull;
//  uint16_t IC1Values[SAMPLE_NUMS];
//  uint16_t IC2Values[SAMPLE_NUMS];
  uint32_t IC1Total;
  uint32_t IC2Total;
  uint16_t Result;
}PWMStat_t;

static PWMStat_t tPWMStat;

static void Debug_Out_Config(void);

//void process_sample_input(void)
//{
//  uint16_t Conversion_Value = 0;
//  while (0 == ADC1_GetFlagStatus(((ADC1_Flag_TypeDef)ADC1_IT_EOC)));
//  Conversion_Value = ADC1_GetConversionValue();
//  ADC1_ClearITPendingBit(ADC1_IT_EOC);
//  
//  if (Conversion_Value > 520 && LOW == timeStat.InputStatus)
//    {
//      timeStat.time[timeStat.curPos] = timecount;
//      timecount = 0;
//      timeStat.cutTime[timeStat.curPos] = timeStat.time[timeStat.curPos] - timeStat.preCutTime;
//      timeStat.totalTime += timeStat.time[timeStat.curPos];
//      timeStat.totalTime -= timeStat.time[(timeStat.curPos + 1) % SAMPLE_NUMS];
//      timeStat.cutTotalTime += timeStat.cutTime[timeStat.curPos];
//      timeStat.cutTotalTime -= timeStat.cutTime[(timeStat.curPos + 1) % SAMPLE_NUMS];
//      if (!timeStat.isFull && timeStat.curPos == (SAMPLE_NUMS - 1))
//      {
//        timeStat.isFull = 1;
//      }
//      timeStat.curPos = ((timeStat.curPos + 1) % SAMPLE_NUMS );
//      if (timeStat.isFull){
//         timeStat.curCompVal= ((timeStat.totalTime - timeStat.cutTotalTime)/timeStat.totalTime);
//
//      }
//      timeStat.InputStatus = HIGH;
//    }
//    else if(Conversion_Value <= 800 && HIGH == timeStat.InputStatus)
//    {
//      timeStat.preCutTime = timecount;
//      timeStat.InputStatus = LOW;
//    }
//      
//}
bool isNeedUpdate(void)
{
  return tPWMStat.isFull;
}

void endUpdate(void)
{
  tPWMStat.isFull = false;
   tPWMStat.IC2Total = 0;
    tPWMStat.IC1Total = 0;
    TIM1->IER |= 0x06;
}

uint16_t get_input_pwm(void)
{
    if (tPWMStat.isFull && tPWMStat.IC1Total >= tPWMStat.IC2Total)
    {
      if (tPWMStat.IC1Total>>1)
        tPWMStat.Result = (uint16_t)((tPWMStat.IC2Total << 12) / (tPWMStat.IC1Total>>1)); // 8 + 5
      else
        printf("error!\r\n");
//      tPWMStat.isFull = false;
//       TIM1->CR1 |= TIM1_CR1_CEN;
      printf("%ld:%ld\r\n", tPWMStat.IC2Total, tPWMStat.IC1Total);
    }
  return tPWMStat.Result;
}

   void Set_Beep_OptionByte(void)
   {
    /* Remap the PC7 as TIM1_CH2 */
  //unlock the flash
  do
  {
     FLASH->DUKR = 0xAE;
     asm( "nop" );
     FLASH->DUKR = 0x56;
     asm( "nop" );
  }while( !(FLASH->IAPSR & 0x08) );
  //enable writing the flash
  FLASH->CR2  = 0x80;
  FLASH->NCR2 = 0x7f;
  //operation of the option bytes
  *((u8*) 0x4803) = 0x03;
  *((u8*) 0x4804) = ~(0x03);
  //lock the flash
  FLASH->IAPSR &= 0xF7;
  }

void TIM1_PWM_Capture_Init(void)
{
  Set_Beep_OptionByte();
//  GPIOC->DDR &= 0xdf;       //PC1ÎªÊäÈë
//  GPIOC->CR1 |= 0x20;
//  GPIOC->CR2 &= 0xdf;
  GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_IN_FL_NO_IT);
  
//  Debug_Out_Config();
  
  GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_LOW_FAST);
#if 0
  TIM1_DeInit();
  TIM1_TimeBaseInit(1600, TIM1_COUNTERMODE_UP, 20000, 0);
  
  TIM1->CCER1 &= 0xee;
  TIM1->PSCRL |= 0x00;
  TIM1->PSCRH |= 0xff;
  TIM1->CCMR1 |= 0x01;
  TIM1->CCER1 &= 0xec;
  TIM1->CCMR2 |= 0x02;
  TIM1->CCER1 |= 0x20;
  TIM1->SMCR |= 0x50;
  TIM1->SMCR |= 0x04;
  TIM1->IER |= 0x06;
  TIM1->CCER1 |= 0x11;
  TIM1_Cmd(ENABLE);
#else
  TIM1_DeInit();
  TIM1_TimeBaseInit(8, TIM1_COUNTERMODE_UP, 0xffff, 0);
  TIM1->CCMR1 |= 0x01;
  TIM1->CCER1 &= (~0x02);
  TIM1->CCMR2 |= 0x02;
  
  TIM1->CCER1 |= 0x20;
  TIM1->SMCR |= 0x54;
  TIM1->CCMR1 |= 0xF0;
  TIM1->CCMR2 |= 0xF0;
  TIM1->CCER1 |= 0x11;
  TIM1->IER |= 0x06;
  
  TIM1_Cmd(ENABLE);
#endif
}

/**
  * @brief  Timer1 Capture/Compare Interrupt routine
  * @param None
  * @retval
  * None
  */
INTERRUPT_HANDLER(TIM1_CAP_COM_IRQHandler, 12)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  if (TIM1_GetITStatus(TIM1_IT_CC1))
  { 
    if (tPWMStat.InsertPos++ == SAMPLE_NUMS)
    {
      tPWMStat.InsertPos = 0;
      tPWMStat.isFull = true;
      //TIM1_Cmd(DISABLE);
      TIM1->IER |= 0x00;
    }
//    tPWMStat.IC1Total -= tPWMStat.IC1Values[tPWMStat.InsertPos];
//    tPWMStat.IC1Values[tPWMStat.InsertPos] += TIM1_GetCapture1();
//    tPWMStat.IC1Total += tPWMStat.IC1Values[tPWMStat.InsertPos];
    tPWMStat.IC1Total += TIM1_GetCapture1();
    GPIOB->ODR ^= (uint8_t)GPIO_PIN_5;
    TIM1_ClearITPendingBit(TIM1_IT_CC1);
  }
  if (TIM1_GetITStatus(TIM1_IT_CC2))
  {
//    tPWMStat.IC2Total -= tPWMStat.IC2Values[tPWMStat.InsertPos];
//    tPWMStat.IC2Values[tPWMStat.InsertPos] = TIM1_GetCapture2();
//    tPWMStat.IC2Total += tPWMStat.IC2Values[tPWMStat.InsertPos];
    tPWMStat.IC2Total += TIM1_GetCapture2();
//    GPIOB->ODR |= (uint8_t)(~GPIO_PIN_5);
    GPIOB->ODR ^= (uint8_t)(GPIO_PIN_5);
    TIM1_ClearITPendingBit(TIM1_IT_CC2);
  }
  return;
}


void Sample_init(void)
{
  TIM1_PWM_Capture_Init();
}


void Debug_Out_Config(void)
{
  GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_SLOW);
}