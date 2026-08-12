#ifndef __HAL_FLASH_H
#define __HAL_FLASH_H

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "hal/hal_common.h"

#include "hal/hal_cache.h"
#include "hal/hal_qspi.h"


/**************  flash ID Define(MID / memType / Capacity)  *******************/
#define TH25Q16HB_ID  0xEB6015 //2MB //OTP area 1024*1 bytes, 1bit lock [erase unit: 1KB]
#define TH25Q16UC_ID  0xCD7115 //2MB //OTP area 1024*3 bytes, 3bit lock [erase unit: 1KB]
#define TH25Q32HA_ID  0xCD6016 //4MB //OTP area 1024*3 bytes, 3bit lock [erase unit: 1KB]
#define GT25Q16B_ID   0xC4601F //2MB //OTP area 1024*1 bytes, 1bit lock [erase unit: 1KB]
#define GT25Q40D_ID   0xC44013 //512 //OTP area 1024*1 bytes, 1bit lock [erase unit: 512Byte]
#define ZB25VQ16E_ID  0x5E4015 //2MB //OTP area 1024*3 bytes, 3bit lock [erase unit: 1KB]


/****************************  Data types and Macros  ************************/
#define FLASH_QUAD_READ_DUMMY_CLOCKS    8

#define FLASH_SECURITY_SIZE_MAX  (4*256)
#define FLASH_SECURITY_PAGE_SIZE (256)
        
#define FLASH_PAGE_SIZE         (256)
#define FALSH_SIZE_4K           (4 *1024)
#define FALSH_SIZE_BLOCK_32K    (32*1024)
#define FALSH_SIZE_BLOCK_64K    (64*1024)
#define FALSH_SIZE_MAX		    (4 *1024*1024)


typedef enum
{
    FLASH_WRITE_NON_VOLATILE_SR = 0x01,
    FLASH_READ_STATUS_REG_1   = 0x05,
    FLASH_READ_STATUS_REG_2   = 0x35,
                              
    FLASH_WRITE_DISABLE       = 0x04,
    FLASH_WRITE_ENABLE        = 0x06,
    FLASH_WRITE_ENALBE_FOR_VOLATILE_SR = 0x50,
                              
    FLASH_STANDARD_READ       = 0x03,
    FLASH_DUAL_FAST_READ      = 0x3B,
    FLASH_QUAD_FAST_READ      = 0x6B,
                              
    FLASH_PAGE_PROGRAM        = 0x02,
    FLASH_QUAD_PAGE_PROGRAMM  = 0x32,
                              
    FLASH_SECTOR_ERASE        = 0x20,
    FLASH_32KB_BLOCK_ERASE    = 0x52,
    FLASH_64KB_BLOCK_ERASE    = 0xd8,
    FLASH_CHIP_ERASE          = 0x60,//0x60 or 0xc7
                              
    FLASH_SUSPEND             = 0x75,//Program/Erase Suspend
    FLASH_RESUME              = 0x7A,//Program/Erase Resume
    FLASH_READ_ID             = 0x9F,//Read Identification
    FLASH_READ_DEVICE_ID      = 0x90,//Read Manufacture ID/Device ID
    FLASH_READ_UNIQUE_ID      = 0x4B,//Read Unique ID
    
    FLASH_SECURITY_ERASE      = 0x44,
    FLASH_SECURITY_PROGRAM    = 0x42,
    FLASH_SECURITY_READ       = 0x48,

    FLASH_POWER_DOWN          = 0xB9,
    FLASH_POWER_ON            = 0xAB,
} flash_cmd_t;

typedef enum
{
    NORMAL_AREA = 0,
    SECURITY_AREA,   
} flash_area_t;


typedef union
{
  struct
  {
      uint8_t WIP   : 1;//Write  In Progress
      uint8_t WEL   : 1;//Write  Enable Latch
      uint8_t BP0_4 : 5;//Block  Protect
      uint8_t SRP   : 1;//Status Register Protect        
  }bits;
  uint8_t reg1_data;
} flash_status_reg1_t;

typedef union
{
  struct
  {
      uint8_t SPR1  : 1;
      uint8_t QE    : 1;/* Quad Enable */
      uint8_t LB    : 1;/* OTP Lock Bit */
      uint8_t Res2  : 3;
      uint8_t CMP   : 1;
      uint8_t SUS   : 1;
  }bits; /* TH25Q16HB, GT25Q16B, GT25Q40D */

  struct
  {
      uint8_t SRP1  : 1;
      uint8_t QE    : 1;/* Quad Enable */
      uint8_t DC_SUS: 1;/* (DC:TH25Q16UC, TH25Q32HA Dummy Clk Cycle Control for Fast Read) */
                        /* (SUS2: ZB25VQ16E 0:Program not suspended, 1:Program suspended) */
      uint8_t LB1   : 1;/* Lock OTP1 */
      uint8_t LB2   : 1;/* Lock OTP2 */
      uint8_t LB3   : 1;/* Lock OTP3 */
      uint8_t CMP   : 1;
      uint8_t SUS   : 1;
  }bits_3LB; /* TH25Q16UC, TH25Q32HA, ZB25VQ16E */
  uint8_t reg2_data;
} flash_status_reg2_t;


typedef enum
{
    ERASE_SECTOR_4KB,
    ERASE_BLOCK_32KB,
    ERASE_BLOCK_64KB,
    ERASE_CHIP,
}flash_erase_type_t;


/****************************  FLASH API  **************************/
void     hal_flash_init(void);
void     hal_flash_deinit(void);
uint8_t  hal_flash_read_by_cache(uint32_t offset, uint32_t length, uint8_t *buffer);
uint8_t  hal_flash_read(uint32_t offset, uint32_t length, uint8_t *buffer);
uint8_t  hal_flash_program(uint32_t offset, uint32_t length, uint8_t *buffer);
void     hal_flash_erase(uint32_t offset, uint32_t length);//@param: offset,length(Note:4K Aligned)
void     hal_flash_chip_erase(void);
uint32_t hal_flash_read_id(void);
void     hal_flash_read_unique_id(uint8_t *unique_id);
uint8_t  hal_flash_read_mid(void);
uint16_t hal_flash_read_device_id(void);
uint8_t  hal_flash_read_sr1(void);
uint8_t  hal_flash_read_sr2(void);
uint16_t hal_flash_read_status(void);
void     hal_flash_program_erase_suspend(void);
void     hal_flash_program_erase_resume(void);
void     hal_flash_write_enable(void);
void     hal_flash_write_enable_for_volatile_sr(void);
void     hal_flash_write_disable(void);

void     hal_flash_lock_otp(uint8_t otp_area_num);
uint8_t  hal_flash_get_otp_state(void);
void     hal_flash_quad_mode_enable(uint8_t enable);
void     hal_flash_operation_wait(void);

void     hal_flash_security_area_erase(uint32_t offset);
uint8_t  hal_flash_security_area_program(uint32_t offset, uint32_t len, uint8_t * buf);
void     hal_flash_security_area_read(uint32_t offset, uint32_t len, uint8_t * buf);



#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __HAL_FLASH_H
