/*
 * Linker Script for STM32H743VIT6
 * RAM: 1MB Total (DTCM: 128KB, ITCM: 64KB, AXI SRAM: 512KB, SRAM1/2/3: 288KB)
 * Flash: 2MB
 *
 * We place Stack and Global Variables (like the Audio Buffers and OctaveGenerator) in DTCM for speed.
 * AXI SRAM is used for general heap or bulk data if needed.
 */

ENTRY(Reset_Handler)

MEMORY
{
  DTCMRAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 128K
  ITCMRAM (xrw)  : ORIGIN = 0x00000000, LENGTH = 64K
  RAM_D1  (xrw)  : ORIGIN = 0x24000000, LENGTH = 512K
  RAM_D2  (xrw)  : ORIGIN = 0x30000000, LENGTH = 288K
  RAM_D3  (xrw)  : ORIGIN = 0x38000000, LENGTH = 64K
  FLASH   (rx)   : ORIGIN = 0x08000000, LENGTH = 2048K
}

_estack = ORIGIN(DTCMRAM) + LENGTH(DTCMRAM); /* End of DTCM */

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } > FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;
  } > FLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } > FLASH

  /* Initialize the stack at the end of DTCM */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + 0x400; /* Min_Heap_Size */
    . = . + 0x800; /* Min_Stack_Size */
    . = ALIGN(8);
  } > DTCMRAM

  /* Initialized data sections goes into RAM, load LMA copy after code */
  _sidata = LOADADDR(.data);

  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > DTCMRAM AT> FLASH

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } > DTCMRAM

  /* Critical DSP code can be copied to ITCM for performance */
  /* For now we stick to Flash execution with Cache enabled */

}
