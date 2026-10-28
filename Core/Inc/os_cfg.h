/**
  ******************************************************************************
  * @file    os_cfg.h
  * @brief   uC/OS-II V2.91 configuration for the STM32F103C8T6 RTOS demo.
  *
  * Only the services used by this project are enabled, which keeps the kernel
  * at roughly 6 KB of code and a few hundred bytes of RAM.
  ******************************************************************************
  */

#ifndef OS_CFG_H
#define OS_CFG_H

/* ------------------------------ miscellaneous ------------------------------ */
#define OS_APP_HOOKS_EN           0u   /* no application hooks needed                 */
#define OS_ARG_CHK_EN             0u   /* disable argument checking (saves code)      */
#define OS_CPU_HOOKS_EN           1u   /* hooks are provided in os_cpu_c.c            */
#define OS_DEBUG_EN               0u   /* no debug constants / os_dbg.c               */

#define OS_EVENT_MULTI_EN         0u
#define OS_EVENT_NAME_EN          0u

#define OS_LOWEST_PRIO           15u   /* priorities 0..15                            */
#define OS_MAX_EVENTS             4u
#define OS_MAX_FLAGS              4u
#define OS_MAX_MEM_PART           2u
#define OS_MAX_QS                 2u
#define OS_MAX_TASKS              8u

#define OS_SCHED_LOCK_EN          1u
#define OS_TICK_STEP_EN           0u
#define OS_TICKS_PER_SEC       1000u   /* 1 tick = 1 ms                               */

/* ----------------------------- task stack sizes ---------------------------- */
#define OS_TASK_TMR_STK_SIZE    128u
#define OS_TASK_STAT_STK_SIZE   128u
#define OS_TASK_IDLE_STK_SIZE   128u

/* ------------------------------ task management ---------------------------- */
#define OS_TASK_CHANGE_PRIO_EN    0u
#define OS_TASK_CREATE_EN         1u
#define OS_TASK_CREATE_EXT_EN     1u
#define OS_TASK_DEL_EN            1u
#define OS_TASK_NAME_EN           1u
#define OS_TASK_PROFILE_EN        0u
#define OS_TASK_QUERY_EN          0u
#define OS_TASK_REG_TBL_SIZE      1u
#define OS_TASK_STAT_EN           1u
#define OS_TASK_STAT_STK_CHK_EN   1u
#define OS_TASK_SUSPEND_EN        0u
#define OS_TASK_SW_HOOK_EN        1u

/* -------------------------------- event flags ------------------------------ */
#define OS_FLAG_EN                0u
#define OS_FLAG_ACCEPT_EN         0u
#define OS_FLAG_DEL_EN            0u
#define OS_FLAG_NAME_EN           0u
#define OS_FLAG_QUERY_EN          0u
#define OS_FLAG_WAIT_CLR_EN       0u
#define OS_FLAGS_NBITS           16u

/* -------------------------------- mailboxes -------------------------------- */
#define OS_MBOX_EN                0u
#define OS_MBOX_ACCEPT_EN         0u
#define OS_MBOX_DEL_EN            0u
#define OS_MBOX_PEND_ABORT_EN     0u
#define OS_MBOX_POST_EN           0u
#define OS_MBOX_POST_OPT_EN       0u
#define OS_MBOX_QUERY_EN          0u

/* ----------------------------- memory manager ------------------------------ */
#define OS_MEM_EN                 0u
#define OS_MEM_NAME_EN            0u
#define OS_MEM_QUERY_EN           0u

/* ---------------------------------- mutex ---------------------------------- */
#define OS_MUTEX_EN               1u
#define OS_MUTEX_ACCEPT_EN        0u
#define OS_MUTEX_DEL_EN           0u
#define OS_MUTEX_QUERY_EN         0u

/* ------------------------------- message queue ----------------------------- */
#define OS_Q_EN                   0u
#define OS_Q_ACCEPT_EN            0u
#define OS_Q_DEL_EN               0u
#define OS_Q_FLUSH_EN             0u
#define OS_Q_PEND_ABORT_EN        0u
#define OS_Q_POST_EN              0u
#define OS_Q_POST_FRONT_EN        0u
#define OS_Q_POST_OPT_EN          0u
#define OS_Q_QUERY_EN             0u

/* -------------------------------- semaphores ------------------------------- */
#define OS_SEM_EN                 0u
#define OS_SEM_ACCEPT_EN          0u
#define OS_SEM_DEL_EN             0u
#define OS_SEM_PEND_ABORT_EN      0u
#define OS_SEM_QUERY_EN           0u
#define OS_SEM_SET_EN             0u

/* ------------------------------ time management ---------------------------- */
#define OS_TIME_DLY_HMSM_EN       1u
#define OS_TIME_DLY_RESUME_EN     0u
#define OS_TIME_GET_SET_EN        1u
#define OS_TIME_TICK_HOOK_EN      0u

/* ------------------------------ timer management --------------------------- */
#define OS_TMR_EN                 0u
#define OS_TMR_CFG_MAX           16u
#define OS_TMR_CFG_NAME_EN        0u
#define OS_TMR_CFG_WHEEL_SIZE     8u
#define OS_TMR_CFG_TICKS_PER_SEC 10u

#endif /* OS_CFG_H */
