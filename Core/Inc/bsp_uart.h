/**
  ******************************************************************************
  * @file    bsp_uart.h
  * @brief   USART1 console (115200 8N1, PA9/PA10) with an RTOS mutex.
  ******************************************************************************
  */

#ifndef __BSP_UART_H
#define __BSP_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void BSP_Uart_Init(void);
void BSP_Uart_CreateLock(void);
void BSP_Uart_Send(const char *str);
void BSP_Uart_Printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_UART_H */
