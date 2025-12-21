  .syntax unified
  .cpu cortex-m7
  .fpu softvfp
  .thumb

.global g_pfnVectors
.global Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word _sidata
/* start address for the .data section. defined in linker script */
.word _sdata
/* end address for the .data section. defined in linker script */
.word _edata
/* start address for the .bss section. defined in linker script */
.word _sbss
/* end address for the .bss section. defined in linker script */
.word _ebss

.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
  ldr   sp, =_estack      /* set stack pointer */

/* Copy the data segment initializers from flash to SRAM */
  ldr r0, =_sdata
  ldr r1, =_edata
  ldr r2, =_sidata
  movs r3, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

LoopCopyDataInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc CopyDataInit

/* Zero fill the bss segment. */
  ldr r2, =_sbss
  ldr r4, =_ebss
  movs r3, #0
  b LoopFillZerobss

FillZerobss:
  str  r3, [r2]
  adds r2, r2, #4

LoopFillZerobss:
  cmp r2, r4
  bcc FillZerobss

/* Call the application's entry point.*/
  bl  main
  bx  lr
.size Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 *
 * @param  None
 * @retval None
*/
    .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler

/******************************************************************************
*
* The minimal vector table for a Cortex-M7.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
  .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object
  .size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_Handler
  .word DebugMon_Handler
  .word 0
  .word PendSV_Handler
  .word SysTick_Handler

  /* External Interrupts */
  /* We need to fill these to reach the DMA/SPI IRQs */
  /* This is a simplified table. In production, use the full table from ST. */
  /* Positions are fixed. */
  .word WWDG_IRQHandler                   /* Window WatchDog              */
  .word PVD_AVD_IRQHandler                /* PVD/AVD through EXTI Line Detection */
  .word TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line */
  .word RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line */
  .word FLASH_IRQHandler                  /* FLASH                        */
  .word RCC_IRQHandler                    /* RCC                          */
  .word EXTI0_IRQHandler                  /* EXTI Line0                   */
  .word EXTI1_IRQHandler                  /* EXTI Line1                   */
  .word EXTI2_IRQHandler                  /* EXTI Line2                   */
  .word EXTI3_IRQHandler                  /* EXTI Line3                   */
  .word EXTI4_IRQHandler                  /* EXTI Line4                   */
  .word DMA1_Stream0_IRQHandler           /* DMA1 Stream 0                */
  .word DMA1_Stream1_IRQHandler           /* DMA1 Stream 1                */
  .word DMA1_Stream2_IRQHandler           /* DMA1 Stream 2                */
  .word DMA1_Stream3_IRQHandler           /* DMA1 Stream 3                */
  .word DMA1_Stream4_IRQHandler           /* DMA1 Stream 4                */
  .word DMA1_Stream5_IRQHandler           /* DMA1 Stream 5                */
  .word DMA1_Stream6_IRQHandler           /* DMA1 Stream 6                */
  .word ADC_IRQHandler                    /* ADC1, ADC2                   */
  .word FDCAN1_IT0_IRQHandler             /* FDCAN1 interrupt line 0      */
  .word FDCAN2_IT0_IRQHandler             /* FDCAN2 interrupt line 0      */
  .word FDCAN1_IT1_IRQHandler             /* FDCAN1 interrupt line 1      */
  .word FDCAN2_IT1_IRQHandler             /* FDCAN2 interrupt line 1      */
  .word EXTI9_5_IRQHandler                /* External Line[9:5]s          */
  .word TIM1_BRK_IRQHandler               /* TIM1 Break                   */
  .word TIM1_UP_IRQHandler                /* TIM1 Update                  */
  .word TIM1_TRG_COM_IRQHandler           /* TIM1 Trigger and Commutation */
  .word TIM1_CC_IRQHandler                /* TIM1 Capture Compare         */
  .word TIM2_IRQHandler                   /* TIM2                         */
  .word TIM3_IRQHandler                   /* TIM3                         */
  .word TIM4_IRQHandler                   /* TIM4                         */
  .word I2C1_EV_IRQHandler                /* I2C1 Event                   */
  .word I2C1_ER_IRQHandler                /* I2C1 Error                   */
  .word I2C2_EV_IRQHandler                /* I2C2 Event                   */
  .word I2C2_ER_IRQHandler                /* I2C2 Error                   */
  .word SPI1_IRQHandler                   /* SPI1                         */
  .word SPI2_IRQHandler                   /* SPI2                         */
  .word USART1_IRQHandler                 /* USART1                       */
  .word USART2_IRQHandler                 /* USART2                       */
  .word USART3_IRQHandler                 /* USART3                       */
  .word EXTI15_10_IRQHandler              /* External Line[15:10]s        */
  .word RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line */
  .word 0                                 /* Reserved                     */
  .word TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12         */
  .word TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13        */
  .word TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14 */
  .word TIM8_CC_IRQHandler                /* TIM8 Capture Compare         */
  .word DMA1_Stream7_IRQHandler           /* DMA1 Stream7                 */
  .word FMC_IRQHandler                    /* FMC                          */
  .word SDMMC1_IRQHandler                 /* SDMMC1                       */
  .word TIM5_IRQHandler                   /* TIM5                         */
  .word SPI3_IRQHandler                   /* SPI3                         */
  .word UART4_IRQHandler                  /* UART4                        */
  .word UART5_IRQHandler                  /* UART5                        */
  .word TIM6_DAC_IRQHandler               /* TIM6 and DAC1&2 underrun errors */
  .word TIM7_IRQHandler                   /* TIM7                         */
  .word DMA2_Stream0_IRQHandler           /* DMA2 Stream 0                */
  .word DMA2_Stream1_IRQHandler           /* DMA2 Stream 1                */
  .word DMA2_Stream2_IRQHandler           /* DMA2 Stream 2                */
  .word DMA2_Stream3_IRQHandler           /* DMA2 Stream 3                */
  .word DMA2_Stream4_IRQHandler           /* DMA2 Stream 4                */
  .word ETH_IRQHandler                    /* Ethernet                     */
  .word ETH_WKUP_IRQHandler               /* Ethernet Wakeup through EXTI line */
  .word FDCAN_CAL_IRQHandler              /* FDCAN calibration unit interrupt*/
  .word 0                                 /* Reserved                     */
  .word 0                                 /* Reserved                     */
  .word 0                                 /* Reserved                     */
  .word 0                                 /* Reserved                     */
  .word DMA2_Stream5_IRQHandler           /* DMA2 Stream 5                */
  .word DMA2_Stream6_IRQHandler           /* DMA2 Stream 6                */
  .word DMA2_Stream7_IRQHandler           /* DMA2 Stream 7                */
  .word USART6_IRQHandler                 /* USART6                       */
  .word I2C3_EV_IRQHandler                /* I2C3 event                   */
  .word I2C3_ER_IRQHandler                /* I2C3 error                   */
  .word OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out   */
  .word OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In    */
  .word OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI */
  .word OTG_HS_IRQHandler                 /* USB OTG HS                   */
  .word DCMI_IRQHandler                   /* DCMI                         */
  .word CRYP_IRQHandler                   /* CRYP crypto                  */
  .word HASH_RNG_IRQHandler               /* Hash and Rng                 */
  .word FPU_IRQHandler                    /* FPU                          */
  .word UART7_IRQHandler                  /* UART7                        */
  .word UART8_IRQHandler                  /* UART8                        */
  .word SPI4_IRQHandler                   /* SPI4                         */
  .word SPI5_IRQHandler                   /* SPI5                         */
  .word SPI6_IRQHandler                   /* SPI6                         */
  .word SAIT_IRQHandler                   /* SAI1 global                  */
  .word LTDC_IRQHandler                   /* LTDC                         */
  .word LTDC_ER_IRQHandler                /* LTDC error                   */
  .word DMA2D_IRQHandler                  /* DMA2D                        */
  .word SAI2_IRQHandler                   /* SAI2 global                  */
  .word QUADSPI_IRQHandler                /* QUADSPI                      */
  .word LPTIM1_IRQHandler                 /* LPTIM1                       */
  .word CEC_IRQHandler                    /* HDMI_CEC                     */
  .word I2C4_EV_IRQHandler                /* I2C4 Event                   */
  .word I2C4_ER_IRQHandler                /* I2C4 Error                   */
  .word SPDIF_RX_IRQHandler               /* SPDIF_RX                     */
  .word OTG_FS_EP1_OUT_IRQHandler         /* USB OTG FS End Point 1 Out   */
  .word OTG_FS_EP1_IN_IRQHandler          /* USB OTG FS End Point 1 In    */
  .word OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI */
  .word OTG_FS_IRQHandler                 /* USB OTG FS                   */
  /* ... Continue as needed ... */

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

  .weak      NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak      HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak      MemManage_Handler
  .thumb_set MemManage_Handler,Default_Handler

  .weak      BusFault_Handler
  .thumb_set BusFault_Handler,Default_Handler

  .weak      UsageFault_Handler
  .thumb_set UsageFault_Handler,Default_Handler

  .weak      SVC_Handler
  .thumb_set SVC_Handler,Default_Handler

  .weak      DebugMon_Handler
  .thumb_set DebugMon_Handler,Default_Handler

  .weak      PendSV_Handler
  .thumb_set PendSV_Handler,Default_Handler

  .weak      SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler

  .weak      DMA1_Stream0_IRQHandler
  .thumb_set DMA1_Stream0_IRQHandler,Default_Handler

  .weak      DMA1_Stream1_IRQHandler
  .thumb_set DMA1_Stream1_IRQHandler,Default_Handler

  .weak      SPI2_IRQHandler
  .thumb_set SPI2_IRQHandler,Default_Handler

  .weak      OTG_FS_IRQHandler
  .thumb_set OTG_FS_IRQHandler,Default_Handler
