#ifndef RTL8721DA_HAL_H
#define RTL8721DA_HAL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "tlsf/tlsf.h"
#include "littlefs/lfs.h"
#include "rtl_kv/kv.h"

//#define PLATFORM_SUPPORTS_OTP

#define __I     volatile const       /*!< Defines 'read only' permissions                 */
#define __O     volatile             /*!< Defines 'write only' permissions                */
#define __IO    volatile             /*!< Defines 'read / write' permissions              */

#define HAL_READ32(base, addr)				((uint32_t)(*((volatile uint32_t*)(base + addr))))
#define HAL_WRITE32(base, addr, value32)	((*((volatile uint32_t*)(base + addr))) = ((uint32_t)(value32)))
#define HAL_READ16(base, addr)				((uint16_t)(*((volatile uint16_t*)(base + addr))))
#define HAL_WRITE16(base, addr, value)		((*((volatile uint16_t*)(base + addr))) = ((uint16_t)(value)))
#define HAL_READ8(base, addr)				(*((volatile uint8_t*)(base + addr)))
#define HAL_WRITE8(base, addr, value)		((*((volatile uint8_t*)(base + addr))) = value)

typedef struct
{
	void (*rdp_decrypt_func)(uint32_t addr, uint8_t* key, uint8_t  cnt, uint8_t* buf);
	uint32_t psram_s_start_addr;
} BOOT_EXPORT_SYMB_TABLE;

typedef struct _RAM_FUNCTION_START_TABLE_
{
	void (*RamStartFun)(void);
	void (*RamWakeupFun)(void);
	void (*RamPatchFun0)(void);
	void (*RamPatchFun1)(void);
	void (*RamPatchFun2)(void);
	void (*FlashStartFun)(void);
	uint32_t Img1ValidCode;
	BOOT_EXPORT_SYMB_TABLE* ExportTable;
} RAM_FUNCTION_START_TABLE, * PRAM_FUNCTION_START_TABLE;

typedef struct
{
	__IO uint32_t LOGUART_UART_DLL;                       /*!< UART DIVISOR LENGTH REGISTER,  Address offset: 0x000 */
	__IO uint32_t LOGUART_UART_IER;                       /*!< UART INTERRUPT ENABLE REGISTER,  Address offset: 0x004 */
	__I  uint32_t LOGUART_UART_IIR;                       /*!< UART INTERRUPT IDENTIFICATION REGISTER,  Address offset: 0x008 */
	__IO uint32_t LOGUART_UART_LCR;                       /*!< UART LINE CONTROL REGISTER,  Address offset: 0x00C */
	__IO uint32_t LOGUART_UART_MCR;                       /*!< UART MODEM CONTROL REGISTER,  Address offset: 0x010 */
	__I  uint32_t LOGUART_UART_LSR;                       /*!< UART LINE STATUS REGISTER,  Address offset: 0x014 */
	__I  uint32_t LOGUART_UART_MSR;                       /*!< UART MODEM STATUS REGISTER,  Address offset: 0x018 */
	__IO uint32_t LOGUART_UART_SCR;                       /*!< UART SCRATCH PAD REGISTER,  Address offset: 0x01C */
	__IO uint32_t LOGUART_UART_STSR;                      /*!< UART STS REGISTER,  Address offset: 0x020 */
	__I  uint32_t LOGUART_UART_RBR;                       /*!< UART RECEIVER BUFFER REGISTER,  Address offset: 0x024 */
	__IO uint32_t LOGUART_UART_MISCR;                     /*!< UART MISC CONTROL REGISTER,  Address offset: 0x028 */
	__IO uint32_t LOGUART_UART_REG_SIR_TX_PWC0;           /*!< UART IRDA SIR TX PULSE WIDTH CONTROL 0 REGISTER,  Address offset: 0x02C */
	__IO uint32_t LOGUART_UART_REG_SIR_RX_PFC;            /*!< UART IRDA SIR RX PULSE FILTER CONTROL REGISTER,  Address offset: 0x030 */
	__IO uint32_t LOGUART_UART_REG_BAUD_MON;              /*!< UART BAUD MONITOR REGISTER,  Address offset: 0x034 */
	__IO uint32_t RSVD0;                                  /*!<  Reserved,  Address offset:0x38 */
	__I  uint32_t LOGUART_UART_REG_DBG;                   /*!< UART DEBUG REGISTER,  Address offset: 0x03C */
	__IO uint32_t LOGUART_UART_REG_RX_PATH_CTRL;          /*!< UART RX PATH CONTROL REGISTER,  Address offset: 0x040 */
	__IO uint32_t LOGUART_UART_REG_MON_BAUD_CTRL;         /*!< UART MONITOR BAUD RATE CONTROL REGISTER,  Address offset: 0x044 */
	__IO uint32_t LOGUART_UART_REG_MON_BAUD_STS;          /*!< UART MONITOR BAUD RATE STATUS REGISTER,  Address offset: 0x048 */
	__I  uint32_t LOGUART_UART_REG_MON_CYC_NUM;           /*!< UART MONITORED CYCLE NUMBER REGISTER,  Address offset: 0x04C */
	__IO uint32_t LOGUART_UART_REG_RX_BYTE_CNT;           /*!< UART RX DATA BYTE COUNT REGISTER,  Address offset: 0x050 */
	__IO uint32_t LOGUART_UART_FCR;                       /*!< UART FIFO CONTROL REGISTER,  Address offset: 0x054 */
	__IO uint32_t LOGUART_UART_AGGC;                      /*!< UART AGG CONTROL REGISTER,  Address offset: 0x058 */
	__O  uint32_t LOGUART_UART_THRx[4];                   /*!< UART TRANSMITTER HOLDING REGISTER x Register,  Address offset: 0x05C-0x068 */
	__IO uint32_t LOGUART_UART_RP_LCR;                    /*!< UART RELAY RX PATH LINE CONTROL REGISTER,  Address offset: 0x06C */
	__IO uint32_t LOGUART_UART_RP_RX_PATH_CTRL;           /*!< UART RELAY RX PATH CONTROL REGISTER,  Address offset: 0x070 */
	__O  uint32_t LOGUART_UART_ICR;                       /*!< UART INTERRUPT CLEAR REGISTER,  Address offset: 0x074 */
	__IO uint32_t LOGUART_INTERRUPT_MASK;                 /*!< LOGUART INTERRUPT MASK REGISTER,  Address offset: 0x078 */
	__IO uint32_t RSVD1[29];                              /*!<  Reserved,  Address offset:0x7C-0xEC */
	__I  uint32_t LOGUART_DUMMY;                          /*!< LOGUART DUMMY REGISTER,  Address offset: 0x0F0 */
} LOGUART_TypeDef;

typedef struct
{
	uint32_t FLASH_Id;				/*!< Specifies the flash vendor ID.
								This parameter can be a value of @ref FLASH_Vendor_ID */
	uint8_t FLASH_cur_bitmode;		/*!< Specifies the current bitmode of SPIC.
								This parameter can be a value of @ref FLASH_Bit_Mode */
	uint8_t FLASH_baud_rate;			/*!< Specifies the spi_sclk divider value. The frequency of spi_sclk is derived from:
								Frequency of spi_sclk = Frequency of oc_clk / (2 * FLASH_baud_rate) */
	uint8_t FLASH_baud_boot;			/*!< Specifies the spi_sclk divider value for rom boot. The frequency of spi_sclk is derived from:
								Frequency of spi_sclk = Frequency of oc_clk / (2 * FLASH_baud_rate) */
								/* debug log */
	uint8_t debug;					/*!< Specifies whether or not to print debug log.*/

	uint32_t FLASH_cur_cmd; 			/*!< Specifies the current read cmd which is used to read data from flash in current bitmode. */

	/* status bits define */
	uint32_t FLASH_QuadEn_bit;		/*!< Specifies the QE bit in status register which is used to enable Quad I/O mode . */
	uint32_t FLASH_Busy_bit;			/*!< Specifies the WIP(Write in Progress) bit in status register which indicates whether
								the device is busy in program/erase/write status register progress. */
	uint32_t FLASH_WLE_bit; 			/*!< Specifies the WEL(Write Enable Latch) bit in status register which indicates whether
								the device will accepts program/erase/write status register instructions*/
	uint32_t FLASH_Status2_exist;	/*!< Specifies whether this flash chip has Status Register2 or not.
								This parameter can be 0/1. 0 means it doesn't have Status Register2, 1 means it has Status Register2.*/

								/* valid R/W command set */
	uint32_t FLASH_rd_dual_o; 			/*!< Specifies dual data read cmd */
	uint32_t FLASH_rd_dual_io; 			/*!< Specifies dual data/addr read cmd */
	uint32_t FLASH_rd_quad_o; 			/*!< Specifies quad data read cmd */
	uint32_t FLASH_rd_quad_io; 			/*!< Specifies quad data/addr read cmd */
	uint32_t FLASH_dual_o_valid_cmd;		/*!< Specifies valid cmd of dual o bitmode to program/read flash in auto mode */
	uint32_t FLASH_dual_io_valid_cmd;	/*!< Specifies valid cmd of dual io bitmode to program/read flash in auto mode */
	uint32_t FLASH_quad_o_valid_cmd;		/*!< Specifies valid cmd of quad o bitmode to program/read flash in auto mode */
	uint32_t FLASH_quad_io_valid_cmd;	/*!< Specifies valid cmd of quad io bitmode to program/read flash in auto mode */
	uint32_t FLASH_rd_fast_single;		/*!< Specifies fast read cmd in auto mode.*/

	/* other command set */
	uint8_t FLASH_cmd_wr_en;			/*!< Specifies the Write Enable(WREN) instruction which is for setting WEL bit*/
	uint8_t FLASH_cmd_rd_id;			/*!< Specifies the Read ID instruction which is for getting the identity of the flash divice.*/
	uint8_t FLASH_cmd_rd_status;		/*!< Specifies the Read Status Register instruction which is for getting the status of flash */
	uint8_t FLASH_cmd_rd_status2;	/*!< Specifies the Read Status Register2 instruction which is for getting the status2 of flash */
	uint8_t FLASH_cmd_wr_status;		/*!< Specifies the Write Status Register instruction which is for setting the status register of flash */
	uint8_t FLASH_cmd_wr_status2;	/*!< Specifies the Write Status Register2 instruction which is for setting the status register2 of flash.
								In some flash chips, status2 write cmd != status1 write cmd, like: GD25Q32C, GD25Q64C,GD25Q128C etc.*/
	uint8_t FLASH_cmd_chip_e;		/*!< Specifies the Erase Chip instruction which is for erasing the whole chip*/
	uint8_t FLASH_cmd_block_e;		/*!< Specifies the Erase Block instruction which is for erasing 64kB*/
	uint8_t FLASH_cmd_sector_e;		/*!< Specifies the Erase Sector instruction which is for erasing 4kB*/
	uint8_t FLASH_cmd_pwdn_release;	/*!< Specifies the Release from Deep Power Down instruction which is for exiting power down mode.*/
	uint8_t FLASH_cmd_pwdn;			/*!< Specifies the Deep Power Down instruction which is for entering power down mode.*/
	uint8_t FLASH_dum_byte;

	/* calibration data */
	uint8_t FLASH_rd_sample_dly_cycle_cal;	/*!< Specifies the read sample delay cycle obtained from calibration. this is cal sample phase get from high speed cal */
	uint8_t FLASH_rd_sample_dly_cycle;		/*!< Specifies the read sample delay cycle obtained from calibration. this is current sample phase */
	uint8_t FLASH_rd_dummy_cycle[5];			/*!< Specifies the read dummy cycle of different bitmode according to
										flash datasheet*/

										/* new calibration */
	uint8_t phase_shift_idx;			/*!< Specifies the phase shift idx in new calibration.*/

	uint8_t FLASH_addr_phase_len;	/*!< Specifies the number of bytes in address phase (between command phase and write/read phase).
								This parameter can be 0/1/2/3. 0 means 4-byte address mode in SPI Flash.*/
	uint8_t FLASH_dum_en;			/*!< Specifies enable to push one dummy byte (DUM_BYTE_VAL) after pushing address to Flash in auto read.
								If RD_DUMMY_LENGTH = 0, it won't push the dummy byte. */
	uint8_t FLASH_pinmux;			/*!< Specifies which pinmux is used. PINMUX_S0 or PINMUX_S1*/
} FLASH_InitTypeDef;

typedef struct
{
	uint8_t isInit;						/*0: not init, 1: init							*/
	uint8_t isIntMode;					/*0: disable interrupt mode, 1: enable interrupt mode*/
	uint8_t isMD5;						/*MD5									*/
	uint8_t isSHA1;						/*SHA1									*/

	uint32_t usedlen;         			 /*!< number of bytes processed  */
	uint32_t state[16] __attribute__((aligned(32)));            /*!< intermediate digest state  */
	uint8_t isSHA2;						/*SHA2									*/
	uint8_t sha2type;					/*SHA2 type: SHA2_224/SHA2_256			*/
	uint8_t seq_hash;					/*1: enable initial value */
	uint8_t initial;
	uint32_t digestlen;					/*digest									*/
	uint32_t auth_type;					/*auth type								*/

	// sequential hash
	uint8_t hmac_seq_hash_first;			/* seq hash the first message payload block		*/
	uint8_t hmac_seq_hash_last;			/* seq hash the last message payload block		*/
	uint8_t hmac_seq_is_recorded;		/* enter seq hash or not						*/
	uint8_t hmac_seq_buf_is_used_bytes;	/* seq buf used bytes(total 64-bytes)			*/
	uint32_t hmac_seq_hash_total_len;	/* seq hash message total length 				*/
	uint32_t lasthash;
	unsigned char buffer[128];   	/*!< data block being processed */
	uint8_t isHMAC;
	unsigned char iopad[256];
} hw_sha_context;

typedef struct
{
	uint32_t	PLL_CLK;
	uint8_t		Vol_Type;
	uint8_t		KM4_CKD;/* KM4/SRAM is the same clock */
	uint8_t		KM0_CKD;
	uint8_t		PSRAMC_CKD;
} SocClk_Info_TypeDef;

typedef struct
{
	__IO uint32_t PLL_CTRL0;                              /*!<  Register,  Address offset: 0x000 */
	__IO uint32_t PLL_CTRL1;                              /*!<  Register,  Address offset: 0x004 */
	__IO uint32_t PLL_CTRL2;                              /*!<  Register,  Address offset: 0x008 */
	__IO uint32_t PLL_CTRL3;                              /*!<  Register,  Address offset: 0x00C */
	__IO uint32_t PLL_TEST;                               /*!<  Register,  Address offset: 0x010 */
	__IO uint32_t PLL_PS;                                 /*!<  Register,  Address offset: 0x014 */
	__IO uint32_t PLL_AUX_BG;                             /*!<  Register,  Address offset: 0x018 */
	__IO uint32_t PLL_TRNG;                               /*!<  Register,  Address offset: 0x01C */
	__IO uint32_t PLL_LPSD;                               /*!<  Register,  Address offset: 0x020 */
	__IO uint32_t PLL_LPAD0;                              /*!<  Register,  Address offset: 0x024 */
	__IO uint32_t PLL_LPAD1;                              /*!<  Register,  Address offset: 0x028 */
	__IO uint32_t PLL_BG_PWR_ON_CNT;                      /*!<  Register,  Address offset: 0x02C */
	__IO uint32_t PLL_ADC_PWR_ON_CNT;                     /*!<  Register,  Address offset: 0x030 */
	__IO uint32_t PLL_STATE;                              /*!<  Register,  Address offset: 0x034 */
	__I  uint32_t PLL_DEBUG;                              /*!<  Register,  Address offset: 0x038 */
	__IO uint32_t PLL_DUMMY;                              /*!<  Register,  Address offset: 0x03C */
} PLL_TypeDef;

typedef enum _SHA2_TYPE_
{
	SHA2_NONE = 0,
	SHA2_224 = 224 / 8,
	SHA2_256 = 256 / 8,
	SHA2_384 = 384 / 8,
	SHA2_512 = 512 / 8
} SHA2_TYPE;

#define PLL_BASE ((PLL_TypeDef *) (0x41008440))

#define IWDG_REG_BASE            0x41008C00

#define BIT_LSYS_CKSL_SPIC_PLL			1
#define BIT_LSYS_CKSL_SPIC_XTAL			0
#define CLKDIV(x)				(x)
#define CLKDIV_ROUND_UP(X, Y)	( ( ( X ) + ( Y ) - 1 ) / ( Y ) )

#define MHZ_TICK_CNT 		(1000000)
#define KM4_0P9V_CLK_LIMIT		(262 * MHZ_TICK_CNT)
#define KM4_1P0V_CLK_LIMIT		(345 * MHZ_TICK_CNT)
#define KM0_0P9V_CLK_LIMIT		(105 * MHZ_TICK_CNT)
#define KM0_1P0V_CLK_LIMIT		(115 * MHZ_TICK_CNT)

#define PSRAMC_CLK_LIMIT		(400 * MHZ_TICK_CNT)
#define SPIC_CLK_LIMIT			(208 * MHZ_TICK_CNT)	/* For Flash run up to 104MHz */
#define QSPI_CLK_LIMIT			(100 * MHZ_TICK_CNT)
#define PERI_CLK_LIMIT			(100 * MHZ_TICK_CNT)
#define SRAM_CLK_LIMIT			(200 * MHZ_TICK_CNT)

#define CLK_KM0_PLL			3 /* 3 for pll */
#define CLK_KM0_XTAL		1 /* Nearly all the Time, Lbus the same with XTAL*/
#define CLK_KM0_ANA_4M		0
#define SYSTEM_CTRL_BASE		0x41008000

#define _LONG_CALL_						__attribute__ ((long_call))
#define FLASH_BASE						0x08000000
#define UARTLOG_REG_BASE         0x4100F000        /* ID:S4-5, Inter. Type:APB4, Top Address:0x4100FFFF, Size(KB):4K, Clk Domain:PERI_LCLK */
#define LOGUART_DEV			((LOGUART_TypeDef		*) UARTLOG_REG_BASE)
#define EraseChip				0
#define EraseBlock				1
#define EraseSector				2
#define APBPeriph_FLASH                            ((uint32_t)(0  << 30) | (0x00000001 << 10))  /*!<R/W 0  Flash spic function enable 1: enable flash 0: disable flash */
#define APBPeriph_SHA                              ((uint32_t)(0  << 30) | (0x00000001 << 9))   /*!<R/W 0  1: enable IPSEC SHA function 0: disable */
#define APBPeriph_LX                               ((uint32_t)(0  << 30) | (0x00000001 << 7))   /*!<R/W 0  1: enable LX1 system function 0: disable */
#define APBPeriph_FLASH_CLOCK                      ((uint32_t)(0  << 30) | (0x00000001 << 10))  /*!<R/W 0  1: Enable SPIC source clock */
#define APBPeriph_SHA_CLOCK                        ((uint32_t)(0  << 30) | (0x00000001 << 9))   /*!<R/W 0  1: Enable ipsec SHA clock 0: Disable */
#define APBPeriph_LX_CLOCK                         ((uint32_t)(0  << 30) | (0x00000001 << 7))   /*!<R/W 0  1: Enable lx1 bus clock 0: Disable */

#define LSYS_BIT_CKSL_LOGUART                      ((uint32_t)0x00000001 << 29)          /*!<R/W 0  Select div_loguart's input clock (div_loguart's output clock is loguart trx clock ) 0: clock is xtal(source is dicided by cksl_loguart_xtal) 1: clock is osc4M */
#define LSYS_CKSL_LOGUART(x)                       ((uint32_t)(((x) & 0x00000001) << 29))
#define LSYS_GET_CKSL_LOGUART(x)                   ((uint32_t)(((x >> 29) & 0x00000001)))
#define LSYS_MASK_CKSL_UART                        ((uint32_t)0x00000007 << 25)          /*!<R/W 0  Select div_uart[n]'s input clock (div_uart[n]'s output clock is uart[n] rx clock). (n=0,1,2) 0: clock is xtal(source is dicided by cksl_uart_xtal) 1: clock is osc4M */
#define LSYS_CKSL_UART(x)                          (((uint32_t)((x) & 0x00000007) << 25))
#define LSYS_GET_CKSL_UART(x)                      ((uint32_t)(((x >> 25) & 0x00000007)))
#define LSYS_BIT_CKSL_LOGUART_XTAL                 ((uint32_t)0x00000001 << 24)          /*!<R/W 0  loguart_xtal clk select 0:clk_xtal_digi 1:clk_xtal_digi_div2 */
#define LSYS_CKSL_LOGUART_XTAL(x)                  ((uint32_t)(((x) & 0x00000001) << 24))
#define LSYS_GET_CKSL_LOGUART_XTAL(x)              ((uint32_t)(((x >> 24) & 0x00000001)))
#define LSYS_MASK_CKSL_UART_XTAL                   ((uint32_t)0x00000007 << 20)          /*!<R/W 0  uart xtal clk select, cksl_uart_xtal[n](n=0,1,2) 0: clk_xtal_digi 1:clk_xtal_digi_div2 */
#define LSYS_CKSL_UART_XTAL(x)                     (((uint32_t)((x) & 0x00000007) << 20))
#define LSYS_GET_CKSL_UART_XTAL(x)                 ((uint32_t)(((x >> 20) & 0x00000007)))
#define LSYS_MASK_CKSL_SPORT                       ((uint32_t)0x00000003 << 18)          /*!<R/W 2'b11  sport clock selection 0: equal to audio codec main clock (40M) 1: from cpu/dsp pll , after pll selection(cksl_i2s_pll) */
#define LSYS_CKSL_SPORT(x)                         (((uint32_t)((x) & 0x00000003) << 18))
#define LSYS_GET_CKSL_SPORT(x)                     ((uint32_t)(((x >> 18) & 0x00000003)))
#define LSYS_BIT_CKSL_OTPC                         ((uint32_t)0x00000001 << 14)          /*!<R/W 0h  otpc clock selection 0: osc4m 1: xtal40m Note: this bit will be set by pmc auto, when SW use otp, it must be set to 1 already */
#define LSYS_CKSL_OTPC(x)                          ((uint32_t)(((x) & 0x00000001) << 14))
#define LSYS_GET_CKSL_OTPC(x)                      ((uint32_t)(((x >> 14) & 0x00000001)))
#define LSYS_BIT_CKSL_IRDA                         ((uint32_t)0x00000001 << 13)          /*!<R/W 0h  IRDA clock selection 0: xtal 40m 1: pll divider out */
#define LSYS_CKSL_IRDA(x)                          ((uint32_t)(((x) & 0x00000001) << 13))
#define LSYS_GET_CKSL_IRDA(x)                      ((uint32_t)(((x >> 13) & 0x00000001)))
#define LSYS_BIT_CKSL_ADC4M                        ((uint32_t)0x00000001 << 12)          /*!<R/W 0  lp adc 4m clk selection 0: from osc4m 1: divider from xtal */
#define LSYS_CKSL_ADC4M(x)                         ((uint32_t)(((x) & 0x00000001) << 12))
#define LSYS_GET_CKSL_ADC4M(x)                     ((uint32_t)(((x >> 12) & 0x00000001)))
#define LSYS_BIT_CKSL_DTIM                         ((uint32_t)0x00000001 << 11)          /*!<R/W 0  dtim clock selection 0: xtal 1: 32k for sleep */
#define LSYS_CKSL_DTIM(x)                          ((uint32_t)(((x) & 0x00000001) << 11))
#define LSYS_GET_CKSL_DTIM(x)                      ((uint32_t)(((x >> 11) & 0x00000001)))
#define LSYS_BIT_CKSL_CTC                          ((uint32_t)0x00000001 << 10)          /*!<R/W 0h  CTC REG clock selection 0: lbus clock 1: ctc ip clock */
#define LSYS_CKSL_CTC(x)                           ((uint32_t)(((x) & 0x00000001) << 10))
#define LSYS_GET_CKSL_CTC(x)                       ((uint32_t)(((x >> 10) & 0x00000001)))
#define LSYS_BIT_CKSL_ADC                          ((uint32_t)0x00000001 << 9)          /*!<R/W 0  ADC INT clock selection 0: lbus clock 1: adc ip clock */
#define LSYS_CKSL_ADC(x)                           ((uint32_t)(((x) & 0x00000001) << 9))
#define LSYS_GET_CKSL_ADC(x)                       ((uint32_t)(((x >> 9) & 0x00000001)))
#define LSYS_BIT_CKSL_GPIO                         ((uint32_t)0x00000001 << 8)          /*!<R/W 0  GPIO INT clock selecton: 0: lbus clock 1: 32.768K clock from SDM */
#define LSYS_CKSL_GPIO(x)                          ((uint32_t)(((x) & 0x00000001) << 8))
#define LSYS_GET_CKSL_GPIO(x)                      ((uint32_t)(((x >> 8) & 0x00000001)))
#define LSYS_BIT_CKSL_PTIM                         ((uint32_t)0x00000001 << 7)          /*!<R/W 0  PMC Timer INT clock selecton: 0: lbus clock 1: 32.768K clock from SDM */
#define LSYS_CKSL_PTIM(x)                          ((uint32_t)(((x) & 0x00000001) << 7))
#define LSYS_GET_CKSL_PTIM(x)                      ((uint32_t)(((x >> 7) & 0x00000001)))
#define LSYS_BIT_CKSL_PSRAM                        ((uint32_t)0x00000001 << 6)          /*!<R/W 1  psram clock select 0: xtal 40m 1: pll divider out */
#define LSYS_CKSL_PSRAM(x)                         ((uint32_t)(((x) & 0x00000001) << 6))
#define LSYS_GET_CKSL_PSRAM(x)                     ((uint32_t)(((x >> 6) & 0x00000001)))
#define LSYS_BIT_CKSL_SPIC                         ((uint32_t)0x00000001 << 4)          /*!<R/W 0  spic clock select 0: xtal 40m 1: pll divider out */
#define LSYS_CKSL_SPIC(x)                          ((uint32_t)(((x) & 0x00000001) << 4))
#define LSYS_GET_CKSL_SPIC(x)                      ((uint32_t)(((x >> 4) & 0x00000001)))
#define LSYS_BIT_CKSL_HPERI                        ((uint32_t)0x00000001 << 3)          /*!<R/W 0  hperi clock seletion 0: xtal 40m 1: pll divider out */
#define LSYS_CKSL_HPERI(x)                         ((uint32_t)(((x) & 0x00000001) << 3))
#define LSYS_GET_CKSL_HPERI(x)                     ((uint32_t)(((x >> 3) & 0x00000001)))
#define LSYS_BIT_CKSL_HP                           ((uint32_t)0x00000001 << 2)          /*!<R/W 0h  HP clock select,used for KM4 and sram 0: xtal 40m 1: pll divider out */
#define LSYS_CKSL_HP(x)                            ((uint32_t)(((x) & 0x00000001) << 2))
#define LSYS_GET_CKSL_HP(x)                        ((uint32_t)(((x >> 2) & 0x00000001)))
#define LSYS_MASK_CKSL_LP                          ((uint32_t)0x00000003 << 0)          /*!<R/W 0h  LP clock select, used for KM0 0: osc4m 1: xtal_40m 2: NotValid 3: pll divider out */
#define LSYS_CKSL_LP(x)                            (((uint32_t)((x) & 0x00000003) << 0))
#define LSYS_GET_CKSL_LP(x)                        ((uint32_t)(((x >> 0) & 0x00000003)))
#define LSYS_MASK_CKD_QSPI                         ((uint32_t)0x00000007 << 28)          /*!<R/WPD 1h  qspi clock divider, it is based on pll 0: div1 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_QSPI(x)                           (((uint32_t)((x) & 0x00000007) << 28))
#define LSYS_GET_CKD_QSPI(x)                       ((uint32_t)(((x >> 28) & 0x00000007)))
#define LSYS_MASK_CKD_UTMIFS                       ((uint32_t)0x0000000F << 20)          /*!<R/WPD 0x7  usb full-speed 48M clock divider, it is based on cpu pll output 0: not valid 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_UTMIFS(x)                         (((uint32_t)((x) & 0x0000000F) << 20))
#define LSYS_GET_CKD_UTMIFS(x)                     ((uint32_t)(((x >> 20) & 0x0000000F)))
#define LSYS_MASK_CKD_HP_MBIST                     ((uint32_t)0x00000003 << 18)          /*!<R/WPD 0h  hp(km4) clock divider, it is based on pll 0: div1 1: div2 2: div3 3/other: divider by this value + 1 Note: this divider just for HW mbist test , SW don’t use it */
#define LSYS_CKD_HP_MBIST(x)                       (((uint32_t)((x) & 0x00000003) << 18))
#define LSYS_GET_CKD_HP_MBIST(x)                   ((uint32_t)(((x >> 18) & 0x00000003)))
#define LSYS_MASK_CKD_SRAM                         ((uint32_t)0x00000003 << 16)          /*!<R/WPD 0h  sram clock divider, it is based on hp_clk 0: div1 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_SRAM(x)                           (((uint32_t)((x) & 0x00000003) << 16))
#define LSYS_GET_CKD_SRAM(x)                       ((uint32_t)(((x >> 16) & 0x00000003)))
#define LSYS_MASK_CKD_IRDA                         ((uint32_t)0x00000007 << 12)          /*!<R/WPD 3h  irda clock divider, it is based on sys pll 0 : not valid 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_IRDA(x)                           (((uint32_t)((x) & 0x00000007) << 12))
#define LSYS_GET_CKD_IRDA(x)                       ((uint32_t)(((x >> 12) & 0x00000007)))
#define LSYS_MASK_CKD_HPERI                        ((uint32_t)0x00000007 << 8)          /*!<R/WPD 3h  peri_hclk clock divider, it is based on sys pll 0 : not valid 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_HPERI(x)                          (((uint32_t)((x) & 0x00000007) << 8))
#define LSYS_GET_CKD_HPERI(x)                      ((uint32_t)(((x >> 8) & 0x00000007)))
#define LSYS_MASK_CKD_LP                           ((uint32_t)0x0000000F << 4)          /*!<R/WPD 3h  KM0 pll clock divider 0: div1 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_LP(x)                             (((uint32_t)((x) & 0x0000000F) << 4))
#define LSYS_GET_CKD_LP(x)                         ((uint32_t)(((x >> 4) & 0x0000000F)))
#define LSYS_MASK_CKD_HP                           ((uint32_t)0x00000007 << 0)          /*!<R/WPD 1h  hp(km4) clock divider, it is based on pll 0: div1 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_HP(x)                             (((uint32_t)((x) & 0x00000007) << 0))
#define LSYS_GET_CKD_HP(x)                         ((uint32_t)(((x >> 0) & 0x00000007)))
#define REG_LSYS_CKD_GRP0                            0x0220
#define REG_LSYS_CKSL_GRP0                           0x0218
#define PLL_BIT_REG_CK_EN                 ((uint32_t)0x00000001 << 12)
#define PLL_BIT_POW_PLL                   ((uint32_t)0x00000001 << 29)
#define PLL_BIT_EN_CK400M_PS              ((uint32_t)0x00000001 << 4)
#define PLL_BIT_EN_CK400M                 ((uint32_t)0x00000001 << 3)
#define PLL_BIT_POW_CKGEN                 ((uint32_t)0x00000001 << 14)
#define PLL_BIT_POW_ERC                   ((uint32_t)0x00000001 << 31)
#define PLL_BIT_CKRDY_PLL                 ((uint32_t)0x00000001 << 0)
#define PLL_BIT_TRIG_FREQ                 ((uint32_t)0x00000001 << 15)
#define PLL_MASK_REG_RS_SET               ((uint32_t)0x00000007 << 16)
#define PLL_REG_RS_SET(x)                 (((uint32_t)((x) & 0x00000007) << 16))
#define PLL_MASK_DIVN_SDM                 ((uint32_t)0x0000003F << 5)
#define PLL_DIVN_SDM(x)                   (((uint32_t)((x) & 0x0000003F) << 5))
#define PLL_MASK_F0F_SDM                  ((uint32_t)0x00001FFF << 19)
#define PLL_F0F_SDM(x)                    (((uint32_t)((x) & 0x00001FFF) << 19))
#define PLL_MASK_F0N_SDM                  ((uint32_t)0x00000007 << 16)
#define PLL_F0N_SDM(x)                    (((uint32_t)((x) & 0x00000007) << 16))
#define PLL_RS_5K				0x5
#define PLL_RS_3K_XTAL_26M		0x6

#define ADDR_3_BYTE				0x3
#define ADDR_4_BYTE				0x4
extern uint8_t __image1_bss_start__[];
extern uint8_t __image1_bss_end__[];
extern FLASH_InitTypeDef flash_init_para;
extern uint32_t SPIC_CALIB_PATTERN[2];

_LONG_CALL_ uint32_t DiagPrintf(const char* fmt, ...);
_LONG_CALL_ void LOGUART_SetBaud(LOGUART_TypeDef* UARTLOG, uint32_t BaudRate);
_LONG_CALL_ void RCC_PeriphClockCmd(uint32_t APBPeriph, uint32_t APBPeriph_Clock, uint8_t NewState);
_LONG_CALL_ void RCC_PeriphClockSource_SPIC(uint32_t Source);
_LONG_CALL_ int CRYPTO_SHA_Init(void* pIE_I);
_LONG_CALL_ int rtl_crypto_sha2(const SHA2_TYPE sha2type, const uint8_t* message, const uint32_t msglen, uint8_t* pDigest);
_LONG_CALL_ int rtl_crypto_sha2_init(const SHA2_TYPE sha2type, hw_sha_context* ctx);
_LONG_CALL_ int rtl_crypto_sha2_process(const uint8_t* message, const uint32_t msglen, uint8_t* pDigest);
_LONG_CALL_ int rtl_crypto_sha2_update(uint8_t* message, uint32_t msglen, hw_sha_context* ctx);
_LONG_CALL_ int rtl_crypto_sha2_update_rom(const uint8_t* message, const uint32_t msglen, hw_sha_context* ctx);
_LONG_CALL_ int rtl_crypto_sha2_final(uint8_t* pDigest, hw_sha_context* ctx);
_LONG_CALL_ uint32_t CPU_ClkGet(void);
_LONG_CALL_ uint32_t XTAL_ClkGet(void);
_LONG_CALL_ uint32_t PLL_ClkGet(void);
_LONG_CALL_ void DelayUs(uint32_t us);
_LONG_CALL_ void DelayClkUpdate(uint32_t CpuClk);
_LONG_CALL_ int OTP_LogicalMap_Read(uint8_t* pbuf, uint32_t addr, uint32_t len);
_LONG_CALL_ void FLASH_Erase(uint32_t EraseType, uint32_t Address);
_LONG_CALL_ void FLASH_TxData(uint32_t StartAddr, uint32_t DataPhaseLen, uint8_t* pData);
_LONG_CALL_ void FLASH_RxCmd(uint8_t cmd, uint32_t read_len, uint8_t* read_data);
_LONG_CALL_ void FLASH_RxData(uint8_t cmd, uint32_t StartAddr, uint32_t read_len, uint8_t* read_data);
_LONG_CALL_ void FLASH_SetSpiMode(FLASH_InitTypeDef* FLASH_InitStruct, uint8_t SpicBitMode);
_LONG_CALL_ void FLASH_StructInit(FLASH_InitTypeDef* FLASH_InitStruct);
_LONG_CALL_ void FLASH_Addr4ByteEn(void);
_LONG_CALL_ void FLASH_DeepPowerDown(uint32_t NewState);
_LONG_CALL_ uint32_t SYSCFG_OTP_SPICAddr4ByteEn(void);

#define SCB_BASE						(0xE000ED00UL)

#define SCB_CCR							(*(volatile uint32_t *)(SCB_BASE + 0x14))
#define SCB_CSSELR						(*(volatile uint32_t *)(SCB_BASE + 0x84))
#define SCB_CCSIDR						(*(volatile uint32_t *)(SCB_BASE + 0x80))
#define SCB_DCIMVAC						(*(volatile uint32_t *)(SCB_BASE + 0x25C))
#define SCB_DCCISW						(*(volatile uint32_t *)(SCB_BASE + 0x274))

#define SCB_CCR_DC_Pos					16U
#define SCB_CCR_DC_Msk					(1UL << SCB_CCR_DC_Pos)

#define SCB_DCCISW_SET_Pos				5U
#define SCB_DCCISW_SET_Msk				(0x1FFUL << SCB_DCCISW_SET_Pos)

#define SCB_DCCISW_WAY_Pos				31U
#define SCB_DCCISW_WAY_Msk				(1UL << SCB_DCCISW_WAY_Pos)

#define SCB_CCSIDR_NUMSETS_Pos			13U
#define SCB_CCSIDR_NUMSETS_Msk			(0x7FFFUL << SCB_CCSIDR_NUMSETS_Pos)

#define SCB_CCSIDR_ASSOCIATIVITY_Pos	3U
#define SCB_CCSIDR_ASSOCIATIVITY_Msk	(0x3FFUL << SCB_CCSIDR_ASSOCIATIVITY_Pos)
#define CCSIDR_WAYS(x)					(((x) & SCB_CCSIDR_ASSOCIATIVITY_Msk) >> SCB_CCSIDR_ASSOCIATIVITY_Pos)
#define CCSIDR_SETS(x)					(((x) & SCB_CCSIDR_NUMSETS_Msk      ) >> SCB_CCSIDR_NUMSETS_Pos      )

__attribute__((always_inline)) static inline void DSB(void)
{
	__asm volatile ("dsb");
}

__attribute__((always_inline)) static inline void ISB(void)
{
	__asm volatile ("isb");
}

static inline void SCB_InvalidateDCache(void)
{
	uint32_t ccsidr;
	uint32_t sets;
	uint32_t ways;

	SCB_CSSELR = 0U;
	DSB();

	ccsidr = SCB_CCSIDR;

	sets = CCSIDR_SETS(ccsidr);
	do
	{
		ways = CCSIDR_WAYS(ccsidr);

		do
		{
			SCB_DCCISW = ((sets << SCB_DCCISW_SET_Pos) & SCB_DCCISW_SET_Msk) | ((ways << SCB_DCCISW_WAY_Pos) & SCB_DCCISW_WAY_Msk);

		} while(ways-- != 0U);

	} while(sets-- != 0U);

	DSB();
	ISB();
}

static inline void SCB_InvalidateDCache_by_Addr(uint32_t* addr, int32_t dsize)
{
	int32_t op_size = dsize;
	uint32_t op_addr = (uint32_t)addr;
	int32_t linesize = 32;

	DSB();

	while(op_size > 0)
	{
		SCB_DCIMVAC = op_addr;
		op_addr += (uint32_t)linesize;
		op_size -= linesize;
	}

	DSB();
	ISB();
}

static inline void DCache_Invalidate(uint32_t Address, uint32_t Bytes)
{
	uint32_t addr = Address, len = Bytes;

	if((Address == 0xFFFFFFFF) && (Bytes == 0xFFFFFFFF))
	{
		SCB_InvalidateDCache();
	}
	else
	{
		if((addr & 0x1F) != 0)
		{
			addr = (Address >> 5) << 5;   //32-byte aligned
			len = ((((Address + Bytes - 1) >> 5) + 1) << 5) - addr; //next 32-byte aligned
		}

		SCB_InvalidateDCache_by_Addr((uint32_t*)addr, len);
	}
}

#define WATCHDOG_REFRESH() WRITE_REG32(IWDG_REG_BASE, 0x5A5A)

#endif
