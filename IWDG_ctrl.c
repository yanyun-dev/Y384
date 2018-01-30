/**
  ******************************************************************************
  * @file    IWDG_ctrl.c
  * @author  AWC_ERD_TEAM3
  * @version V1.0
  * @date    2018-1-17
  * @brief   µ÷ÊÔÄ£¿é
  ******************************************************************************
  */


/* include *************************************************/

#include "stm8s.h"
#include "stm8s_conf.h"

/* Private Micros ---------------------------------------------------------*/
#define  LsiFreq   128000

/** @addtogroup IWDG_ctrl
  * @{
  */


/**
  * @brief  Configures the IWDG to generate a Reset if it is not refreshed at the
  *         correct time. 
  * @param  None
  * @retval None
  */
void IWDG_Config(void)
{
  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
  
  /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  
  /* IWDG counter clock: LSI/128 */
  IWDG_SetPrescaler(IWDG_Prescaler_128);
  
  /* Set counter reload value to obtain 250ms IWDG Timeout.
    Counter Reload Value = 250ms/IWDG counter clock period
                         = 250ms / (LSI/128)
                         = 0.25s / (LsiFreq/128)
                         = LsiFreq/(128 * 4)
                         = LsiFreq/512
   */
  IWDG_SetReload((uint8_t)(LsiFreq/512));
  
  /* Reload IWDG counter */
  IWDG_ReloadCounter();
}

/**
  * @}
  */