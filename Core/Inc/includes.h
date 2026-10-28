/**
  ******************************************************************************
  * @file    includes.h
  * @brief   Common include file used by the uC/OS-II sources and the BSP.
  ******************************************************************************
  */

#ifndef INCLUDES_H
#define INCLUDES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "stm32f1xx_hal.h"

/* os_cpu.h must be seen before ucos_ii.h (data types + critical sections) */
#include "os_cpu.h"

/* ucos_ii.h pulls in app_cfg.h and os_cfg.h */
#include "ucos_ii.h"

#ifdef __cplusplus
}
#endif

#endif /* INCLUDES_H */
