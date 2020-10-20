/******************************************************************************
 *
 * blank_slate.ld - Linker script for applications using startup_gnu.c
 *
 *****************************************************************************/
ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00010000, LENGTH = 960K /*Modified from 0xC000 to work with SparkFun ABL*/
    SRAM (rwx) : ORIGIN = 0x10000000, LENGTH = 384K
}

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        KEEP(*(.patch))
        *(.text)
        *(.text*)

        . = ALIGN(4);  
        __init_array_start = .;
        KEEP(*(.init_array))        /* C++ constructors */ 
        KEEP(*(.ctors))             /* and vtable init */
        __init_array_end = .; 

        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* User stack section initialized by startup code. */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        *(.stack)
        *(.stack*)
        . = ALIGN(8);
    } > SRAM

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM AT>FLASH

    /* used by startup to initialize data */
    _init_data = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM

    .heap (COPY):
	{
		__end__ = .;
		PROVIDE(end = .);
		*(.heap*)
		__HeapLimit = .;
	} > SRAM

    /* TODO: Collision Checking (at link time) */

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
