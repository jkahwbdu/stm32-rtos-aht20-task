/**
  ******************************************************************************
  * @file    app_cfg.h
  * @brief   uC/OS-II application configuration: task priorities and stacks.
  ******************************************************************************
  */

#ifndef APP_CFG_H
#define APP_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------ task priorities ---------------------------- */
#define APP_TASK_START_PRIO       3u
#define APP_TASK_AHT20_PRIO       4u
#define APP_TASK_LED1_PRIO        5u
#define APP_TASK_LED2_PRIO        6u

/* ------------------------------- stack sizes ------------------------------- */
#define APP_TASK_START_STK_SIZE   256u
#define APP_TASK_AHT20_STK_SIZE   384u
#define APP_TASK_LED1_STK_SIZE    128u
#define APP_TASK_LED2_STK_SIZE    128u

/* ------------------------------ task periods ------------------------------- */
#define APP_LED1_HALF_PERIOD_MS   500u    /* 1 s blink period  */
#define APP_LED2_HALF_PERIOD_MS   1500u   /* 3 s blink period  */
#define APP_AHT20_PERIOD_MS       2000u   /* 2 s sampling      */

/* Task 3 personality:
 *   0 = task one only  : send the "hello uc/OS!" greeting every 2 s
 *   1 = task two/three : read AHT20 every 2 s and report over UART
 */
#define APP_TASK3_SENSOR          1

#ifdef __cplusplus
}
#endif

#endif /* APP_CFG_H */
