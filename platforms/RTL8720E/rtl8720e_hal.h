#ifndef RTL8721DA_HAL_H
#define RTL8721DA_HAL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "tlsf/tlsf.h"
#include "littlefs/lfs.h"
#include "rtl_kv/kv.h"

#define PLATFORM_SUPPORTS_OTP

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

typedef enum _SHA2_TYPE_
{
	SHA2_NONE = 0,
	SHA2_224 = 224 / 8,
	SHA2_256 = 256 / 8,
	SHA2_384 = 384 / 8,
	SHA2_512 = 512 / 8
} SHA2_TYPE;

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
	uint32_t FLASH_wr_dual_i; 			/*!< Specifies dual data write cmd */
	uint32_t FLASH_wr_dual_ii;			/*!< Specifies dual data/addr write cmd */
	uint32_t FLASH_wr_quad_i; 			/*!< Specifies quad data write cmd */
	uint32_t FLASH_wr_quad_ii;			/*!< Specifies quad data/addr write cmd */
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
} LOGUART_TypeDef;

typedef struct
{
	__IO uint32_t PLL_CPUPLL_CTRL0;                       /*!<  Register,  Address offset: 0x000 */
	__IO uint32_t PLL_CPUPLL_CTRL1;                       /*!<  Register,  Address offset: 0x004 */
	__IO uint32_t PLL_CPUPLL_CTRL2;                       /*!<  Register,  Address offset: 0x008 */
	__IO uint32_t PLL_CPUPLL_CTRL3;                       /*!<  Register,  Address offset: 0x00C */
	__IO uint32_t PLL_PERIPLL_CTRL0;                      /*!<  Register,  Address offset: 0x010 */
	__IO uint32_t PLL_PERIPLL_CTRL1;                      /*!<  Register,  Address offset: 0x014 */
	__IO uint32_t PLL_PERIPLL_CTRL2;                      /*!<  Register,  Address offset: 0x018 */
	__IO uint32_t PLL_PERIPLL_CTRL3;                      /*!<  Register,  Address offset: 0x01C */
	__IO uint32_t PLL_TEST;                               /*!<  Register,  Address offset: 0x020 */
	__IO uint32_t PLL_CPUPLL_PS;                          /*!<  Register,  Address offset: 0x024 */
	__IO uint32_t PLL_PERIPLL_PS;                         /*!<  Register,  Address offset: 0x028 */
	__IO uint32_t PLL_AUX_BG;                             /*!<  Register,  Address offset: 0x02C */
	__I  uint32_t PLL_STATE;                              /*!<  Register,  Address offset: 0x030 */
	__IO uint32_t PLL_DUMMY;                              /*!<  Register,  Address offset: 0x034 */
} PLL_TypeDef;

typedef struct
{
	uint32_t	PLLM_CLK;
	uint32_t	PLLD_CLK;
	uint8_t		Vol_Type;
	uint8_t		CPU_CKD;/* KM4/KR4/SRAM is the same clock */
	uint8_t		PSRAMC_CKD;
} SocClk_Info_TypeDef;

typedef enum
{
	FLASH_CLK_DIV2 = 1,
	FLASH_CLK_DIV3,
	FLASH_CLK_DIV4,
	FLASH_CLK_DIV5,
	FLASH_CLK_DIV6,
	FLASH_CLK_DIV7,
	FLASH_CLK_DIV8,
	FLASH_CLK_DIV9,
	FLASH_CLK_DIV10,
} FlashDivInt_E;

typedef struct
{
	__IO uint32_t OTPC_OTP_ALD;                           /*!<  Register,  Address offset: 0x000 */
	__IO uint32_t OTPC_OTP_SCAN;                          /*!<  Register,  Address offset: 0x004 */
	__IO uint32_t OTPC_OTP_AS;                            /*!<  Register,  Address offset: 0x008 */
	__IO uint32_t OTPC_OTP_AS_SEC;                        /*!<  Register,  Address offset: 0x00C */
	__IO uint32_t OTPC_OTP_TEST_MODE;                     /*!<  Register,  Address offset: 0x0010 */
	__IO uint32_t OTPC_OTP_CTRL;                          /*!<  Register,  Address offset: 0x014 */
	__IO uint32_t OTPC_OTP_TEST;                          /*!<  Register,  Address offset: 0x018 */
	__IO uint32_t OTPC_OTP_TIME;                          /*!<  Register,  Address offset: 0x01C */
	__IO uint32_t OTPC_OTP_ALD_EN;                        /*!<  Register,  Address offset: 0x020 */
	__IO uint32_t OTPC_OTP_DEBUG;                         /*!<  Register,  Address offset: 0x024 */
	__IO uint32_t RSVD0[2];                               /*!<  Reserved,  Address offset:0x28-0x2C */
	__I  uint32_t OTPC_OTP_CRC_DATA;                      /*!<  Register,  Address offset: 0x030 */
	__IO uint32_t OTPC_OTP_CRC_RESULT;                    /*!<  Register,  Address offset: 0x034 */
	__IO uint32_t OTPC_OTP_REPAIR_TIME;                   /*!<  Register,  Address offset: 0x038 */
	__IO uint32_t RSVD1;                                  /*!<  Reserved,  Address offset:0x3C */
	__IO uint32_t OTPC_OTP_PARAM;                         /*!<  Register,  Address offset: 0x040 */
} OTPC_TypeDef;


#define ISPLLM					0
#define ISPLLD					0x0080

#define CLKDIV(x)				(x)
#define CLKDIV_ROUND_UP(X, Y)	( ( ( X ) + ( Y ) - 1 ) / ( Y ) )

#define CPU_0P9V_CLK_LMT_ACUT	(200 * MHZ_TICK_CNT)
#define CPU_1P0V_CLK_LMT_ACUT	(240 * MHZ_TICK_CNT)
#define CPU_0P9V_CLK_LMT_BCUT	(300 * MHZ_TICK_CNT)
#define CPU_1P0V_CLK_LMT_BCUT	(400 * MHZ_TICK_CNT)

#define DSP_0P9V_CLK_LIMIT		(400 * MHZ_TICK_CNT)
#define DSP_1P0V_CLK_LIMIT		(500 * MHZ_TICK_CNT)

#define HBUS_0P9V_CLK_LIMIT		(100 * MHZ_TICK_CNT)
#define HBUS_1P0V_CLK_LIMIT		(150 * MHZ_TICK_CNT)

#define PSRAMC_CLK_LIMIT		(500 * MHZ_TICK_CNT)
#define SPIC_CLK_LIMIT			(208 * MHZ_TICK_CNT)
#define ECDSA_CLK_LIMIT			(100 * MHZ_TICK_CNT)
#define SRAM_0P9V_CLK_LIMIT		(200 * MHZ_TICK_CNT)
#define SRAM_1P0V_CLK_LIMIT		(240 * MHZ_TICK_CNT)

#define CLK_CPU_DPLL		3
#define CLK_CPU_MPLL		2
//#define CLK_CPU_LBUS		1 /* Both 0 and 1 are The same in HW*/
#define CLK_CPU_LBUS		0
#define CLK_CPU_XTAL		0 /* Nearly all the Time, Lbus the same with XTAL*/
#define MHZ_TICK_CNT 		(1000000)
#define LSYS_GET_CKSL_PLFM(x)                      ((uint32_t)(((x >> 1) & 0x00000003)))
#define REG_LSYS_FEN_GRP0                            0x0204
#define REG_LSYS_FEN_GRP1                            0x0208
#define REG_LSYS_CKE_GRP0                            0x020C
#define REG_LSYS_CKE_GRP1                            0x0210
#define REG_LSYS_CKE_GRP2                            0x0214
#define REG_LSYS_CKSL_GRP0                           0x0218
#define REG_LSYS_CKSL_GRP1                           0x021C
#define REG_LSYS_CKD_GRP0                            0x0220
#define REG_LSYS_CKD_GRP1                            0x0224
#define LSYS_MASK_CKD_GDMA_AXI                     ((uint32_t)0x0000000F << 28)          /*!<R/WPD 5h  gdma_axi clock divider, it is based on cpu/dsp pll 0~1: not valid 2: div3 3: div4 4: div5 5: div6 other: divider by this value + 1 */
#define LSYS_CKD_GDMA_AXI(x)                       (((uint32_t)((x) & 0x0000000F) << 28))
#define LSYS_MASK_CKD_PSRAM                        ((uint32_t)0x0000000F << 24)          /*!<R/WPD 1h  psram clock divider, it is based on cpu/dsp pll 0: div1 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_PSRAM(x)                          (((uint32_t)((x) & 0x0000000F) << 24))
#define LSYS_MASK_CKD_HBUS                         ((uint32_t)0x0000000F << 20)          /*!<R/WPD 5h  hbus clock divider, it is based on cpu/dsp pll 0~1: not valid 2: div3 3: div4 4: div5 other: divider by this value + 1 */
#define LSYS_CKD_HBUS(x)                           (((uint32_t)((x) & 0x0000000F) << 20))
#define LSYS_MASK_CKD_SRAM                         ((uint32_t)0x0000000F << 16)          /*!<R/WPD 3h  sram clock divider, it is based on sys pll 0 : not valid 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_SRAM(x)                           (((uint32_t)((x) & 0x0000000F) << 16))
#define LSYS_MASK_CKD_KR4                          ((uint32_t)0x0000000F << 4)          /*!<R/WPD 3h  KR4 clock divider, it is based on cpu/dsp pll 0: div1 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_KR4(x)                            (((uint32_t)((x) & 0x0000000F) << 4))
#define LSYS_MASK_CKD_KM4                          ((uint32_t)0x0000000F << 0)          /*!<R/WPD 3h  KM4 clock divider, it is based on cpu/dsp pll 0: div1 1: div2 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_KM4(x)                            (((uint32_t)((x) & 0x0000000F) << 0))
#define LSYS_MASK_CKD_ECDSA                        ((uint32_t)0x0000000F << 4)          /*!<R/WPD 5h  ECDSA clock divider, it is based on cpu pll output 0~1: not valid 2: div3 3/other: divider by this value + 1 */
#define LSYS_CKD_ECDSA(x)                          (((uint32_t)((x) & 0x0000000F) << 4))
#define LSYS_MASK_CKSL_PLFM                        ((uint32_t)0x00000003 << 1)          /*!<R/W 0h  KM4/KR4/HBUS/SRAM clock seletion, they are sync except HBUS 00/01: from lbus clock 10: from cpu pll divider output 11: from dsp pll divider output */
#define LSYS_CKSL_PLFM(x)                          (((uint32_t)((x) & 0x00000003) << 1))

#define PLL_BIT_PERIPLL_CK_EN                 ((uint32_t)0x00000001 << 12)          /*!<R/W 1  output clk enable */
#define PLL_BIT_CPUPLL_CK_EN                  ((uint32_t)0x00000001 << 12)          /*!<R/W 1  output clk enable */

#define BIT_LSYS_CKSL_PSRAM_CPUPLL		0x00
#define BIT_LSYS_CKSL_PSRAM_DSPPLL		0x01
#define BIT_LSYS_CKSL_PSRAM_LBUS		0x03

#define PLL_MASK_CPUPLL_DIVN_SDM              ((uint32_t)0x0000003F << 5)          /*!<R/W 6'b1101  393.216M (98.304M / 24.576M) 40M: 000111 26M: 001101 */
#define PLL_CPUPLL_DIVN_SDM(x)                ((uint32_t)(((x) & 0x0000003F) << 5))
#define PLL_MASK_CPUPLL_F0F_SDM               ((uint32_t)0x00001FFF << 19)          /*!<R/W 0  393.216M (98.304M / 24.576M) 40M: 1 0100 1001 0101 26M: 1 1111 1010 1010 */
#define PLL_CPUPLL_F0F_SDM(x)                 ((uint32_t)(((x) & 0x00001FFF) << 19))
#define PLL_MASK_CPUPLL_F0N_SDM               ((uint32_t)0x00000007 << 16)          /*!<R/W 0  393.216M (98.304M / 24.576M) 40M: 110 26M: 000 */
#define PLL_CPUPLL_F0N_SDM(x)                 ((uint32_t)(((x) & 0x00000007) << 16))
#define PLL_BIT_CPUPLL_TRIG_FREQ              ((uint32_t)0x00000001 << 15)          /*!<R/W 0  freq. step up or step down enable */
#define PLL_MASK_PERIPLL_DIVN_SDM             ((uint32_t)0x0000003F << 5)          /*!<R/W 6'b1000  1000M */
#define PLL_PERIPLL_DIVN_SDM(x)               ((uint32_t)(((x) & 0x0000003F) << 5))
#define PLL_MASK_PERIPLL_F0F_SDM              ((uint32_t)0x00001FFF << 19)          /*!<R/W 0000000000000  600M 40M: 0 0000 0000 0000 26M: 1 0011 1011 0001 */
#define PLL_PERIPLL_F0F_SDM(x)                ((uint32_t)(((x) & 0x00001FFF) << 19))
#define PLL_MASK_PERIPLL_F0N_SDM              ((uint32_t)0x00000007 << 16)          /*!<R/W 000  600M 40M: 000 26M: 000 */
#define PLL_PERIPLL_F0N_SDM(x)                ((uint32_t)(((x) & 0x00000007) << 16))
#define PLL_BIT_PERIPLL_TRIG_FREQ             ((uint32_t)0x00000001 << 15)          /*!<R/W 0  freq. step up or step down enable */

#define _LONG_CALL_						__attribute__ ((long_call))
#define FLASH_BASE						0x08000000
#define OTPC_REG_BASE					0x41000000
#define SYSTEM_CTRL_BASE				0x4100C000
#define UARTLOG_REG_BASE				0x41014000
#define IWDG_REG_BASE					0x4100CC00
#define PLL_BASE			((PLL_TypeDef *) (0x4100C440))
#define LOGUART_DEV			((LOGUART_TypeDef		*) UARTLOG_REG_BASE)
#define OTPC_DEV			((OTPC_TypeDef			*) (OTPC_REG_BASE))
#define EraseChip				0
#define EraseBlock				1
#define EraseSector				2
#define APBPeriph_SHA                              ((uint32_t)(0  << 30) | (0x00000001 << 9))   /*!<R/W 0  1: enable IPSEC SHA function 0: disable */
#define APBPeriph_LX                               ((uint32_t)(0  << 30) | (0x00000001 << 7))   /*!<R/W 0  1: enable LX1 system function 0: disable */
#define APBPeriph_SHA_CLOCK                        ((uint32_t)(0  << 30) | (0x00000001 << 9))   /*!<R/W 0  1: Enable ipsec SHA clock 0: Disable */
#define APBPeriph_LX_CLOCK                         ((uint32_t)(0  << 30) | (0x00000001 << 7))   /*!<R/W 0  1: Enable lx1 bus clock 0: Disable */
#define APBPeriph_PLL_CLOCK                        ((uint32_t)(1  << 30) | (0x00000001 << 21))  /*!<R/W 1  1: Enable PLL dividor for KM4/KR4/SRAM 0: Disable when modify KM4/KR4/SRAM dividor param, need set this bit 0 and 1 before switch KM4/KR4/SRAM to PLL from XTAL/OSC4M */

#define OTP_REAL_CONTENT_LEN						0x800
#define AVAILABLE_EFUSE_ADDR(addr)					(addr < OTP_REAL_CONTENT_LEN)
#define OTP_ACCESS_PWD								0x69
#define OTP_POLL_TIMES								20000
#define OTPC_BIT_EF_RD_WR_NS      ((uint32_t)0x00000001 << 31)          /*!<R/WPD/ET 0  Write this bit will trig an indirect read or write. Write 1: trigger write write 0: trigger read After this operation done, this bit will toggle. */
#define OTPC_MASK_EF_ADDR_NS      ((uint32_t)0x000007FF << 8)          /*!<R/W 0  OTP indirect read or write address */
#define OTPC_EF_ADDR_NS(x)        ((uint32_t)(((x) & 0x000007FF) << 8))
#define OTPC_MASK_EF_DATA_NS      ((uint32_t)0x000000FF << 0)          /*!<R/W/ES 0  OTP indirect read or write data */
#define OTPC_MASK_EF_PG_PWD       ((uint32_t)0x000000FF << 24)          /*!<R/W 8'b0  OTP program password. OTP can be programed only when this value is 8'h69. */
#define OTPC_EF_PG_PWD(x)         ((uint32_t)(((x) & 0x000000FF) << 24))
#define AON_BIT_PWC_AON_OTP                ((uint32_t)0x00000001 << 1)          /*!<R/W 0  OTP power cut enable for core power, high active. 1:enable 0:disable */
#define REG_AON_PWC                                  0x0000
#define OTP_LMAP_LEN								0x400 /*!< logical map len in byte 0x400~0x7FF resvd*/
#define LOGICAL_MAP_SECTION_LEN					0x1FD /*!< logical mapping efuse len in physical address */
#define OTP_LTYP0									0x00UL
#define OTP_LTYP1									0x01UL
#define OTP_LTYP2									0x02UL
#define OTP_LTYP3									0x03UL
#define OTP_GET_LTYP(x)								((uint32_t)(((x >> 28) & 0x0000000F)))
#define OTP_LBASE_EFUSE							0x07UL
#define OTP_LBASE_PAD								0x06UL
#define OTP_LBASE_REG								0x0FUL
#define OTP_LBASE_ROM								0x0EUL

#define OTP_GET_LTYP1_DATA(x)						((uint32_t)(((x >> 16) & 0x000000FF)))
#define OTP_GET_LTYP1_BASE(x)						((uint32_t)(((x >> 12) & 0x0000000F)))
#define OTP_GET_LTYP1_OFFSET(x)					((uint32_t)(((x >> 0) & 0x00000FFF)))

#define OTP_GET_LTYP2_LEN(x)						((uint32_t)((((x >> 24) & 0x0000000F) + 1) << 2))
#define OTP_SET_LTYP2_LEN(x)						((uint32_t)( ((x & 0x03F) >> 2) - 1))
#define OTP_GET_LTYP2_BASE(x)						((uint32_t)(((x >> 12) & 0x0000000F)))
#define OTP_GET_LTYP2_OFFSET(x)					((uint32_t)(((x >> 0) & 0x00000FFF)))

#define OTP_GET_LTYP3_OFFSET(x)					((uint32_t)(((x >> 0) & 0x0FFFFFFF)))

#define ADDR_3_BYTE				0x3
#define ADDR_4_BYTE				0x4
extern uint8_t __image1_bss_start__[];
extern uint8_t __image1_bss_end__[];
extern FLASH_InitTypeDef flash_init_para;
extern uint32_t SPIC_CALIB_PATTERN[2];
extern uint32_t RBSS_UDELAY_CLK;

_LONG_CALL_ uint32_t DiagPrintf(const char* fmt, ...);
_LONG_CALL_ void LOGUART_SetBaud(LOGUART_TypeDef* UARTLOG, uint32_t BaudRate);
_LONG_CALL_ void RCC_PeriphClockCmd(uint32_t APBPeriph, uint32_t APBPeriph_Clock, uint8_t NewState);
_LONG_CALL_ void RCC_PeriphClockSource_SPIC(uint32_t Source);
_LONG_CALL_ void RCC_PeriphClockSource_PSRAM(uint32_t Source);
_LONG_CALL_ int CRYPTO_SHA_Init(void* pIE_I);
_LONG_CALL_ int rtl_crypto_sha2(const SHA2_TYPE sha2type, const uint8_t* message, const uint32_t msglen, uint8_t* pDigest);
_LONG_CALL_ int rtl_crypto_sha2_init(const SHA2_TYPE sha2type, hw_sha_context* ctx);
_LONG_CALL_ int rtl_crypto_sha2_process(const uint8_t* message, const uint32_t msglen, uint8_t* pDigest);
_LONG_CALL_ int rtl_crypto_sha2_update(uint8_t* message, uint32_t msglen, hw_sha_context* ctx);
_LONG_CALL_ int rtl_crypto_sha2_update_rom(const uint8_t* message, const uint32_t msglen, hw_sha_context* ctx);
_LONG_CALL_ int rtl_crypto_sha2_final(uint8_t* pDigest, hw_sha_context* ctx);
_LONG_CALL_ uint32_t CPU_ClkGet(void);
_LONG_CALL_ uint32_t XTAL_ClkGet(void);
_LONG_CALL_ uint32_t PLL_ClkGet(uint8_t pll_type);
_LONG_CALL_ void DelayUs(uint32_t us);
_LONG_CALL_ void DelayClkUpdate(uint32_t CpuClk);
_LONG_CALL_ void FLASH_Erase(uint32_t EraseType, uint32_t Address);
_LONG_CALL_ void FLASH_TxData(uint32_t StartAddr, uint32_t DataPhaseLen, uint8_t* pData);
_LONG_CALL_ void FLASH_TxCmd(uint8_t cmd, uint8_t DataPhaseLen, uint8_t* pData);
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
