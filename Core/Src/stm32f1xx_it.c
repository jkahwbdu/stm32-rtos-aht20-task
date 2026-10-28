/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Cortex-M3 exception handlers.
  *
  * The PendSV handler is provided by the uC/OS-II port (os_cpu_a.asm) and must
  * not be defined here.  SysTick feeds both the HAL time base and the uC/OS-II
  * tick, so HAL_Delay()/HAL timeouts keep working inside the tasks.
  ******************************************************************************
  */

#include "main.h"
#include "stm32f1xx_it.h"

/* from os_cpu_c.c */
extern void OS_CPU_SysTickHandler(void);

void NMI_Handler(void)
{
    for (;;)
    {
    }
}

void HardFault_Handler(void)
{
    for (;;)
    {
    }
}

void SysTick_Handler(void)
{
    HAL_IncTick();
    OS_CPU_SysTickHandler();
}
