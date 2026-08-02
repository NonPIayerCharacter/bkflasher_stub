#ifndef RDA5981_HAL_H
#define RDA5981_HAL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "rda5981_bootrom_api.h"

#define FLASH_CTL_REG_BASE          (0x17FFF000U)
#define FLASH_CTL_TX_CMD_ADDR_REG   (FLASH_CTL_REG_BASE + 0x00)
#define FLASH_CTL_TX_BLOCK_SIZE_REG (FLASH_CTL_REG_BASE + 0x04)
#define FLASH_CTL_RX_FIFO_DATA_REG  (FLASH_CTL_REG_BASE + 0x10)
#define WRITE_REG32(REG, VAL)       ((*(volatile unsigned int*)(REG)) = (unsigned int)(VAL))
#define OR_REG32(REG, VAL)          ((*(volatile unsigned int*)(REG)) |= (unsigned int)(VAL))
#define WRITE_REG8(REG, VAL)        ((*(volatile unsigned char*)(REG)) = (unsigned char)(VAL))
#define ADDR2REG(addr)              (*((volatile unsigned int *)(addr)))
#define RF_SPI_REG                  ADDR2REG(0x4001301CUL)
#define BIT31                       (1ul << 31)

#define __I     volatile const       /*!< Defines 'read only' permissions                 */
#define __O     volatile             /*!< Defines 'write only' permissions                */
#define __IO    volatile             /*!< Defines 'read / write' permissions              */
typedef struct
{
	__IO uint32_t CLKGATE0;          /* 0x00 : Clock Gating 0              */
	__IO uint32_t PWRCTRL;           /* 0x04 : Power Control               */
	__IO uint32_t CLKGATE1;          /* 0x08 : Clock Gating 1              */
	__IO uint32_t CLKGATE2;          /* 0x0C : Clock Gating 2              */
	__IO uint32_t RESETCTRL;         /* 0x10 : Power Control               */
	__IO uint32_t CLKGATE3;          /* 0x14 : Clock Gating 3              */
	__IO uint32_t CORECFG;           /* 0x18 : Core Config                 */
	__IO uint32_t CPUCFG;            /* 0x1C : CPU Config                  */
	__IO uint32_t FTMRINITVAL;       /* 0x20 : Free Timer Initial Value    */
	__IO uint32_t FTMRTS;            /* 0x24 : Free Timer Timestamp        */
	__IO uint32_t CLKGATEBP;         /* 0x28 : Clock Gating Bypass         */
	uint32_t RESERVED0[2];
	__IO uint32_t PWMCFG;            /* 0x34 : PWM Config                  */
	__IO uint32_t FUN0WAKEVAL;       /* 0x38 : SDIO Func0 Wake Val         */
	__IO uint32_t FUN1WAKEVAL;       /* 0x3C : SDIO Func1 Wake Val         */
	__IO uint32_t BOOTJUMPADDR;      /* 0x40 : Boot Jump Addr              */
	__IO uint32_t SDIOINTVAL;        /* 0x44 : SDIO Int Val                */
	__IO uint32_t I2SCLKDIV;         /* 0x48 : I2S Clock Divider           */
	__IO uint32_t BOOTJUMPADDRCFG;   /* 0x4C : Boot Jump Addr Config       */
	__IO uint32_t FTMRPREVAL;        /* 0x50 : Free Timer Prescale Init Val*/
	__IO uint32_t PWROPENCFG;        /* 0x54 : Power Open Config           */
	__IO uint32_t PWRCLOSECFG;       /* 0x58 : Power Close Config          */
} RDA_SCU_TypeDef;

typedef struct
{
	__IO uint32_t CTRL;              /* 0x00 : GPIO Control                */
	uint32_t RESERVED0;
	__IO uint32_t DOUT;              /* 0x08 : GPIO Data Output            */
	__IO uint32_t DIN;               /* 0x0C : GPIO Data Input             */
	__IO uint32_t DIR;               /* 0x10 : GPIO Direction              */
	__IO uint32_t SLEW0;             /* 0x14 : GPIO Slew Config 0          */
	__IO uint32_t SLEWIOMUX;         /* 0x18 : GPIO IOMUX Slew Config      */
	__IO uint32_t INTCTRL;           /* 0x1C : GPIO Interrupt Control      */
	__IO uint32_t IFCTRL;            /* 0x20 : Interface Control           */
	__IO uint32_t SLEW1;             /* 0x24 : GPIO Slew Config 1          */
	__IO uint32_t REVID;             /* 0x28 : ASIC Reversion ID           */
	__IO uint32_t LPOSEL;            /* 0x2C : LPO Select                  */
	uint32_t RESERVED1;
	__IO uint32_t INTSEL;            /* 0x34 : GPIO Interrupt Select       */
	uint32_t RESERVED2;
	__IO uint32_t SDIOCFG;           /* 0x3C : SDIO Config                 */
	__IO uint32_t MEMCFG;            /* 0x40 : Memory Config               */
	__IO uint32_t IOMUXCTRL[8];      /* 0x44 - 0x60 : IOMUX Control        */
	__IO uint32_t PCCTRL;            /* 0x64 : Pulse Counter Control       */
} RDA_GPIO_TypeDef;

typedef struct
{
	union
	{
		__I  uint32_t RBR;           /* 0x00 : UART Receive buffer register      */
		__O  uint32_t THR;           /* 0x00 : UART Transmit holding register    */
		__IO uint32_t DLL;           /* 0x00 : UART Divisor latch(low)           */
	};
	union
	{
		__IO uint32_t DLH;           /* 0x04 : UART Divisor latch(high)          */
		__IO uint32_t IER;           /* 0x04 : UART Interrupt enable register    */
	};
	union
	{
		__I uint32_t IIR;            /* 0x08 : UART Interrupt id register        */
		__O uint32_t FCR;            /* 0x08 : UART Fifo control register        */
	};
	__IO uint32_t LCR;               /* 0x0C : UART Line control register        */
	__IO uint32_t MCR;               /* 0x10 : UART Moderm control register      */
	__I  uint32_t LSR;               /* 0x14 : UART Line status register         */
	__I  uint32_t MSR;               /* 0x18 : UART Moderm status register       */
	__IO uint32_t SCR;               /* 0x1C : UART Scratchpad register          */
	__I  uint32_t FSR;               /* 0x20 : UART FIFO status register         */
	__IO uint32_t FRR;               /* 0x24 : UART FIFO tx/rx trigger resiger   */
	__IO uint32_t DL2;               /* 0x28 : UART Baud rate adjust register    */
	__I  uint32_t RESERVED0[4];
	__I  uint32_t BAUD;              /* 0x3C : UART Auto baud counter            */
	__I  uint32_t DL_SLOW;           /* 0x40 : UART Divisor Adjust when slow clk */
	__I  uint32_t DL_FAST;           /* 0x44 : UART Divisor Adjust when fast clk */
} RDA_UART_TypeDef;

typedef struct
{
	__IO uint32_t dma_ctrl;          /* 0x00 : DMA ctrl             */
	__IO uint32_t dma_src;           /* 0x04 : DMA src              */
	__IO uint32_t dma_dst;           /* 0x08 : DMA dst              */
	__IO uint32_t dma_len;           /* 0x0c : DMA len              */
	__IO uint32_t crc_gen;           /* 0x10 : CRC gen              */
	__IO uint32_t dma_func_ctrl;     /* 0x14 : DMA func ctrl        */
	__IO uint32_t aes_key0;          /* 0x18 : AES key 0            */
	__IO uint32_t aes_key1;          /* 0x1c : AES key 1            */
	__IO uint32_t aes_key2;          /* 0x20 : AES key 2            */
	__IO uint32_t aes_key3;          /* 0x24 : AES key 2            */
	__IO uint32_t aes_iv0;           /* 0x28 : AES iv 0             */
	__IO uint32_t aes_iv1;           /* 0x2c : AES iv 1             */
	__IO uint32_t aes_iv2;           /* 0x30 : AES iv 2             */
	__IO uint32_t aes_iv3;           /* 0x34 : AES iv 2             */
	__IO uint32_t aes_mode;          /* 0x38 : AES mode             */
	__IO uint32_t cios_ctrl;         /* 0x3c : cios ctrl            */
	__IO uint32_t cios_reg0;         /* 0x40 : cios reg 0           */
	__IO uint32_t crc_init_val;      /* 0x44 : CRC init val         */
	__IO uint32_t crc_out_xorval;    /* 0x48 : CRC out xorval       */
	__I  uint32_t crc_out_val;       /* 0x4c : CRC out val          */
	uint32_t RESERVED0[12];
	__IO uint32_t dma_int_out;       /* 0x80 : DMA int out          */
	__IO uint32_t dma_int_mask;      /* 0x84 : DMA int mask         */
	uint32_t RESERVED1[30];
	__IO uint32_t trng_ctrl;         /* 0x100 : TRNG ctrl           */
	__IO uint32_t prng_ctrl;         /* 0x104 : PRNG ctrl           */
	__IO uint32_t prng_seed;         /* 0x108 : PRNG seed           */
	__IO uint32_t prng_timer_int;    /* 0x10c : PRNG timer init     */
	__IO uint32_t prng_timer;        /* 0x110 : PRNG timer          */
	__I  uint32_t trng_data0;        /* 0x114 : TRNG data 0         */
	__I  uint32_t trng_data0_mask;   /* 0x118 : TRNG data 0 mask    */
	__I  uint32_t trng_data1;        /* 0x11c : TRNG data 1         */
	__I  uint32_t trng_data1_mask;   /* 0x120 : TRNG data 1 mask    */
	__I  uint32_t prng_data;         /* 0x124 : PRNG data           */
	__I  uint32_t trng_hc;           /* 0x128 : TRNG hc             */
	uint32_t RESERVED2[437];
	__IO uint32_t cios_data_base;    /* 0x800 : CIOS data base      */
} RDA_DMACFG_TypeDef;

typedef struct
{
	__IO uint32_t WDTCFG;
} RDA_WDT_TypeDef;

#define RDA_SYS_CLK_FREQUENCY_40M   ( 40000000UL)
#define RDA_SYS_CLK_FREQUENCY_80M   ( 80000000UL)
#define RDA_SYS_CLK_FREQUENCY_160M  (160000000UL)
#define RDA_BUS_CLK_FREQUENCY_40M   ( 40000000UL)
#define RDA_BUS_CLK_FREQUENCY_80M   ( 80000000UL)
#define CLK_FREQ_40M                (0x00U)
#define CLK_FREQ_80M                (0x01U)
#define CLK_FREQ_160M               (0x02U)
#define SYS_CPU_CLK                 CLK_FREQ_160M
#define AHB_BUS_CLK                 CLK_FREQ_80M

#define RDA_AHB0_BASE               (0x40000000UL)
#define RDA_AHB1_BASE               (0x40100000UL)
#define RDA_APB_BASE                (RDA_AHB0_BASE)
#define RDA_SCU_BASE                (RDA_APB_BASE  + 0x00000)
#define RDA_GPIO_BASE               (RDA_APB_BASE  + 0x01000)
#define RDA_WDT_BASE                (RDA_SCU_BASE  + 0x0000C)
#define RDA_DMACFG_BASE             (RDA_AHB1_BASE + 0x81000)
#define RDA_SCU                     ((RDA_SCU_TypeDef       *) RDA_SCU_BASE      )
#define RDA_GPIO                    ((RDA_GPIO_TypeDef      *) RDA_GPIO_BASE     )
#define RDA_WDT                     ((RDA_WDT_TypeDef       *) RDA_WDT_BASE      )
#define RDA_DMACFG                  ((RDA_DMACFG_TypeDef    *) RDA_DMACFG_BASE   )

#define UART_BASE                   ((volatile uint32_t *)0x40012000U)
#define RXFIFO_EMPTY_MASK           (0x01UL << 0)

#define WDT_EN_BIT                  (9)
#define WDT_CLR_BIT                 (10)
#define WDT_TMRCNT_OFST             (11)
#define WDT_TMRCNT_WIDTH            (4)

#define FLASH_BASE                  (0x18000000U)
#define CMD_64KB_BLOCK_ERASE        (0x000000d8UL)
#define CMD_CHIP_ERASE              (0x00000060UL)

static inline void uart_putc(uint8_t c)
{
	UART_SEND_BYTE(c);
}

#endif
