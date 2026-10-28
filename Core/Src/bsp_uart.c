/**
  ******************************************************************************
  * @file    bsp_uart.c
  * @brief   USART1 console driver.  A uC/OS-II mutex serialises the tasks that
  *          share the console (task three prints the sensor values, the start
  *          task prints the boot banner).
  ******************************************************************************
  */

#include "includes.h"
#include "bsp_uart.h"

#define BSP_UART_TX_TIMEOUT_MS   200u

static OS_EVENT *s_uart_lock = (OS_EVENT *)0;

void BSP_Uart_Init(void)
{
    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 115200u;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

void BSP_Uart_CreateLock(void)
{
    INT8U err = OS_ERR_NONE;

    s_uart_lock = OSMutexCreate((INT8U)1u, &err);
    (void)err;
}

void BSP_Uart_Send(const char *str)
{
    if (str != (const char *)0)
    {
        (void)HAL_UART_Transmit(&huart1, (uint8_t *)str,
                                (uint16_t)strlen(str), BSP_UART_TX_TIMEOUT_MS);
    }
}

void BSP_Uart_Printf(const char *fmt, ...)
{
    char    buf[160];
    va_list ap;
    int     len;
    INT8U   err = OS_ERR_NONE;

    if (s_uart_lock != (OS_EVENT *)0)
    {
        (void)OSMutexPend(s_uart_lock, 0u, &err);
        (void)err;
    }

    va_start(ap, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    if (len > 0)
    {
        if (len > (int)(sizeof(buf) - 1u))
        {
            len = (int)(sizeof(buf) - 1u);
        }
        (void)HAL_UART_Transmit(&huart1, (uint8_t *)buf, (uint16_t)len,
                                BSP_UART_TX_TIMEOUT_MS);
    }

    if (s_uart_lock != (OS_EVENT *)0)
    {
        (void)OSMutexPost(s_uart_lock);
    }
}
