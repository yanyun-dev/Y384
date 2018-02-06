#include "PWM_out.h"
#include "stm8s_conf.h"
#include "stm8s.h"

static void PWM_Out_Config(void);

void PWM_Init(void)
{
  PWM_Out_Config();
}

void set_out_pwm(uint16_t rate)
{
  TIM2_SetCompare3(rate);
}

static void PWM_Out_Config(void)
{
   TIM2_TimeBaseInit(TIM2_PRESCALER_1, 0xFFFF >> 4);
   TIM2_OC3Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 0, TIM2_OCPOLARITY_HIGH);
  TIM2_OC3PreloadConfig(ENABLE);

  TIM2_ARRPreloadConfig(ENABLE);
  TIM2_Cmd(ENABLE);
}

