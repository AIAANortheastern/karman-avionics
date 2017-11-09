/**
 * @file n25q_512.c
 *
 * @brief External 512 Mb flash memory.
 *
 * Created: 3/15/2017 12:06:51 AM
 * Author: Andrew Kaster
 */ 

#include <compiler.h>

#include <string.h>
#include "n25q_512.h"
#include "FlashMem.h"
#include "Spi_service.h"
#include "Spi_bg_task.h"
#include "ISRUtils.h"

#define EXTFLASH_MOSI (FLASH_MOSI) /**< Internal definition of flash MOSI */
#define EXTFLASH_MISO (FLASH_MISO) /**< Internal definition of flash MISO */
#define EXTFLASH_SCK  (FLASH_SCLK) /**< Internal definition of flash SCLK */
#define EXTFLASH_SPI (FLASH_SPI)   /**< Internal definition of flash SPI instance (actually UART) */
#define EXTFLASH_SPI_PORT (FLASH_PORT) /**< Internal definition of flash SPI port */

#define EXTFLASH_PAGE_MASK      (0x000000FF) /**< Mask to know if data fits in one page. Page size 256 bytes */
#define EXTFLASH_READ_DATA_CMD  (0x03) /**< Read command */
#define EXTFLASH_READ_SR_CMD    (0x05) /**< Read Status register Command */
#define EXTFLASH_WRITE_ENABLE   (0x06) /**< Write enable command. Write enable must be sent before page program */
#define EXTFLASH_4BYTEMODE      (0xB7) /**< Config value for 4 byte address mode */
#define EXTFLASH_PAGE_PROGRAM   (0x02) /**< Page program command. i.e. actually write something. */

#define EXTFLASH_CS_PORT (FLASH_PORT) /**< Internal definition of chip select port */
#define EXTFLASH_CS_BM   (FLASH_CS)   /**< Internal definition of chip select pin */

#define EXTFLASH_WREN_LATCH  (1 << 1) /**< Status register mask for write enable */

#define SPI_BAUD_RATE (1000000) /**< 1MHz */

#ifndef F_CPU
#define F_CPU (sysclk_get_per_hz()) /**< CPU Frequency */
#endif

/**
 * @brief Calculate Baud control value for USART.
 * 
 * See https://github.com/abcminiuser/lufa/blob/master/LUFA/Drivers/Peripheral/XMEGA/SerialSPI_XMEGA.h 
 */
#define SPI_BAUDCTRLVAL(Baud)       ((Baud < (F_CPU / 2)) ? ((F_CPU / (2 * Baud)) - 1) : 0)

/** SPI Master instance. */
spi_master_t extflashSpiMaster;

/** Control structure */
extflash_ctrl_t gExtflashControl;

/** 
 * Initialize all things the external flash needs.
*/
void init_extflash(void)
{
    /** Initialize USART in SPI Master Mode */
    /* See XMEGA AU Manual page 146, page 280 */
    /* NOTE PINS ARE SETUP TO USE USART IN SPI MASTER MODE! */

    uint16_t baudrate = SPI_BAUDCTRLVAL(SPI_BAUD_RATE);

    sysclk_enable_peripheral_clock(&EXTFLASH_SPI);
    EXTFLASH_SPI.BAUDCTRLB = (uint8_t)((baudrate) >> 8); /* MSBs of Baud rate value. */
    EXTFLASH_SPI.BAUDCTRLA = (uint8_t)(baudrate & 0xFF); /* LSBs of Baud rate value. */
    EXTFLASH_SPI.CTRLA = 0x10; /* RXCINTLVL = 1, other 2 disabled */
    EXTFLASH_SPI.CTRLB = 0x18; /* Enable RX and TX */
    EXTFLASH_SPI.CTRLC = 0xC0; /* MSB first, mode 0. PMODE, SBMODE, CHSIZE ignored by SPI */

    init_spi_master_service(&extflashSpiMaster, &EXTFLASH_SPI, &EXTFLASH_SPI_PORT, spi_bg_task);
    spi_bg_add_master(&extflashSpiMaster);

    /* Initialize chip select and other control variables */
    gExtflashControl.cs_info.csPort = &EXTFLASH_CS_PORT;
    gExtflashControl.cs_info.pinBitMask = EXTFLASH_CS_BM;
    gExtflashControl.spi_master = &extflashSpiMaster;
    
    gExtflashControl.send_complete = false;
    gExtflashControl.task_inprog = false;

    extflash_initialize_regs();
}

/** Interrupt service routine for the USART interrupt. */
ISR(FLASH_SPI_INT)
{
    spi_master_ISR(&extflashSpiMaster);
}

/** Initialize non-volatile control registers */
void extflash_initialize_regs(void)
{
    Bool block = true;

    /* Enable 4 byte addressing */
    /* WRITE ENABLE 06h --> ENTER 4-BYTE ADDRESS MODE B7h  */
    /* Send write enable command to allow writing to registers */
    extflash_write_enable(block);

    /* We just want to enable 4 byte address mode */
    gExtflashControl.spi_send_buffer[0] = EXTFLASH_4BYTEMODE;

    /* Send a blocking request to the flash memory to enable this mode */
    (void)spi_master_blocking_send_request(&(extflashSpiMaster),
                                           &(gExtflashControl.cs_info),
                                           (void *)(gExtflashControl.spi_send_buffer),
                                           1,
                                           (void *)(gExtflashControl.spi_recv_buffer),
                                           0,
                                           &(gExtflashControl.send_complete),
                                           false);
}

/** 
 * @brief Read a value from the flash memory
 *
 * @param addr Address to read from
 * @param num_bytes How many bytes to read
 * @param[out] buf Buffer to store the data in
 * @param block Use blocking/nonblocking path
 * @return true on success, false on failure
 *
 * Read num_bytes from the flash memory starting at address addr and store them in buf
 * NOTE: Using 4 byte address mode -- 
 *        http://www.micron.com/~/media/Documents/Products/Data%20Sheet/NOR%20Flash/Serial%20NOR/N25Q/n25q_512mb_1ce_3v_65nm.pdf
 */
Bool extflash_read(uint32_t addr, size_t num_bytes, uint8_t *buf, Bool block)
{
    Bool retVal = false; /* true on success */

    if(gExtflashControl.task_inprog)
    {
        retVal = false; /* BUSY yo */
    }
    else
    {
        /* READ Command is 0x03 for normal read. Format: CMD ADDR[3 - 0] {DUMMY BYTES} */
        /* We're not using any dummy clock cycles in standard SPI mode. */
        //memset((void *)(gExtflashControl.spi_send_buffer), 0, sizeof(gExtflashControl.spi_send_buffer)/sizeof(gExtflashControl.spi_send_buffer[0]));

        /* SPI is MSB FIRST in mode 0. AVR-GCC treats larger integers as little endian. */
        gExtflashControl.spi_send_buffer[0] = EXTFLASH_READ_DATA_CMD;
        /* Ensure that our bytes are sent MSB first. */
        gExtflashControl.spi_send_buffer[1] = (uint8_t)((num_bytes & 0xFF000000) >> 24);
        gExtflashControl.spi_send_buffer[2] = (uint8_t)((num_bytes & 0x00FF0000) >> 16);
        gExtflashControl.spi_send_buffer[3] = (uint8_t)((num_bytes & 0x0000FF00) >> 8);
        gExtflashControl.spi_send_buffer[4] = (uint8_t)((num_bytes & 0x000000FF));

        if(block)
        {
            /* We want to send 5 bytes and recieive as many as the caller asked for. */
            retVal = spi_master_blocking_send_request(&(extflashSpiMaster),
                                                      &(gExtflashControl.cs_info),
                                                      (void *)(gExtflashControl.spi_send_buffer),
                                                      EXTFLASH_CMDADDR_SIZE,
                                                      (void *)(gExtflashControl.spi_recv_buffer),
                                                      EXTFLASH_CMDADDR_SIZE + num_bytes,
                                                      &(gExtflashControl.send_complete),
                                                      false);
            /* Copy data into caller's buffer, ignoring garbage data from sending command */
            memcpy((void *)buf, (void *)(&(gExtflashControl.spi_recv_buffer[5])), num_bytes);
        }
        else
        {
            /* NOTE: It is the responsiblity of the CALLER to discard the first 5 bytes of this buffer!!! */
            retVal = spi_master_enqueue(&(extflashSpiMaster),
                                    &(gExtflashControl.cs_info),
                                    (void *)(gExtflashControl.spi_send_buffer),
                                    EXTFLASH_CMDADDR_SIZE,
                                    (void *)buf,
                                    num_bytes,
                                    &(gExtflashControl.send_complete),
                                    false);

            gExtflashControl.task_inprog = true;
        }
    }
    /* spi_master_enqueue and spi_blocking_send_request return true on success*/
    return retVal;
} 

/** @brief Get the status of the module in non-blocking mode.
 * 
 * @return false -- not busy, true -- busy
 *
 * Use to check if the transfer is complete
 *
 * NOTE: In non-blocking mode there are a lot more responsibilities on the CALLER's buffer! */
Bool extflash_is_busy(void)
{
    Bool retVal = false; /* False means NOT busy! */

    /* If we were doing something before.. */
    if(gExtflashControl.task_inprog)
    {
        /* If we finished what we were doing... */
        if(gExtflashControl.send_complete == true)
        {
            /* We're not doing anything anymore */
            gExtflashControl.task_inprog = false;
        }
        else
        {
            /* We're still busy! */
            retVal = true;
        }
    }

    return retVal;
}


/** 
   @brief Read the status register of the flash memory. 
 
   @param[out] buf Place to store status register value
   @param block Use blocking/nonblocking path
   @return true on success, false on failure

   Before writing to any register or any memory, the write enable command must be sent.
   After writing, the read status register command must be sent.
   That means 3 separate SPI transactions with CS being driven high after each one.
   Maximum number of bytes per write operation is 256 bytes. 

   Required before writing to a lot of places is considered "done"
   NOTE: Result will be in most significant byte of buf. i.e. to access you need to look at ((buf & 0xFF00) >> 8) 
*/
Bool extflash_read_status_reg(uint16_t *buf, Bool block)
{
    Bool retVal = false;

    /* Clear the send buffer */
    //memset((void *)(gExtflashControl.spi_send_buffer), 0, sizeof(gExtflashControl.spi_send_buffer)/sizeof(gExtflashControl.spi_send_buffer[0]));

    /* All we want to send is the read status register command. */
    gExtflashControl.spi_send_buffer[0] = EXTFLASH_READ_SR_CMD;

    if(block)
    {
        /* Send a blocking request. NOTE: In order to finish the request, the status register needs to be clocked out. */
        retVal = spi_master_blocking_send_request(&(extflashSpiMaster),
                                                  &(gExtflashControl.cs_info),
                                                  (void *)(gExtflashControl.spi_send_buffer),
                                                  1,
                                                  (void *)buf,
                                                  2,
                                                  &(gExtflashControl.send_complete),
                                                  false);
    }
    else
    {
        /* Non-blocking read of status register not implemented...*/
        retVal = false;
    }

    return retVal;
}

/** @brief Enable writing to the flash memory module. 
 *
 * @return true on success, false on failure
 *
 * This is finished when a read of the status regsiter shows that the write enable latch
 *  Is clear
*/
Bool extflash_write_enable(Bool block)
{
     Bool retVal = false;
     uint16_t statusreg;

     /* Clear the send buffer. */
     memset((void *)(gExtflashControl.spi_send_buffer), 0, sizeof(gExtflashControl.spi_send_buffer)/sizeof(gExtflashControl.spi_send_buffer[0]));

     if(block)
     {
        /* All we're sending is the write enable command. */
        gExtflashControl.spi_send_buffer[0] = EXTFLASH_WRITE_ENABLE;

        /* Send that one byte to the peripheral */
        retVal = spi_master_blocking_send_request(&(extflashSpiMaster),
                                                   &(gExtflashControl.cs_info),
                                                   (void *)(gExtflashControl.spi_send_buffer),
                                                   1,
                                                   (void *)(gExtflashControl.spi_recv_buffer),
                                                   0,
                                                   &(gExtflashControl.send_complete),
                                                   false);

        /* Keep reading the status register until the write enable is confirmed. No timeout. this is pretty dangerous tbh. */
        do 
        {
            /* Read status register to confirm that write has been enabled. */
            retVal = extflash_read_status_reg(&statusreg, block);

        } while (~((statusreg & 0xFF00) >> 8) & EXTFLASH_WREN_LATCH); /* This do while ensures the write enable is completed */
     }
     else
     {
        /* Non-blocking not implemented yet... */
         retVal = false;
     }

     return retVal;
}

/** @brief Send a valid number of bytes to the flash memory.
 * 
 * @return true on success, false on failure.
 *
 * The caller is responsible for write enable and for read status register
 * NOTE: For internal use only! use extflash_write instead! It has the proper error checking
*/
Bool extflash_write_one(uint16_t num_bytes, uint32_t addr, uint8_t *buf, uint16_t buff_offset, Bool block)
{
    Bool retVal = true; /* no issues */

    /* Clear the send buffer. */
    //memset((void *)(gExtflashControl.spi_send_buffer), 0, sizeof(gExtflashControl.spi_send_buffer)/sizeof(gExtflashControl.spi_send_buffer[0]));

    if(block)
    {
        /* We want a page program at the address the caller gave us. */
        gExtflashControl.spi_send_buffer[0] = EXTFLASH_PAGE_PROGRAM;
        /* Ensure that the address is MSB first */
        gExtflashControl.spi_send_buffer[1] = (uint8_t)((addr & 0xFF000000) >> 24);
        gExtflashControl.spi_send_buffer[2] = (uint8_t)((addr & 0x00FF0000) >> 16);
        gExtflashControl.spi_send_buffer[3] = (uint8_t)((addr & 0x0000FF00) >> 8);
        gExtflashControl.spi_send_buffer[4] = (uint8_t)((addr & 0x000000FF));

        /* Copy the request into the send buffer. The request can be up to 256 bytes. */
        memcpy((void *)(&(gExtflashControl.spi_send_buffer[5])), (void *)(&(buf[buff_offset])), num_bytes);

        /* Send up to 256 + 5 bytes, expecting none in return. */
        retVal = spi_master_blocking_send_request(&(extflashSpiMaster),
                                                   &(gExtflashControl.cs_info),
                                                   (void *)(gExtflashControl.spi_send_buffer),
                                                   num_bytes + EXTFLASH_CMDADDR_SIZE,
                                                   (void *)(gExtflashControl.spi_recv_buffer),
                                                   0,
                                                   &(gExtflashControl.send_complete),
                                                   false);
    }
    else
    {
        /* non-blocking not implemented... */
         retVal = false;
    }

    return retVal;
}

/** @brief Write any number of bytes to the flash memory. 
 * 
 * @param addr Address to write to
 * @param num_bytes Number of bytes to write
 * @param buf Buffer to send bytes from 
 * @param block Use blocking/nonblocking path
 * @return True on success, false on failure 
 *
 * Cannot write more than 65536 bytes. Why we would ever need to write that many is a mystery.
*/
Bool extflash_write(uint32_t addr, size_t num_bytes, uint8_t *buf, Bool block)
{
    /** ALL WRITES MUST BE 256 BYTE ALIGNED
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
     *      Send up to ***256*** bytes of data (RAISE CS)
     *      Send Read Status Register Command (CLK OUT 1 READ BYTE)
     */

    Bool retVal = true; /* no issues. */
    uint16_t dummy = 0;
    uint32_t curr_addr = addr;
    size_t rem_bytes = num_bytes;
    uint16_t num_bytes_to_send = num_bytes;
    uint16_t buffer_offset = 0;

    /* Validate address. Not too big and won't overflow the max number of bytes. */
    if((addr > EXTFLASH_SIZE) || ((addr + num_bytes) > EXTFLASH_SIZE))
    {
        return false;
    }

    if(block)
    {
        /* If we're not page aligned, we need to write up to a page boundary in one write cycle. */
        if(curr_addr & EXTFLASH_PAGE_MASK) /* not page aligned */
        {
            /* If we have enough bytes that we would overflow the page, and need a new one. */
            if((EXTFLASH_PAGE_MASK - (curr_addr & EXTFLASH_PAGE_MASK)) < rem_bytes) /* write would overflow page boundary */
            {
                /* Enable writing. */
                retVal = extflash_write_enable(block);

                /* Send 0xFF - address & 0xff bytes. This will fill up the rest of the page. */
                num_bytes_to_send = EXTFLASH_PAGE_MASK - (curr_addr & EXTFLASH_PAGE_MASK);
                rem_bytes -= num_bytes_to_send;
                /* Send that many bytes over SPI */
                retVal &= extflash_write_one(num_bytes_to_send, curr_addr, buf, buffer_offset, block);
                /* Page align curr_addr. If the lsb is not 0x00 we have a problem. */
                /* (Num bytes to send + curr_addr) & 0xff is 0xFF by design.*/
                curr_addr = curr_addr + num_bytes_to_send + 1;
                buffer_offset += num_bytes_to_send;

                /* Read the status register to finish the transaction. true means there is something wrong. */
                retVal &= extflash_read_status_reg(&dummy, block);
            }
        }

        /* Write full pages while we have more than 256 bytes left. */
        while((rem_bytes > EXTFLASH_PAGE_SIZE) && (retVal == true)) /* Write full pages */
        {
            /* Enable writing. */
            retVal &= extflash_write_enable(block);

            /* Send 256 bytes */
            num_bytes_to_send = EXTFLASH_PAGE_SIZE;
            rem_bytes -= num_bytes_to_send;
            /* Send 256 bytes over SPI */
            retVal &= extflash_write_one(num_bytes_to_send, curr_addr, buf, buffer_offset, block);

            /* Add 256 to how many bytes we've sent. */
            curr_addr += num_bytes_to_send;
            buffer_offset += num_bytes_to_send;

            /* Read the status register to finish the transaction. */
            retVal &= extflash_read_status_reg(&dummy, block);
        }

        /* If we still have bytes to write, write the rest to a new page. Because of the checks above, */
        /* this is guaranteed not to overflow the page. */
        if((rem_bytes > 0) && (retVal == true)) /* Write remaining bytes to new page*/
        {
            retVal &= extflash_write_enable(block);

            num_bytes_to_send = rem_bytes;
            /* Send that many bytes over SPI */
            retVal &= extflash_write_one(num_bytes_to_send, curr_addr, buf, buffer_offset, block);

            retVal &= extflash_read_status_reg(&dummy, block);
        }
    }
    else /* non-blocking flash memory code not implemented! */
    {
        retVal = false;
    }

    return retVal;

}


