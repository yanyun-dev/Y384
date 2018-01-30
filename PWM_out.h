#ifndef PWM_OUT_H
#define PWM_OUT_H

#include "stm8s.h"

void PWM_Init(void);
void set_out_pwm(uint16_t rate);

#endif