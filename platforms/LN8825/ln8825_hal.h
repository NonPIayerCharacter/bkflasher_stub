#ifndef LN8825_HAL_H
#define LN8825_HAL_H

#include <stdint.h>
//#include <stdlib.h>
//#include <stdio.h>
#include <stdbool.h>

#include "ln88xx.h"
#include "hal/cache.h"
#include "hal/hal_efuse.h"
#include "hal/flash.h"
#include "hal/hal_gpio.h"
#include "hal/hal_uart.h"
#include "hal/hal_syscon.h"
#include "qspi.h"
#include "proj_config.h"

#define FLASH_BASE CACHE_FLASH_BASE
#undef FLASH_PAGE_SIZE

extern uint8_t __StackTop[];

uint16_t crc16_ccitt(const char* buf, int len);
uint32_t ln_crc32_signle_cal(uint8_t* ptr, int len);

#endif
