#include <nds32_init.inc>

.section .nds32_init, "ax"

    .align 2
    j _boot_start

.text
.global _boot_start
_boot_start:
    nds32_init
    bal boot_main
