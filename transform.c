/**
  ******************************************************************************
  * @文件    transform.c
  * @作者  AWC_ERD_TEAM3
  * @版本 V1.0
  * @日期    2018-1-12
  * @简述   控制模块
  ******************************************************************************
  */

#include "transform.h"
#include "stm8s.h"
#include "stdbool.h"
#include "assert.h"
#include "stdio.h"
#include "sample.h"
#include "PWM_out.h"

#define CHANGE_POINT 4400
#define TRANSFORM_TABLE_SIZE 256
#define DIFF_VAL_H ((65536/2)/(6254-CHANGE_POINT))
#define DIFF_VAL_L ((65536/2-3276)/(CHANGE_POINT - 1720))

typedef struct {
  uint16_t index_record;
  uint16_t current_pwm;
  uint16_t table[TRANSFORM_TABLE_SIZE];
} TransTable_t;

TransTable_t tTransTable = {
  .index_record = 0,
  .current_pwm = 0,
#if 0
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
#else
  .table = {																				
3238			,3238				,3238				,3238				,3238				,3238				,3238				,3238	     ,
3238			,3238				,3238				,3238				,3238				,3238				,3238				,3238	     ,
3238			,3238				,3238				,3238				,3238				,3238				,3238				,3238	     ,
3238			,3238				,3238				,3238				,3238				,3238				,3238				,3238	     ,
3238			,3238				,3238				,3238				,3238				,3238				,3238				,3238	     ,
3238			,3238				,3238				,3238				,3238				,3238				,3238				,3238	     ,
3238			,3238				,3238				,3238				,3238				,3238				,3238				,3238	     ,
3238			,3238				,3238				,3238				,3238				,3238				,3238				,3238	     ,
3238			,3238				,3238				,3238				,3238				,3238				,3238				,3238	     ,
3238			,3238				,3238				,3238				,3238       	,3238			,3817			,4396	   ,
4975		,5554			,6133			,6712			,7291			,7870			,8449			,9028	   ,
9607		,10186		,10765		,11344		,11923		,12502		,13081		,13660	 ,
14239		,14818		,15397		,15976		,16555		,17134		,17713		,18292	 ,
18871		,19450		,20029		,20608		,21187		,21766		,22345		,22924	 ,
23503		,24082		,24661		,25240		,25819		,26398		,26977		,27556	 ,
28135		,28714		,29293		,29872		,30451		,31030		,31609		,32188	 ,
32767		,32767 		,33410 		,34052 		,34695 		,35337 		,35980 		,36622 	 ,
37265 	,37907 		,38550 		,39192 		,39835 		,40477 		,41120 		,41762 	 ,
42405 	,43047 		,43690 		,44332 		,44975 		,45617 		,46260 		,46902 	 ,
47545 	,48187 		,48830 		,49472 		,50115 		,50757 		,51400 		,52042 	 ,
52685 	,53327 		,53970 		,54612 		,55255 		,55897 		,56540 		,57182 	 ,
57825 	,58467 		,59110 		,59752 		,60395 		,61037 		,61680 		,62322 	 ,
62965 	,63607 		,64250 		,64892 		,65535 		,65535		,65535		,65535   , 
65535 	,65535		,65535		,65535		,65535 		,65535		,65535		,65535   ,
65535 	,65535		,65535		,65535		,65535 		,65535		,65535		,65535   ,
65535 	,65535		,65535		,65535		,65535 		,65535		,65535		,65535   ,
65535 	,65535		,65535		,65535		,65535 		,65535		,65535		,65535   ,
65535 	,65535		,65535		,65535		,65535 		,65535		,65535		,65535   ,
65535 	,65535		,65535		,65535		,65535 		,65535		,65535		,65535   ,
65535 	,65535		,65535		,65535		,65535 		,65535		,65535		,65535   ,
65535 	,65535		,65535		,65535		,65535 		,65535		,65535		,65535   ,
65535 	,65535		,65535		,65535		,65535 		,65535		,65535		,65535  
  }
  
#endif
};

void transform_init(void)
{
  PWM_Init();
  Sample_init();
}


void transfrom_handle(void)
{
  uint8_t quotient, remainder, direction, inc_count;
  uint16_t  gap;
  uint16_t result, target_pwm;
  static uint16_t pwm_record;
  if (isNeedUpdate())
  {
    result = get_input_pwm();
    tTransTable.index_record = result;
    if(result <= 2866)
      target_pwm = 3276;
    else if(result >= 5734)
      target_pwm = 65535;
    else if(result <= 4096)
      target_pwm = 3276 + 18 * (result - 2866);
    else
      target_pwm = 26214 + 24 * (result - 4096);
    
    quotient = (uint8_t)(result >> 5);
    remainder = (uint8_t)(result & (uint16_t)0x1F);
//    target_pwm = tTransTable.table[quotient] + ((tTransTable.table[quotient + 1] - tTransTable.table[quotient])>>5)*remainder;
//    target_pwm = 
    gap = target_pwm > tTransTable.current_pwm ? ( direction = 1, target_pwm - tTransTable.current_pwm) : ( direction = 0, tTransTable.current_pwm - target_pwm);
    if (gap>>4)
    {
//      printf("%d : %d\r\n", result, gap);
      if (direction)
      {
//        inc_count = tTransTable.current_pwm + gap >> 3;
//        inc_count = tTransTable.table[tTransTable.index_record>>5] + (tTransTable.table[quotient] - tTransTable.table[tTransTable.index_record>>5])>>3;
        tTransTable.current_pwm += (gap >> 3);
//        tTransTable.index_record += inc_count; 
      }
      else
      {
//        inc_count = tTransTable.table[tTransTable.index_record>>5] - (tTransTable.table[tTransTable.index_record>>5] - tTransTable.table[quotient])>>3;
//        tTransTable.index_record -= inc_count;
        tTransTable.current_pwm -= (gap >> 3);
      }
//      tTransTable.index_record = result;
//    assert(tTransTable.index_record < (uint16_t)16384);
#if 1
      if(tTransTable.current_pwm > 63324)
      {
        tTransTable.current_pwm = 65535;
        TIM2_SetCompare3(65535);
      }
      else
        TIM2_SetCompare3((tTransTable.current_pwm)>>4);
//  set_out_pwm(tTransTable.table[tTransTable.index_record]);
//    if(quotient > tTransTable.current_pwm)
//      TIM2_SetCompare3(tTransTable.table[++tTransTable.current_pwm]>>4);
//    else if (quotient < tTransTable.current_pwm)
//    {   
//     TIM2_SetCompare3(tTransTable.table[--tTransTable.current_pwm]>>4);
//    }
//    else
//      ;
//    if (tTransTable.table[quotient + 1] > tTransTable.table[quotient])
//      TIM2_SetCompare3((tTransTable.table[quotient] + ((uint16_t)((tTransTable.table[quotient + 1] - tTransTable.table[quotient])>>3) * (remainder >> 2)))>>4);
//    else
//      TIM2_SetCompare3((tTransTable.table[quotient] + ((uint16_t)((tTransTable.table[quotient] - tTransTable.table[quotient + 1])>>3) * (remainder >> 2)))>>4);
#else
  if(result >= 6254)
    TIM2_SetCompare3(0xFFFF);
  else if(result <= 1720)
    TIM2_SetCompare3(3276>>4);
  else if(result >=CHANGE_POINT)
  {
//    target_pwm = ((uint16_t)(((uint16_t)(result - CHANGE_POINT))*((uint16_t)DIFF_VAL_H)) + (uint16_t)(65536>>1))>>4;
//    if (target_pwm > pwm_record)
//      TIM2_SetCompare3(pwm_record++);
//    else
//      TIM2_SetCompare3(pwm_record--);
    TIM2_SetCompare3(((uint16_t)(((uint16_t)(result - CHANGE_POINT))*((uint16_t)DIFF_VAL_H)) + (uint16_t)(65536>>1))>>4);
  }
  else
  {
//    target_pwm = (((uint16_t)(result - 1720))*((uint16_t)DIFF_VAL_L) + 3276)>>4;
//     if (target_pwm > pwm_record)
//      TIM2_SetCompare3(pwm_record++);
//    else
//      TIM2_SetCompare3(pwm_record--);
    TIM2_SetCompare3((((uint16_t)(result - 1720))*((uint16_t)DIFF_VAL_L) + 3276)>>4);
  } 
#endif
    }
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

uint16_t table_index_get(void)
{
  return get_input_pwm();
//  printf("%d\r\n", (uint16_t)tTransTable.index_record);
}
bool table_index_value_set(uint16_t rate)
{
  tTransTable.table[tTransTable.index_record>>5] = rate;
  return true;
//  printf("Ok!\r\n");
}
uint16_t table_index_value_get(void)
{
  return tTransTable.table[tTransTable.index_record];
//  printf("%d\r\n", (uint16_t)tTransTable.table[tTransTable.index_record]);
}
#endif