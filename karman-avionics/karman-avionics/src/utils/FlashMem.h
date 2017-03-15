/*
 * FlashMem.h
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
    uint8_t version_str[VERSION_SIZE];
    uint16_t entry_size;
    uint16_t num_entries;
} flash_data_hdr_t;



#endif /* FLASHMEM_H_ */