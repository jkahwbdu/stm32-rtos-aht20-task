/**
  ******************************************************************************
  * @file    main.h
  * @brief   Board pin definitions for the STM32F103C8T6 uC/OS-II demo.
  ******************************************************************************
  */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

/* ------------------------------- LED pins ---------------------------------- */
/* On-board LED, active low */
#define LED1_Pin            GPIO_PIN_13
#define LED1_GPIO_Port      GPIOC

/* External LED on PB12, active low (3V3 -> 1k -> LED -> PB12) */
#define LED2_Pin            GPIO_PIN_12
#define LED2_GPIO_Port      GPIOB

/* ------------------------------ AHT20 / I2C -------------------------------- */
#define AHT20_SCL_Pin       GPIO_PIN_6
#define AHT20_SDA_Pin       GPIO_PIN_7
#define AHT20_GPIO_Port     GPIOB

extern UART_HandleTypeDef huart1;
extern I2C_HandleTypeDef  hi2c1;

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
