/**
 * @file ms5607-02ba03.c
 *
 * Created: 2/1/2017 7:50:33 PM
 * Author: Andrew Kaster
 *
 * @brief Altimeter driver
 *
 * Description: This driver corresponds to Barometric Pressure Sensor, with stainless
 * steel cap. 
 *
 * The barometer is capable of measuring altitude with a resolution of 
 * 20 cm. It uses SPI as its communication interface.
 */
 #include "conf_board.h"
 #include "ms5607-02ba03.h"
 #include "Timer.h"
 #include <string.h>

 #define ALTIMETER_RESET            (0x1E) /**< Reset command */
 #define ALTIMETER_CONVERT_D1_256   (0x40) /**< D1 Conversion command with 8 bits of data */
 #define ALTIMETER_CONVERT_D1_512   (0x42) /**< D1 Conversion command with 9 bits of data */
 #define ALTIMETER_CONVERT_D1_1024  (0x44) /**< D1 Conversion command with 10 bits of data */
 #define ALTIMETER_CONVERT_D1_2048  (0x46) /**< D1 Conversion command with 11 bits of data */
 #define ALTIMETER_CONVERT_D1_4096  (0x48) /**< D1 Conversion command with 12 bits of data */
 #define ALTIMETER_CONVERT_D2_256   (0x50) /**< D2 Conversion command with 8 bits of data */
 #define ALTIMETER_CONVERT_D2_512   (0x52) /**< D2 Conversion command with 9 bits of data */
 #define ALTIMETER_CONVERT_D2_1024  (0x54) /**< D2 Conversion command with 10 bits of data */
 #define ALTIMETER_CONVERT_D2_2048  (0x56) /**< D2 Conversion command with 11 bits of data */
 #define ALTIMETER_CONVERT_D2_4096  (0x58) /**< D2 Conversion command with 12 bits of data */
 #define ALTIMETER_CONVERT_D1       (ALTIMETER_CONVERT_D1_2048) /**< Use most ADC bits */
 #define ALTIMETER_CONVERT_D2       (ALTIMETER_CONVERT_D2_2048) /**< Use most ADC bits */
 #define ALTIMETER_PROM_BASE        (0xA0) /**< PROM addresses are 0xA0 to 0xAE */
 #define ALTIMETER_ADC_READ         (0x00) /**< ADC Read command */
 #define ALTIMETER_NUM_CAL          (6)    /**< There are six calibration values */

 /** File scope global variable with control data for the altimeter */
 ms5607_02ba03_control_t gAltimeterControl;

/** 
* @brief Initialize the Altimeter.
*
* spi_master is a reference to the SPI controller for the sensor SPI Bus.
* This is the initializer for the altimeter driver. 
* It initializes buffers for SPI and other control data. Hardware initialization is done in init.c
* Also resets the altimeter and retrieves the configuration data.
*/
void ms5607_02ba03_init(spi_master_t *spi_master)
{
    gAltimeterControl.cs_info.csPort = &ALTIMETER_PORT;
    gAltimeterControl.cs_info.pinBitMask = ALTIMETER_CS;
    gAltimeterControl.spi_master = spi_master;
    memset((void *)gAltimeterControl.spi_recv_buffer, 0, sizeof(gAltimeterControl.spi_recv_buffer));
    memset((void *)gAltimeterControl.spi_send_buffer, 0, sizeof(gAltimeterControl.spi_send_buffer));
    gAltimeterControl.send_complete = false;

    memset((void *)(&(gAltimeterControl.raw_vals)), 0, sizeof(gAltimeterControl.raw_vals));
    memset((void *)(&(gAltimeterControl.final_vals)), 0, sizeof(gAltimeterControl.final_vals));

    /** Call initial functions to prepare altimeter. */
    ms5607_02ba03_reset();
    timer_delay_ms(3); /** Delay 3 ms to allow for reset */
    gAltimeterControl.cs_info.csPort->OUTSET = gAltimeterControl.cs_info.pinBitMask; /** Pull CS High to allow continued operation */
    ms5607_02ba03_read_prom();
}


/**  
* @brief BLOCKING reset function
*
* As in it waits for the reset to return and halts all other processing until
* the reset is complete.
*/
 void ms5607_02ba03_reset(void)
 {
    gAltimeterControl.spi_send_buffer[0] = ALTIMETER_RESET;

    /** Send BLOCKING request as this is done during initialization */
    /** Tell SPI to NOT pull CS high after transaction to allow for reset time. */
    spi_master_blocking_send_request(gAltimeterControl.spi_master,
                       &(gAltimeterControl.cs_info),
                       gAltimeterControl.spi_send_buffer,
                       1,
                       gAltimeterControl.spi_recv_buffer,
                       0,
                       &(gAltimeterControl.send_complete),
                       true);
 }

 /** 
  * @brief Read configuratin values from the Altimeter PROM
  *
  * Where PROM is Programmable Read Only Memory 
  * It reads 6 16 bit words of data.
  */
 void ms5607_02ba03_read_prom(void)
 {
    uint8_t i;
    uint16_t *calptr = (uint16_t *)&(gAltimeterControl.calibration_vals);
    /** Grab addresses 1 through 6 of PROM (datasheet page 11) */
    /** Each one is 16 bits. */
    memset((void *)gAltimeterControl.spi_send_buffer, 0, sizeof(gAltimeterControl.spi_send_buffer));

    for(i = ALTIMETER_PROM_BASE + 2; i < ALTIMETER_PROM_BASE + 0x0E; i = i+2)
    {
        gAltimeterControl.spi_send_buffer[0] = i;
        /** Send BLOCKING request as this is done during initialization */
        spi_master_blocking_send_request(gAltimeterControl.spi_master,
                                        &(gAltimeterControl.cs_info),
                                        gAltimeterControl.spi_send_buffer,
                                        1,
                                        gAltimeterControl.spi_recv_buffer,
                                        3,
                                        &(gAltimeterControl.send_complete),
                                        false);
        *calptr = ((uint16_t)gAltimeterControl.spi_recv_buffer[1] << 8) | gAltimeterControl.spi_recv_buffer[0];
        calptr++;
    }
 }

 /** 
  * @brief Get the pressure value and convert it to reasonable units.
  *
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
    &gAltimeterControl.send_complete,
    false);
 }

/** 
 * @brief Get the temperature value and converts it into reasonable units
 *
 * After calling this you have to wait 8.2 ms between conversion and ADC read 
 * It partially relies on the pressure value and must be called after the d1 convert is called
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
     &gAltimeterControl.send_complete,
     false);
 }

 /** 
  * @brief Enqueue SPI Request for an ADC read.
  *
  * This tells the SPI library to get the pressure and temp data and puts it
  * into the recieve buffer
  *
  * 24 bits for pressure/temperature. NOTE: Always convert d1 and d2 first 
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
        &gAltimeterControl.send_complete,
        false);
 }

/** 
 * @brief Helper function to re-order bytes to use as a 32 bit value.
 *
 *
 * This is what is called to actually pull the data out of the buffer the data is pushed to
 */
 static inline uint32_t get_data_from_buffer24(volatile uint8_t *buff)
 {
    return (uint32_t)(((uint32_t)buff[1] << 16) | ((uint32_t)buff[2] << 8) | (uint32_t)buff[3]);
 }

/**
 * @brief State machine for Altimeter
 * 
 * This function is called repeatedly from SensorTask.c in tasks 
 * When this is called it does its current states job and increments 
 * this objects state to the next value. 
 * Eventually this will return successful and the data will be pulled out of the
 * appropriate global buffers.
 */
 sensor_status_t ms5607_02ba03_run(void)
 {
    /** 1. Enqueue D1 convert command */
    /** 2. Wait for that to finish */
    /** 3. Wait additional 8.2ms for conversion */
    /** 4. Do adc read to get D1 */
    /** 5. Enqueue D2 convert command */
    /** 6. Wait for that to finish */
    /** 7. Wait additional 8.2ms for conversion */
    /** 8. Do adc read to get D2 */
    /** 9. Calculate new temperature and pressure */

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
            if(get_timer_count() - gAltimeterControl.time_start > EIGHT_MS)
            {
                ms5607_02ba03_read_data();
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
            if(get_timer_count() - gAltimeterControl.time_start > EIGHT_MS)
            {
                ms5607_02ba03_read_data();
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
 * @brief Convert raw adc temp value to sensible units. 
 * 
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
 * @brief Convert raw adc pressure value to sensible units. 
 *
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

/**
 * @brief Copies Altimeter data
 *
 * @param[out] out_data The caller's struct that data will be copied into
 */
void ms5607_02ba03_get_data(ms5607_02ba03_data_t *out_data)
{
    out_data->temp = gAltimeterControl.final_vals.temp;
    out_data->pressure = gAltimeterControl.final_vals.pressure;
}
