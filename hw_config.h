#pragma once

#include <cstdint>

// Forward declarations
void SystemClock_Config();
void MX_GPIO_Init();
void MX_DMA_Init();
void MX_I2S2_Init();
void MX_USB_DEVICE_Init();

// Audio Buffer Sizes
#define AUDIO_BLOCK_SIZE 48
#define DMA_BUFFER_SIZE (AUDIO_BLOCK_SIZE * 2) // Double buffering

// Simple HAL-like types/defs if not present
#ifndef HAL_OK
typedef enum {
  HAL_OK = 0x00U,
  HAL_ERROR = 0x01U,
  HAL_BUSY = 0x02U,
  HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;
#endif

// Mock for non-ARM environments
#ifndef STM32H7xx_HAL_H
// (removed conflicting typedefs)
void HAL_Init();
void HAL_Delay(uint32_t Delay);
#endif

void Error_Handler(void);
