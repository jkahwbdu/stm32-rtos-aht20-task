/**
  ******************************************************************************
  * @file    aht20.c
  * @brief   AHT20 temperature / humidity sensor driver (I2C address 0x38).
  *
  * Measurement sequence (AHT20 datasheet 6.1):
  *   1. wait >= 40 ms after power on
  *   2. read the status byte; if the calibration bit (0x08) is clear, send
  *      0xBE 0x08 0x00 and wait 10 ms
  *   3. send 0xAC 0x33 0x00 to start a conversion, wait >= 80 ms
  *   4. read 7 bytes: status, 20-bit humidity, 20-bit temperature
  ******************************************************************************
  */

#include "includes.h"
#include "aht20.h"

#if AHT20_USE_SOFT_I2C
#include "soft_i2c.h"
#endif

#define AHT20_CMD_CALIBRATE   0xBEu
#define AHT20_CMD_TRIGGER     0xACu

#define AHT20_STATUS_BUSY     0x80u
#define AHT20_STATUS_CAL      0x08u

#define AHT20_I2C_TIMEOUT_MS  100u

static uint8_t AHT20_Write(const uint8_t *buf, uint16_t len)
{
#if AHT20_USE_SOFT_I2C
    return (SoftI2C_Write(AHT20_I2C_ADDR7, buf, len) == 0u) ? 0u : 1u;
#else
    return (HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDR_WRITE,
                                    (uint8_t *)buf, len,
                                    AHT20_I2C_TIMEOUT_MS) == HAL_OK) ? 0u : 1u;
#endif
}

static uint8_t AHT20_ReadBytes(uint8_t *buf, uint16_t len)
{
#if AHT20_USE_SOFT_I2C
    return (SoftI2C_Read(AHT20_I2C_ADDR7, buf, len) == 0u) ? 0u : 1u;
#else
    return (HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDR_READ,
                                   buf, len,
                                   AHT20_I2C_TIMEOUT_MS) == HAL_OK) ? 0u : 1u;
#endif
}

/**
  * @brief  Delay that yields the CPU when the scheduler is already running.
  */
static void AHT20_DelayMs(uint32_t ms)
{
    if (OSRunning == OS_TRUE)
    {
        OSTimeDly((INT16U)(ms * ((uint32_t)OS_TICKS_PER_SEC / 1000u)));
    }
    else
    {
        HAL_Delay(ms);
    }
}

uint8_t AHT20_Init(void)
{
    uint8_t cmd[3];
    uint8_t status = 0u;

    AHT20_DelayMs(40u);

    if (AHT20_ReadBytes(&status, 1u) != 0u)
    {
        return AHT20_ERR_I2C;
    }

    if ((status & AHT20_STATUS_CAL) == 0u)
    {
        cmd[0] = AHT20_CMD_CALIBRATE;
        cmd[1] = 0x08u;
        cmd[2] = 0x00u;
        if (AHT20_Write(cmd, 3u) != 0u)
        {
            return AHT20_ERR_I2C;
        }
        AHT20_DelayMs(10u);
    }

    return AHT20_OK;
}

uint8_t AHT20_Read(int32_t *temp_centi, int32_t *hum_centi)
{
    uint8_t  cmd[3] = {AHT20_CMD_TRIGGER, 0x33u, 0x00u};
    uint8_t  buf[7];
    uint8_t  retry;
    uint32_t raw_hum;
    uint32_t raw_temp;

    if ((temp_centi == (int32_t *)0) || (hum_centi == (int32_t *)0))
    {
        return AHT20_ERR_I2C;
    }

    if (AHT20_Write(cmd, 3u) != 0u)
    {
        return AHT20_ERR_I2C;
    }

    /* conversion needs about 75 ms, poll the busy flag afterwards */
    AHT20_DelayMs(80u);
    for (retry = 0u; retry < 5u; retry++)
    {
        if (AHT20_ReadBytes(buf, 1u) != 0u)
        {
            return AHT20_ERR_I2C;
        }
        if ((buf[0] & AHT20_STATUS_BUSY) == 0u)
        {
            break;
        }
        AHT20_DelayMs(10u);
    }
    if (retry >= 5u)
    {
        return AHT20_ERR_BUSY;
    }

    if (AHT20_ReadBytes(buf, 7u) != 0u)
    {
        return AHT20_ERR_I2C;
    }
    if ((buf[0] & AHT20_STATUS_BUSY) != 0u)
    {
        return AHT20_ERR_BUSY;
    }

    raw_hum  = ((uint32_t)buf[1] << 12) |
               ((uint32_t)buf[2] << 4)  |
               ((uint32_t)buf[3] >> 4);
    raw_temp = (((uint32_t)buf[3] & 0x0Fu) << 16) |
               ((uint32_t)buf[4] << 8) |
               (uint32_t)buf[5];

    /* humidity     = raw / 2^20 * 100 %   -> 0.01 %RH units */
    *hum_centi  = (int32_t)(((uint64_t)raw_hum * 10000u) >> 20);

    /* temperature  = raw / 2^20 * 200 - 50 degC -> 0.01 degC units */
    *temp_centi = (int32_t)(((uint64_t)raw_temp * 20000u) >> 20) - 5000;

    return AHT20_OK;
}
