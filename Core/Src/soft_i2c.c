/**
  ******************************************************************************
  * @file    soft_i2c.c
  * @brief   Software (bit-banged) I2C master on PB6 (SCL) / PB7 (SDA).
  *
  * The pins are configured as open-drain outputs with the internal pull-up
  * enabled.  External 4.7 k pull-ups are still recommended.  Reading the SDA
  * line works because the input data register always samples the pin level,
  * even when the pin is configured as an open-drain output.
  ******************************************************************************
  */

#include "soft_i2c.h"

#define SCL_HIGH()   HAL_GPIO_WritePin(AHT20_GPIO_Port, AHT20_SCL_Pin, GPIO_PIN_SET)
#define SCL_LOW()    HAL_GPIO_WritePin(AHT20_GPIO_Port, AHT20_SCL_Pin, GPIO_PIN_RESET)
#define SDA_HIGH()   HAL_GPIO_WritePin(AHT20_GPIO_Port, AHT20_SDA_Pin, GPIO_PIN_SET)
#define SDA_LOW()    HAL_GPIO_WritePin(AHT20_GPIO_Port, AHT20_SDA_Pin, GPIO_PIN_RESET)
#define SDA_READ()   HAL_GPIO_ReadPin(AHT20_GPIO_Port, AHT20_SDA_Pin)

/* ~1.4 us per half period at 72 MHz -> about 350 kHz.
 * Increase the loop count for longer wiring / slower sensors.
 */
static void SoftI2C_Delay(void)
{
    volatile uint32_t i;

    for (i = 0u; i < 40u; i++)
    {
        __NOP();
    }
}

void SoftI2C_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    SDA_HIGH();
    SCL_HIGH();

    gpio.Pin   = AHT20_SCL_Pin | AHT20_SDA_Pin;
    gpio.Mode  = GPIO_MODE_OUTPUT_OD;
    gpio.Pull  = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(AHT20_GPIO_Port, &gpio);

    SDA_HIGH();
    SCL_HIGH();
}

static void SoftI2C_Start(void)
{
    SDA_HIGH();
    SoftI2C_Delay();
    SCL_HIGH();
    SoftI2C_Delay();
    SDA_LOW();
    SoftI2C_Delay();
    SCL_LOW();
    SoftI2C_Delay();
}

static void SoftI2C_Stop(void)
{
    SDA_LOW();
    SoftI2C_Delay();
    SCL_HIGH();
    SoftI2C_Delay();
    SDA_HIGH();
    SoftI2C_Delay();
}

/* returns 0 when the slave acknowledged the byte */
static uint8_t SoftI2C_WriteByte(uint8_t byte)
{
    uint8_t i;
    uint8_t nack;

    for (i = 0u; i < 8u; i++)
    {
        if ((byte & 0x80u) != 0u)
        {
            SDA_HIGH();
        }
        else
        {
            SDA_LOW();
        }
        SoftI2C_Delay();
        SCL_HIGH();
        SoftI2C_Delay();
        SCL_LOW();
        SoftI2C_Delay();
        byte = (uint8_t)(byte << 1);
    }

    SDA_HIGH();                  /* release SDA for the ACK bit */
    SoftI2C_Delay();
    SCL_HIGH();
    SoftI2C_Delay();
    nack = (SDA_READ() == GPIO_PIN_SET) ? 1u : 0u;
    SCL_LOW();
    SoftI2C_Delay();

    return nack;
}

static uint8_t SoftI2C_ReadByte(uint8_t ack)
{
    uint8_t i;
    uint8_t byte = 0u;

    SDA_HIGH();                  /* release SDA so the slave can drive it */
    for (i = 0u; i < 8u; i++)
    {
        SoftI2C_Delay();
        SCL_HIGH();
        SoftI2C_Delay();
        byte = (uint8_t)(byte << 1);
        if (SDA_READ() == GPIO_PIN_SET)
        {
            byte |= 0x01u;
        }
        SCL_LOW();
        SoftI2C_Delay();
    }

    if (ack != 0u)
    {
        SDA_LOW();               /* ACK  : pull SDA low  */
    }
    else
    {
        SDA_HIGH();              /* NACK : leave SDA high */
    }
    SoftI2C_Delay();
    SCL_HIGH();
    SoftI2C_Delay();
    SCL_LOW();
    SoftI2C_Delay();
    SDA_HIGH();

    return byte;
}

uint8_t SoftI2C_Write(uint8_t addr7, const uint8_t *buf, uint16_t len)
{
    uint16_t i;

    SoftI2C_Start();
    if (SoftI2C_WriteByte((uint8_t)(addr7 << 1)) != 0u)
    {
        SoftI2C_Stop();
        return 1u;
    }

    for (i = 0u; i < len; i++)
    {
        if (SoftI2C_WriteByte(buf[i]) != 0u)
        {
            SoftI2C_Stop();
            return 2u;
        }
    }

    SoftI2C_Stop();
    return 0u;
}

uint8_t SoftI2C_Read(uint8_t addr7, uint8_t *buf, uint16_t len)
{
    uint16_t i;

    SoftI2C_Start();
    if (SoftI2C_WriteByte((uint8_t)((addr7 << 1) | 0x01u)) != 0u)
    {
        SoftI2C_Stop();
        return 1u;
    }

    for (i = 0u; i < len; i++)
    {
        buf[i] = SoftI2C_ReadByte((i + 1u < len) ? 1u : 0u);
    }

    SoftI2C_Stop();
    return 0u;
}
