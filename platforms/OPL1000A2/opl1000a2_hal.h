#ifndef OPL1000A2_HAL_H
#define OPL1000A2_HAL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define PLATFORM_NO_XIP
#define FLASH_BASE 0xF000000 // fake
#undef BUF_SIZE
#define BUF_SIZE 0x8000

typedef enum
{
	SPI_IDX_0,
	SPI_IDX_1,
	SPI_IDX_2,
	SPI_IDX_MAX
} E_SpiIdx_t;

typedef enum
{
	APS_CLKTREE_SRC_RC_BB,
	APS_CLKTREE_SRC_XTAL,
	APS_CLKTREE_SRC_XTAL_X2,
	APS_CLKTREE_SRC_XTAL_X4,
	APS_CLKTREE_SRC_DECI,
	APS_CLKTREE_SRC_1P2G_DIV,
	APS_CLKTREE_SRC_EXTERNAL,
	APS_CLKTREE_SRC_1P2G_MIN,
	APS_CLKTREE_SRC_1P2G_078MHZ = APS_CLKTREE_SRC_1P2G_MIN,
	APS_CLKTREE_SRC_1P2G_081MHZ,
	APS_CLKTREE_SRC_1P2G_084MHZ,
	APS_CLKTREE_SRC_1P2G_087MHZ,
	APS_CLKTREE_SRC_1P2G_090MHZ,
	APS_CLKTREE_SRC_1P2G_093MHZ,
	APS_CLKTREE_SRC_1P2G_097MHZ,
	APS_CLKTREE_SRC_1P2G_101MHZ,
	APS_CLKTREE_SRC_1P2G_106MHZ,
	APS_CLKTREE_SRC_1P2G_110MHZ,
	APS_CLKTREE_SRC_1P2G_116MHZ,
	APS_CLKTREE_SRC_1P2G_122MHZ,
	APS_CLKTREE_SRC_1P2G_128MHZ,
	APS_CLKTREE_SRC_1P2G_135MHZ,
	APS_CLKTREE_SRC_1P2G_143MHZ,
	APS_CLKTREE_SRC_1P2G_152MHZ,
	APS_CLKTREE_SRC_1P2G_MAX = APS_CLKTREE_SRC_1P2G_152MHZ
} E_ApsClkTreeSrc_t;

typedef struct
{
	volatile uint32_t DATA;         //0x00
	volatile uint32_t STATE;        //0x04
	volatile uint32_t CTRL;         //0x08
	volatile uint32_t INT_STATUS;   //0x0C
	volatile uint32_t BAUD_DIV;     //0x10
} S_DbgUart_Reg_t;

#define APS_PERIPH_BASE           ((uint32_t)0x30000000)
#define UART_DBG_BASE             (APS_PERIPH_BASE + 0x1000)
#define DBG_UART                  ((S_DbgUart_Reg_t *) UART_DBG_BASE)
#define DBG_UART_STATE_TX_FULL    (1<<0)
#define DBG_UART_STATE_RX_FULL    (1<<1)
#define DBG_UART_INT_STATUS_TX    (1<<0)

typedef uint32_t(*T_Hal_DbgUart_BaudRateSet)(uint32_t u32Baud);
typedef uint32_t(*T__Hal_Flash_ManufDeviceId)(E_SpiIdx_t u32SpiIdx, uint32_t* pu32Manufacturer, uint32_t* pu32MemoryType, uint32_t* pu32MemoryDensity);
typedef uint32_t(*T_Hal_Flash_4KSectorAddrErase_Internal)(E_SpiIdx_t u32SpiIdx, uint32_t u32SecAddr);
typedef uint32_t(*T_Hal_Flash_AddrRead_Internal)(E_SpiIdx_t u32SpiIdx, uint32_t u32StartAddr, uint8_t u8UseQuadMode, uint32_t u32Size, uint8_t* pu8Data);
typedef uint32_t(*T_Hal_Flash_AddrProgram_Internal)(E_SpiIdx_t u32SpiIdx, uint32_t u32StartAddr, uint8_t u8UseQuadMode, uint32_t u32Size, uint8_t* pu8Data);
typedef uint8_t* (*T_Hal_Sys_OtpRead)(uint16_t u16Offset, uint8_t* u8aBuf, uint16_t u16BufSize);
typedef uint32_t(*T_Hal_Sys_ApsClkTreeSetup)(E_ApsClkTreeSrc_t eClkTreeSrc, uint8_t u8ClkDivEn, uint8_t u8PclkDivEn);
typedef uint32_t(*T_Hal_Sys_RetRamTurnOn)(uint32_t u32RetRamIdxs);

extern T_Hal_DbgUart_BaudRateSet              Hal_DbgUart_BaudRateSet;
extern T__Hal_Flash_ManufDeviceId             _Hal_Flash_ManufDeviceId;
extern T_Hal_Flash_AddrRead_Internal          Hal_Flash_AddrRead_Internal;
extern T_Hal_Flash_4KSectorAddrErase_Internal Hal_Flash_4KSectorAddrErase_Internal;
extern T_Hal_Flash_AddrProgram_Internal       Hal_Flash_AddrProgram_Internal;
extern T_Hal_Sys_OtpRead                      Hal_Sys_OtpRead;
extern T_Hal_Sys_ApsClkTreeSetup              Hal_Sys_ApsClkTreeSetup;
extern T_Hal_Sys_RetRamTurnOn                 Hal_Sys_RetRamTurnOn;
extern uint32_t Crc32Table[256];
extern uint8_t __bss_start__[];
extern uint8_t __bss_end__[];

#define TC_SHA256_BLOCK_SIZE (64)
#define TC_SHA256_DIGEST_SIZE (32)
#define TC_SHA256_STATE_BLOCKS (TC_SHA256_DIGEST_SIZE/4)

struct tc_sha256_state_struct
{
	unsigned int iv[TC_SHA256_STATE_BLOCKS];
	uint64_t bits_hashed;
	uint8_t leftover[TC_SHA256_BLOCK_SIZE];
	size_t leftover_offset;
};

typedef struct tc_sha256_state_struct* TCSha256State_t;

extern int tc_sha256_init(TCSha256State_t s);
extern int tc_sha256_update(TCSha256State_t s, const uint8_t* data, size_t datalen);
extern int tc_sha256_final(uint8_t* digest, TCSha256State_t s);

#endif
