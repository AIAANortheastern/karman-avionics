/*
 * n25q_512.c
 *
 * External 512 Mb flash memory.
 *
 * Created: 3/15/2017 12:06:51 AM
 *  Author: Andrew Kaster
 */ 

#include <compiler.h>

#include <string.h>
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

#define EXTFLASH_PAGE_SIZE      (0x100) /* Writes that cross page boundary cause unwanted behavior */
#define EXTFLASH_READ_DATA_CMD  (0x03)
#define EXTFLASH_READ_SR_CMD    (0x05)
#define EXTFLASH_WRITE_ENABLE   (0x06)
#define EXTFLASH_4BYTEMODE      (0xB7)

 #define EXTFLASH_CS_PORT (PORTA)
 #define EXTFLASH_CS_BM   (1 << 5)

 #define EXTFLASH_WREN_LATCH  (1 << 1)

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
    gExtflashControl.task_inprog = false;

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
    Bool block = true;

    extflash_write_enable(block);

    gExtflashControl.spi_send_buffer[0] = EXTFLASH_4BYTEMODE;

    (void)spi_master_blocking_send_request(&(extflashSpiMaster),
                                           &(gExtflashControl.cs_info),
                                           (void *)(gExtflashControl.spi_send_buffer),
                                           1,
                                           (void *)(gExtflashControl.spi_recv_buffer),
                                           0,
                                           &(gExtflashControl.send_complete));
}


extflash_hdrstatus_t extflash_verify_header(flash_data_hdr_t *header)
{
    extflash_hdrstatus_t retVal = HDR_INVALID;
    Bool block = true;

    /* read sizeof(data_hdr) bytes from flash memory starting at byte 0x000...0 */
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


/* NOTE: Using 4 byte address mode. Caller MUST add 5 dummy bytes to beginning of recieve buffer.
         http://www.micron.com/~/media/Documents/Products/Data%20Sheet/NOR%20Flash/Serial%20NOR/N25Q/n25q_512mb_1ce_3v_65nm.pdf
*/
Bool extflash_read(uint32_t addr, size_t num_bytes, uint8_t *buf, Bool block)
{
    Bool retVal = false; /* False means no error! */

    if(gExtflashControl.task_inprog)
    {
        retVal = true; /* BUSY yo */
    }
    else
    {
        /* READ Command is 0x03 for normal read. Format: CMD ADDR[3 - 0] {DUMMY BYTES} */
        memset((void *)(gExtflashControl.spi_send_buffer), 0, sizeof(gExtflashControl.spi_send_buffer)/sizeof(gExtflashControl.spi_send_buffer[0]));

        /* SPI is MSB FIRST in mode 0. AVR-GCC treats larger integers as little endian. */
        gExtflashControl.spi_send_buffer[0] = EXTFLASH_READ_DATA_CMD;
        gExtflashControl.spi_send_buffer[1] = (uint8_t)((num_bytes & 0xFF000000) >> 24);
        gExtflashControl.spi_send_buffer[2] = (uint8_t)((num_bytes & 0x00FF0000) >> 16);
        gExtflashControl.spi_send_buffer[3] = (uint8_t)((num_bytes & 0x0000FF00) >> 8);
        gExtflashControl.spi_send_buffer[4] = (uint8_t)((num_bytes & 0x000000FF));

        if(block)
        {
            retVal = spi_master_blocking_send_request(&(extflashSpiMaster),
                                                      &(gExtflashControl.cs_info),
                                                      (void *)(gExtflashControl.spi_send_buffer),
                                                      EXTFLASH_CMDADDR_SIZE,
                                                      (void *)buf,
                                                      num_bytes,
                                                      &(gExtflashControl.send_complete));
        }
        else
        {
            retVal = spi_master_enqueue(&(extflashSpiMaster),
                                    &(gExtflashControl.cs_info),
                                    (void *)(gExtflashControl.spi_send_buffer),
                                    EXTFLASH_CMDADDR_SIZE,
                                    (void *)buf,
                                    num_bytes,
                                    &(gExtflashControl.send_complete));

            gExtflashControl.task_inprog = true;
        }
    }
    return retVal;
} 

Bool extflash_get_status(void)
{
    Bool retVal = false; /* False means NOT busy! */

    if(gExtflashControl.task_inprog)
    {
        if(gExtflashControl.send_complete == true)
        {
            gExtflashControl.task_inprog = false;
        }
        else
        {
            retVal = true;
        }
    }

    return retVal;
}


/* Before writing to any register or any memory, the write enable command must be sent.
   After writing, the read status register command must be sent.
   That means 3 separate SPI transactions with CS being driven high after each one.
   Maximum number of bytes per write operation is 64 bytes. Therefore, 
*/

/* NOTE: Result will be in most significant byte of buf. i.e. to access you need to look at ((buf & 0xFF00) >> 8) */
Bool extflash_read_status_reg(uint16_t *buf, Bool block)
{
    Bool retVal = false;

    memset((void *)(gExtflashControl.spi_send_buffer), 0, sizeof(gExtflashControl.spi_send_buffer)/sizeof(gExtflashControl.spi_send_buffer[0]));

    gExtflashControl.spi_send_buffer[0] = EXTFLASH_READ_SR_CMD;

    if(block)
    {
        retVal = spi_master_blocking_send_request(&(extflashSpiMaster),
                                                  &(gExtflashControl.cs_info),
                                                  (void *)(gExtflashControl.spi_send_buffer),
                                                  1,
                                                  (void *)buf,
                                                  2,
                                                  &(gExtflashControl.send_complete));
    }
    else
    {
        retVal = true;
    }

    return retVal;
}

Bool extflash_write_enable(Bool block)
{
     Bool retVal = false;
     uint16_t statusreg;

     memset((void *)(gExtflashControl.spi_send_buffer), 0, sizeof(gExtflashControl.spi_send_buffer)/sizeof(gExtflashControl.spi_send_buffer[0]));

     if(block)
     {
        gExtflashControl.spi_send_buffer[0] = EXTFLASH_WRITE_ENABLE;

        retVal = spi_master_blocking_send_request(&(extflashSpiMaster),
                                                   &(gExtflashControl.cs_info),
                                                   (void *)(gExtflashControl.spi_send_buffer),
                                                   1,
                                                   (void *)(gExtflashControl.spi_recv_buffer),
                                                   0,
                                                   &(gExtflashControl.send_complete));

        do 
        {
            extflash_read_status_reg(&statusreg, block);
        } while (~((statusreg & 0xFF00) >> 8) & EXTFLASH_WREN_LATCH); /* This do while ensures the write enable is completed */
     }
     else
     {
         retVal = true;
     }

     return retVal;
}

Bool extflash_write(uint32_t addr, size_t num_bytes, uint8_t *buf, Bool block)
{
    /* ALL WRITES MUST BE 256 BYTE ALIGNED
     *  This means that if the address you want to write to is 0xXXXXXXF0 and you want to write more than more than 8 bytes, you must use more than one write operation.
     *  Any write that goes over the 256 byte page boundary will reset to the beginning of the 256 byte page. (NOT GOOD).
     *
     * Steps:
     *      1) IF address is page aligned, go to step 3
     *      2) Write bytes up to page boundary (256 byte pages)
     *      3) Loop while > 256 bytes left: Write 256 bytes per page
     *      4) Write excess bytes (up to 256) on new page (i.e. what's left over after: writing to first page boundary AND writing integer number of pages)
     *  
     *  For each write:
     *      Send Write Enable command (RAISE CS)
     *      Send Page program + 4 byte address (DON'T RAISE CS)
     *      Send up to 256 bytes of data (RAISE CS)
     *      Send Read Status Register Command (CLK OUT 1 READ BYTE)
     */

    Bool retVal = false; /* no issues. */
    uint16_t dummy;

    /* Validate address */
    if(addr > 0x4000000)
    {
        return true;
    }

    if(block)
    {

        retVal = extflash_write_enable(block);

        if(!(addr & 0xFF)) /* not page aligned */
        {
            
        }

        retVal += extflash_read_status_reg(&dummy, block);
    }
    else
    {
        retVal = true;
    }
    return retVal;
}


