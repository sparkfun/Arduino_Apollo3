ENTRY(Reset_Handler)
MEMORY
{
  FLASH (rx) : ORIGIN = 0x0000C000, LENGTH = 0x000F4000
  RAM_NVIC (rwx) : ORIGIN = 0x10000000, LENGTH = 0x100
  RAM (rwx) : ORIGIN = (0x10000000 + 0x100), LENGTH = (384K - (0x100))
}
SECTIONS
{
  .text :
  {
    . = ALIGN(4);
    _stext = .;
    KEEP(*(.isr_vector))
    KEEP(*(.ble_patch))
    *(.text)
    *(.text*)
    KEEP(*(.init))
    KEEP(*(.fini))
    *crtbegin.o(.ctors)
    *crtbegin?.o(.ctors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    *(SORT(.ctors.*))
    *(.ctors)
    *crtbegin.o(.dtors)
    *crtbegin?.o(.dtors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    *(SORT(.dtors.*))
    *(.dtors)
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    KEEP(*(.eh_frame*))
    . = ALIGN(4);
  } > FLASH
  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > FLASH
  __exidx_start = .;
  .ARM.exidx :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > FLASH
  __exidx_end = .;
  __etext = ALIGN(8);
    .data : AT (__etext)
    {
        __data_start__ = .;
        *(.data*)
        . = ALIGN(8);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(8);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(8);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
        KEEP(*(.jcr*))
        . = ALIGN(8);
        __data_end__ = .;
    } > RAM
    .uninitialized (NOLOAD):
    {
        . = ALIGN(32);
        __uninitialized_start = .;
        *(.uninitialized)
        KEEP(*(.keep.uninitialized))
        . = ALIGN(32);
        __uninitialized_end = .;
    } > RAM
  .bss :
  {
    . = ALIGN(8);
    _sbss = .;
    __bss_start__ = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(8);
    _ebss = .;
    __bss_end__ = .;
  } > RAM
  .heap (NOLOAD):
  {
    . = ALIGN(4);
    __end__ = .;
    PROVIDE( end = . );
    _sheap = .;
    . = ORIGIN(RAM) + LENGTH(RAM) - 0x400 -8;
    __HeapLimit = .;
  } >RAM
  .stack_dummy (NOLOAD):
  {
      . = ALIGN(8);
      *(.stack*)
  } > RAM
  __StackTop = ORIGIN(RAM) + LENGTH(RAM)-8;
  __StackLimit = __StackTop - 0x400;
  PROVIDE(__stack = __StackTop);
  PROVIDE(_sstack = __StackTop);
}