/**
  ******************************************************************************
  * @file    app.h
  * @brief   Application entry points for the uC/OS-II demo.
  ******************************************************************************
  */

#ifndef __APP_H
#define __APP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  Initialise uC/OS-II, create the start task and configure the OS tick.
  *         Must be called with interrupts disabled, right before OSStart().
  */
void App_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_H */
