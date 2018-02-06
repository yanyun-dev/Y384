/**
  ******************************************************************************
  * @file    DebugPort.c
  * @author  AWC_ERD_TEAM3
  * @version V1.0
  * @date    2018-1-12
  * @brief   ����ģ��
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "DebugPort.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "assert.h"

/** @addtogroup DebugPort
  * @{
  */

  /** @addtogroup DebugPort_Private_Functions
    * @{
    */
static void DebugPort_GetCurPWMRateIn(void);
static void table_item_value_display(void);
static void DebugPort_SetCurPWMRateOut(void);
static void table_display(void);
static void DebugPort_UnknownCmd(void);

  /**
  * @}
  */

/**
  * @brief  ���������������С
  */
#define DEBUG_PORT_MAX_TX_BYTES 40 /*!< �����������С*/
#define DEBUG_PORT_MAX_RX_BYTES 10  /*!< ���뻺������С */


#define DEBUG_PORT_EOL_DELIMITER 0x0D /*!< ֡������־�� */


/** 
  * @brief  ����ģ����ƽṹ��  
  */
typedef struct 
{
   uint8_t txBufCurrOutPos; /*!< ���ͻ��������λ�� */
   __IO uint8_t txBufCurrInPos; /*!< ���ͻ���������λ�� */
   __IO uint8_t rxBufCurrPos : 4; /*!< ���ջ���������λ�� */
   uint8_t rxdFrameLen : 4; /*!< ֡���� */
   __IO uint8_t rxdNewPacket : 1; /*!< �����°���־ */
   __IO uint8_t transmittingData :1;  /*!< �������ݱ�־*/
   uint8_t printingHelp : 1;  /*!< ��������λ */
    __IO uint8_t txBuf[DEBUG_PORT_MAX_TX_BYTES]; /*!< ���ͻ����� */
    __IO uint8_t rxBuf[DEBUG_PORT_MAX_RX_BYTES];  /*!< ���ջ����� */
    int16_t currTimeoutVal_ms;  /*!< ��ʱ���� */
}DebugPortCtrl_t;

/** 
  * @brief  ��������  
  */
typedef enum 
{
  GET_CUR_INPUT_PWM_RATE = 'a', /*!< ��ȡ����PWMռ�ձ�����*/
//  SET_CUR_OUTPUT_PWM_RATE = 'b',
//  DISPLAY_TABLE_ITEM_VALUE = 'c',
  
  SET_CUR_OUTPUT_PWM_RATE = 'b',   /*!< �������PWMռ�ձ����� */
  
    DISPLAY_TABLE_ITEM_VALUE= 'c',  /*!< �鿴��ǰPWM������� */
  TABLE_DISPLAY = 'd',  /*!< ���ұ�������� */
  UNKNOWN_COMMAND /*!< δ֪���� */
} Command;

typedef void (*cmdFunc)(void);

/** 
  * @brief  ��������ṹ��
  */
typedef struct 
{
    Command cmd;  /*!< ������ */
    cmdFunc func;   /*!<  �����*/
//    const char help[HELP_MAX_SIZE];
} CommandItem_t;

static DebugPortCtrl_t tDebugPortCtrLoc = 
{
    .txBufCurrOutPos = 0,
    .txBufCurrInPos = 0,
    .txBuf = {0},
    .rxBufCurrPos = 0,
    .rxBuf = {0},
    .rxdNewPacket = false,
    .rxdFrameLen = 0,
    .transmittingData = false,
    .currTimeoutVal_ms = 0,
};

/**
  * @brief  ��ӡ����PWMռ�ձ�.
  * @param  ��
  * @retval ��
  */
static void DebugPort_GetCurPWMRateIn(void)
{
  printf("%d\r\n", (uint16_t)table_index_get());
}

/**
  * @brief  ��ӡ���PWMֵ.
  * @param  ��
  * @retval ��
  */
static void table_item_value_display(void)
{
//  uint16_t value= (uint16_t)table_index_value_get();
  printf("res:%d\r\n", (uint16_t)table_index_value_get());
}

/**
  * @brief  �������PWMֵ.
  * @param  ��
  * @retval ��
  */
static void DebugPort_SetCurPWMRateOut(void)
{
  char *  identifierPos = strchr((char *)tDebugPortCtrLoc.rxBuf, ':');
  if (identifierPos == NULL)
    printf("format error\r\n");
  else {
    char **pstr = NULL;
    uint16_t arg1 = strtoul(identifierPos+1, pstr, 0);
    printf("arg1:%d\r\n", arg1);
    if(table_index_value_set(arg1 * 655))
      printf("OK\r\n");
  }
}

/**
  * @brief  ���ұ��������.
  * @param  ��
  * @retval ��
  */
static void table_display(void)
{
  print_table();
}

/**
  * @brief  δ֪����.
  * @param  ��
  * @retval ��
  */
static void DebugPort_UnknownCmd(void)
{
    printf("Unknown cmd!!!\r\n");
}



static const CommandItem_t tCommandItems[] = {
  {GET_CUR_INPUT_PWM_RATE, DebugPort_GetCurPWMRateIn/*, "'a':Get PWM Rate"*/},
  {SET_CUR_OUTPUT_PWM_RATE, DebugPort_SetCurPWMRateOut/*, "'b':Set PWM"*/},
  {DISPLAY_TABLE_ITEM_VALUE, table_item_value_display/*, "'f':Display Table"*/},
  {TABLE_DISPLAY, table_display},
  {UNKNOWN_COMMAND, DebugPort_UnknownCmd/*, {0}*/}
};


/**
  * @brief  ���Դ��ڳ�ʼ��.
  * @param  ��
  * @retval ��
  */
static void StartDataXmissionIfNeeded(void)
{
    if ((tDebugPortCtrLoc.txBufCurrInPos != tDebugPortCtrLoc.txBufCurrOutPos) && !tDebugPortCtrLoc.transmittingData)
    {
        tDebugPortCtrLoc.transmittingData = true;
        UART1_ITConfig(UART1_IT_TXE, ENABLE);
    }
}

/**
  * @brief  ���Դ��ڳ�ʼ��.
  * @param  ��
  * @retval ��
  */
void DebugPort_Init(void)
{
    UART1_DeInit();
    UART1_Init(115200,  //������
             UART1_WORDLENGTH_8D, //�ֳ���8
             UART1_STOPBITS_1 , //ֹͣλ��1
             UART1_PARITY_NO, //��żУ��λ��0
             UART1_SYNCMODE_CLOCK_DISABLE, //�ر�ͬ��ģʽ
             UART1_MODE_TXRX_ENABLE //���ͽ���ʹ��
            );
    
    while ((UART1->SR & UART1_SR_TC) == 0)
    {
      (void)0;
    }
}

/**
  * @brief  �򿪵��Դ���.
  * @param  ��
  * @retval ��
  */
void DebugPort_Open(void)
{
    tDebugPortCtrLoc.txBufCurrOutPos = 0;
    tDebugPortCtrLoc.txBufCurrInPos = 0;
    tDebugPortCtrLoc.rxBufCurrPos = 0;  
    
        UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
    UART1_ITConfig(UART1_IT_TXE, DISABLE);

}

/**
  * @brief  �رյ��Դ���.
  * @param  ��
  * @retval ��
  */
void DebugPort_Close(void)
{
    UART1_ITConfig(UART1_IT_RXNE_OR, DISABLE);
    UART1_ITConfig(UART1_IT_TC, DISABLE);
}

/**
  * @brief  ���Դ����ַ��������.
  * @param  buf:�ַ���ʼ��ַ
  * @param  bufLen:����ַ�����
  * @retval ��
  */
bool DebugPort_Write(const char * const buf, uint8_t bufLen)
{   
    for (uint8_t i = 0; i < bufLen; ++i)
    {
        tDebugPortCtrLoc.txBufCurrInPos = (1+tDebugPortCtrLoc.txBufCurrInPos) % DEBUG_PORT_MAX_TX_BYTES;
        if(tDebugPortCtrLoc.txBufCurrInPos != tDebugPortCtrLoc.txBufCurrOutPos)
        {
            tDebugPortCtrLoc.txBuf[tDebugPortCtrLoc.txBufCurrInPos] = buf[i];
        }
        else
        {
            tDebugPortCtrLoc.txBufCurrInPos = (tDebugPortCtrLoc.txBufCurrInPos + (DEBUG_PORT_MAX_TX_BYTES - i)) % DEBUG_PORT_MAX_TX_BYTES;
            return false;
        }
    }
    return true;
}
/**
  * @brief  ���Դ����ַ������.
  * @param  buf:�ַ���ʼ��ַ
  * @param  bufLen:����ַ�����
  * @retval ��
  */
bool DebugPort_WriteString(const char *  str)
{
    return DebugPort_Write(str, strlen(str));
}

bool DebugPort_WriteLine(const char * str)
{
    if (DebugPort_WriteString(str))
    {
        char eolString = DEBUG_PORT_EOL_DELIMITER;
        return DebugPort_Write((char *)&eolString, 1);
    }
    else
    {
        return false;
    }
}

/**
  * @brief  ���Դ���.
  * @param  ��
  * @retval ��
  */
void DebugPort_Handler(void)
{
    StartDataXmissionIfNeeded();
    if (tDebugPortCtrLoc.rxdNewPacket)
    {
//        printf("%d\r\n",sizeof(tCommandItems));
        if(tDebugPortCtrLoc.rxBuf[0] - 'a' < (UNKNOWN_COMMAND - GET_CUR_INPUT_PWM_RATE))
        {
            tCommandItems[tDebugPortCtrLoc.rxBuf[0] - 'a'].func();
        }
        else
        {
            tCommandItems[UNKNOWN_COMMAND - GET_CUR_INPUT_PWM_RATE].func();
        }
//      switch(tDebugPortCtrLoc.rxBuf[0]) {
////      case '\0':
////      case '\r':
////      case '\n':
////      case ' ':
////        break;
//      case 'a':
//        DebugPort_GetCurPWMRateIn();
//        break;
//      case 'b':
//        DebugPort_SetCurPWMRateOut();
//        break;
//      case 'c':
//        table_item_value_display();
//        break;
//      default:
//        for (uint8_t i = 0; tDebugPortCtrLoc.rxBuf[i] != DEBUG_PORT_EOL_DELIMITER; ++i)
//          printf("%d\r\n", (uint16_t)tDebugPortCtrLoc.rxBuf[i]);
//        DebugPort_UnknownCmd();
//        break;
//      }
        tDebugPortCtrLoc.rxBufCurrPos = 0;
        tDebugPortCtrLoc.rxdNewPacket = false;
    }
}

/**
  * @brief  UART1 TX Interrupt routine
  * @param None
  * @retval
  * None
  */
INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
    if ((tDebugPortCtrLoc.txBufCurrInPos != tDebugPortCtrLoc.txBufCurrOutPos))
    {   
      if (++tDebugPortCtrLoc.txBufCurrOutPos == 40)
        tDebugPortCtrLoc.txBufCurrOutPos = 0;
       UART1_SendData8(tDebugPortCtrLoc.txBuf[tDebugPortCtrLoc.txBufCurrOutPos]);
    }
    else
    {
        tDebugPortCtrLoc.transmittingData = false;
        UART1_ITConfig(UART1_IT_TXE, DISABLE);
        UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
    }
  
}

/**
  * @brief  UART1 RX Interrupt routine
  * @param None
  * @retval
  * None
  */
INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */  
    if(UART1_GetITStatus(UART1_IT_RXNE))
    {
      if(!tDebugPortCtrLoc.rxdNewPacket)
      {
          tDebugPortCtrLoc.rxBuf[tDebugPortCtrLoc.rxBufCurrPos++] = UART1_ReceiveData8();
          UART1_ClearITPendingBit(UART1_IT_RXNE);	
         
          if(DEBUG_PORT_EOL_DELIMITER == tDebugPortCtrLoc.rxBuf[tDebugPortCtrLoc.rxBufCurrPos - 1])
          {
              tDebugPortCtrLoc.rxBuf[tDebugPortCtrLoc.rxBufCurrPos - 1] = '\0';
              tDebugPortCtrLoc.rxdNewPacket = true;
  //            UART1_ITConfig(UART1_IT_RXNE_OR, DISABLE);
          }
      }
      else
      {
          UART1_ReceiveData8();//����
  //        UART1_ITConfig(UART1_IT_RXNE_OR, DISABLE);
      }
    }
    if (UART1_GetITStatus(UART1_IT_OR))
    {
      UART1_ReceiveData8();//����
    }
}

/**
  * @}
  */
