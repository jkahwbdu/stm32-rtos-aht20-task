/**
  ******************************************************************************
  * @file    main.c
  * @brief   STM32F103C8T6 + uC/OS-II + AHT20 demo.
  *
  * Boot flow:
  *   HAL_Init -> clock 72 MHz -> GPIO / USART1 / I2C1 -> disable IRQ
  *   -> OSInit / create start task / SysTick for the OS tick -> OSStart
  ******************************************************************************
  */

#include "includes.h"
#include "app.h"
#include "bsp_led.h"
#include "bsp_uart.h"
#include "aht20.h"

#if AHT20_USE_SOFT_I2C
#include "soft_i2c.h"
#endif

UART_HandleTypeDef huart1 = {0};
I2C_HandleTypeDef  hi2c1  = {0};

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
#if !AHT20_USE_SOFT_I2C
static void MX_I2C1_Init(void);
#endif

int main(void)
{
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();
    BSP_LED_Init();
    BSP_Uart_Init();

#if AHT20_USE_SOFT_I2C
    SoftI2C_Init();
#else
    MX_I2C1_Init();
#endif

    /* uC/OS-II must be initialised with interrupts disabled; OSStartHighRdy()
     * enables them again when the first task is launched.
     */
    __disable_irq();

    App_Init();

    OSStart();

    for (;;)
    {
    }
}

/**
  * @brief  System clock: HSE 8 MHz -> PLL x9 -> SYSCLK 72 MHz,
  *         AHB 72 MHz, APB1 36 MHz, APB2 72 MHz.
  */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    osc.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
    osc.HSEState            = RCC_HSE_ON;
    osc.HSEPredivValue      = RCC_HSE_PREDIV_DIV1;
    osc.HSIState            = RCC_HSI_ON;
    osc.PLL.PLLState        = RCC_PLL_ON;
    osc.PLL.PLLSource       = RCC_PLLSOURCE_HSE;
    osc.PLL.PLLMUL          = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&osc) != HAL_OK)
    {
        Error_Handler();
    }

    clk.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                         RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV2;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief  Enable the GPIO clocks.  The LED / I2C pins are configured by the
  *         BSP and by HAL_I2C_MspInit().
  */
static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();
}

#if !AHT20_USE_SOFT_I2C
/**
  * @brief  I2C1: 400 kHz, 7-bit addressing, PB6 = SCL, PB7 = SDA.
  */
static void MX_I2C1_Init(void)
{
    hi2c1.Instance             = I2C1;
    hi2c1.Init.ClockSpeed      = 400000u;
    hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1     = 0u;
    hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2     = 0u;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}
#endif

void Error_Handler(void)
{
    __disable_irq();
    for (;;)
    {
    }
}
