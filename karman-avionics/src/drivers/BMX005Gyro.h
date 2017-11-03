/*
 * BMX005Gyro.h
 *
 * Created: 2/14/2017 10:32 PM
 *  Author: Alex Mendola
 */ 

#ifndef BMX005GYRO_H
#define BMX005GYRO_H

#include "Spi_service.h"
#include "bmg160.h"  // Driver for bmg160 gyroscope sensor within BMX005


#define GYRO_SPI_BUFF_SIZE (48)
#define GYRO_SEND_READ_CODE (0x82)

//--Init codes--
#define GYRO_POWER_MODE (0x11)
#define GYRO_SELF_TEST (0x3C)
#define GYRO_SLOW_OFFSET (0x31)
#define GYRO_OFFSET_VALUES (0x36) /* 0x36 to 0x39 */
//--------------

#define BYTES_PER_AXIS_SAMPLE (2)
void bmx500Gyro_init(spi_master_t *spi_master);

void bmx500Read_data(void);

//+++++++++++++++++++++++++++++++
typedef struct gyroscope_cal_s
{
   // uint16_t sens;          /* pressure sensitivity */
    //uint16_t offset;        /* pressure offset */
    //uint16_t tcs;           /* temperature coefficient of pressure sensitivity */
    //uint16_t tco;           /* temp coefficient of pressure offset */
    //uint16_t t_ref;         /* reference temperature */
    //uint16_t temp_sens;     /* temperature coefficient of temperature */
} BMX005_gyro_cal_t;

typedef struct gyroscope_control_s
{
    spi_master_t        *spi_master;
    chip_select_info_t  cs_info;
    volatile uint8_t   spi_send_buffer[GYRO_SPI_BUFF_SIZE];
    volatile uint8_t   spi_recv_buffer[GYRO_SPI_BUFF_SIZE];
    volatile Bool       send_complete;
    gyro_init_vals_t    init_vals;
    gyro_data_raw_t     raw_data;
    gyro_state_t        data_state;
} gyroscope_control_t;

typedef struct gyro_init_vals_s
{

} gyro_init_vals_t;


typedef struct gyro_data_raw_s
{
    int16_t x_rot_data;     /* raw x rotation rate value */
    int16_t y_rot_data;     /* raw y rotation rate value */
    int16_t z_rot_data;     /* raw z rotation rate value */
} gyro_data_raw_t;

// If conversion is needed
    /*typedef struct altimeter_data_s
    {
        int32_t temp;
        int32_t pressure;
    } ms5607_02ba03_data_t;
    */


typedef enum
{
    ENQUEUE_GYRO_XYZ_READ,
    WAIT_XYZ_READ,
    XYZ_DATA_CONVERT
} gyro_state_t;



void ms5607_02ba03_init(spi_master_t *spi_master);

void gyro_get_data(void);

void ms5607_02ba03_reset(void);

/* 128 bits of calibration */
void ms5607_02ba03_read_prom(void);

void ms5607_02ba03_d1_convert(void);

void ms5607_02ba03_d2_convert(void);

/* 24 bits pressure/temperature */
void ms5607_02ba03_read_data(void);

void ms5607_02ba03_calculate_temp(void);

#endif /* MS5607-02BA03_H_ */