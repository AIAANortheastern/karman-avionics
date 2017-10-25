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

/** Data Entry in the flash memory */
typedef struct flashentry
{
    uint32_t timestamp; /**< Timestamp in ticks since start of the timer */
    sensor_data_t data; /**< All the sensor information that will be logged */
    uint16_t chksum;    /**< 8 bit checksum */
} flash_data_entry_t;

/** Size of the version string */
#define VERSION_SIZE (10)

/** Current version string. Change this to clear flash memory */
#define VERSION_STRING ("DEBUG-001")
/** Random hex value to check against memory corruption */
#define MAGIC_NUMBER   (0xCAFE)

/** Header structure stored at address 0x0000_0000 */
typedef struct  
{
    uint16_t magic;                 /**< Stores MAGIC_NUMBER */
    char version_str[VERSION_SIZE]; /**< Stores VERSION_STRING */
    uint16_t entry_size;            /**< The size of each data entry */
    uint16_t num_entries;           /**< Current number of entries */
} flash_data_hdr_t;

/**Address for num_entries */
#define FLASHMEM_ENTRIES_ADDR (0x0000000EL)

/** Control structure for the flash memory */
typedef struct  
{
    flash_data_hdr_t header; /**< Local storage of the header */
    uint32_t data_addr;      /**< The current data address to write to next */
} flashmem_ctrl_t;

/** Status of reading/parsing the data header */
typedef enum
{
    HDR_INVALID,   /**< Header fields don't match hard coded values */
    HDR_ZERO,      /**< The flash memory is empty */
    HDR_VALID,     /**< Header is ok */
    HDR_READFAIL,  /**< Unable to read from flash memory */
} flashmem_hdrstatus_t;

/** 
 * @brief Determine the validity of the flash memory data header
 *
 * @param header The header to verify
 * @return flashmem_hdrstatus_t indicating success/failure
 */
flashmem_hdrstatus_t flashmem_verify_header(flash_data_hdr_t *header);

/**
 * @brief Write a data entry to the flash memory
 * 
 * @param entry Pointer to the data entry to write
 * @returns True on failure, false on success
 */
Bool flashmem_write_entry(flash_data_entry_t *entry);

/**
 * @brief Read the entry addressed by index
 *
 * @param[out] entry Pointer to write the entry to
 * @param index Data entry to read
 * @return True on failure, false on success
 *
 * Read the entry addressed by index and put it in the caller's struct.
 */
Bool flashmem_read_entry(flash_data_entry_t *entry, uint32_t index);

/** @brief Write the header to flash
 *  @return True on failure, false on success
 * 
 * Write the data header stored in gFlashmemCtrl.header to the flash memory 
 */
Bool flashmem_write_header(void);

/**
 * @brief Initialize the flash memory module 
 */
void init_flashmem(void);

/** Flash memory control data */
extern flashmem_ctrl_t gFlashmemCtrl;


#endif /* FLASHMEM_H_ */
