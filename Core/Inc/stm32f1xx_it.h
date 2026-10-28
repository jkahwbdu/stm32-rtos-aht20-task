/**
  ******************************************************************************
  * @file    stm32f1xx_it.h
  * @brief   Interrupt handler prototypes.
  ******************************************************************************
  */

#ifndef __STM32F1xx_IT_H
#define __STM32F1xx_IT_H

#ifdef __cplusplus
extern "C" {
#endif

void NMI_Handler(void);
void HardFault_Handler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
