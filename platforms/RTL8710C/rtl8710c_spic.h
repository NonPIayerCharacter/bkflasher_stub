/**************************************************************************//**
 * @file     rtl8710c_spic.h
 * @brief    The header file of rtl8710c_spic.c.
 * @version  1.00
 * @date     2017-08-22
 *
 * @note
 *
 ******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#ifndef RTL8710C_SPIC_H
#define RTL8710C_SPIC_H

#include "rtl8710c_flash.h"
 /**
   * @brief SPI Controller to control 4IO & 8IO DTR Flash (SPIC)
   */

typedef struct
{                                /*!< (@ 0x40020000) SPIC Structure                                             */

    union
    {
        __IOM uint32_t ctrlr0;                      /*!< (@ 0x00000000) SPIC Control Register 0                                    */

        struct
        {
            __IM  uint32_t : 6;
            __IOM uint32_t scph : 1;            /*!< [6..6] Serial Clock Phase.                                                */
            __IOM uint32_t scpol : 1;            /*!< [7..7] Serial Clock Polarity.                                             */
            __IOM uint32_t tmod : 2;            /*!< [9..8] Transfer mode.                                                     */
            __IM  uint32_t : 6;
            __IOM uint32_t addr_ch : 2;            /*!< [17..16] Indicate channel number of address phase in transmitting
                                                           or receiving data. Data phase is used to send data after
                                                           address phase. 0 : single channel, 1 : dual channels, 2
                                                           : quad channels, 3 : octal channel.                                       */
            __IOM uint32_t data_ch : 2;            /*!< [19..18] Indicate channel number of data phase in transmitting
                                                           or receiving data. Data phase is used to send data after
                                                           address phase. 0 : single channel, 1 : dual channels, 2
                                                           : quad channels, 3 : octal channel.                                       */
            __IOM uint32_t cmd_ch : 2;            /*!< [21..20] Indicate channel number of command phase in transmitting
                                                           or receiving data. Data phase is used to send data after
                                                           address phase. 0 : single channel, 1 : dual channels, 2
                                                           : quad channels, 3 : octal channel.                                       */
            __IOM uint32_t fast_rd : 1;            /*!< [22..22] Indicate to use fast read command in user mode. If
                                                           setting to 1, SPIC would use FBAUDR to derive spi_sclk.                   */
            __IOM uint32_t ck_mtimes : 5;            /*!< [27..23] Indicate the check time.                                         */
            __IOM uint32_t addr_ddr_en : 1;           /*!< [28..28] Enable address phase ddr mode.                                   */
            __IOM uint32_t data_ddr_en : 1;           /*!< [29..29] Enable data phase ddr mode.                                      */
            __IOM uint32_t cmd_ddr_en : 1;            /*!< [30..30] Enable command phase ddr mode. Always 2-byte command
                                                           type.                                                                     */
        } ctrlr0_b;
    };

    union
    {
        __IOM uint32_t ctrlr1;                      /*!< (@ 0x00000004) SPIC Control Register 1                                    */

        struct
        {
            __IOM uint32_t ndf : 12;           /*!< [11..0] ndf                                                               */
        } ctrlr1_b;
    };

    union
    {
        __IOM uint32_t ssienr;                      /*!< (@ 0x00000008) SPIC Enable Register                                       */

        struct
        {
            __IOM uint32_t spic_en : 1;            /*!< [0..0] Enable or disable SPIC.                                            */
            __OM  uint32_t atck_cmd : 1;            /*!< [1..1] Set to enable ATCK_CMD implementation. After this bit
                                                           is set, SPIC would not accept any command until checking
                                                           flash is not busy or timeout.                                             */
        } ssienr_b;
    };
    __IM  uint32_t  RESERVED;

    union
    {
        __IOM uint32_t ser;                         /*!< (@ 0x00000010) Slave Enable Register                                      */

        struct
        {
            __IOM uint32_t ser : 1;            /*!< [0..0] SPIC only has one slave select line. This bit should
                                                           be always set.                                                            */
        } ser_b;
    };

    union
    {
        __IOM uint32_t baudr;                       /*!< (@ 0x00000014) Baud Rate Select Register                                  */

        struct
        {
            __IOM uint32_t sckdv : 12;           /*!< [11..0] This register controls the frequency of spi_sclk. spi_sclk
                                                           = frequency of bus_clk / (2*sckdv)                                        */
        } baudr_b;
    };

    union
    {
        __IOM uint32_t txftlr;                      /*!< (@ 0x00000018) Transmit FIFO Threshold Level                              */

        struct
        {
            __IOM uint32_t tft : 8;            /*!< [7..0] Transmit FIFO Threshold. Controls the level of entries
                                                           (or below) at which the transmit FIFO controller triggers
                                                           an interrupt.                                                             */
        } txftlr_b;
    };

    union
    {
        __IOM uint32_t rxftlr;                      /*!< (@ 0x0000001C) Receive FIFO Threshold Level                               */

        struct
        {
            __IOM uint32_t rft : 8;            /*!< [7..0] Receive FIFO Threshold. Controls the level of entries
                                                           (or above) at which the receive FIFO controller triggers
                                                           an interrupt.                                                             */
        } rxftlr_b;
    };

    union
    {
        __IM  uint32_t txflr;                       /*!< (@ 0x00000020) Transmit FIFO Level Register                               */

        struct
        {
            __IM  uint32_t txtfl : 8;            /*!< [7..0] Transmit FIFO Level. Contains the number of valid data
                                                           entries in the transmit FIFO.                                             */
        } txflr_b;
    };

    union
    {
        __IM  uint32_t rxflr;                       /*!< (@ 0x00000024) Receive FIFO Level Register                                */

        struct
        {
            __IM  uint32_t rxtfl : 8;            /*!< [7..0] Receive FIFO Level. Contains the number of valid data
                                                           entries in the receive FIFO.                                              */
        } rxflr_b;
    };

    union
    {
        __IM  uint32_t sr;                          /*!< (@ 0x00000028) Status Register                                            */

        struct
        {
            __IM  uint32_t busy : 1;            /*!< [0..0] SPIC Busy Flag. When set, indicates that a serial transfer
                                                           is in progress; when cleared indicates that the SPIC is
                                                           idle or disabled.                                                         */
            __IM  uint32_t tfnf : 1;            /*!< [1..1] Transmit FIFO Not Full. Set when the transmit FIFO contains
                                                           one or more empty locations, and is cleared when the FIFO
                                                           is full. 0 : Transmit FIFO is full 1 : Transmit FIFO is
                                                           not full                                                                  */
            __IM  uint32_t tfe : 1;            /*!< [2..2] Transmit FIFO Empty. When the transmit FIFO is completely
                                                           empty, this bit is set. When the transmit FIFO contains
                                                           one or more valid entries, this bit is cleared. This bit
                                                           field does not request an interrupt. 0 : Transmit FIFO
                                                           is not empty 1 : Transmit FIFO is empty                                   */
            __IM  uint32_t rfne : 1;            /*!< [3..3] Receive FIFO Not Empty. Set when the receive FIFO contains
                                                           one or more entries and is cleared when the receive FIFO
                                                           is empty. This bit can be polled by software to completely
                                                           empty the receive FIFO. 0 : Receive FIFO is empty 1 : Receive
                                                           FIFO is not empty                                                         */
            __IM  uint32_t rff : 1;            /*!< [4..4] Receive FIFO Full. When the receive FIFO is completely
                                                           full, this bit is set. When the receive FIFO contains one
                                                           or more empty location, this bit is cleared. 0 : Receive
                                                           FIFO is not full 1 : Receive FIFO is full                                 */
            __IM  uint32_t txe : 1;            /*!< [5..5] Transmission Error. Set if the transmit FIFO is empty
                                                           when a transfer is started. This bit can be set only when
                                                           the DW_apb_ssi is configured as a slave device. Data from
                                                           the previous transmission is resent on the txd line. This
                                                           bit is cleared when read. 0 : No error 1 : Transmission
                                                           error                                                                     */
            __IM  uint32_t dcol : 1;            /*!< [6..6] Transmitting Status. This bit is set when SPIC is transmitting
                                                           command, address or data to the data register. By observing
                                                           this bit, users can avoid reading wrong data at the wrong
                                                           time if data is not ready to be read.                                     */
        } sr_b;
    };

    union
    {
        __IOM uint32_t imr;                         /*!< (@ 0x0000002C) Interrupt Mask Register                                    */

        struct
        {
            __IOM uint32_t txeim : 1;            /*!< [0..0] Transmit FIFO Empty Interrupt Mask 0 : spi_txeir_r interrupt
                                                           is masked 1 : spi_txeir_r interrupt is not masked                         */
            __IOM uint32_t txoim : 1;            /*!< [1..1] Transmit FIFO Overflow Interrupt Mask 0 : spi_txoir_r
                                                           interrupt is masked 1 : spi_txoir_r interrupt is not masked               */
            __IOM uint32_t rxuim : 1;            /*!< [2..2] Receive FIFO Underflow Interrupt Mask 0 : spi_rxuir_r
                                                           interrupt is masked 1 : spi_rxuir_r interrupt is not masked               */
            __IOM uint32_t rxoim : 1;            /*!< [3..3] Receive FIFO Overflow Interrupt Mask 0 : spi_rxoir_r
                                                           interrupt is masked 1 : spi_rxoir_r interrupt is not masked               */
            __IOM uint32_t rxfim : 1;            /*!< [4..4] Receive FIFO Full Interrupt Mask 0 : spi_rxfir_r interrupt
                                                           is masked 1 : spi_rxfir_r interrupt is not masked                         */
            __IOM uint32_t fseim : 1;            /*!< [5..5] FIFO Size Error Interrupt Mask. 0 : spi_fseir_r interrupt
                                                           is masked 1 : spi_fseir_r interrupt is not masked                         */
            __IOM uint32_t wbeim : 1;            /*!< [6..6] Write Burst Error Interrupt Mask. 0 : spi_wbier_r interrupt
                                                           is masked 1 : spi_wbier_r interrupt is not masked                         */
            __IOM uint32_t byeim : 1;            /*!< [7..7] Byte Enable Error Interrupt Mask 0 : spi_byeir_r interrupt
                                                           is masked 1 : spi_byeir_r interrupt is not masked                         */
            __IOM uint32_t aceim : 1;            /*!< [8..8] Auto-check Timeout Error Interrupt Mask. 0 : spi_aceir_r
                                                           interrupt is masked 1 : spi_aceir_r interrupt is not masked               */
            __IOM uint32_t txsim : 1;            /*!< [9..9] Transmit Split Interrupt Mask. 0 : spi_tx_sir_r interrupt
                                                           is masked 1 : spi_tx_sir_r interrupt is not masked                        */
        } imr_b;
    };

    union
    {
        __IM  uint32_t isr;                         /*!< (@ 0x00000030) Interrupt Status Register                                  */

        struct
        {
            __IM  uint32_t txeis : 1;            /*!< [0..0] Transmit FIFO Empty Interrupt Status after Masking 0
                                                           : spi_txeir_r interrupt is not active after masking 1 :
                                                           spi_txeir_r interrupt is active after masking                             */
            __IM  uint32_t txois : 1;            /*!< [1..1] Transmit FIFO Overflow Interrupt Status after Masking
                                                           0 : spi_txoir_r interrupt is not active after masking 1
                                                           : spi_txoir_r interrupt is active after masking                           */
            __IM  uint32_t rxuis : 1;            /*!< [2..2] Receive FIFO Underflow Interrupt Status after Masking
                                                           0 : spi_rxuir_r interrupt is not active after masking 1
                                                           : spi_rxuir_r interrupt is active after masking                           */
            __IM  uint32_t rxois : 1;            /*!< [3..3] Receive FIFO Overflow Interrupt Status after Masking
                                                           0 : spi_rxoir_r interrupt is not active after masking 1
                                                           : spi_rxoir_r interrupt is active after masking                           */
            __IM  uint32_t rxfis : 1;            /*!< [4..4] Receive FIFO Full Interrupt Status after Masking 0 :
                                                           spi_rxfir_r interrupt is not active after masking 1 : spi_rxfir_r
                                                           interrupt is active after masking                                         */
            __IM  uint32_t fseis : 1;            /*!< [5..5] FIFO Size Error Interrupt Status after Masking. 0 : spi_fseir_r
                                                           interrupt is not active after masking 1 : spi_fseir_r interrupt
                                                           is active after masking                                                   */
            __IM  uint32_t wbeis : 1;            /*!< [6..6] Write Burst Error Interrupt Status after Masking. 0 :
                                                           spi_wbier_r interrupt is not active after masking 1 : spi_wbier_r
                                                           interrupt is active after masking                                         */
            __IM  uint32_t byeis : 1;            /*!< [7..7] Byte Enable Error Interrupt Status after Masking 0 :
                                                           spi_byeir_r interrupt is not active after masking 1 : spi_byeir_r
                                                           interrupt is active after masking                                         */
            __IM  uint32_t aceis : 1;            /*!< [8..8] Auto-check Timeout Error Interrupt Status after Masking.
                                                           0 : spi_aceir_r interrupt is not active after masking 1
                                                           : spi_aceir_r interrupt is active after masking                           */
            __IM  uint32_t txsis : 1;            /*!< [9..9] Transmit Split Interrupt Status after Masking. 0 : spi_tx_sir_r
                                                           interrupt is not active after masking 1 : spi_tx_sir_r
                                                           interrupt is active after masking                                         */
            __IM  uint32_t rxsis : 1;            /*!< [10..10] Receive Split Interrupt Status after Masking. 0 : spi_rx_sir_r
                                                           interrupt is not active after masking 1 : spi_rx_sir_r
                                                           interrupt is active after masking                                         */
        } isr_b;
    };

    union
    {
        __IM  uint32_t risr;                        /*!< (@ 0x00000034) Raw Interrupt Status Register                              */

        struct
        {
            __IM  uint32_t txeir : 1;            /*!< [0..0] Transmit FIFO Empty Interrupt Raw Status before Masking
                                                           0 : spi_txeir_r interrupt is not acitve before masking
                                                           1 : spi_txeir_r interrupt is acitve before masking                        */
            __IM  uint32_t txoir : 1;            /*!< [1..1] Transmit FIFO Overflow Interrupt Raw Status before Masking
                                                           0 : spi_txoir_r interrupt is not acitve before masking
                                                           1 : spi_txoir_r interrupt is acitve before masking                        */
            __IM  uint32_t rxuir : 1;            /*!< [2..2] Receive FIFO Underflow Interrupt Raw Status before Masking
                                                           0 : spi_rxuir_r interrupt is not acitve before masking
                                                           1 : spi_rxuir_r interrupt is acitve before masking                        */
            __IM  uint32_t rxoir : 1;            /*!< [3..3] Receive FIFO Overflow Interrupt Raw Status before Masking
                                                           0 : spi_rxoir_r interrupt is not acitve before masking
                                                           1 : spi_rxoir_r interrupt is acitve before masking                        */
            __IM  uint32_t rxfir : 1;            /*!< [4..4] Receive FIFO Full Interrupt Raw Status before Masking
                                                           0 : spi_rxfir_r interrupt is not acitve before masking
                                                           1 : spi_rxfir_r interrupt is acitve before masking                        */
            __IM  uint32_t fseir : 1;            /*!< [5..5] FIFO Size Error Interrupt Raw Status before Masking.
                                                           0 : spi_fseir_r interrupt is not acitve before masking
                                                           1 : spi_fseir_r interrupt is acitve before masking                        */
            __IM  uint32_t wbeir : 1;            /*!< [6..6] Write Burst Error Interrupt Raw Status before Masking.
                                                           0 : spi_wbier_r interrupt is not acitve before masking
                                                           1 : spi_wbier_r interrupt is acitve before masking                        */
            __IM  uint32_t byeir : 1;            /*!< [7..7] Byte Enable Error Interrupt Raw Status before Masking
                                                           0 : spi_byeir_r interrupt is not acitve before masking
                                                           1 : spi_byeir_r interrupt is acitve before masking                        */
            __IM  uint32_t aceir : 1;            /*!< [8..8] Auto-check Timeout Error Interrupt Raw Status before
                                                           Masking. 0 : spi_aceir_r interrupt is not acitve before
                                                           masking 1 : spi_aceir_r interrupt is acitve before masking                */
        } risr_b;
    };

    union
    {
        __IM  uint32_t txoicr;                      /*!< (@ 0x00000038) Transmit FIFO Overflow Interrupt Clear Register            */

        struct
        {
            __IM  uint32_t txoicr : 1;            /*!< [0..0] Clear Transmit FIFO Overflow Interrupt. This register
                                                           reflects the status of the interrupt. A read from this
                                                           register clears the ssi_txo_intr interrupt; writing has
                                                           no effect.                                                                */
        } txoicr_b;
    };

    union
    {
        __IM  uint32_t rxoicr;                      /*!< (@ 0x0000003C) Receive FIFO Overflow Interrupt Clear Register             */

        struct
        {
            __IM  uint32_t rxoicr : 1;            /*!< [0..0] Clear Receive FIFO Overflow Interrupt. This register
                                                           reflects the status of the interrupt. A read from this
                                                           register clears the ssi_rxo_intr interrupt; writing has
                                                           no effect.                                                                */
        } rxoicr_b;
    };

    union
    {
        __IM  uint32_t rxuicr;                      /*!< (@ 0x00000040) Receive FIFO Underflow Interrupt Clear Register            */

        struct
        {
            __IM  uint32_t rxuicr : 1;            /*!< [0..0] Clear Receive FIFO Underflow Interrupt. This register
                                                           reflects the status of the interrupt. A read from this
                                                           register clears the ssi_rxu_intr interrupt; writing has
                                                           no effect.                                                                */
        } rxuicr_b;
    };

    union
    {
        __IM  uint32_t faeicr;                      /*!< (@ 0x00000044) Frame Alignment Interrupt Clear Register                   */

        struct
        {
            __IM  uint32_t faeicr : 1;            /*!< [0..0] Clear Frame Alignment Interrupt. This register reflects
                                                           the status of the interrupt. A read from this register
                                                           clears the ssi_fae_intr interrupt; writing has no effect.                 */
        } faeicr_b;
    };

    union
    {
        __IM  uint32_t icr;                         /*!< (@ 0x00000048) Interrupt Clear Register                                   */

        struct
        {
            __IM  uint32_t icr : 1;            /*!< [0..0] Clear Interrupts. This register is set if any of the
                                                           interrupts below are active. A read clears the ssi_txu_intr,
                                                           ssi_txo_intr, ssi_rxu_intr, ssi_rxo_intr, and the ssi_fae_intr
                                                           interrupts. Writing to this register has no effect.                       */
        } icr_b;
    };

    union
    {
        __IOM uint32_t dmacr;                       /*!< (@ 0x0000004C) DMA Control Register                                       */

        struct
        {
            __IOM uint32_t rx_dmac_en : 1;            /*!< [0..0] Receive DMA Enable. This bit enables/disables the receive
                                                           FIFO DMA channel 0 : Receive DMA disabled 1 : Receive DMA
                                                           enabled                                                                   */
            __IOM uint32_t tx_dmac_en : 1;            /*!< [1..1] Transmit DMA Enable. This bit enables/disables the transmit
                                                           FIFO DMA channel. 0 : Transmit DMA disabled 1 : Transmit
                                                           DMA enabled                                                               */
        } dmacr_b;
    };

    union
    {
        __IOM uint32_t dmatdlr;                     /*!< (@ 0x00000050) DMA Transmit Data Level Register                           */

        struct
        {
            __IOM uint32_t dmatdl : 8;            /*!< [7..0] Transmit Data Level. This bit field controls the level
                                                           at which a DMA request is made by the transmit logic. It
                                                           is equal to the watermark level; that is, the dma_tx_req
                                                           signal is generated when the number of valid data entries
                                                           in the transmit FIFO is equal to or below this field value,
                                                           and tx_dmac_en = 1.                                                       */
        } dmatdlr_b;
    };

    union
    {
        __IOM uint32_t dmardlr;                     /*!< (@ 0x00000054) DMA Receive Data Level Register                            */

        struct
        {
            __IOM uint32_t dmardl : 8;            /*!< [7..0] Receive Data Level. This bit field controls the level
                                                           at which a DMA request is made by the receive logic. The
                                                           watermark level = DMARDL+1; that is, dma_rx_req is generated
                                                           when the number of valid data entries in the receive FIFO
                                                           is equal to or above this field value + 1, and rx_dmac_en
                                                           = 1.                                                                      */
        } dmardlr_b;
    };
    __IM  uint32_t  RESERVED1[2];

    union
    {
        union
        {
            __IOM uint32_t dr_word;                   /*!< (@ 0x00000060) Data Register                                              */

            struct
            {
                __IOM uint32_t word : 32;           /*!< [31..0] Access FIFO as if its width is 4 byte per data item               */
            } dr_word_b;
        };

        union
        {
            __IOM uint16_t dr_half_word;              /*!< (@ 0x00000060) Data Register                                              */

            struct
            {
                __IOM uint16_t half_word : 16;          /*!< [15..0] Access FIFO as if its width is 2 byte per data item               */
            } dr_half_word_b;
        };

        union
        {
            __IOM uint8_t dr_byte;                    /*!< (@ 0x00000060) Data Register                                              */

            struct
            {
                __IOM uint8_t byte : 8;            /*!< [7..0] Access FIFO as if its width is 1 byte per data item                */
            } dr_byte_b;
        };
    };
    __IM  uint32_t  RESERVED2[31];

    union
    {
        union
        {
            __IOM uint32_t read_fast_single;          /*!< (@ 0x000000E0) Fast Read Command Register                                 */

            struct
            {
                __IOM uint32_t frd_cmd : 8;            /*!< [7..0] Fast read command.                                                 */
            } read_fast_single_b;
        };

        union
        {
            __IOM uint32_t rd_octal_io;               /*!< (@ 0x000000E0) Fast Read Command Register                                 */

            struct
            {
                __IOM uint32_t frd_octal_cmd : 16;      /*!< [15..0] Fast read command for Octal IO mode.                              */
            } rd_octal_io_b;
        };
    };

    union
    {
        __IOM uint32_t read_dual_data;              /*!< (@ 0x000000E4) Dual Read Command Register                                 */

        struct
        {
            __IOM uint32_t rd_dual_o_cmd : 8;         /*!< [7..0] Dual data read command, 1-1-2 mode.                                */
        } read_dual_data_b;
    };

    union
    {
        __IOM uint32_t read_dual_addr_data;         /*!< (@ 0x000000E8) Dual IO Read Command Register                              */

        struct
        {
            __IOM uint32_t rd_dual_io_cmd : 8;        /*!< [7..0] Dual address and data read command, 1-2-2 mode.                    */
        } read_dual_addr_data_b;
    };

    union
    {
        __IOM uint32_t read_quad_data;              /*!< (@ 0x000000EC) Quad Read Command Register                                 */

        struct
        {
            __IOM uint32_t rd_quad_o_cmd : 8;         /*!< [7..0] Quad data read command, 1-1-4 mode.                                */
        } read_quad_data_b;
    };

    union
    {
        __IOM uint32_t read_quad_addr_data;         /*!< (@ 0x000000F0) Quad IO Read Command Register                              */

        struct
        {
            __IOM uint32_t rd_quad_io_cmd : 8;        /*!< [7..0] Quad address and data read command, 1-4-4 mode.                    */
            __IM  uint32_t : 8;
            __IOM uint32_t prm_value : 8;            /*!< [23..16] High Performance Read Mode Control Value.                        */
        } read_quad_addr_data_b;
    };

    union
    {
        union
        {
            __IOM uint32_t write_single;              /*!< (@ 0x000000F4) Single IO Page Program Command Register                    */

            struct
            {
                __IOM uint32_t wr_cmd : 8;            /*!< [7..0] One bit mode page program command.                                 */
            } write_single_b;
        };

        union
        {
            __IOM uint32_t write_octal_io;            /*!< (@ 0x000000F4) Octal IO Page Program Command Register                     */

            struct
            {
                __IOM uint32_t wr_octal_cmd : 16;       /*!< [15..0] Octal IO page program command.                                    */
            } write_octal_io_b;
        };
    };

    union
    {
        __IOM uint32_t write_dual_data;             /*!< (@ 0x000000F8) Dual Page Program Command Register                         */

        struct
        {
            __IOM uint32_t rd_dual_io_cmd : 8;        /*!< [7..0] Dual page program command, 1-1-2 mode.                             */
        } write_dual_data_b;
    };

    union
    {
        __IOM uint32_t write_dual_addr_data;        /*!< (@ 0x000000FC) Dual IO Page Program Command Register                      */

        struct
        {
            __IOM uint32_t wr_dual_ii_cmd : 8;        /*!< [7..0] Dual IO page program command, 1-2-2 mode.                          */
        } write_dual_addr_data_b;
    };

    union
    {
        __IOM uint32_t write_quad_data;             /*!< (@ 0x00000100) Quad Page Program Command Register                         */

        struct
        {
            __IOM uint32_t wr_quad_i_cmd : 8;         /*!< [7..0] Quad page program command, 1-1-4 mode.                             */
        } write_quad_data_b;
    };

    union
    {
        __IOM uint32_t write_quad_addr_data;        /*!< (@ 0x00000104) Quad IO Page Program Command Register                      */

        struct
        {
            __IOM uint32_t wr_quad_ii_cmd : 8;        /*!< [7..0] Quad IO page program command, 1-4-4 mode.                          */
        } write_quad_addr_data_b;
    };

    union
    {
        __IOM uint32_t write_enable;                /*!< (@ 0x00000108) Write Enable Command Register                              */

        struct
        {
            __IOM uint32_t wr_en_cmd : 16;           /*!< [15..0] Write Enable Command. The second command byte is for
                                                           DTR mode.                                                                 */
        } write_enable_b;
    };

    union
    {
        __IOM uint32_t read_status;                 /*!< (@ 0x0000010C) Read Status Command Register                               */

        struct
        {
            __IOM uint32_t rd_st_cmd : 16;           /*!< [15..0] Read flash status register command.                               */
        } read_status_b;
    };

    union
    {
        __IOM uint32_t ctrlr2;                      /*!< (@ 0x00000110) SPIC Control Register 2                                    */

        struct
        {
            __IOM uint32_t so_dnum : 1;            /*!< [0..0] Indicate SO input pin of SPI Flash is connected to spi_sout[0]
                                                           or spi_sout[1]. Set 1 to support multi-channels connection
                                                           in default.                                                               */
            __IM  uint32_t : 2;
            __IOM uint32_t seq_en : 1;            /*!< [3..3] Set to enable data-split program / read.                           */
            __IOM uint32_t tx_fifo_entry : 4;         /*!< [7..4] Indicate the valid entry of transmit FIFO.                         */
            __IOM uint32_t rx_fifo_entry : 4;         /*!< [11..8] Indicate the valid entry of receive FIFO.                         */
            __IOM uint32_t cs_active_hold : 2;        /*!< [13..12] For flash chip select active hold time after SCLK resing
                                                           edge.                                                                     */
        } ctrlr2_b;
    };

    union
    {
        __IOM uint32_t fbaudr;                      /*!< (@ 0x00000114) Fast Read Baud Rate Select Register                        */

        struct
        {
            __IOM uint32_t fsckdv : 12;           /*!< [11..0] This register controls the frequency of spi_sclk for
                                                           fast read command. spi_sclk = frequency of bus_clk / (2*fsckdv)           */
        } fbaudr_b;
    };

    union
    {
        __IOM uint32_t addr_length;                 /*!< (@ 0x00000118) Address Byte Length Register                               */

        struct
        {
            __IOM uint32_t addr_phase_length : 3;     /*!< [2..0] Indicate the number of bytes in address phase. addr_phase_length
                                                           [2] is an extended bit for 4-Byte Address with PRM mode.
                                                           Set to 5, SPIC can send 4 byte address and 1 byte PRM mode
                                                           value. 3-Byte Address with PRM mode need to set this bit
                                                           to 0, SPIC can send 3byte address and 1byte PRM mode value.
                                                           1 : One byte address, 2 : Two byte address, 3 : Three byte
                                                           address, 4: Four byte address with PRM value, 0 : Four
                                                           byte address / Three bytee addree with PRM value.                         */
        } addr_length_b;
    };

    union
    {
        union
        {
            __IOM uint32_t auto_length;               /*!< (@ 0x0000011C) Auto Mode Address Byte Length Register                     */

            struct
            {
                __IOM uint32_t rd_dummy_length : 12;    /*!< [11..0] Indicate dummy cycles for receiving data. It is referenced
                                                             by bus_clk.                                                               */
                __IM  uint32_t : 4;
                __IOM uint32_t auto_addr_length : 2;    /*!< [17..16] Number of address bytes in read/write command in auto
                                                             mode                                                                      */
                __IOM uint32_t auto_dum_len : 8;        /*!< [25..18] Dummy cycle is used to check flash status in auto_write
                                                             operation if delay time of read data 1 cycle.                             */
                __IOM uint32_t cs_h_rd_dum_len : 2;     /*!< [27..26] Dummy cycle between sending read command to SPI flash.
                                                             Using the dummy cycles can avoid the timing violation of
                                                             CS high time.                                                             */
                __IOM uint32_t cs_h_wr_dum_len : 4;     /*!< [31..28] Dummy cycle between sending write command to SPI flash.
                                                             Using the dummy cycles can avoid the timing violation of
                                                             CS high time.                                                             */
            } auto_length_b;
        };

        union
        {
            __IOM uint32_t auto_length_seq;           /*!< (@ 0x0000011C) Sequential Auto Mode Address Byte Length Register          */

            struct
            {
                __IOM uint32_t rd_dummy_length : 12;    /*!< [11..0] Indicate dummy cycles for receiving data. It is referenced
                                                             by bus_clk.                                                               */
                __IOM uint32_t in_physical_cyc : 4;     /*!< [15..12] Indicate how many SPIC clk cycles from pad to internal
                                                             SPIC.                                                                     */
                __IOM uint32_t auto_addr_length : 2;    /*!< [17..16] Number of address bytes in read/write command in auto
                                                             mode                                                                      */
                __IOM uint32_t spic_cyc_per_byte : 8;   /*!< [25..18] Indicate how many SPIC clk cycles for one byte. Formula
                                                             = BAUD*2*Byte / CH, the maximum value is 256                              */
                __IOM uint32_t cs_h_rd_dum_len : 2;     /*!< [27..26] Dummy cycle between sending read command to SPI flash.
                                                             Using the dummy cycles can avoid the timing violation of
                                                             CS high time.                                                             */
                __IOM uint32_t cs_h_wr_dum_len : 4;     /*!< [31..28] Dummy cycle between sending write command to SPI flash.
                                                             Using the dummy cycles can avoid the timing violation of
                                                             CS high time.                                                             */
            } auto_length_seq_b;
        };
    };

    union
    {
        __IOM uint32_t valid_cmd;                   /*!< (@ 0x00000120) Valid Command Register                                     */

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
        } valid_cmd_b;
    };

    union
    {
        __IOM uint32_t flash_size;                  /*!< (@ 0x00000124) Write Enable Command Register                              */

        struct
        {
            __IOM uint32_t flash_size : 4;            /*!< [3..0] The size of flash size to select the target SPI flash
                                                           in auto mode.                                                             */
        } flash_size_b;
    };

    union
    {
        __IOM uint32_t flush_fifo;                  /*!< (@ 0x00000128) Read Status Command Register                               */

        struct
        {
            __OM  uint32_t flush_fifio : 1;           /*!< [0..0] Write to flush SPIC FIFO.                                          */
        } flush_fifo_b;
    };
} SPIC_Type;                                    /*!< Size = 300 (0x12c) */
/**

        \addtogroup hs_hal_spic Flash Controller
        \ingroup 8710c_hal
        \brief The Flash Controller (SPIC) module of the AmebaZ2 platform.
        @{
*/


#define MAX_DELAY_LINE 99               //!< Define maximum delay line level, fine-tune with digital PHY
#define CPU_CLK_TYPE_NO 3               //!< Define the number of clock options for CPU
#define MIN_BAUD_RATE 0x02              //!< Define minimum baud rate of flash controller
#define MAX_BAUD_RATE 0x06              //!< Define maximum baud rate of flash controller
#define MAX_AUTO_LENGTH 0x14            //!< Define maximum dummy cycle levels for timing tuning by flash controller(not PHY)
#define SPI_FLASH_BASE 0x98000000       //!< Define flash memory address base

/**
  \brief  Enumeration to define flash IO mode
*/
enum spic_io_mode_e {
    SpicOneIOMode = 0,                  //!< Define One IO mode, 1-1-1
    SpicDualOutputMode = 1,             //!< Define Dual Output mode, 1-1-2
    SpicDualIOMode = 2,                 //!< Define Dual IO mode, 1-2-2
    SpicQuadOutputMode = 3,             //!< Define Quad Output mode, 1-1-4
    SpicQuadIOMode = 4,                 //!< Define Quad IO mode, 1-4-4
    SpicQpiMode = 5,                    //!< Define QPI mode, 4-4-4
};
typedef uint8_t spic_io_mode_t;

/**
  \brief  Enumeration to define transfer mode
*/
enum spic_tmod_mode_e {
    TxMode = 0,                         //!< Transmit mode
    RxMode = 3                          //!< Receive mode
};
typedef uint8_t spic_tmod_mode_t;

/**
  \brief  Enumeration to define the channle number used by the controller to access flash
*/
enum spic_channel_number_e {
    SingleChnl = 0,                     //!< Single channel
    DualChnl = 1,                       //!< Dual channel
    QuadChnl = 2,                       //!< Quad channel
};
typedef uint8_t spic_channel_number_t;

/**
  \brief  Enumeration to define the byte number of address phase
*/
enum spic_address_phase_length_e {
    FourBytesLength = 0,                //!< Four bytes length
    OneByteLength = 1,                  //!< One byte length
    TwoBytesLength = 2,                 //!< Two bytes length
    ThreeBytesLength = 3                //!< Three bytes length
};
typedef uint8_t spic_address_phase_length_t;

/**
  \brief  Enumeration to define pinmux selection
*/
enum spic_pin_sel_e {
    SpicPinS0 = 0,                  //!< Flash pin selection 0, on GPIOB port
    SpicPinS1 = 1,                  //!< Flash pin selection 1, on GPIOA port
    SpicPinS2 = 2                   //!< Flash pin selection 2, on GPIOA port
};
typedef uint8_t spic_pin_sel_t;

/**
  \brief  Enumeration to define SPIC source clock
*/
enum spic_clk_sel_e {
    Clk100MHz = 0,                      //!< 100 MHz Clock
    Clk50MHz = 1,                       //!< 50 MHz Clock
    Clk25MHz = 2                        //!< 25 MHz Clock
};
typedef uint8_t spic_clk_sel_t;

/**
  \brief  The data struct of calibration setting
*/
typedef struct _spic_init_para_s {
    u8  baud_rate;                      //!< Valid baud rate setting
    u8  rd_dummy_cycle;                 //!< Valid flash controller's dummy cycle setting
    u8  delay_line;                     //!< Valid DPHY delay setting
    u8  valid;                          //!< Indicate data stored in the struct is valid or not
}spic_init_para_t, *pspic_init_para_t;

/**
  \brief  The data struct of flash pins selected by users
*/
typedef struct _flash_pin_sel_s {
    u8 pin_cs;                          //!< flash chip select pin
    u8 pin_clk;                         //!< flash clock pin
    u8 pin_d0;                          //!< flash data pin 0
    u8 pin_d1;                          //!< flash data pin 1
    u8 pin_d2;                          //!< flash data pin 2
    u8 pin_d3;                          //!< flash data pin 3
} flash_pin_sel_t, *pflash_pin_sel_t;


typedef struct irq_config_s
{
    irq_handler_t   irq_fun;
    void* data;
    int16_t         irq_num;
    uint16_t        priority;
} irq_config_t, * pirq_config_t;
/**
  \brief  The data struct of SPIC adaptor
*/
typedef struct _hal_spic_adaptor_s {
    irq_config_t irq_handle;                                //!< Interrupt handler, reserved
    SPIC_Type *spic_dev;                                    //!< The register base of SPIC
    spic_init_para_t spic_init_data[6][CPU_CLK_TYPE_NO];    //!< Calibration settings for various IO mode with different CPU clock
    pflash_cmd_t cmd;                                       //!< The pointer pointing to flash commands
    pflash_dummy_cycle_t dummy_cycle;                       //!< The pointer pointing to various dummy cycles
    void (*rx_done_callback)(void *para);                   //!< Reserved
    void *rx_done_cb_para;                                  //!< Reserved
    void *rx_data;                                          //!< Reserved
    void (*tx_done_callback)(void *para);                   //!< Reserved
    void *tx_done_cb_para;                                  //!< Reserved
    void *tx_data;                                          //!< Reserved
    u32  interrupt_priority;                                //!< Reserved
    u32  rx_length;                                         //!< Reserved
    u32  rx_threshold_level;                                //!< Reserved
    u32  tx_length;                                         //!< Reserved
    u32  tx_threshold_level;                                //!< Reserved
    flash_pin_sel_t flash_pin_sel;                         //!< Pinmux selection
    u8   read_cmd;                                          //!< Flash read command for current IO mode
    u8   quad_pin_sel;                                      //!< Record if the quad IO pins are used
    u8   interrupt_mask;                                    //!< Reserved
    u8   flash_id[3];                                       //!< Flash ID
    u8   flash_type;                                        //!< Flash type
    u8   cmd_byte_num;                                      //!< The byte number of command phase
    u8   addr_byte_num;                                     //!< The byte number of address phase
    u8   spic_bit_mode;                                     //!< Current IO mode used by the adaptor
    u8   spic_send_cmd_mode;                                //!< IO mode to send flash commands
    u8   cmd_chnl;                                          //!< The channel number of command phase used by SPIC
    u8   addr_chnl;                                         //!< The channel number of address phase used by SPIC
    u8   data_chnl;                                         //!< The channel number of data phase used by SPIC
}hal_spic_adaptor_t, *phal_spic_adaptor_t;

/**
  \brief  The data struct of valid windows for flash calibration
*/
typedef struct _valid_windows_s {
    u16 baud_rate;                      //!< Temporarily baud rate setting for calibration
    u16 auto_length;                    //!< Temporarily dummy cycle setting for calibration
    u32 dly_line_sp;                    //!< Temporarily available window of DPHY delay line starting point
    u32 dly_line_ep;                    //!< Temporarily available window of DPHY delay line ending point
}valid_windows_t, *pvalid_windows_t;

/**
  \brief  The data struct of flash controller stub functions. ROM code functions are accessed in RAM code through stub functions.
*/
typedef struct hal_spic_func_stubs_s {
    void (*spic_load_default_setting) (pspic_init_para_t pspic_init_data);
    u8 (*spic_query_system_clk) (void);
    void (*spic_clock_ctrl) (u8 ctl);
    void (*spic_pin_ctrl) (u8 io_pin_sel, u8 ctl);
    hal_status_t (*spic_init_setting) (phal_spic_adaptor_t phal_spic_adaptor, u8 spic_bit_mode);
    void (*spic_config_auto_mode) (phal_spic_adaptor_t phal_spic_adaptor);
    void (*spic_config_user_mode) (phal_spic_adaptor_t phal_spic_adaptor);
    BOOL (*spic_verify_calibration_para) (void);
    void (*spic_set_chnl_num) (phal_spic_adaptor_t phal_spic_adaptor);
    void (*spic_set_delay_line) (u8 delay_line);
    void (*spic_rx_cmd) (phal_spic_adaptor_t phal_spic_adaptor, u8 cmd, u8 data_phase_len, u8 *pdata);
    void (*spic_tx_cmd_no_check) (phal_spic_adaptor_t phal_spic_adaptor, u8 cmd, u8 data_phase_len, u8 *pdata);
    void (*spic_tx_cmd) (phal_spic_adaptor_t phal_spic_adaptor, u8 cmd, u8 data_phase_len, u8 *pdata);
    void (*spic_wait_ready) (SPIC_Type *spic_dev);
    void (*spic_flush_fifo) (SPIC_Type *spic_dev);
    uint32_t reserved[10];  // reserved space for next ROM code version function table extending.
} hal_spic_func_stubs_t, *phal_spic_func_stubs_t;

/// @cond DOXYGEN_ROM_HAL_API

/**

        \addtogroup hs_hal_spic_rom_func Flash Controller HAL ROM APIs
        @{
*/

/** \brief Description of spic_enable_rtl8710c
 *
 *    spic_enable_rtl8710c is used to enable flash controller.
 *
 *   \param SPIC_Type *spic_dev:      The pointer of the flash controller register base.
 *
 *   \return void.
 */
static inline void spic_enable_rtl8710c(SPIC_Type *spic_dev)
{
    spic_dev->ssienr = ENABLE;
}

/** \brief Description of spic_disable_rtl8710c
 *
 *    spic_disable_rtl8710c is used to disable flash controller.
 *    Some registers should call this funciton first to disable the flash controller so that values can be correctly set to registers.
 *
 *   \param SPIC_Type *spic_dev:      The pointer of the flash controller register base.
 *
 *   \return void.
 */
static inline void spic_disable_rtl8710c(SPIC_Type *spic_dev)
{
    spic_dev->ssienr = DISABLE;
}


/** \brief Description of spic_set_ctrl1_rtl8710c
 *
 *    spic_set_ctrl1_rtl8710c is used to set number of data frames (data length)expect to receive in user mode.
 *    The chip select line does not de-assert until the number of data received reaches the data length.
 *    The flash controller should be disabled to set the register correctly.
 *
 *   \param SPIC_Type *spic_dev:      The pointer of the flash controller register base.
 *   \param u32 length:      data length, the unit is byte.
 *
 *   \return void.
 */
static inline void spic_set_ctrl1_rtl8710c(SPIC_Type *spic_dev, u32 length)
{
    spic_dev->ctrlr1_b.ndf = length;
}

/** \brief Description of spic_set_baudr_rtl8710c
 *
 *    spic_set_baudr_rtl8710c is used to flush FIFO of the flash controller.
 *    The flash controller should be disabled to set the register correctly.
 *
 *   \param SPIC_Type *spic_dev:      The pointer of the flash controller register base.
 *   \param u8 baudr:      The value of baud rate divider.
 *
 *   \return void.
 */
static inline void spic_set_baudr_rtl8710c(SPIC_Type *spic_dev, u8 baudr)
{
    spic_dev->baudr_b.sckdv = baudr;
}

/** \brief Description of spic_set_fbaudr_rtl8710c
 *
 *    spic_set_fbaudr_rtl8710c is used to set baud rate of the flash controller for fast read.
 *    The flash controller should be disabled to set the register correctly.
 *
 *   \param SPIC_Type *spic_dev:      The pointer of the flash controller register base.
 *   \param u8 fbaudr:      The value of baud rate divider.
 *
 *   \return void.
 */
static inline void spic_set_fbaudr_rtl8710c(SPIC_Type *spic_dev, u8 fbaudr)
{
    spic_dev->fbaudr_b.fsckdv = fbaudr;
}

/** \brief Description of spic_set_dummy_cycle_rtl8710c
 *
 *    spic_set_dummy_cycle_rtl8710c is used to tune timing to receive data correctly.
 *    This dummy cycle adjustment mechanism is implemented in the flash controller.
 *    The unit of dummy cycle is bus clock.
 *    The flash controller should be disabled to set the register correctly.
 *
 *   \param SPIC_Type *spic_dev:      The pointer of the flash controller register base.
 *   \param u8 dummy_cycle:      The level of dummy cycles, can be up to MAX_AUTO_LENGTH.
 *
 *   \return void.
 */
static inline void spic_set_dummy_cycle_rtl8710c(SPIC_Type *spic_dev, u8 dummy_cycle)
{
    spic_dev->auto_length_b.rd_dummy_length = (u16)dummy_cycle;
}

/** \brief Description of spic_get_baudr_rtl8710c
 *
 *    spic_get_baudr_rtl8710c is used to get baud rate setting of the flash controller.
 *
 *   \param SPIC_Type *spic_dev:      The pointer of the flash controller register base.
 *
 *   \return u8: value of baud rate divider.
 */
static inline u8 spic_get_baudr_rtl8710c(SPIC_Type *spic_dev)
{
    u8 baudr = 0;

    baudr = (u8)spic_dev->baudr;
    return baudr;
}

/** \brief Description of spic_get_fbaudr_rtl8710c
 *
 *    spic_get_fbaudr_rtl8710c is used to get baud rate setting of the flash controller for fast read.
 *
 *   \param SPIC_Type *spic_dev:      The pointer of the flash controller register base.
 *
 *   \return u8: value of baud rate divider.
 */
static inline u8 spic_get_fbaudr_rtl8710c(SPIC_Type *spic_dev)
{
    u8 fbaudr = 0;

    fbaudr = (u8)spic_dev->fbaudr;
    return fbaudr;
}

void spic_load_default_setting_rtl8710c(pspic_init_para_t pspic_init_data);
u8 spic_query_system_clk_rtl8710c(void);
void spic_clock_ctrl_rtl8710c(u8 ctl);
hal_status_t spic_init_setting_rtl8710c(phal_spic_adaptor_t phal_spic_adaptor, u8 spic_bit_mode);
void spic_config_auto_mode_rtl8710c(phal_spic_adaptor_t phal_spic_adaptor);
void spic_config_user_mode_rtl8710c(phal_spic_adaptor_t phal_spic_adaptor);
BOOL spic_verify_calibration_para_rtl8710c(void);
void spic_set_chnl_num_rtl8710c(phal_spic_adaptor_t phal_spic_adaptor);
void spic_set_delay_line_rtl8710c(u8 delay_line);
void spic_rx_cmd_rtl8710c(phal_spic_adaptor_t phal_spic_adaptor, u8 cmd, u8 data_phase_len, u8 *pdata);
void spic_tx_cmd_no_check_rtl8710c(phal_spic_adaptor_t phal_spic_adaptor, u8 cmd, u8 data_phase_len, u8 *pdata);
void spic_tx_cmd_rtl8710c(phal_spic_adaptor_t phal_spic_adaptor, u8 cmd, u8 data_phase_len, u8 *pdata);
void spic_wait_ready_rtl8710c(SPIC_Type *spic_dev);
void spic_flush_fifo_rtl8710c(SPIC_Type *spic_dev);

/** *@} */ /* End of group hs_hal_spic_rom_func */

/// @endcond /* End of condition DOXYGEN_ROM_HAL_API */

/** *@} */ /* End of group hs_hal_spic */

#endif /* RTL8710C_SPIC_H */
