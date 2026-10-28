/**
  ******************************************************************************
  * @file    aht20.h
  * @brief   AHT20 temperature / humidity sensor driver.
  *
  * The driver returns fixed point values so that the application never needs
  * floating point printf:
  *     temperature : 0.01 degC   (2500  -> 25.00 degC)
  *     humidity    : 0.01 %RH    (4820  -> 48.20 %RH)
  ******************************************************************************
  */

#ifndef __AHT20_H
#define __AHT20_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* 0 = hardware I2C1 (PB6/PB7, 400 kHz)
 * 1 = software bit-banged I2C on the same pins
 */
#ifndef AHT20_USE_SOFT_I2C
#define AHT20_USE_SOFT_I2C   0
#endif

#define AHT20_I2C_ADDR7      0x38u
#define AHT20_ADDR_WRITE     (AHT20_I2C_ADDR7 << 1)
#define AHT20_ADDR_READ      ((AHT20_I2C_ADDR7 << 1) | 1u)

/* return codes */
#define AHT20_OK             0u
#define AHT20_ERR_I2C        1u
#define AHT20_ERR_BUSY       2u

uint8_t AHT20_Init(void);
uint8_t AHT20_Read(int32_t *temp_centi, int32_t *hum_centi);

#ifdef __cplusplus
}
#endif

#endif /* __AHT20_H */
