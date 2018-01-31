/**
  ******************************************************************************
  * @�ļ�    transform.c
  * @����  AWC_ERD_TEAM3
  * @�汾 V1.0
  * @����    2018-1-12
  * @����   ����ģ��
  ******************************************************************************
  */

#include "transform.h"
#include "stm8s.h"
#include "stdbool.h"
#include "assert.h"
#include "stdio.h"
#include "sample.h"
#include "PWM_out.h"

#define TRANSFORM_TABLE_SIZE 256

typedef struct {
  uint16_t index_record;
  uint16_t table[TRANSFORM_TABLE_SIZE];
} TransTable_t;

TransTable_t tTransTable = {
  .index_record = 0,
  .table = {																				
0,	2827,	3855,	4626,	5397,	5911,	6682,	7196,	7453,	7967,	8481,	8738,	9252,	9509,	10023,	10280,	10794,	11051,	11308,	11565,	12079,
12336,	12593,	12850,	13107,	13364,	13621,	13878,	14135,	14392,	14649,	14906,	15163,	15420,	15677,	15934,	16191,	16448,	16705,	16962,	17219,
17219,	17476,	17733,	17990,	18247,	18504,	18761,	18761,	19018,	19275,	19532,	19789,	19789,	20046,	20303,	20560,	20560,	20817,	21074,	21331,	
21331,	21588,	21845,	22102,	22102,	22359,	22616,	22873,	22873,	23130,	23387,	23387,	23644,	23901,	24158,	24158,	24415,	24672,	24672,	24929,	
25186,	25186,	25443,	25700,	25700,	25957,	26214,	26214,	26471,	26728,	26728,	26985,	27242,	27242,	27499,	27756,	27756,	28013,	28270,	28270,	
28527,	28784,	28784,	29041,	29298,	29298,	29555,	29812,	29812,	30069,	30069,	30326,	30583,	30583,	30840,	31097,	31097,	31354,	31611,	31611,	
31868,	31868,	32125,	32382,	32382,	32639,	32896,	32896,	33153,	33410,	33410,	33667,	33924,	33924,	34181,	34181,	34438,	34695,	34695,	34952,	
35209,	35209,	35466,	35723,	35723,	35980,	35980,	36237,	36494,	36494,	36751,	37008,	37008,	37265,	37522,	37522,	37779,	38036,	38036,	38293,	
38550,	38550,	38807,	39064,	39064,	39321,	39578,	39578,	39835,	40092,	40092,	40349,	40606,	40606,	40863,	41120,	41120,	41377,	41634,	41634,	
41891,	42148,	42405,	42405,	42662,	42919,	42919,	43176,	43433,	43690,	43690,	43947,	44204,	44461,	44461,	44718,	44975,	45232,	45232,	45489,	
45746,	46003,	46003,	46260,	46517,	46774,	47031,	47031,	47288,	47545,	47802,	48059,	48316,	48573,	48573,	48830,	49087,	49344,	49601,	49858,	
50115,	50372,	50629,	50886,	51143,	51400,	51657,	51914,	52171,	52428,	52685,	52942,	53199,	53456,	53713,	54227,	54484,	54741,	54998,	55512,	
55769,	56283,	56540,	57054,	57311,	57825,	58339,	58596,	59110,	59881,	60395,	61166,	61937,	62965,	65535						
}
};

void transform_init(void)
{
  PWM_Init();
  Sample_init();
}


void transfrom_handle(void)
{
  uint8_t quotient, remainder;
  if (isNeedUpdate())
  {
    tTransTable.index_record = get_input_pwm();
    quotient = (uint8_t)(tTransTable.index_record >> 5);
    remainder = (uint8_t)(tTransTable.index_record & (uint16_t)0x1F);
    assert(tTransTable.index_record < (uint16_t)16384);
#if 1
//  set_out_pwm(tTransTable.table[tTransTable.index_record]);
    if (tTransTable.table[quotient + 1] > tTransTable.table[quotient])
      TIM2_SetCompare3(tTransTable.table[quotient] + ((uint16_t)((tTransTable.table[quotient + 1] - tTransTable.table[quotient])>>3) * (remainder >> 2)));
    else
      TIM2_SetCompare3(tTransTable.table[quotient] + ((uint16_t)((tTransTable.table[quotient] - tTransTable.table[quotient + 1])>>3) * (remainder >> 2)));
#else
  set_out_pwm(tTransTable.index_record << 9 - 1);
#endif
    endUpdate();
  }
}


#if defined(DEBUG)
void print_table(void)
{
  uint8_t start = (tTransTable.index_record >> 3) << 3;
  printf("%d: ", start);
  for (uint8_t i = 0; i < 8; ++i)
  {
    printf("%d ",tTransTable.table[start + i]); 
  }
  printf("\r\n");
}

uint8_t table_index_get(void)
{
  return tTransTable.index_record;
//  printf("%d\r\n", (uint16_t)tTransTable.index_record);
}
bool table_index_value_set(uint16_t rate)
{
  tTransTable.table[tTransTable.index_record] = rate;
  return true;
//  printf("Ok!\r\n");
}
uint16_t table_index_value_get(void)
{
  return tTransTable.table[tTransTable.index_record];
//  printf("%d\r\n", (uint16_t)tTransTable.table[tTransTable.index_record]);
}
#endif