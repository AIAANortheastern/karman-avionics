/**
 * @file FlashMem.h
 *
 * @brief Flash Memory API
 *
 * Created: 3/15/2017 2:56:07 PM
 *  Author: Andrew Kaster
 */ 


#ifndef FLASHMEM_H_
#define FLASHMEM_H_

#include "SensorDefs.h"

typedef struct  
{
    uint32_t timestamp;
    sensor_data_t data;
    uint16_t chksum;
} flash_data_entry_t;

#define VERSION_SIZE (10)

#define VERSION_STRING ("DEBUG-001")
#define MAGIC_NUMBER   (0xCAFE)

typedef struct  
{
    uint16_t magic;
    char version_str[VERSION_SIZE];
    uint16_t entry_size;
    uint16_t num_entries;
} flash_data_hdr_t;

#define FLASHMEM_ENTRIES_ADDR (0x0000000EL)

typedef struct  
{
    flash_data_hdr_t header;
    uint32_t data_addr;
} flashmem_ctrl_t;

typedef enum
{
    HDR_INVALID,
    HDR_ZERO,
    HDR_VALID,
    HDR_READFAIL,
} flashmem_hdrstatus_t;

flashmem_hdrstatus_t flashmem_verify_header(flash_data_hdr_t *header);
Bool flashmem_write_entry(flash_data_entry_t *entry);
Bool flashmem_read_entry(flash_data_entry_t *entry, uint32_t index);
Bool flashmem_write_header(void);
void init_flashmem(void);

extern flashmem_ctrl_t gFlashmemCtrl;


#endif /* FLASHMEM_H_ */
