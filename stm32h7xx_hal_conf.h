#ifndef __STM32H7xx_HAL_CONF_H
#define __STM32H7xx_HAL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* ########################## Module Selection ############################## */
#define HAL_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2S_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
// #define HAL_PCD_MODULE_ENABLED // Enable for USB

/* ########################## Oscillator Values adaptation ####################*/
#if !defined  (HSE_VALUE)
  #define HSE_VALUE    ((uint32_t)25000000) /*!< Value of the External oscillator in Hz */
#endif

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)64000000) /*!< Value of the Internal oscillator in Hz*/
#endif

#if !defined  (CSI_VALUE)
  #define CSI_VALUE    ((uint32_t)4000000) /*!< Value of the Internal oscillator in Hz*/
#endif

#if !defined  (LSI_VALUE)
  #define LSI_VALUE    ((uint32_t)32000) /*!< Value of the Internal Low Speed oscillator in Hz*/
#endif

#if !defined  (LSE_VALUE)
  #define LSE_VALUE    ((uint32_t)32768) /*!< Value of the External Low Speed oscillator in Hz*/
#endif

/* ########################## SysTick Timer Selection ############################## */
#define  TICK_INT_PRIORITY            ((uint32_t)0x0F)

/* ########################## Assert Selection ############################## */
/* #define USE_FULL_ASSERT    1U */

/* ########################## Register Callback feature ######################### */
#define  USE_HAL_I2S_REGISTER_CALLBACKS     0U
#define  USE_HAL_DMA_REGISTER_CALLBACKS     0U

/* ########################## SPI/I2S Peripheral configuration ################# */
#define USE_SPI_CRC                     0U

/* ########################## Include module's header file ################## */
#ifdef HAL_RCC_MODULE_ENABLED
  #include "stm32h7xx_hal_rcc.h"
#endif
#ifdef HAL_GPIO_MODULE_ENABLED
  #include "stm32h7xx_hal_gpio.h"
#endif
#ifdef HAL_DMA_MODULE_ENABLED
  #include "stm32h7xx_hal_dma.h"
#endif
#ifdef HAL_PWR_MODULE_ENABLED
  #include "stm32h7xx_hal_pwr.h"
#endif
#ifdef HAL_I2S_MODULE_ENABLED
  #include "stm32h7xx_hal_i2s.h"
#endif
#ifdef HAL_CORTEX_MODULE_ENABLED
  #include "stm32h7xx_hal_cortex.h"
#endif
#ifdef HAL_FLASH_MODULE_ENABLED
  #include "stm32h7xx_hal_flash.h"
#endif

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32H7xx_HAL_CONF_H */
