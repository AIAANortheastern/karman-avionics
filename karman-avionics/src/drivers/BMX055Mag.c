/*
 * BMX055Mag.c
 *
 * Created: 10/26/2017 8:04:39 PM
 *  Author: Adam Mirza
 */ 

void bmx055_mag_init(spi_master_t *spi_master)
{
	//assign the spi_master to master struct for high g accelerometer
	// clear buffers with memset
	
	// set power control bit to 1
	// set repetitions xy
	// set repetitions z
	// set data rate & set power mode (normal mode probably) (same register)
	
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