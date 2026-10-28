/**
  ******************************************************************************
  * @file    bsp_led.h
  * @brief   LED helpers (PC13 on-board LED, PB12 external LED, both active low).
  ******************************************************************************
  */

#ifndef __BSP_LED_H
#define __BSP_LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef enum
{
    BSP_LED_1 = 0,
    BSP_LED_2
} BSP_LED_Id;

void BSP_LED_Init(void);
void BSP_LED_On(BSP_LED_Id led);
void BSP_LED_Off(BSP_LED_Id led);
void BSP_LED_Toggle(BSP_LED_Id led);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_LED_H */
