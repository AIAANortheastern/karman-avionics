/*
 * ms5607_02ba03.c
 *
 * Created: 2/1/2017 7:50:33 PM
 *  Author: Andrew Kaster
 */ 

 #include "ms5607-02ba03.h"
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
 #define ALTIMETER_CONVERT_D1       (ALTIMETER_CONVERT_D1_2048)
 #define ALTIMETER_CONVERT_D2       (ALTIMETER_CONVERT_D2_2048)
 #define ALTIMETER_PROM_BASE        (0xA0) /* 0xA0 to 0xAE */
 #define ALTIMETER_NUM_CAL          (6)

 #define ALTIMETER_CS_PORT (PORTA)
 #define ALTIMETER_CS_BM   (1 << 6)
 /* File scope global variable */
 ms5607_02ba03_control_t gAltimeterControl;

void ms5607_02ba03_init(spi_master_t *spi_master)
{
    gAltimeterControl.cs_info.csPort = &ALTIMETER_CS_PORT;
    gAltimeterControl.cs_info.pinBitMask = ALTIMETER_CS_BM;
    gAltimeterControl.spi_master = spi_master;
    memset((void *)gAltimeterControl.spi_recv_buffer, 0, sizeof(gAltimeterControl.spi_recv_buffer));
    memset((void *)gAltimeterControl.spi_send_buffer, 0, sizeof(gAltimeterControl.spi_send_buffer));
    gAltimeterControl.send_complete = false;

    gAltimeterControl.cs_info.csPort->DIRSET = gAltimeterControl.cs_info.pinBitMask;

    memset((void *)(&(gAltimeterControl.raw_vals)), 0, sizeof(gAltimeterControl.raw_vals));
    memset((void *)(&(gAltimeterControl.final_vals)), 0, sizeof(gAltimeterControl.final_vals));

    /* Call inital functions to prepare altimeter. */
    ms5607_02ba03_reset();
    ms5607_02ba03_read_prom();
}

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

 /* 128 bits of calibration data? No, only 6*16 bits */
 void ms5607_02ba03_read_prom(void)
 {
    /* Grab addresses 1 through 6 of PROM (datasheet page 11) */
    /* Each one is 16 bits. */
    gAltimeterControl.spi_send_buffer[0] = ALTIMETER_PROM_BASE + 0x2;
    gAltimeterControl.spi_send_buffer[1] = ALTIMETER_PROM_BASE + 0x4;
    gAltimeterControl.spi_send_buffer[2] = ALTIMETER_PROM_BASE + 0x6;
    gAltimeterControl.spi_send_buffer[3] = ALTIMETER_PROM_BASE + 0x8;
    gAltimeterControl.spi_send_buffer[4] = ALTIMETER_PROM_BASE + 0xA;
    gAltimeterControl.spi_send_buffer[5] = ALTIMETER_PROM_BASE + 0xC;

    /* Send BLOCKING request as this is done during initialization */
    spi_master_blocking_send_request(gAltimeterControl.spi_master,
                                    &(gAltimeterControl.cs_info),
                                    gAltimeterControl.spi_send_buffer,
                                    ALTIMETER_NUM_CAL,
                                    gAltimeterControl.spi_recv_buffer,
                                    2 * ALTIMETER_NUM_CAL,
                                    &(gAltimeterControl.send_complete));

    gAltimeterControl.calibration_vals.sens = *(uint16_t *)(&(gAltimeterControl.spi_recv_buffer[0]));
    gAltimeterControl.calibration_vals.offset = *(uint16_t *)(&(gAltimeterControl.spi_recv_buffer[2]));
    gAltimeterControl.calibration_vals.tcs = *(uint16_t *)(&(gAltimeterControl.spi_recv_buffer[4]));
    gAltimeterControl.calibration_vals.tco = *(uint16_t *)(&(gAltimeterControl.spi_recv_buffer[6]));
    gAltimeterControl.calibration_vals.t_ref = *(uint16_t *)(&(gAltimeterControl.spi_recv_buffer[8]));
    gAltimeterControl.calibration_vals.temp_sens = *(uint16_t *)(&(gAltimeterControl.spi_recv_buffer[10]));
 }

 void ms5607_02ba03_d1_convert(void)
 {

 }

 void ms5607_02ba03_d2_convert(void)
 {

 }

 /* 24 bits pressure/temperature NOTE: Always convert d1 and d2 first */
 void ms5607_02ba03_read_data(void)
 {

 }