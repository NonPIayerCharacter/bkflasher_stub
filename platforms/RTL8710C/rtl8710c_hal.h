#ifndef RTL8710C_HAL_H
#define RTL8710C_HAL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define __I        volatile const
#define __O        volatile
#define __IO       volatile
#define __IM       volatile const
#define __OM       volatile
#define __IOM      volatile
typedef uint32_t   hal_status_t;
typedef uint32_t   HAL_Status;
typedef void* phal_gdma_adaptor_t;
typedef void* hal_gdma_adaptor_t;
typedef void* irq_handler_t;
typedef bool BOOL;
typedef bool BOOLEAN;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
#define ENABLE 1
#define DISABLE 0
#define HAL_WAIT_FOREVER            (0xFFFFFFFF)
#define HAL_OK              (0x00)
#define HAL_BUSY            (0x01)
#define HAL_TIMEOUT         (0x02)
#define HAL_ERR_PARA        (0x03)  // error with invaild parameters 
#define HAL_ERR_MEM         (0x04)  // error with memory allocation failed
#define HAL_ERR_HW          (0x05)  // error with hardware error
#define HAL_NOT_READY       (0x06)  // error with data not ready
#define HAL_NOT_FOUND       (0x07)  // error with data not found
#define HAL_NO_RESOURCE     (0x08)  // error with HW resource insufficent 
#define HAL_ERR_CONFLICT    (0x09)  // error with HW conflict
#define HAL_ERR_OS          (0x0A)  // error with RTOS functions
#define HAL_ERR_UNKNOWN     (0xEE)  // unknown error
#define _TRUE        true
#define _FALSE	     false
#define  DBG_SPIF_ERR(...)
#define  DBG_SPIF_WARN(...)
#define  DBG_SPIF_INFO(...)
#ifndef   __ASM
#define __ASM                                  __asm
#endif
#ifndef   __STATIC_INLINE
#define __STATIC_INLINE                        static inline
#endif
#ifndef   __STATIC_FORCEINLINE                 
#define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static inline
#endif  
__STATIC_FORCEINLINE void __DSB(void)
{
	__ASM volatile ("dsb 0xF":::"memory");
}
__STATIC_FORCEINLINE void hal_irq_enable(void* n)
{
}
#define UART_IRQn NULL

#include "rtl8710c_uart.h"
#include "hal_uart.h"
#include "rtl8710c_spic.h"

#define FLASH_TYPE_GD 4
#define FLASH_TYPE_GD32 5
#define FLASH_TYPE_XTX 6
#define FLASH_TYPE_EON 7
#define FLASH_TYPE_BOYA 8
#define FLASH_TYPE_XMC 9
#define FLASH_TYPE_ZBIT 10
#define FLASH_TYPE_TSTE 11
#define FLASH_TYPE_PUYA 12
#define FLASH_TYPE_GTEC 13

#define FLASH_BASE				SPI_FLASH_BASE
#define UART2_BASE				0x40040400UL
#define UART2					((UART0_Type*)UART2_BASE)
#define EFUSE_CTRL_SETTING		(uint32_t)(0x33300000)
#define LDO_OUT_DEFAULT_VOLT	0x0

extern uint8_t __bss_start__[];
extern uint8_t __bss_end__[];
extern const flash_pin_sel_t spic_flash_pins[3];
extern hal_spic_adaptor_t hal_spic_adaptor;
extern hal_spic_adaptor_t* pglob_spic_adaptor;
typedef struct utility_func_stubs_s
{
	uint32_t* config_debug_err;
	uint32_t* config_debug_warn;
	uint32_t* config_debug_info;

	int (*memcmp)(const void* av, const void* bv, size_t len);
	void* (*memcpy)(void* s1, const void* s2, size_t n);
	void* (*memmove)(void* destaddr, const void* sourceaddr, unsigned length);
	void* (*memset)(void* dst0, int val, size_t length);

	void (*dump_bytes)(u8* pdata, u32 len);
	void (*dump_words)(u8* src, u32 len);

	// B-Cut
	int (*memcmp_s)(const void* av, const void* bv, size_t len);

	uint32_t reserved[7];  // reserved space for next ROM code version function table extending.
} utility_func_stubs_t;

typedef struct hal_efuse_func_stubs_s
{
	uint32_t(*hal_efuse_autoload_en) (uint8_t enable, uint8_t l25out_voltage);
	uint32_t(*hal_efuse_hci_autoload_en) (uint8_t enable, uint8_t l25out_voltage);
	uint32_t(*hal_efuse_read) (uint32_t ctrl_setting, uint16_t addr, uint8_t* data, uint8_t l25out_voltage);
	uint32_t(*hal_efuse_write) (uint32_t ctrl_setting, uint16_t addr, uint8_t data, uint8_t l25out_voltage);
	uint32_t(*hal_user_otp_get) (uint32_t ctrl_setting, uint8_t* puser_otp, uint8_t l25out_voltage);
	uint32_t(*hal_user_otp_set) (uint32_t ctrl_setting, uint8_t* puser_otp, uint8_t l25out_voltage);
	uint32_t(*hal_sec_efuse_read) (uint32_t ctrl_setting, uint16_t addr, uint8_t* pdata, uint8_t l25out_voltage);
	uint32_t(*hal_sec_efuse_write) (uint32_t ctrl_setting, uint16_t addr, uint8_t data, uint8_t l25out_voltage);
	uint32_t(*hal_sec_key_get) (uint32_t ctrl_setting, uint8_t* psec_key, uint8_t key_num, uint32_t length, uint8_t l25out_voltage);
	uint32_t(*hal_sec_key_write) (uint32_t ctrl_setting, uint8_t* psec_key, uint8_t key_num, uint8_t l25out_voltage);
	uint32_t(*hal_susec_key_get) (uint32_t ctrl_setting, uint8_t* psusec_key, uint8_t l25out_voltage);
	uint32_t(*hal_susec_key_write) (uint32_t ctrl_setting, uint8_t* psusec_key, uint8_t l25out_voltage);
	uint32_t(*hal_sec_zone_write) (uint32_t ctrl_setting, uint8_t offset, uint8_t length, uint8_t* pcontent, uint8_t l25out_voltage);
	uint32_t(*hal_s_jtag_key_write) (uint32_t ctrl_setting, uint8_t* pkey, uint8_t l25out_voltage);
	uint32_t(*hal_ns_jtag_key_write) (uint32_t ctrl_setting, uint8_t* pkey, uint8_t l25out_voltage);
	uint32_t reserved[12]; // reserved space for next ROM code version function table extending.
}hal_efuse_func_stubs_t;

typedef union
{
	__IOM uint32_t w;                           /*!< (@ 0x00000120) Valid Command Register                                     */

	struct
	{
		__IOM uint32_t frd_single : 1;            /*!< [0..0] Execute fast read for auto read mode.                              */
		__IOM uint32_t rd_dual_i : 1;            /*!< [1..1] Execute dual data write for auto read mode. (1-1-2)                */
		__IOM uint32_t rd_dual_io : 1;            /*!< [2..2] Execute dual address data read for auto read mode. (1-2-2)         */
		__IOM uint32_t rd_quad_o : 1;            /*!< [3..3] Execute quad data write for auto read mode. (1-1-4)                */
		__IOM uint32_t rd_quad_io : 1;            /*!< [4..4] Execute quad address data read for auto read mode. (1-4-4)         */
		__IOM uint32_t wr_dual_i : 1;            /*!< [5..5] Execute dual data write for auto write mode. (1-1-2)               */
		__IOM uint32_t wr_dual_ii : 1;            /*!< [6..6] Execute dual address data write for auto write mode.
													   (1-2-2)                                                                   */
		__IOM uint32_t wr_quad_i : 1;            /*!< [7..7] Execute quad data write for auto write mode. (1-1-4)               */
		__IOM uint32_t wr_quad_ii : 1;            /*!< [8..8] Execute quad address data write for auto write mode.
													   (1-4-4)                                                                   */
		__IOM uint32_t wr_blocking : 1;           /*!< [9..9] Accept next operation after the write data push to FIFO
													   and FIFO is pop data to empty by SPIC. Should always be
													   1.                                                                        */
		__IM  uint32_t : 1;
		__IOM uint32_t prm_en : 1;            /*!< [11..11] Enable SPIC performance read mode in auto mode.                  */
		__IOM uint32_t ctrlr0_ch : 1;            /*!< [12..12] Set this bit, then SPIC refers cmd_ch / data_ch / addr_en
													   / cmd_ddr_en / data_ddr_en / addr_ddr_en bit fields in
													   Control Register 0 in auto mode.                                          */
		__IM  uint32_t : 1;
		__IOM uint32_t seq_trans_en : 1;          /*!< [14..14] Set 1 to enable read sequential transaction determination
													   function. If the auto read address is sequenctial, users
													   can save command phase and address phase under this mode.                 */
	} b;                                        /*!< bit fields for spic_valid_cmd */
} spic_valid_cmd_t, * pspic_valid_cmd_t;

typedef struct hal_cache_func_stubs_s
{
	void (*icache_enable) (void);
	void (*icache_disable) (void);
	void (*icache_invalidate) (void);
	void (*dcache_enable) (void);
	void (*dcache_disable) (void);
	void (*dcache_invalidate) (void);
	void (*dcache_clean) (void);
	void (*dcache_clean_invalidate) (void);
	void (*dcache_invalidate_by_addr) (uint32_t* addr, int32_t dsize);
	void (*dcache_clean_by_addr) (uint32_t* addr, int32_t dsize);
	void (*dcache_clean_invalidate_by_addr) (uint32_t* addr, int32_t dsize);
	uint32_t reserved[4];  // reserved space for next ROM code version function table extending.
} hal_cache_func_stubs_t;

int hal_crypto_engine_init(void);
int hal_crypto_sha2_256_init(void);
int hal_crypto_sha2_256_update(const u8* message, const u32 msglen);
int hal_crypto_sha2_256_final(u8* pDigest);

extern const hal_efuse_func_stubs_t hal_efuse_stubs;
extern const hal_uart_func_stubs_t hal_uart_stubs;
extern const utility_func_stubs_t utility_stubs;
extern const hal_spic_func_stubs_t hal_spic_stubs;
extern const hal_flash_func_stubs_t hal_flash_stubs;
extern const hal_cache_func_stubs_t hal_cache_stubs;

__STATIC_INLINE void dcache_clean_invalidate(void)
{
	hal_cache_stubs.dcache_clean_invalidate();
}

__STATIC_INLINE void dcache_invalidate_by_addr(uint32_t* addr, int32_t dsize)
{
	hal_cache_stubs.dcache_invalidate_by_addr(addr, dsize);
}
#endif
