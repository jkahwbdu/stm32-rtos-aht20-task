/**
  ******************************************************************************
  * @file    soft_i2c.h
  * @brief   Bit-banged (software) I2C master on PB6/PB7.
  ******************************************************************************
  */

#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void    SoftI2C_Init(void);
uint8_t SoftI2C_Write(uint8_t addr7, const uint8_t *buf, uint16_t len);
uint8_t SoftI2C_Read(uint8_t addr7, uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __SOFT_I2C_H */
