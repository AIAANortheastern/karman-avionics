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

/* Initialize the flash memory module */
void init_flashmem(void)
{
    /* Setup default number of entries */
    gFlashmemCtrl.data_addr = INITIAL_DATA_ADDR;
    gFlashmemCtrl.header.entry_size = sizeof(flash_data_entry_t);
    gFlashmemCtrl.header.magic = MAGIC_NUMBER;
    gFlashmemCtrl.header.num_entries = 0;
    sprintf(gFlashmemCtrl.header.version_str, VERSION_STRING);

    /* Initialize flash memory driver */
    init_extflash();

    /* Parse flash memory header */
    flashmem_hdrstatus_t headerStatus;
    flash_data_hdr_t header;

    headerStatus = flashmem_verify_header(&header);

    /* try once again on read failure */
    if(headerStatus == HDR_READFAIL)
    {
        headerStatus = flashmem_verify_header(&header);
        /* If we failed again, just write it off as a loss */
        if(headerStatus == HDR_READFAIL)
        {
            headerStatus = HDR_INVALID;
        }
    }

    /* If the header is invalid, write the one we formed at the top of the function */
    if(headerStatus != HDR_VALID)
    {
        /* Try to write the default header twice */
        flashmem_write_header();
        
        /* If what we read isn't what we wrote, try again... */
        if(HDR_VALID != flashmem_verify_header(&header))
        {
            /* If this fails we're kinda screwed as far as flash memory goes. */
            flashmem_write_header();
        }
    }
    else
    {
        /* Use the value we found already on the flash memory to inform where to write data */
        /* This means that any new data will be written following the pre-exisiting entries. */
        /* In order to ignore the previous entries, the size of entries or the version string */
        /* must be different from what the code expects. Or the magic number is not correct. */
        memcpy((void *)&gFlashmemCtrl.header, (void *)&header, sizeof(flash_data_hdr_t));
    }
}

/* Write the data header stored in gFlashmemCtrl.header to the flash memory */
Bool flashmem_write_header(void)
{
    Bool retVal = false;
    Bool block = true;

    /* The header address is the beginning of memory */
    retVal = extflash_write(0x00000000L, sizeof(flash_data_hdr_t), (uint8_t *)(&(gFlashmemCtrl.header)), block );

    return retVal;
}

/* Determine the validity of the flash memory data header */
flashmem_hdrstatus_t flashmem_verify_header(flash_data_hdr_t *header)
{
    flashmem_hdrstatus_t retVal = HDR_INVALID;
    Bool block = true;

    /* read sizeof(data_hdr) bytes from flash memory starting at byte 0x0000_0000 */
    if(true == extflash_read( 0x00000000L,  sizeof(flash_data_hdr_t), (uint8_t *)&header, block))
    {
        retVal = HDR_READFAIL;
    }
    
    /* If we successfully read the data from the flash memory, verify the information we read */
    if(retVal != HDR_READFAIL)
    {
        /* Check the magic number */
        switch(header->magic)
        {
            case 0:
                /* If it's zero, there's no data at the header location. */
                retVal = HDR_ZERO;
                break;
            case MAGIC_NUMBER:
                /* If we found the magic number, we're halfway there. Check the data entry size and version string next */
                if((header->entry_size == sizeof(flash_data_entry_t)) && (0 == strcmp(header->version_str, gFlashmemCtrl.header.version_str)))
                {
                    /* If there is a pre-existing header, write new entries starting at the existing entry address */
                    gFlashmemCtrl.data_addr += (header->num_entries * sizeof(flash_data_entry_t));
                    /* The header is valid */
                    retVal = HDR_VALID;
                }
                break;
            default:
                /* If we found a garbage magic number, the retVal will be HDR_INVAID */
                break;
        }
    }
    return retVal;
}

/* Write a data entry to the flash memory */
Bool flashmem_write_entry(flash_data_entry_t *entry)
{
    Bool retVal = false;
    Bool block = true;

    /* Increment the number of entries */
    gFlashmemCtrl.header.num_entries++;
    /* Write the new number of entries to the data header at the beginning of the flash memory */
    retVal = extflash_write(FLASHMEM_ENTRIES_ADDR, 2, (uint8_t *)(gFlashmemCtrl.header.num_entries), block);

    /* If no bad things happened, go ahead and write the data. */
    if(retVal == false)
    {
        /* Write to the current data address. This might fail if the data memory is full (i.e. already 512Mb of data on it) */
        retVal = extflash_write(gFlashmemCtrl.data_addr, sizeof(flash_data_entry_t), (uint8_t *)entry, block);
        /* If there were still no bad things that happend, increment the data addresss. */
        if(retVal == false)
        {
            gFlashmemCtrl.data_addr+= sizeof(flash_data_entry_t);
        }
    }
    return retVal;
}

/* Read the entry addressed by index and put it in the caller's struct. */
Bool flashmem_read_entry(flash_data_entry_t *entry, uint32_t index)
{
    Bool retVal = false;
    Bool block = true;

    /* Calculate the address of the indexed entry */
    uint32_t addr = INITIAL_DATA_ADDR + (index * sizeof(flash_data_entry_t));

    /* Read sizeof(entry) bytes from the flash memory */
    retVal = extflash_read(addr, sizeof(flash_data_entry_t), (uint8_t *)entry, block);

    return retVal;
}
