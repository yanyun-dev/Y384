#ifndef TIMER_H
#define TIMER_H

#include "stm8s.h"

extern uint8_t timer_flag;
void timer_init(void);
void timer_start(void);
void timer_stop(void);

#endif