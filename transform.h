#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "stm8s.h"
#include "stdbool.h"

#if defined(DEBUG)
uint16_t table_index_get(void);
bool table_index_value_set(uint16_t rate);
uint16_t table_index_value_get(void);
void print_table(void);
#endif
void transfrom_handle(void);
void transform_init(void);

#endif
