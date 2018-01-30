/**
  ******************************************************************************
  * @文件    main.c
  * @作者  AWC_ERD_TEAM3
  * @版本 V1.0
  * @日期    2018-1-12
  * @简述   主程序
  ******************************************************************************
  */


/**
  * @简述  主函数.
  * @参数  无
  * @返回值 无
  */
/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "stdio.h"
#include "DebugPort.h"
#include "assert.h"
#include "transform.h"

#define PUTCHAR_PROTOTYPE int putchar (int c)
#define GETCHAR_PROTOTYPE int getchar (void)

int main( void )
{
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  DebugPort_Init();
  DebugPort_Open();
  transform_init();
  enableInterrupts(); 
  IWDG_Config();
  while(1)
  {
    /* Reload IWDG counter */
    IWDG_ReloadCounter();  
    DebugPort_Handler();
    transfrom_handle();
  }
  return 0;
}

PUTCHAR_PROTOTYPE
{
  /* Write a character to the UART1 */
  char d = (char)c;
  DebugPort_Write(&d, 1);
  
  return (c);
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    
    /* Infinite loop */
    
    printf("%s:%d\r\n", (char *)file, line);
    while (1)
    {
    }
}
#endif