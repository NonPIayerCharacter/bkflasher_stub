#ifndef GD32VW553_HAL_H
#define GD32VW553_HAL_H

#include <stdint.h>
//#include <stdlib.h>
//#include <stdio.h>
#include <stdbool.h>
#include "gd32vw55x_crc.h"
#include "gd32vw55x_fmc.h"
#include "gd32vw55x_hau.h"
#include "rom_export.h"
#include "rom_export_mbedtls.h"
#include "mbedtls/entropy_poll.h"

#define RF_EFUSE_BASE ((volatile uint8_t *)0x40022200)

void rom_symbol_init(void);

#endif
