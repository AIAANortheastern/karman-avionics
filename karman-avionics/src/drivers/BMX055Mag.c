/*
 * BMX055Mag.c
 *
 * Created: 10/26/2017 8:04:39 PM
 *  Author: Adam Mirza
 */ 

#include "BMX055Mag.h"

bmx055_mag_control_t gMagnetometer;

// read-write 0 on write

void bmx055_mag_init(spi_master_t *spi_master)
{

	gMagnetometer.cs_info.csPort = &IMU_MAG1_PORT;
	gMagnetometer.cs_info.pinBitMask = &IMU_MAG1_CS;
	gMagnetometer.spi_master = spi_master;
	
	/* clear buffers */
	memset((void *)gMagnetometer.spi_send_buffer, 0, sizeof(gMagnetometer.spi_send_buffer));
	memset((void *)gMagnetometer.spi_recv_buffer, 0, sizeof(gMagnetometer.spi_recv_buffer));
	gMagnetometer.send_complete = false;
	
	memset((void *)gMagnetometer.final_vals, 0, sizeof(gMagnetometer.final_vals));
	
	/* set power control bit to 1  */
	set_power_mode(BMX055_POWER_MODE_ON);
	/* set repetitions xy */
	set_repetitions_xy(BMX055_REPETITIONS_XY_MAX);
	/* set repetitions z */
	set_repetitions_z(BMX055_REPETITIONS_Z_MAX);
	/* set data rate & set power mode to normal mode */
	set_control(BMX055_DATA_RATE_30HZ | BMX055_OPERATION_MODE_NORMAL);
	
}

// make three functions. one for set, one for verify, and one for write

void set_power_mode(bmx055_mag_power_mode_t powerMode)
{
	
	while() // write is complete
	{
		write_power_mode(powerMode);
	}
	
}

void write_power_mode(bmx055_mag_power_mode_t powerMode)
{
	
	gMagnetometer.spi_send_buffer[0] = BMX055_WRITE | BMM050_POWER_CONTROL;
	gMagnetometer.spi_send_buffer[1] = powerMode;
	
	spi_master_blocking_send_request(gMagnetometer.spi_master,
		&(gMagnetometer.cs_info),
		gMagnetometer.spi_send_buffer,
		2,
		gMagnetometer.spi_recv_buffer,
		0,
		&(gMagnetometer.send_complete));
	
}

void set_repetitions_xy(uint8_t repetitions)
{
	
	gMagnetometer.spi_send_buffer[0] = BMX055_WRITE | BMM050_REP_XY;
	gMagnetometer.spi_send_buffer[1] = repetitions;
	
	spi_master_blocking_send_request(gMagnetometer.spi_master,
		&(gMagnetometer.cs_info),
		gMagnetometer.spi_send_buffer,
		2,
		gMagnetometer.spi_recv_buffer,
		0,
		&(gMagnetometer.send_complete));
	
}

void set_repetitions_z(uint8_t repetitions)
{
	
	gMagnetometer.spi_send_buffer[0] = BMX055_WRITE | BMM050_REP_Z;
	gMagnetometer.spi_send_buffer[1] = repetitions;
	
	spi_master_blocking_send_request(gMagnetometer.spi_master,
		&(gMagnetometer.cs_info),
		gMagnetometer.spi_send_buffer,
		2,
		gMagnetometer.spi_recv_buffer,
		0,
		&(gMagnetometer.send_complete));
	
}

void set_control(uint8_t data)
{
	
	gMagnetometer.spi_send_buffer[0] = BMX055_WRITE | BMM050_CONTROL;
	gMagnetometer.spi_send_buffer[1] = data;
	
	spi_master_blocking_send_request(gMagnetometer.spi_master,
		&(gMagnetometer.cs_info),
		gMagnetometer.spi_send_buffer,
		2,
		gMagnetometer.spi_recv_buffer,
		0,
		&(gMagnetometer.send_complete));
	
}

void bmx055_mag_reset(void)
{
	
	// bit 8 or 2 of 0x4B
	
}


sensor_status_t bmx055_mag_get_data(void)
{
	
	// enqueue
	
	// data read
	
}