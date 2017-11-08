/*
 * BMX055Mag.h
 *
 * Created: 10/26/2017 8:04:49 PM
 *  Author: Adam Mirza
 */ 


#ifndef BMX055MAG_H_
#define BMX055MAG_H_


#include "Spi_service.h"
#include "SensorTask.h"

/** Holds the current SPI input/output */
#define MAGNETOMETER_SPI_BUFF_SIZE (8)

/********************************************/
/**\name	CHIP ID       */
/********************************************/
/* Fixed Data Registers */
#define BMM050_CHIP_ID                     (0x40)
/********************************************/
/**\name	DATA REGISTERS       */
/********************************************/
/* Data Registers*/
#define BMM050_DATA_X_LSB                   (0x42)
#define BMM050_DATA_X_MSB                   (0x43)
#define BMM050_DATA_Y_LSB                   (0x44)
#define BMM050_DATA_Y_MSB                   (0x45)
#define BMM050_DATA_Z_LSB                   (0x46)
#define BMM050_DATA_Z_MSB                   (0x47)
#define BMM050_DATA_R_LSB                   (0x48)
#define BMM050_DATA_R_MSB                   (0x49)
/********************************************/
/**\name	REMAPPED DATA REGISTERS      */
/********************************************/
/* Data Registers for remapped axis(XandY)
 * this only applicable for BMX055 */
#define BMM050_REMAPPED_BMX055_DATA_Y_LSB      (0x42)
#define BMM050_REMAPPED_BMX055_DATA_Y_MSB      (0x43)
#define BMM050_REMAPPED_BMX055_DATA_X_LSB      (0x44)
#define BMM050_REMAPPED_BMX055_DATA_X_MSB      (0x45)
/********************************************/
/**\name	INTERRUPT STATUS      */
/********************************************/
/* Status Registers */
#define BMM050_INT_STAT_REG                    (0x4A)
/********************************************/
/**\name	POWER MODE DEFINITIONS      */
/********************************************/
/* Control Registers */
#define BMM050_POWER_CONTROL               (0x4B)
#define BMM050_CONTROL                     (0x4C)
#define BMM050_INT_CONTROL                 (0x4D)
#define BMM050_SENS_CONTROL                (0x4E)
#define BMM050_LOW_THRES                   (0x4F)
#define BMM050_HIGH_THRES                  (0x50)
/********************************************/
/**\name XY AND Z REPETITIONS DEFINITIONS  */
/********************************************/
#define BMM050_REP_XY                      (0x51)
#define BMM050_REP_Z                       (0x52)
/********************************************/

/* Constant Definitions */

#define BMX055_REPETITIONS_XY_MAX(0xFF)
#define BMX055_REPETITIONS_Z_MAX(0xFF)

#define BMX055_DATA_RATE_10HZ(0x00 << 3)
#define BMX055_DATA_RATE_2HZ(0x01 << 3)
#define BMX055_DATA_RATE_6HZ(0x02 << 3)
#define BMX055_DATA_RATE_8HZ(0x03 << 3)
#define BMX055_DATA_RATE_15HZ(0x04 << 3)
#define BMX055_DATA_RATE_20HZ(0x05 << 3)
#define BMX055_DATA_RATE_25HZ(0x06 << 3)
#define BMX055_DATA_RATE_30HZ(0x07 << 3)

#define BMX055_OPERATION_MODE_NORMAL(0x00 << 1)
#define BMX055_OPERATION_MODE_FORCED(0x01 << 1)
#define BMX055_OPERATION_MODE_SLEEP(0x03 << 1)

#define BMX055_WRITE(0x00)
#define BMX055_READ(1 << 7)

/** Final data */
typedef struct magnotometer_data_s
{
	int16_t x;           /**x data*/
	int16_t y;       /**y data */
	int16_t z;		/* z data*/
	int16_t rhall;	/* hall resistance*/
} bmx055_mag_data_t;

typedef enum
{
	ENQUEUE, /**< Send SPI Request */
	XYZH_DATA_READ        /**< Convert the data and return the final result */
} bmx055_mag_state_t;

typedef enum
{
	
	BMX055_POWER_MODE_OFF,
	BMX055_POWER_MODE_ON
	
} bmx055_mag_power_mode_t;

typedef struct magnetometer_control_s
{
	spi_master_t        *spi_master; /**< Pointer to the task's SPI Master */
	chip_select_info_t  cs_info;     /**< Chip select info for the device */
	volatile uint8_t    spi_send_buffer[MAGNETOMETER_SPI_BUFF_SIZE]; /**< Data to be sent out */
	volatile uint8_t    spi_recv_buffer[MAGNETOMETER_SPI_BUFF_SIZE]; /**< Data received by the device */
	volatile Bool       send_complete; /**< Boolean to know when transaction complete */
	//bmx055_mag_cal_t calibration_vals; /**< PROM calibration values */
	//bmm055_mag_raw_t raw_vals;         /**< Raw ADC Values */
	bmx055_mag_data_t final_vals;      /**< Usable values */
	bmx055_mag_state_t get_data_state; /**< state machine */
	uint32_t            time_start;       /**< For keeping track of time */
} bmx055_mag_control_t;



#endif /* BMX055MAG_H_ */