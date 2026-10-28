/**
  ******************************************************************************
  * @file    os_cpu_c.c
  * @brief   uC/OS-II port for the ARM Cortex-M3 (STM32F103, Keil ARMCC).
  *
  * This file contains the C part of the port:
  *   - the task stack initialisation (OSTaskStkInit)
  *   - the uC/OS-II hooks (empty in this project)
  *   - the SysTick handler / initialisation used as the OS tick source
  *
  * The assembly part (context switch, critical sections) lives in os_cpu_a.asm.
  ******************************************************************************
  */

#define  OS_CPU_GLOBALS
#include <includes.h>

#if OS_CPU_HOOKS_EN > 0u

void OSInitHookBegin(void)
{
}

void OSInitHookEnd(void)
{
}

void OSTaskCreateHook(OS_TCB *ptcb)
{
    (void)ptcb;
}

void OSTaskDelHook(OS_TCB *ptcb)
{
    (void)ptcb;
}

void OSTaskIdleHook(void)
{
}

void OSTaskStatHook(void)
{
}

void OSTaskSwHook(void)
{
}

void OSTCBInitHook(OS_TCB *ptcb)
{
    (void)ptcb;
}

/* called if a task ever returns from its body (task one never does) */
void OSTaskReturnHook(OS_TCB *ptcb)
{
    (void)ptcb;
}

#if OS_TIME_TICK_HOOK_EN > 0u
void OSTimeTickHook(void)
{
}
#endif

#endif /* OS_CPU_HOOKS_EN */

/**
  * @brief  Initialise the stack of a task so that it looks like the task was
  *         interrupted by an exception.
  * @param  task  entry point of the task
  * @param  p_arg first argument passed to the task
  * @param  ptos  top of stack (highest address, stack grows downwards)
  * @param  opt   task options (unused)
  * @retval new stack pointer
  */
OS_STK *OSTaskStkInit(void (*task)(void *p_arg), void *p_arg, OS_STK *ptos,
                      INT16U opt)
{
    OS_STK *stk;

    (void)opt;
    stk = ptos;

    /* Registers stacked as if auto-saved on exception                    */
    *(stk)   = (OS_STK)0x01000000UL;              /* xPSR (Thumb bit set)  */
    *(--stk) = (OS_STK)(uint32_t)task;            /* PC                    */
    *(--stk) = (OS_STK)0xFFFFFFFEUL;              /* LR                    */
    *(--stk) = (OS_STK)0x12121212UL;              /* R12                   */
    *(--stk) = (OS_STK)0x03030303UL;              /* R3                    */
    *(--stk) = (OS_STK)0x02020202UL;              /* R2                    */
    *(--stk) = (OS_STK)0x01010101UL;              /* R1                    */
    *(--stk) = (OS_STK)(uint32_t)p_arg;           /* R0 : task argument    */

    /* Registers saved by the PendSV handler (R4-R11)                     */
    *(--stk) = (OS_STK)0x11111111UL;              /* R11                   */
    *(--stk) = (OS_STK)0x10101010UL;              /* R10                   */
    *(--stk) = (OS_STK)0x09090909UL;              /* R9                    */
    *(--stk) = (OS_STK)0x08080808UL;              /* R8                    */
    *(--stk) = (OS_STK)0x07070707UL;              /* R7                    */
    *(--stk) = (OS_STK)0x06060606UL;              /* R6                    */
    *(--stk) = (OS_STK)0x05050505UL;              /* R5                    */
    *(--stk) = (OS_STK)0x04040404UL;              /* R4                    */

    return (stk);
}

/**
  * @brief  uC/OS-II tick handler.  Must be called from the SysTick exception.
  */
void OS_CPU_SysTickHandler(void)
{
    OSIntEnter();
    OSTimeTick();
    OSIntExit();
}

/**
  * @brief  Configure SysTick to generate OS_TICKS_PER_SEC interrupts per second
  *         at the lowest NVIC priority.
  */
void OS_CPU_SysTickInit(void)
{
    (void)SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);
    NVIC_SetPriority(SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
}

/**
  * @brief  Return the SysTick input clock (HCLK) in Hz.
  */
INT32U OS_CPU_SysTickClkFreq(void)
{
    return (INT32U)SystemCoreClock;
}
