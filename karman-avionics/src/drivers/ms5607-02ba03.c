/**
 * @file ms5607_02ba03.c
 *
 * Created: 2/1/2017 7:50:33 PM
 *  Author: Andrew Kaster
 *
 * This driver corresponds to Barometric Pressure Sensor, with stainless
 * steel cap. 
 * The barometer is capable of measuring altitude with a resolution of 
 * 20 cm. It uses I2c as its communication interface.
 */ 
 #include "conf_board.h"
 #include "ms5607-02ba03.h"
 #include "Timer.h"
 #include <string.h>

 #define ALTIMETER_RESET            (0x1E)
 #define ALTIMETER_CONVERT_D1_256   (0x40)
 #define ALTIMETER_CONVERT_D1_512   (0x42)
 #define ALTIMETER_CONVERT_D1_1024  (0x44)
 #define ALTIMETER_CONVERT_D1_2048  (0x46)
 #define ALTIMETER_CONVERT_D1_4096  (0x48)
 #define ALTIMETER_CONVERT_D2_256   (0x50)
 #define ALTIMETER_CONVERT_D2_512   (0x52)
 #define ALTIMETER_CONVERT_D2_1024  (0x54)
 #define ALTIMETER_CONVERT_D2_2048  (0x56)
 #define ALTIMETER_CONVERT_D2_4096  (0x58)
 #define ALTIMETER_CONVERT_D1       (ALTIMETER_CONVERT_D1_4096)
 #define ALTIMETER_CONVERT_D2       (ALTIMETER_CONVERT_D2_4096)
 #define ALTIMETER_PROM_BASE        (0xA0) /* 0xA0 to 0xAE */
 #define ALTIMETER_ADC_READ         (0x00)
 #define ALTIMETER_NUM_CAL          (6)

 /* File scope global variable */
 ms5607_02ba03_control_t gAltimeterControl;

/** 
* ms5607_02ba03_init(spi_master_t *spi_master)
* spi_master is a reference to the SPI controller for the entire program
* This is the initializer for the driver. 
* It initializes buffers for SPI, prepares ports, and initializes the hardware
*/
void ms5607_02ba03_init(spi_master_t *spi_master)
{
    gAltimeterControl.cs_info.csPort = &ALTIMETER_PORT;
    gAltimeterControl.cs_info.pinBitMask = ALTIMETER_CS;
    gAltimeterControl.spi_master = spi_master;
    memset((void *)gAltimeterControl.spi_recv_buffer, 0, sizeof(gAltimeterControl.spi_recv_buffer));
    memset((void *)gAltimeterControl.spi_send_buffer, 0, sizeof(gAltimeterControl.spi_send_buffer));
    gAltimeterControl.send_complete = false;

    /* Set chip select pin high. */
    gAltimeterControl.cs_info.csPort->DIRSET = gAltimeterControl.cs_info.pinBitMask;
    gAltimeterControl.cs_info.csPort->OUTSET = gAltimeterControl.cs_info.pinBitMask;

    memset((void *)(&(gAltimeterControl.raw_vals)), 0, sizeof(gAltimeterControl.raw_vals));
    memset((void *)(&(gAltimeterControl.final_vals)), 0, sizeof(gAltimeterControl.final_vals));

    /* Call initial functions to prepare altimeter. */
    ms5607_02ba03_reset();
    timer_delay_ms(3); /* Delay 3 ms to allow for reset */
    ms5607_02ba03_read_prom();
}


/**  
* BLOCKING reset function
* As in it waits for the reset to return and halts all other processing until
* The reset is complete.
*/
 void ms5607_02ba03_reset(void)
 {
    gAltimeterControl.spi_send_buffer[0] = ALTIMETER_RESET;

    /* Send BLOCKING request as this is done during initialization */
    spi_master_blocking_send_request(gAltimeterControl.spi_master,
                       &(gAltimeterControl.cs_info),
                       gAltimeterControl.spi_send_buffer,
                       1,
                       gAltimeterControl.spi_recv_buffer,
                       0,
                       &(gAltimeterControl.send_complete));
 }

 /** 
  * Function ms5607_02ba03_read_prom
  * This function reads directly from the PROM memory
  * Where PROM is Programmable Read Only Memory
  * 
  * It reads 6 chunks of 16 bits of data
  * 
  * 128 bits of calibration data? No, only 6*16 bits 
  * 
  */
 void ms5607_02ba03_read_prom(void)
 {
    /* Grab addresses 1 through 6 of PROM (datasheet page 11) */
    /* Each one is 16 bits. */
    memset((void *)gAltimeterControl.spi_send_buffer, 0, sizeof(gAltimeterControl.spi_send_buffer));
    gAltimeterControl.spi_send_buffer[0] = ALTIMETER_PROM_BASE + 0x2; /* 1st cal value*/
    gAltimeterControl.spi_send_buffer[3] = ALTIMETER_PROM_BASE + 0x4;
    gAltimeterControl.spi_send_buffer[6] = ALTIMETER_PROM_BASE + 0x6;
    gAltimeterControl.spi_send_buffer[9] = ALTIMETER_PROM_BASE + 0x8;
    gAltimeterControl.spi_send_buffer[12] = ALTIMETER_PROM_BASE + 0xA;
    gAltimeterControl.spi_send_buffer[15] = ALTIMETER_PROM_BASE + 0xC;

    /* Send BLOCKING request as this is done during initialization */
    spi_master_blocking_send_request(gAltimeterControl.spi_master,
                                    &(gAltimeterControl.cs_info),
                                    gAltimeterControl.spi_send_buffer,
                                    3 * ALTIMETER_NUM_CAL,
                                    gAltimeterControl.spi_recv_buffer,
                                    3 * ALTIMETER_NUM_CAL,
                                    &(gAltimeterControl.send_complete));

	/* The above code puts the prom into read only mode. When it is done the spi_recv_buffer
	 * Should be written to allowing us to assign specific variables to the parts of that data*/
    gAltimeterControl.calibration_vals.sens = *(uint16_t *)(&(gAltimeterControl.spi_recv_buffer[1]));
    gAltimeterControl.calibration_vals.offset = *(uint16_t *)(&(gAltimeterControl.spi_recv_buffer[4]));
    gAltimeterControl.calibration_vals.tcs = *(uint16_t *)(&(gAltimeterControl.spi_recv_buffer[7]));
    gAltimeterControl.calibration_vals.tco = *(uint16_t *)(&(gAltimeterControl.spi_recv_buffer[10]));
    gAltimeterControl.calibration_vals.t_ref = *(uint16_t *)(&(gAltimeterControl.spi_recv_buffer[13]));
    gAltimeterControl.calibration_vals.temp_sens = *(uint16_t *)(&(gAltimeterControl.spi_recv_buffer[16]));
 }

 /** 
  * Get the pressure value and convert it to reasonable units.
  * After calling this you have to wait 8.2 ms between conversion and ADC read 
  */
 void ms5607_02ba03_d1_convert(void)
 {
    memset((void *)gAltimeterControl.spi_send_buffer, 0, sizeof(gAltimeterControl.spi_send_buffer));
    
    gAltimeterControl.spi_send_buffer[0] = ALTIMETER_CONVERT_D1;

    spi_master_enqueue(gAltimeterControl.spi_master,
    &gAltimeterControl.cs_info,
    gAltimeterControl.spi_send_buffer,
    1,
    gAltimeterControl.spi_recv_buffer,
    0,
    &gAltimeterControl.send_complete);
 }

/** 
 *This gets the temperature value and converts it into reasonable units
 *It partially relies on the pressure value and must be called after the d1 convert is called
 *
 */
 void ms5607_02ba03_d2_convert(void)
 {
     memset((void *)gAltimeterControl.spi_send_buffer, 0, sizeof(gAltimeterControl.spi_send_buffer));
     
     gAltimeterControl.spi_send_buffer[0] = ALTIMETER_CONVERT_D2;

     spi_master_enqueue(gAltimeterControl.spi_master,
     &gAltimeterControl.cs_info,
     gAltimeterControl.spi_send_buffer,
     1,
     gAltimeterControl.spi_recv_buffer,
     0,
     &gAltimeterControl.send_complete);
 }

 /** 
  * ms5607_02ba03_read_data 
  * This gets the pressure and temp data and puts it into the 24 bit buffer 
  * read from below
  * 24 bits pressure/temperature NOTE: Always convert d1 and d2 first 
  */
 void ms5607_02ba03_read_data(void)
 {
        memset((void *)gAltimeterControl.spi_send_buffer, 0, sizeof(gAltimeterControl.spi_send_buffer));
        
        gAltimeterControl.spi_send_buffer[0] = ALTIMETER_ADC_READ;

        spi_master_enqueue(gAltimeterControl.spi_master,
        &gAltimeterControl.cs_info,
        gAltimeterControl.spi_send_buffer,
        1,
        gAltimeterControl.spi_recv_buffer,
        4,
        &gAltimeterControl.send_complete);
 }

/** 
 * This is what is called to actually pull the data out of the buffer the data is pushed to
 */
 static inline uint32_t get_data_from_buffer24(volatile uint8_t *buff)
 {
    return (uint32_t)(((uint32_t)buff[1] << 16) | ((uint32_t)buff[2] << 8) | (uint32_t)buff[3]);
 }

/** 
 * ms5607_02ba03_get_data
 * This function is called repeatedly from SensorTask.c in tasks 
 * When this is called it does its current states job and increments 
 * this objects state to the next value. 
 * Eventually this will return successful and the data will be pulled out of the
 * appropriate global buffers.
 */
 sensor_status_t ms5607_02ba03_get_data(void)
 {
    /* 1. Enqueue D1 convert command */
    /* 2. Wait for that to finish */
    /* 3. Wait additional 8.2ms for conversion */
    /* 4. Do adc read to get D1 */
    /* 5. Enqueue D2 convert command */
    /* 6. Wait for that to finish */
    /* 7. Wait additional 8.2ms for conversion */
    /* 8. Do adc read to get D2 */
    /* 9. Calculate new temperature and pressure */

    sensor_status_t returnStatus = SENSOR_BUSY;

    switch(gAltimeterControl.get_data_state)
    {
        case ENQUEUE_D1_CONVERT:
            ms5607_02ba03_d1_convert();
            gAltimeterControl.get_data_state = WAIT_D1_CONVERT;
            break;        
        case WAIT_D1_CONVERT:
            if(true == gAltimeterControl.send_complete)
            {
                /* Record time when we started conversion */
                gAltimeterControl.time_start = get_timer_count();
                gAltimeterControl.get_data_state = WAIT_8ms_D1;
            }
            break;
        case WAIT_8ms_D1:
            /* wait 8ms */
            /* if 8ms done */
            if(get_timer_count() - gAltimeterControl.time_start >= EIGHT_MS)
            {
                ms5607_02ba03_get_data();
                gAltimeterControl.get_data_state = WAIT_D1_READ;
                returnStatus = SENSOR_WAITING;
            }
            break;
        case WAIT_D1_READ:
            if(true == gAltimeterControl.send_complete)
            {
                gAltimeterControl.raw_vals.dig_press = get_data_from_buffer24(gAltimeterControl.spi_recv_buffer);
                gAltimeterControl.get_data_state = ENQUEUE_D2_CONVERT;
            }
            break;
        case ENQUEUE_D2_CONVERT:
            ms5607_02ba03_d2_convert();
            gAltimeterControl.get_data_state = WAIT_D2_CONVERT;
            break;
        case WAIT_D2_CONVERT:
            if(true == gAltimeterControl.send_complete)
            {
                /* Record time when we started conversion */
                gAltimeterControl.time_start = get_timer_count();
                gAltimeterControl.get_data_state = WAIT_8ms_D2;
            }
            break;
        case WAIT_8ms_D2:
            /* wait 8ms */
            /* if 8ms done */
            if(get_timer_count() - gAltimeterControl.time_start >= EIGHT_MS)
            {
                ms5607_02ba03_get_data();
                gAltimeterControl.get_data_state = WAIT_D2_READ;
                returnStatus = SENSOR_WAITING;
            }
            break;
        case WAIT_D2_READ:
            if(true == gAltimeterControl.send_complete)
            {
                gAltimeterControl.raw_vals.dig_temp = get_data_from_buffer24(gAltimeterControl.spi_recv_buffer);
                /* Do math */
                ms5607_02ba03_calculate_temp();
                ms5607_02ba03_calculate_press();
                gAltimeterControl.get_data_state = ENQUEUE_D1_CONVERT;
                returnStatus = SENSOR_COMPLETE;
            }
            break;
    }
    return returnStatus;
 }

/** 
 * This does some fun math to figure out the temp.
 * If you want details I suggest seeing the driver doc 
 *
 */
void ms5607_02ba03_calculate_temp(void)
{    
    /* dT = D2 - TREF = D2 - C5 * 2^8 */
    gAltimeterControl.raw_vals.t_diff = (int32_t)(gAltimeterControl.raw_vals.dig_temp - ((uint32_t)gAltimeterControl.calibration_vals.t_ref << 8));

    /* TEMP =20°C +dT* TEMPSENS =2000 + dT * C6 / 2^23 */
    gAltimeterControl.final_vals.temp = (int32_t)(2000 + ((gAltimeterControl.raw_vals.t_diff * (uint32_t)gAltimeterControl.calibration_vals.temp_sens) >> 23));
}

/** 
 * This does some fun math to figure out the pressure.
 * If you want details I suggest seeing the driver doc 
 *
 */
void ms5607_02ba03_calculate_press(void)
{
    int64_t press_offs, press_sens;

    /* OFF = OFFT1 +TCO* dT = C2 * 2^17 +(C4 *dT )/2^6 */
    press_offs = (int64_t)(((int64_t)gAltimeterControl.calibration_vals.offset << 17) +
               (((int64_t)gAltimeterControl.calibration_vals.tco * (int64_t)gAltimeterControl.raw_vals.t_diff) >> 6));

    /* SENS = SENST1 + TCS* dT= C1 * 2^16 + (C3 * dT ) / 2^7 */
    press_sens = (int64_t)(((int64_t)gAltimeterControl.calibration_vals.sens << 16) + 
                (((int64_t)gAltimeterControl.calibration_vals.tcs * (int64_t)gAltimeterControl.raw_vals.t_diff) >> 7));

    /* P = D1 * SENS - OFF = (D1 * SENS / 2^21 - OFF) / 2^15  */
    gAltimeterControl.final_vals.pressure = (int32_t)( ( ( ((int64_t)gAltimeterControl.raw_vals.dig_press * press_sens) >> 21) - press_offs) >> 15 );

}

