#ifndef LN882H_HAL_H
#define LN882H_HAL_H

#include <stdint.h>
//#include <stdlib.h>
//#include <stdio.h>
#include <stdbool.h>

#include "ln882h.h"
#include "hal/hal_cache.h"
#include "hal/hal_clock.h"
#include "hal/hal_efuse.h"
#include "hal/hal_flash.h"
#include "hal/hal_gpio.h"
#include "hal/hal_misc.h"
#include "hal/hal_uart.h"

//#define PLATFORM_SUPPORTS_OTP

#define FLASH_BASE CACHE_FLASH_BASE
#undef FLASH_PAGE_SIZE

extern uint8_t __StackTop[];

uint16_t crc16_ccitt(const char* buf, int len);
uint32_t ln_crc32_signle_cal(uint8_t* ptr, int len);

#endif
