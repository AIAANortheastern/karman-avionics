/*
 * n25q_512.h
 *
 * Created: 3/15/2017 12:06:16 AM
 *  Author: Andrew Kaster
 */ 


#ifndef N25Q_512_H_
#define N25Q_512_H_

#include <asf.h>
#include "Spi_service.h"
#include "FlashMem.h"

typedef struct  
{
    spi_master_t        *spi_master;
    chip_select_info_t  cs_info;
    volatile uint8_t    *spi_send_buffer;
    volatile uint8_t    *spi_recv_buffer;
    volatile Bool       send_complete;
} extflash_ctrl_t;

void init_extflash(void);

void extflash_initialize_regs(void);

typedef enum
{
    HDR_INVALID,
    HDR_ZERO,
    HDR_VALID,
    HDR_READFAIL,
} extflash_hdrstatus_t;

extflash_hdrstatus_t extflash_verify_header(flash_data_hdr_t *header);

Bool extflash_read(uint32_t addr, size_t num_bytes, uint8_t *buf, Bool block);

Bool extflash_write(uint32_t addr, size_t num_bytes, uint8_t *buf, Bool block);


#endif /* N25Q_512_H_ */