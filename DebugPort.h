/**
  ******************************************************************************
  * @file    DebugPort.h
  * @author  AWC_ERD_TEAM3
  * @version V1.0
  * @date    2018-1-12
  * @brief   µ÷ÊÔÄ£¿é
  ******************************************************************************
  */

#ifndef DEBUG_PORT_H
#define DEBUG_PORT_H

#include "stm8s.h"
#include "stdbool.h"
#include "transform.h"

/** @addtogroup DebugPort
  * @{
  */

  /** @addtogroup DebugPort_Exported_Functions
    * @{
    */
void DebugPort_Init(void);
void DebugPort_Open(void);
void DebugPort_Close(void);
bool DebugPort_Write(const char * const buf, uint8_t bufLen);
bool DebugPort_WriteString(const char * const str);
bool DebugPort_WriteLine(const char * const str);
void DebugPort_Handler(void);

  /**
    * @}
    */

/**
  * @}
  */
#endif