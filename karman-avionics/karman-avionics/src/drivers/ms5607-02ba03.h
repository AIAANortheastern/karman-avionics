/*
 * ms5607_02ba03.h
 *
 * Created: 2/1/2017 7:24:32 PM
 *  Author: Andrew Kaster
 */ 


#ifndef MS5607_02BA03_H_
#define MS5607_02BA03_H_

#include "Spi_service.h"

#define ALTIMETER_SPI_BUFF_SIZE (12)

typedef struct altimeter_cal_s
{
    uint16_t sens;          /* pressure sensitivity */
    uint16_t offset;        /* pressure offset */
    uint16_t tcs;           /* temperature coefficient of pressure sentivity */
    uint16_t tco;           /* temp coefficient of pressure offset */
    uint16_t t_ref;         /* reference temperature */
    uint16_t temp_sens;     /* temperature coefficient of temperature */
} ms5607_02ba03_cal_t;

typedef struct altimeter_raw_s
{
    uint32_t dig_press;     /* d1 */
    uint32_t dig_temp;      /* d2 */
    uint32_t t_diff;        /* dt */
} ms5607_02ba03_raw_t;

typedef struct altimeter_data_s
{
    int32_t temp;
    int32_t pressure;
} ms5607_02ba03_data_t;

typedef struct altimeter_control_s
{
    spi_master_t        *spi_master;
    chip_select_info_t  cs_info;
    volatile uint8_t    spi_send_buffer[ALTIMETER_SPI_BUFF_SIZE];
    volatile uint8_t    spi_recv_buffer[ALTIMETER_SPI_BUFF_SIZE];
    volatile Bool       send_complete;
    ms5607_02ba03_cal_t calibration_vals;
    ms5607_02ba03_raw_t raw_vals;
    ms5607_02ba03_data_t final_vals;
} ms5607_02ba03_control_t;


void ms5607_02ba03_init(spi_master_t *spi_master);

void ms5607_02ba03_reset(void);

/* 128 bits of calibration */
void ms5607_02ba03_read_prom(void);

void ms5607_02ba03_d1_convert(void);

void ms5607_02ba03_d2_convert(void);

/* 24 bits pressure/temperature */
void ms5607_02ba03_read_data(void);


#endif /* MS5607-02BA03_H_ */