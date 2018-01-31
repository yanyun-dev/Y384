#ifndef SAMPLE_H
#define SAMPLE_H

#include "stm8s.h"
#include "stdbool.h"

uint16_t get_input_pwm(void);
void process_sample_input(void);
void Sample_init(void);
void endUpdate(void);
bool isNeedUpdate(void);

#endif