/*
 * FlashMem.c
 *
 * Created: 3/16/2017 7:22:59 PM
 *  Author: Andrew Kaster
 */ 

#include "FlashMem.h"
#include "n25q_512.h"

#include <string.h>

#define INITIAL_DATA_ADDR (0x00000010L)


/* This variable is truly global */
flashmem_ctrl_t gFlashmemCtrl;

void init_flashmem(void)
{
    gFlashmemCtrl.data_addr = INITIAL_DATA_ADDR;
    gFlashmemCtrl.header.entry_size = sizeof(flash_data_entry_t);
    gFlashmemCtrl.header.magic = MAGIC_NUMBER;
    gFlashmemCtrl.header.num_entries = 0;
    sprintf(gFlashmemCtrl.header.version_str, VERSION_STRING);

    init_extflash();
}

Bool flashmem_write_header(void)
{
    Bool retVal = false;
    Bool block = true;

    retVal = extflash_write(0x00000000L, sizeof(flash_data_hdr_t), (uint8_t *)(&(gFlashmemCtrl.header)), block );

    return retVal;
}

flashmem_hdrstatus_t flashmem_verify_header(flash_data_hdr_t *header)
{
    flashmem_hdrstatus_t retVal = HDR_INVALID;
    Bool block = true;

    /* read sizeof(data_hdr) bytes from flash memory starting at byte 0x000...0 */
    if(extflash_read( 0x00000000L,  sizeof(flash_data_hdr_t), (uint8_t *)&header, block))
    {
        retVal = HDR_READFAIL;
    }
    
    if(retVal != HDR_READFAIL)
    {
        switch(header->magic)
        {
            case 0:
                retVal = HDR_ZERO;
                break;
            case MAGIC_NUMBER:
                retVal = HDR_VALID;
                break;
            default:
                break;
        }
    }
    return retVal;
}

Bool flashmem_write_entry(flash_data_entry_t *entry)
{
    Bool retVal = false;
    Bool block = true;

    gFlashmemCtrl.header.num_entries++;
    retVal = extflash_write(FLASHMEM_ENTRIES_ADDR, 2, (uint8_t *)(gFlashmemCtrl.header.num_entries), block);

    if(!retVal)
    {
        retVal = extflash_write(gFlashmemCtrl.data_addr, sizeof(flash_data_entry_t), (uint8_t *)entry, block);
        gFlashmemCtrl.data_addr+= sizeof(flash_data_entry_t);
    }
    return retVal;
}

