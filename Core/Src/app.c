/**
  ******************************************************************************
  * @file    app.c
  * @brief   uC/OS-II application: three tasks + start task.
  *
  *   Task        Prio  Period  Work
  *   ----------  ----  ------  ----------------------------------------------
  *   App_TaskStart  3   once   create the other tasks, print boot banner
  *   App_TaskAht20  4   2 s    read AHT20 (I2C1) and report over USART1
  *   App_TaskLed1   5   1 s    toggle the on-board LED (PC13)
  *   App_TaskLed2   6   3 s    toggle the external LED (PB12)
  ******************************************************************************
  */

#include "includes.h"
#include "app.h"
#include "bsp_led.h"
#include "bsp_uart.h"
#include "aht20.h"

/* UTF-8 bytes of "hello uc/OS! ..." Chinese part, kept as escapes so that the
 * ARMCC sources stay pure ASCII (Keil would otherwise read them as GBK).
 */
#define APP_TEXT_HELLO_CN \
    "\xE6\xAC\xA2\xE8\xBF\x8E\xE6\x9D\xA5\xE5\x88\xB0\x52\x54\x4F\x53" \
    "\xE5\xA4\x9A\xE4\xBB\xBB\xE5\x8A\xA1\xE7\x8E\xAF\xE5\xA2\x83\xEF\xBC\x81"

#define APP_CPU_REPORT_EVERY   5u

static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
static OS_STK AppTaskAht20Stk[APP_TASK_AHT20_STK_SIZE];
static OS_STK AppTaskLed1Stk[APP_TASK_LED1_STK_SIZE];
static OS_STK AppTaskLed2Stk[APP_TASK_LED2_STK_SIZE];

static void App_TaskStart(void *p_arg);
static void App_TaskAht20(void *p_arg);
static void App_TaskLed1(void *p_arg);
static void App_TaskLed2(void *p_arg);

static void App_FormatTime(char *buf, uint16_t size)
{
    INT32U ticks = OSTimeGet();
    INT32U secs  = ticks / (INT32U)OS_TICKS_PER_SEC;
    INT32U msecs = ticks % (INT32U)OS_TICKS_PER_SEC;

    (void)snprintf(buf, size, "%02u:%02u.%03u",
                   (unsigned)(secs / 60u), (unsigned)(secs % 60u),
                   (unsigned)msecs);
}

static void App_CreateTask(void (*task)(void *p_arg), OS_STK *stk_top,
                           OS_STK *stk_base, INT16U stk_size, INT8U prio,
                           const char *name)
{
    INT8U err = OS_ERR_NONE;

    (void)OSTaskCreateExt(task, (void *)0, stk_top, prio, prio, stk_base,
                          stk_size, (void *)0,
                          OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK);

    (void)OSTaskNameSet(prio, (INT8U *)name, &err);
    (void)err;
}

/**
  * @brief  Create the start task and configure the OS tick.
  *         Called with interrupts disabled immediately before OSStart().
  */
void App_Init(void)
{
    OSInit();

    (void)OSTaskCreateExt(App_TaskStart, (void *)0,
                          &AppTaskStartStk[APP_TASK_START_STK_SIZE - 1u],
                          APP_TASK_START_PRIO,
                          APP_TASK_START_PRIO,
                          &AppTaskStartStk[0],
                          APP_TASK_START_STK_SIZE,
                          (void *)0,
                          OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK);

    OS_CPU_SysTickInit();
}

static void App_TaskStart(void *p_arg)
{
    (void)p_arg;

    BSP_Uart_CreateLock();

    BSP_Uart_Printf("\r\n==================================================\r\n");
    BSP_Uart_Printf("  uC/OS-II V%u.%02u   STM32F103C8T6 @ 72 MHz\r\n",
                    (unsigned)(OS_VERSION / 100u),
                    (unsigned)(OS_VERSION % 100u));
    BSP_Uart_Printf("  Tick: 1 ms        Tasks: LED1, LED2, AHT20\r\n");
#if APP_TASK3_SENSOR
    BSP_Uart_Printf("  LED1: 1 s (PC13)  LED2: 3 s (PB12)\r\n");
    BSP_Uart_Printf("  Task3: AHT20 over I2C1 (PB6/PB7), 2 s\r\n");
#else
    BSP_Uart_Printf("  LED1: 1 s (PC13)  LED2: 3 s (PB12)\r\n");
    BSP_Uart_Printf("  Task3: greeting over USART1, 2 s\r\n");
#endif
    BSP_Uart_Printf("--------------------------------------------------\r\n");
    BSP_Uart_Printf("  hello uc/OS! %s\r\n", APP_TEXT_HELLO_CN);
    BSP_Uart_Printf("==================================================\r\n");

    /* the statistics task must be calibrated while only the start task and
     * the idle task are ready, otherwise OSCPUUsage is meaningless
     */
    OSStatInit();

    App_CreateTask(App_TaskAht20, &AppTaskAht20Stk[APP_TASK_AHT20_STK_SIZE - 1u],
                   &AppTaskAht20Stk[0], APP_TASK_AHT20_STK_SIZE,
                   APP_TASK_AHT20_PRIO, "AHT20");

    App_CreateTask(App_TaskLed1, &AppTaskLed1Stk[APP_TASK_LED1_STK_SIZE - 1u],
                   &AppTaskLed1Stk[0], APP_TASK_LED1_STK_SIZE,
                   APP_TASK_LED1_PRIO, "LED1");

    App_CreateTask(App_TaskLed2, &AppTaskLed2Stk[APP_TASK_LED2_STK_SIZE - 1u],
                   &AppTaskLed2Stk[0], APP_TASK_LED2_STK_SIZE,
                   APP_TASK_LED2_PRIO, "LED2");

    (void)OSTaskDel(OS_PRIO_SELF);

    for (;;)                                   /* never reached */
    {
    }
}

static void App_TaskAht20(void *p_arg)
{
#if APP_TASK3_SENSOR
    int32_t  temp;
    int32_t  hum;
    int32_t  abs_temp;
    char     ts[16];
    char     sign;
    uint8_t  ret;
    uint32_t seq = 0u;
    uint32_t t0;
    uint32_t elapsed;

    (void)p_arg;

    ret = AHT20_Init();
    if (ret == AHT20_OK)
    {
        BSP_Uart_Printf("[boot] AHT20 calibrated, sampling every 2 s\r\n");
    }
    else
    {
        BSP_Uart_Printf("[boot] AHT20 init failed (code %u), check wiring\r\n",
                        (unsigned)ret);
    }

    for (;;)
    {
        t0  = OSTimeGet();
        seq++;

        ret = AHT20_Read(&temp, &hum);
        App_FormatTime(ts, sizeof(ts));

        if (ret == AHT20_OK)
        {
            sign     = (temp < 0) ? '-' : '+';
            abs_temp = (temp < 0) ? -temp : temp;
            BSP_Uart_Printf("[%s] #%02u  T=%c%ld.%02ld C   H=%ld.%02ld %%RH\r\n",
                            ts, (unsigned)seq, sign,
                            (long)(abs_temp / 100), (long)(abs_temp % 100),
                            (long)(hum / 100), (long)(hum % 100));
        }
        else
        {
            BSP_Uart_Printf("[%s] #%02u  sensor error (code %u)\r\n",
                            ts, (unsigned)seq, (unsigned)ret);
        }

        if ((seq % APP_CPU_REPORT_EVERY) == 0u)
        {
            BSP_Uart_Printf("         CPU usage: %u %%\r\n",
                            (unsigned)OSCPUUsage);
        }

        /* keep the sampling instant exactly 2 s apart, whatever the I2C and
         * UART time was
         */
        elapsed = OSTimeGet() - t0;
        if (elapsed < APP_AHT20_PERIOD_MS)
        {
            OSTimeDly((INT16U)(APP_AHT20_PERIOD_MS - elapsed));
        }
        else
        {
            OSTimeDly(1u);
        }
    }
#else
    char ts[16];

    (void)p_arg;

    for (;;)
    {
        App_FormatTime(ts, sizeof(ts));
        BSP_Uart_Printf("[%s] hello uc/OS! %s\r\n", ts, APP_TEXT_HELLO_CN);
        OSTimeDly((INT16U)APP_AHT20_PERIOD_MS);
    }
#endif
}

static void App_TaskLed1(void *p_arg)
{
    (void)p_arg;

    for (;;)
    {
        BSP_LED_Toggle(BSP_LED_1);
        OSTimeDlyHMSM(0u, 0u, 0u, (INT16U)APP_LED1_HALF_PERIOD_MS);
    }
}

static void App_TaskLed2(void *p_arg)
{
    (void)p_arg;

    for (;;)
    {
        BSP_LED_Toggle(BSP_LED_2);
        OSTimeDlyHMSM(0u, 0u, 0u, (INT16U)APP_LED2_HALF_PERIOD_MS);
    }
}
