MEMORY
{
    TCM    (rwx) : ORIGIN = 0x00100000, LENGTH = 64K
    SYSRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 256K - 32K
}

ENTRY(Reset_Handler)

SECTIONS
{
    .text ORIGIN(SYSRAM) : ALIGN(4)
    {
        KEEP(*(.vectors))
        KEEP(*(.Reset_Handler))
        *(.text*)
        *(.rodata*)
    } > SYSRAM

    .data : ALIGN(4)
    {
        *(.data*)
    } > SYSRAM

    .bss (NOLOAD) : ALIGN(4)
    {
        __bss_start__ = .;
        *(.bss*)
        __bss_end__ = .;
    } > SYSRAM

    .dram1 (NOLOAD) :
    {
        *(.dram1*)
    } > TCM

    .dram2 (NOLOAD) :
    {
        *(.dram2*)
    } > TCM

    __StackTop = ORIGIN(SYSRAM) + LENGTH(SYSRAM);
    /DISCARD/ :
    {
        *(.ARM.exidx*)
        *(.ARM.extab*)
        *(.comment*)
        *(.note*)
    }
}
