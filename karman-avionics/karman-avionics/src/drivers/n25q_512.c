/*
 * n25q_512.c
 *
 * External 512 Mb flash memory.
 *
 * Created: 3/15/2017 12:06:51 AM
 *  Author: Andrew Kaster
 */ 

#include <compiler.h>

#include "n25q_512.h"
#include "FlashMem.h"
#include "Spi_service.h"
#include "Spi_bg_task.h"

#define EXTFLASH_MOSI (1 << 5) /* 0x20 */
#define EXTFLASH_MISO (1 << 6) /* 0x40 */
#define EXTFLASH_SCK  (1 << 7) /* 0x80 */
#define EXTFLASH_SPI (SPIE)
#define EXTFLASH_SPI_PORT (PORTE)
#define EXTFLASH_SPI_CTRL_VALUE (SPI_MODE_0_gc | SPI_PRESCALER_DIV4_gc | SPI_ENABLE_bm | SPI_MASTER_bm)

#define EXTFLASH_MAX_TRANS_SIZE (256) /* Transfers larger than 256 bytes at a time are undefined (page size) */

 #define EXTFLASH_CS_PORT (PORTA)
 #define EXTFLASH_CS_BM   (1 << 5)


spi_master_t extflashSpiMaster;

extflash_ctrl_t gExtflashControl;

/* Initialize all things the external flash needs.*/
void init_extflash(void)
{
    /* Initialize SPI interface on port E */
    /* See XMEGA AU Manual page 146, page 276 */
    sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_SPI);
    EXTFLASH_SPI_PORT.DIRSET = EXTFLASH_MOSI;
    EXTFLASH_SPI_PORT.DIRSET = EXTFLASH_SCK;
    EXTFLASH_SPI_PORT.DIRCLR = EXTFLASH_MOSI;
    EXTFLASH_SPI.CTRL = EXTFLASH_SPI_CTRL_VALUE;
    EXTFLASH_SPI.INTCTRL = SPI_INTLVL_LO_gc;

    init_spi_master_service(&extflashSpiMaster, &EXTFLASH_SPI, &EXTFLASH_SPI_PORT, spi_bg_task);
    spi_bg_add_master(&extflashSpiMaster);

    /* Initialize chip select and other control variables */
    gExtflashControl.cs_info.csPort = &EXTFLASH_CS_PORT;
    gExtflashControl.cs_info.pinBitMask = EXTFLASH_CS_BM;
    gExtflashControl.spi_master = &extflashSpiMaster;
    
    gExtflashControl.send_complete = false;

    /* Set CS pin high */
    gExtflashControl.cs_info.csPort->DIRSET = gExtflashControl.cs_info.pinBitMask;

    extflash_initialize_regs();
}

/* Interrupt service routine for the SPI interrupt on port E. */
ISR(SPIE_INT_vect)
{
    spi_master_ISR(&extflashSpiMaster);
}

void extflash_initialize_regs(void)
{
    /* Enable 4 byte addressing */
    /* WRITE ENABLE 06h --> ENTER 4-BYTE ADDRESS MODE B7h  */

}


extflash_hdrstatus_t extflash_verify_header(flash_data_hdr_t *header)
{
    extflash_hdrstatus_t retVal = HDR_INVALID;
    Bool block = true;

    /* read sizeof(datahdr) bytes from flash memory starting at byte 0x000...0 */
    if(extflash_read( 0x0L,  sizeof(flash_data_hdr_t), (uint8_t *)&header, block))
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


/* NOTE: To access more than 128 Mb of memory, one must modify the extended address register.
         To access more than 256 Mb of memory, one must do other crazy stuff see device sheet
         http://www.micron.com/~/media/Documents/Products/Data%20Sheet/NOR%20Flash/Serial%20NOR/N25Q/n25q_512mb_1ce_3v_65nm.pdf
*/
Bool extflash_read(uint32_t addr, size_t num_bytes, uint8_t *buf, Bool block)
{
    return false;
} 

/* Before writing to any register or any memory, the write enable command must be sent.
   After writing, the read status register command must be sent.
   That means 3 separate SPI transactions with CS being driven high after each one.
   Maximum number of bytes per write operation is 64 bytes. Therefore, 
*/
Bool extflash_write(uint32_t addr, size_t num_bytes, uint8_t *buf, Bool block)
{
    /* ALL WRITES MUST BE 64 BYTE ALIGNED
     *  This means that if the address you want to write to is 0xXXXXXXF0 and you want to write more than more than 8 bytes, you must use more than one write operation.
     *  Any write that goes over the 64 byte page boundary will reset to the beginning of the 64 byte page. (NOT GOOD).
     *
     * Steps:
     *      1) IF address is page aligned, go to step 3
     *      2) Write bytes up to page boundary
     *      3) Loop while > 64 bytes left: Write 64 bytes per page
     *      4) Write excess bytes (up to 64) on new page (i.e. what's left over after: writing to first page boundary AND writing integer number of pages)
     *  
     *  For each write:
     *      Send Write Enable command (RAISE CS)
     *      Send Page program + 4 byte address (DON'T RAISE CS)
     *      Send up to 64 bytes of data (RAISE CS)
     *      Send Read Status Register Command (CLK OUT 1 READ BYTE)
     *
     */
    return false;
}


