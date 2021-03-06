/**
 * @file n25q_512.h
 *
 * @brief Flash Memory Driver
 *
 * Created: 3/15/2017 12:06:16 AM
 * Author: Andrew Kaster
 */ 


#ifndef N25Q_512_H_
#define N25Q_512_H_

#include <asf.h>
#include "Spi_service.h"
#include "FlashMem.h"

#define EXTFLASH_CMDADDR_SIZE   (5)         /**< 5 bytes for 1 byte command and 4 byte address */
#define EXTFLASH_PAGE_SIZE      (0x100)     /**< Writes that cross page boundary cause unwanted behavior */
#define EXTFLASH_SIZE           (0x1000000) /**< 128 Mebibit */

/** Control structure. */
typedef struct  
{
    spi_master_t        *spi_master; /**< Pointer to local spi master struct */
    chip_select_info_t  cs_info;     /**< Chip Select pin info */
    volatile uint8_t    spi_send_buffer[EXTFLASH_CMDADDR_SIZE + EXTFLASH_PAGE_SIZE]; /**< Need to support writing entire page at once ... */
    volatile uint8_t    spi_recv_buffer[EXTFLASH_CMDADDR_SIZE + EXTFLASH_PAGE_SIZE]; /**< Default read buffer. */
    volatile Bool       send_complete; /**< Keep track of if our transfers are complete */
    Bool                task_inprog;   /**< Are we in progress? */
    uint8_t             num_active_requests; /**< How many active requests there are */
} extflash_ctrl_t;

void init_extflash(void);

void extflash_initialize_regs(void);

Bool extflash_get_status(void);

Bool extflash_read(uint32_t addr, size_t num_bytes, uint8_t *buf, Bool block);

Bool extflash_write(uint32_t addr, size_t num_bytes, uint8_t *buf, Bool block);

Bool extflash_write_enable(Bool block);

Bool extflash_read_status_reg(uint16_t *buf, Bool block);

Bool extflash_write_one(uint16_t num_bytes, uint32_t addr, uint8_t *buf, uint16_t buff_offset, Bool block);

#endif /* N25Q_512_H_ */
