#ifndef MT7697_HAL_H
#define MT7697_HAL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//#define PLATFORM_NO_XIP
#define PLATFORM_SUPPORTS_OTP
#define FLASH_BASE 0x10000000 // cached
//#define FLASH_BASE 0x30000000

#define CACHE_CTRL_BASE							((volatile uint32_t *)0x01530000)
#define CACHE_OP_BASE							((volatile uint32_t *)0x01530004)
#define CACHE_CHANNEL_EN						((volatile uint32_t *)0x0153002C)
#define CACHE_ENTRY_START						((volatile uint32_t *)0x01540000)
#define CACHE_ENTRY_END							((volatile uint32_t *)0x01540040)

#define CACHE_DISABLE							(0x00000000)
#define CACHE_ENABLE							(0x0000030D)
#define CACHE_START_ADDR						(0x10000000)
#define CACHE_END_ADDR							(0x11000000)
#define CACHE_INVALIDATE_ALL					(0x03)

#define CM4_UART1_BASE							0x83030000
#define CM4_TOPCFGAON_BASE						0x83008000
#define TOP_AON_CM4_STRAP_STA					(CM4_TOPCFGAON_BASE + 0x1C0)
#define TOP_AON_CM4_PWRCTLCR					(CM4_TOPCFGAON_BASE + 0x1B8)
#define BITS(m,n)								(~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_MASK			BITS(0, 10)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_20M_OFFSET	(4)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_40M_OFFSET	(9)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_26M_OFFSET	(6)
#define CM4_PWRCTLCR_CM4_XTAL_FREQ_52M_OFFSET	(10)

#define UART_RBR								(0x0)
#define UART_THR								(0x0)
#define UART_IER								(0x4)
#define UART_IIR								(0x8)
#define UART_FCR								(0x8)
#define UART_LCR								(0xc)
#define UART_MCR								(0x10)
#define UART_LSR								(0x14)
#define UART_MSR								(0x18)
#define UART_SCR								(0x1c)
#define UART_DLL								(0x0)
#define UART_DLH								(0x4)
#define UART_RATE_STEP							(0x24)
#define UART_STEP_COUNT							(0x28)
#define UART_SAMPLE_COUNT						(0x2c)
#define UART_VFIFO_EN_REG						(0x4c)
#define UART_FRACDIV_L							(0x54)
#define UART_FRACDIV_M							(0x58)
#define UART_LCR_DLAB							0x0080
#define UART_DATA_MASK							0x0003
#define UART_PARITY_MASK						0x0038
#define UART_STOP_MASK							0x0004

#define CM4_SFC_BASE							0x83070000
#define MAX_SFC_GPRAM_SIZE						160
#define RW_SFC_MAC_CTL							(CM4_SFC_BASE + 0x0000)
#define RW_SFC_DIRECT_CTL						(CM4_SFC_BASE + 0x0004)
#define RW_SFC_MAC_OUTL							(CM4_SFC_BASE + 0x0010)
#define RW_SFC_MAC_INL							(CM4_SFC_BASE + 0x0014)
#define RW_SFC_GPRAM_DATA						(CM4_SFC_BASE + 0x0800)
#define SFC_WIP									BIT(0)
#define SFC_WIP_READY							BIT(1)
#define SFC_TRIG								BIT(2)
#define SFC_MAC_EN								BIT(3)
#define SFC_MAC_XIO_SEL							BIT(4)
#define SFC_MAC_SEL								BIT(28)
#define SFC_QPI_EN								BIT(0)

#define EFUSE_SIZE								0x200
#define EFUSE_BLOCK_SIZE						16
#define EFUSE_BLOCKS							(EFUSE_SIZE / EFUSE_BLOCK_SIZE)
#define SYS_EFUSE_BASE_ADDR						(0x81070000)
#define SYS_EE_CTRL								(0x00 + SYS_EFUSE_BASE_ADDR)
#define SYS_EE_CTRL_EXT_EE_SEL					BIT(30)
#define SYS_EFUSE_CTRL							(0x08 + SYS_EFUSE_BASE_ADDR)
#define SYS_EFUSE_RDATA0						(0x30 + SYS_EFUSE_BASE_ADDR)
#define SYS_EFUSE_RDATA1						(0x34 + SYS_EFUSE_BASE_ADDR)
#define SYS_EFUSE_RDATA2						(0x38 + SYS_EFUSE_BASE_ADDR)
#define SYS_EFUSE_RDATA3						(0x3C + SYS_EFUSE_BASE_ADDR)

#define TOP_CFG_AON_CM4							CM4_TOPCFGAON_BASE
#define TOP_CFG_CM4_PWR_CTL_CR					(TOP_CFG_AON_CM4+0x01B8)
#define CM4_MCU_960_EN_SHIFT					18
#define CM4_MCU_960_EN_MASK						0x1
#define CM4_MCU_960_EN_DISABLE					0
#define CM4_MCU_960_EN_ENABLE					1
#define CM4_MPLL_EN_SHIFT						28
#define CM4_MPLL_EN_MASK						0x3
#define CM4_MPLL_EN_PLL1_OFF_PLL2_OFF			0
#define CM4_MPLL_EN_PLL1_ON_PLL2_OFF			2
#define CM4_MPLL_EN_PLL1_ON_PLL2_ON				3
#define CM4_BT_PLL_RDY_SHIFT					27
#define CM4_BT_PLL_RDY_MASK						0x1
#define CM4_BT_PLL_RDY							0x1
#define CM4_WF_PLL_RDY_SHIFT					26
#define CM4_WF_PLL_RDY_MASK						0x1
#define CM4_WF_PLL_RDY							0x1
#define CM4_NEED_RESTORE_SHIFT					24
#define CM4_NEED_RESTORE_MASK					0x1
#define CM4_NEED_RESTORE						0x1

#define TOP_CFG_CM4_CKG_EN0						(TOP_CFG_AON_CM4+0x01B0)
#define CM4_WBTAC_MCU_CK_SEL_SHIFT				14
#define CM4_WBTAC_MCU_CK_SEL_MASK				0x3
#define CM4_WBTAC_MCU_CK_SEL_XTAL				0
#define CM4_WBTAC_MCU_CK_SEL_WIFI_PLL_960		1
#define CM4_WBTAC_MCU_CK_SEL_WIFI_PLL_320		2
#define CM4_WBTAC_MCU_CK_SEL_BBP_PLL			3
#define CM4_MCU_DIV_SEL_SHIFT					4
#define CM4_MCU_DIV_SEL_MASK					0x3F
#define CM4_HCLK_SEL_SHIFT						0
#define CM4_HCLK_SEL_MASK						0x7
#define CM4_HCLK_SEL_PLL						4

#define TOP_CFG_HCLK_2M_CKGEN					(TOP_CFG_AON_CM4+0x01B4)
#define SF_TOP_CLK_SEL_SHIFT					13
#define SF_TOP_CLK_SEL_MASK						0x3
#define SF_TOP_CLK_SEL_SYS_64M					1

#define CRYPTO_BASE								0x830F0000
#define CRYPTO_ENGINE_CTRL						(CRYPTO_BASE + 0x0004)
#define CRYPTO_TOTAL_LEN						(CRYPTO_BASE + 0x000C)
#define CRYPTO_SOURCE_ADDR						(CRYPTO_BASE + 0x0010)
#define CRYPTO_DEST_ADDR						(CRYPTO_BASE + 0x0020)
#define SHA256_IV_BASE							(CRYPTO_BASE + 0x3000)
#define SHA256_MODE								(CRYPTO_BASE + 0x3200)
#define CRYPTO_START_BIT						BIT(4)
#define SHA256_ENGINE_CFG						0x00000106

#define SFC_MISC1_DEL_LATCH_LATENCY_ADDR		(0x83070008)
#define SFC_MISC1_DEL_LATCH_LATENCY_MASK		BITS(0,1)
#define SFC_MISC1_DEL_LATCH_LATENCY_SHFT		0

#define SFC_MISC1_SMPCK_INV_ADDR				(0x83070008)
#define SFC_MISC1_SMPCK_INV_MASK				BIT(4)
#define SFC_MISC1_SMPCK_INV_SHFT				4

#define SFC_DELAY_CONTROL3_SFCK_SAM_DLY_ADDR	(0x83070028)
#define SFC_DELAY_CONTROL3_SFCK_SAM_DLY_MASK	BITS(0,5)
#define SFC_DELAY_CONTROL3_SFCK_SAM_DLY_SHFT	0

#define ADDR(Field) ((uint32_t volatile *) (Field##_ADDR  ))
#define MASK(Field) (Field##_MASK)
#define SHFT(Field) (Field##_SHFT)

#define mSetHWEntry(Field, Value) { uint32_t volatile *addr = ADDR(Field); *addr = ((((uint32_t)(Value) << SHFT(Field)) & MASK(Field)) | (*addr & ~MASK(Field))); }

#endif
