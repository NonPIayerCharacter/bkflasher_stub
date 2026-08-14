#ifndef RTL8710B_HAL_H
#define RTL8710B_HAL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//#define PLATFORM_SUPPORTS_OTP

#define __I     volatile const       /*!< Defines 'read only' permissions                 */
#define __O     volatile             /*!< Defines 'write only' permissions                */
#define __IO    volatile             /*!< Defines 'read / write' permissions              */

typedef struct _RAM_FUNCTION_START_TABLE_
{
	void(*RamStartFun) (void);
	void(*RamWakeupFun) (void);
	void(*RamPatchFun0) (void);
	void(*RamPatchFun1) (void);
	void(*RamPatchFun2) (void);
	void(*FlashStartFun) (void);
}RAM_FUNCTION_START_TABLE, * PRAM_FUNCTION_START_TABLE;

typedef struct
{
	uint32_t FLASH_Id;				/*!< Specifies the flash vendor ID.
									This parameter can be a value of @ref FLASH_VENDOR_ID_definitions */
	uint8_t FLASH_cur_bitmode;		/*!< Specifies the current bitmode of SPIC.
									This parameter can be a value of @ref FLASH_BIT_Mode_definitions */
	uint8_t FLASH_baud_rate;		/*!< Specifies the spi_sclk divider value. The frequency of spi_sclk is derived from:
									Frequency of spi_sclk = Frequency of oc_clk / (2 * FLASH_baud_rate) */
	uint8_t FLASH_baud_boot;		/*!< Specifies the spi_sclk divider value for rom boot. The frequency of spi_sclk is derived from:
									Frequency of spi_sclk = Frequency of oc_clk / (2 * FLASH_baud_rate) */
	uint32_t FLASH_cur_cmd; 		/*!< Specifies the current read cmd which is used to read data from flash
									in current bitmode. */

									/* status bits define */
	uint32_t FLASH_QuadEn_bit;		/*!< Specifies the QE bit in status register which is used to enable Quad I/O mode . */
	uint32_t FLASH_Busy_bit;		/*!< Specifies the WIP(Write in Progress) bit in status register which indicates whether
									the device is busy in program/erase/write status register progress. */
	uint32_t FLASH_WLE_bit; 		/*!< Specifies the WEL(Write Enable Latch) bit in status register which indicates whether
									the device will accepts program/erase/write status register instructions*/
	uint32_t FLASH_Status2_exist;	/*!< Specifies whether this flash chip has Status Register2 or not.
									This parameter can be 0/1. 0 means it doesn't have Status Register2, 1 means
									it has Status Register2.*/

									/* calibration data */
	uint8_t FLASH_rd_sample_phase;	/*!< Specifies the read sample phase obtained from calibration.*/
	uint8_t FLASH_rd_dummy_cyle[3];	/*!< Specifies the read dummy cycle of different bitmode according to
									flash datasheet*/

									/* valid R/W command set */
	uint32_t FLASH_rd_dual_o; 		/*!< Specifies dual data read cmd */
	uint32_t FLASH_rd_dual_io; 		/*!< Specifies dual data/addr read cmd */
	uint32_t FLASH_rd_quad_o; 		/*!< Specifies quad data read cmd */
	uint32_t FLASH_rd_quad_io; 		/*!< Specifies quad data/addr read cmd */
	uint32_t FLASH_wr_dual_i; 		/*!< Specifies dual data write cmd */
	uint32_t FLASH_wr_dual_ii;		/*!< Specifies dual data/addr write cmd */
	uint32_t FLASH_wr_quad_i; 		/*!< Specifies quad data write cmd */
	uint32_t FLASH_wr_quad_ii;		/*!< Specifies quad data/addr write cmd */
	uint32_t FALSH_dual_valid_cmd;	/*!< Specifies valid cmd of dual bitmode to program/read flash in auto mode */
	uint32_t FALSH_quad_valid_cmd;	/*!< Specifies valid cmd of quad bitmode to program/read flash in auto mode */

	/* other command set */
	uint8_t FLASH_cmd_wr_en;		/*!< Specifies the Write Enable(WREN) instruction which is for setting WEL bit*/
	uint8_t FLASH_cmd_rd_id;		/*!< Specifies the Read ID instruction which is for getting the identity of the flash divice.*/
	uint8_t FLASH_cmd_rd_status;	/*!< Specifies the Read Status Register instruction which is for getting the status of flash */
	uint8_t FLASH_cmd_rd_status2;	/*!< Specifies the Read Status Register2 instruction which is for getting the status2 of flash */
	uint8_t FLASH_cmd_wr_status;	/*!< Specifies the Write Status Register instruction which is for setting the status register of flash */
	uint8_t FLASH_cmd_wr_status2;	/*!< Specifies the Write Status Register2 instruction which is for setting the status register2 of flash.
									In some flash chips, status2 write cmd != status1 write cmd,
									like: GD25Q32C, GD25Q64C,GD25Q128C etc.*/
	uint8_t FLASH_cmd_chip_e;		/*!< Specifies the Erase Chip instruction which is for erasing the whole chip*/
	uint8_t FLASH_cmd_block_e;		/*!< Specifies the Erase Block instruction which is for erasing 64kB*/
	uint8_t FLASH_cmd_sector_e;		/*!< Specifies the Erase Sector instruction which is for erasing 4kB*/
	uint8_t FLASH_cmd_pwdn_release;	/*!< Specifies the Deep Power Down instruction which is for entering power down mode.*/
	uint8_t FLASH_cmd_pwdn;			/*!< Specifies the Release from Deep Power Down instruction
									which is for exiting power down mode.*/

									/* debug log */
	uint8_t debug;					/*!< Specifies whether or not to print debug log.*/

	/* new calibration */
	uint8_t phase_shift_idx;		/*!< Specifies the phase shift idx in new calibration.*/
} FLASH_InitTypeDef;

typedef struct
{
	__IO uint32_t DLL;				/*!< UART Divisor Latch register(not used in Amebaz),	Address offset: 0x00*/
	__IO uint32_t DLH_INTCR;		/*!< UART interrupt enable register,              			Address offset: 0x04*/
	__IO uint32_t INTID;			/*!< UART interrupt identification register,              	Address offset: 0x08*/
	__IO uint32_t LCR;				/*!< UART line control register,              				Address offset: 0x0C*/
	__IO uint32_t MCR;				/*!< UART modem control register,              			Address offset: 0x10*/
	__I uint32_t LSR;				/*!< UART line status register,              				Address offset: 0x14*/
	__I uint32_t MDSR;				/*!< UART modem status register,              			Address offset: 0x18*/
	__IO uint32_t SPR;				/*!< UART scratch pad register,              				Address offset: 0x1C*/
	__IO uint32_t STSR;				/*!< UART STS register,              					Address offset: 0x20*/
	__IO uint32_t RB_THR;			/*!< UART receive buffer/transmitter holding register,	Address offset: 0x24*/
	__IO uint32_t MISCR;			/*!< UART MISC control register,              			Address offset: 0x28*/
	__IO uint32_t TXPLSR;			/*!< UART IrDA SIR TX Pulse Width Control register,	Address offset: 0x2C*/

	__IO uint32_t RXPLSR;			/*!< UART IrDA SIR RX Pulse Width Control register,	Address offset: 0x30*/
	__IO uint32_t BAUDMONR;			/*!< UART baud monitor register,              			Address offset: 0x34*/
	__IO uint32_t RSVD2;			/*!< UART reserved field,              					Address offset: 0x38*/
	__IO uint32_t DBG_UART;			/*!< UART debug register,              					Address offset: 0x3C*/

	/* AmebaZ add for power save */
	__IO uint32_t RX_PATH;			/*!< UART rx path control register,              			Address offset: 0x40*/
	__IO uint32_t MON_BAUD_CTRL;	/*!< UART monitor baud rate control register,              	Address offset: 0x44*/
	__IO uint32_t MON_BAUD_STS;		/*!< UART monitor baud rate status register,              	Address offset: 0x48*/
	__IO uint32_t MON_CYC_NUM;		/*!< UART monitor cycle number register,              		Address offset: 0x4c*/
	__IO uint32_t RX_BYTE_CNT;		/*!< UART rx byte counter register,              			Address offset: 0x50*/

	/* AmebaZ change */
	__IO uint32_t FCR;				/*!< UART FIFO Control register,              			Address offset: 0x54*/
} UART_TypeDef;

typedef struct
{
	__IO uint32_t ctrlr0;				/*!< SPIC control register0,		Address offset: 0x000 */
	__IO uint32_t ctrlr1;				/*!< SPIC control register1,		Address offset: 0x004 */
	__IO uint32_t ssienr;				/*!< SPIC enable register,		Address offset: 0x008 */
	__IO uint32_t mwcr;					/*!< N/A,					Address offset: 0x00C */
	__IO uint32_t ser;					/*!< SPIC slave enable register,	Address offset: 0x010 */
	__IO uint32_t baudr;				/*!< SPIC baudrate select register,	Address offset: 0x014 */
	__IO uint32_t txftlr;				/*!< SPIC transmit FIFO threshold level,	Address offset: 0x018 */
	__IO uint32_t rxftlr;				/*!< SPIC receive FIFO threshold level,	Address offset: 0x01C */
	__IO uint32_t txflr;				/*!< SPIC transmit FIFO level register,	Address offset: 0x020 */
	__IO uint32_t rxflr;				/*!< SPIC receive FIFO level register,	Address offset: 0x024 */
	__IO uint32_t sr;					/*!< SPIC status register,				Address offset: 0x028 */
	__IO uint32_t imr;					/*!< SPIC interrupt mask register,		Address offset: 0x02C */
	__IO uint32_t isr;					/*!< SPIC interrupt status register,		Address offset: 0x030 */
	__IO uint32_t risr;					/*!< SPIC raw interrupt status register,	Address offset: 0x034 */
	__IO uint32_t txoicr;				/*!< SPIC transmit FIFO overflow interrupt clear register,	Address offset: 0x038 */
	__IO uint32_t rxoicr;				/*!< SPIC receive FIFO overflow interrupt clear register,	Address offset: 0x03C */
	__IO uint32_t rxuicr;				/*!< SPIC receive FIFO underflow interrupt clear register,	Address offset: 0x040 */
	__IO uint32_t msticr;				/*!< SPIC master error interrupt clear register,	Address offset: 0x044 */
	__IO uint32_t icr;					/*!< SPIC interrupt clear register,	Address offset: 0x048 */
	__IO uint32_t dmacr;				/*!< N/A,					Address offset: 0x04C */
	__IO uint32_t dmatdlr;				/*!< N/A,					Address offset: 0x050 */
	__IO uint32_t dmardlr;				/*!< N/A,					Address offset: 0x054 */
	__IO uint32_t idr;					/*!< SPIC Identiation register,		Address offset: 0x058 */
	__IO uint32_t spi_flash_version;	/*!< SPIC version ID register,		Address offset: 0x05C */
	union
	{
		__IO uint8_t  byte;
		__IO uint16_t half;
		__IO uint32_t word;
	} dr[32];							/*!< SPIC data register,					Address offset: 0x060~0x0DC */
	__IO uint32_t rd_fast_single;		/*!< Fast read data command of SPI Flash,	Address offset: 0x0E0 */
	__IO uint32_t rd_dual_o;			/*!< Dual output read command of SPI Flash,	Address offset: 0x0E4 */
	__IO uint32_t rd_dual_io;			/*!< Dual I/O read command of SPI Flash,	Address offset: 0x0E8 */
	__IO uint32_t rd_quad_o; 			/*!< Quad output read command of SPI Flash,	Address offset: 0x0EC */
	__IO uint32_t rd_quad_io;			/*!< Quad I/O read command of SPI Flash,	Address offset: 0x0F0 */
	__IO uint32_t wr_single;			/*!< Page program command of SPI Flash,	Address offset: 0x0F4 */
	__IO uint32_t wr_dual_i;			/*!< Dual data input program command of SPI Flash,			Address offset: 0x0F8 */
	__IO uint32_t wr_dual_ii;			/*!< Dual address and data input program command of SPI Flash,	Address offset: 0x0FC */
	__IO uint32_t wr_quad_i;			/*!< Quad data input program command of SPI Flash,			Address offset: 0x100 */
	__IO uint32_t wr_quad_ii;			/*!< Quad address and data input program command of SPI Flash,	Address offset: 0x104 */
	__IO uint32_t wr_enable;			/*!< Write enabe command of SPI Flash,	Address offset: 0x108 */
	__IO uint32_t rd_status;			/*!< Read status command of SPI Flash,	Address offset: 0x10C */
	__IO uint32_t ctrlr2;				/*!< SPIC control register2,			Address offset: 0x110 */
	__IO uint32_t fbaudr;				/*!< SPIC fast baudrate select,			Address offset: 0x114 */
	__IO uint32_t addr_length;			/*!< SPIC address length register,		Address offset: 0x118 */
	__IO uint32_t auto_length;			/*!< SPIC auto address length register,	Address offset: 0x11C */
	__IO uint32_t valid_cmd;			/*!< SPIC valid command register,		Address offset: 0x120 */
	__IO uint32_t flash_size;			/*!< SPIC flash size register,			Address offset: 0x124 */
	__IO uint32_t flush_fifo;			/*!< SPIC flush FIFO register,			Address offset: 0x128 */
} SPIC_TypeDef;

#define _LONG_CALL_						__attribute__ ((long_call))
#define FLASH_BASE						0x08000000
#define BIT_SOC_ACTCK_FLASH_EN			BIT(8)
#define BIT_SOC_FLASH_EN				BIT(4)
#define APBPeriph_FLASH					BIT_SOC_FLASH_EN
#define APBPeriph_FLASH_CLOCK			BIT_SOC_ACTCK_FLASH_EN
#define SPI_FLASH_CTRL_BASE				0x40020000
#define LOG_UART_REG_BASE				0x40003000
#define UART2_DEV						((UART_TypeDef*)LOG_UART_REG_BASE)
#define SPIC							((SPIC_TypeDef*)SPI_FLASH_CTRL_BASE)
#define EraseChip						0
#define EraseBlock						1
#define EraseSector						2
#define FLASH_ID_OTHERS					0
#define FLASH_ID_MXIC					1
#define FLASH_ID_WINBOND				2
#define FLASH_ID_MICRON					3
#define FLASH_ID_EON					4
#define FLASH_ID_GD						5
#define FLASH_ID_BOHONG					6
#define ReadQuadIOMode					0
#define ReadQuadOMode					1
#define ReadDualIOMode					2
#define ReadDualOMode 					3
#define ReadOneMode						4
#define SpicOneBitMode					0
#define SpicDualBitMode					1
#define SpicQuadBitMode					2
#define BIT_WR_BLOCKING					(0x00000001 << 9)
#define BIT_WR_QUAD_II					(0x00000001 << 8)
#define BIT_WR_QUAD_I					(0x00000001 << 7)
#define BIT_WR_DUAL_II					(0x00000001 << 6)
#define BIT_WR_DUAL_I					(0x00000001 << 5)
#define BIT_RD_QUAD_IO					(0x00000001 << 4)
#define BIT_RD_QUAD_O					(0x00000001 << 3)
#define BIT_RD_DUAL_IO					(0x00000001 << 2)
#define BIT_RD_DUAL_I					(0x00000001 << 1)
#define BIT_FRD_SINGEL					(0x00000001)
#define SPIC_VALID_CMD_MASK				(0x7fff)
#define DUAL_PRM_CYCLE_NUM				4
#define QUAD_PRM_CYCLE_NUM				2
#define FLASH_DM_CYCLE_2O				0x08
#define FLASH_DM_CYCLE_2IO				0x04
#define FLASH_DM_CYCLE_4O				0x08
#define FLASH_DM_CYCLE_4IO				0x06
#define FLASH_CMD_PP					0x02            //Page Program
#define FLASH_CMD_READ					0x03            //read data
#define FLASH_CMD_DREAD					0x3B            //Double Output Mode command
#define FLASH_CMD_FREAD					0x0B            //fast read data
#define FLASH_CMD_2READ					0xBB            // 2 x I/O read  command
#define FLASH_CMD_4READ					0xEB            // 4 x I/O read  command
#define FLASH_CMD_QREAD					0x6B            // 1I / 4O read command
#define SPIC_LOWSPEED_SAMPLE_PHASE		1

#define BIT_CMD_CH(x)				(((x) & 0x00000003) << 20)
#define BIT_DATA_CH(x)			(((x) & 0x00000003) << 18)
#define BIT_ADDR_CH(x)			(((x) & 0x00000003) << 16)
#define BIT_TMOD(x)				(((x) & 0x00000003) << 8)
#define BIT_SCPOL					(0x00000001 << 7)
#define BIT_SCPH					(0x00000001 << 6)

extern uint8_t __image1_bss_start__[];
extern uint8_t __image1_bss_end__[];
extern FLASH_InitTypeDef flash_init_para;
_LONG_CALL_ void RCC_PeriphClockCmd(uint32_t APBPeriph, uint32_t APBPeriph_Clock, uint8_t NewState);
_LONG_CALL_ void PINMUX_Ctrl(uint32_t  Function, uint32_t  PinLocation, bool Operation);
_LONG_CALL_ void CPU_ClkSet(uint8_t CPU_CLOCK_SEL_VALUE);
_LONG_CALL_ void DelayUs(uint32_t us);
_LONG_CALL_ uint8_t FLASH_Init(uint8_t SpicBitMode);
_LONG_CALL_ void FLASH_StructInit(FLASH_InitTypeDef* FLASH_InitStruct);
_LONG_CALL_ uint32_t FLASH_CalibrationNewCmd(uint32_t NewStatus);
_LONG_CALL_ void FLASH_RxCmd(uint8_t cmd, uint32_t read_len, uint8_t* read_data);
_LONG_CALL_ void FLASH_RxData(uint8_t cmd, uint32_t StartAddr, uint32_t read_len, uint8_t* read_data);
_LONG_CALL_ void FLASH_SetSpiMode(FLASH_InitTypeDef* FLASH_InitStruct, uint8_t SpicBitMode);
_LONG_CALL_ void FLASH_Erase(uint32_t EraseType, uint32_t Address);
_LONG_CALL_ uint32_t FLASH_ClockDiv(uint8_t Div);
_LONG_CALL_ void FLASH_Erase(uint32_t EraseType, uint32_t Address);
_LONG_CALL_ void FLASH_SetStatus(uint8_t Cmd, uint32_t Len, uint8_t* Status);
_LONG_CALL_ void FLASH_TxData12B(uint32_t StartAddr, uint8_t DataPhaseLen, uint8_t* pData);
_LONG_CALL_ void Cache_Enable(uint32_t Enable);
_LONG_CALL_ void Cache_Flush(void);
_LONG_CALL_ uint32_t UART_Writable(UART_TypeDef* UARTx);
_LONG_CALL_ uint32_t UART_Readable(UART_TypeDef* UARTx);
_LONG_CALL_ void UART_CharPut(UART_TypeDef* UARTx, uint8_t TxData);
_LONG_CALL_ void UART_CharGet(UART_TypeDef* UARTx, uint8_t* pRxByte);
_LONG_CALL_ void UART_SetBaud(UART_TypeDef* UARTx, uint32_t BaudRate);
_LONG_CALL_ uint32_t crc32_get(void* addr, uint32_t length);
_LONG_CALL_ extern uint8_t EFUSE_LogicalMap_Read(uint8_t* pbuf);
_LONG_CALL_ void sha256(const unsigned char* input, size_t ilen, unsigned char output[32], int is224);
_LONG_CALL_ extern uint32_t EFUSE_OneByteReadROM(uint32_t CtrlSetting, uint16_t Addr, uint8_t* Data, uint8_t L25OutVoltage);

#endif
