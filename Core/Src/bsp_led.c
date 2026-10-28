/**
  ******************************************************************************
  * @file    bsp_led.c
  * @brief   LED driver: PC13 on-board LED and PB12 external LED, both active low.
  ******************************************************************************
  */

#include "bsp_led.h"

static GPIO_TypeDef *const s_led_port[2] =
{
    LED1_GPIO_Port,
    LED2_GPIO_Port
};

static const uint16_t s_led_pin[2] =
{
    LED1_Pin,
    LED2_Pin
};

void BSP_LED_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* active low: drive high to switch the LED off */
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);

    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    gpio.Pin = LED1_Pin;
    HAL_GPIO_Init(LED1_GPIO_Port, &gpio);

    gpio.Pin = LED2_Pin;
    HAL_GPIO_Init(LED2_GPIO_Port, &gpio);
}

void BSP_LED_On(BSP_LED_Id led)
{
    if (led <= BSP_LED_2)
    {
        HAL_GPIO_WritePin(s_led_port[led], s_led_pin[led], GPIO_PIN_RESET);
    }
}

void BSP_LED_Off(BSP_LED_Id led)
{
    if (led <= BSP_LED_2)
    {
        HAL_GPIO_WritePin(s_led_port[led], s_led_pin[led], GPIO_PIN_SET);
    }
}

void BSP_LED_Toggle(BSP_LED_Id led)
{
    if (led <= BSP_LED_2)
    {
        HAL_GPIO_TogglePin(s_led_port[led], s_led_pin[led]);
    }
}
