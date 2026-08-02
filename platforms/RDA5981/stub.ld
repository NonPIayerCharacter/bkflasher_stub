MEMORY
{
    IRAM  (rwx) : ORIGIN = 0x00100000, LENGTH = 128K
    DRAM1 (rwx) : ORIGIN = 0x00180000, LENGTH = 96K
    DRAM2 (rwx) : ORIGIN = 0x40100000, LENGTH = 128K
}

ENTRY(Reset_Handler)

SECTIONS
{
    .text ORIGIN(IRAM) : ALIGN(4)
    {
        KEEP(*(.Reset_Handler))
        *(.text*)
        *(.rodata*)
    } > IRAM

    .data : ALIGN(4)
    {
        *(.data*)
    } > IRAM

    .bss (NOLOAD) : ALIGN(4)
    {
        __bss_start__ = .;
        *(.bss*)
        __bss_end__ = .;
    } > IRAM

    .dram1 (NOLOAD) :
    {
        *(.dram1*)
    } > DRAM1

    .dram2 (NOLOAD) :
    {
        *(.dram2*)
    } > DRAM2

    __StackTop = ORIGIN(IRAM) + LENGTH(IRAM);
    /DISCARD/ :
    {
        *(.ARM.exidx*)
        *(.ARM.extab*)
        *(.comment*)
        *(.note*)
    }
}
