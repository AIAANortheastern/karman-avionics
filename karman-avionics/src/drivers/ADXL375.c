 /**
 * @file ADXL375.c
 *
 * Created: 10/10/2017 8:41 PM
 * Author: Douglas Schonholtz
 *
 * @brief High G Accelerometer Driver
 *
 * Driver for the ADXL375 High G Accelerometer module.
 */ 
 
 #include "ADXL375.h"
 
 #define  HIGH_G_ACCELEROMETER_READ_DATA (0x32) /* Read register address */
 /* high g accel reset address */
 #define  HIGH_G_ACCELEROMETER_READ_BIT(0xC0)		/* Read/Write bit for spi read request */
 #define  HIGH_G_ACCELEROMETER_WRITE_BIT(0x00)		/* Read/Write bit for spi write request */
 
 
 
 /* global High G Accelerometer control structure */
 ADXL375_control_t gHighGAccelerometer;

/**
 * @brief Initialize the High G Accelerometer
 *
 * sets default values in global struct, then resets the High G Accelerometer
 * 
 */
void ADXL375_init(spi_master_t *spi_master) {
	
	/* assign the spi_master to master struct for high g accelerometer */
	gHighGAccelerometer.cs_info.csPort = &HIGHG_ACC1_PORT;
	gHighGAccelerometer.cs_info.pinBitMask = HIGHG_ACC1_CS;
	gHighGAccelerometer.spi_master = spi_master;
	
	/* reset values to defaults */
	memset((void *)gHighGAccelerometer.spi_send_buffer, 0, sizeof(gHighGAccelerometer.spi_send_buffer));
	memset((void *)gHighGAccelerometer.spi_recv_buffer, 0, sizeof(gHighGAccelerometer.spi_recv_buffer));
	gHighGAccelerometer.send_complete = false;
	memset((void *)(&(gHighGAccelerometer).raw_vals), 0, sizeof(gHighGAccelerometer.raw_vals));
	memset((void *)(&(gHighGAccelerometer).final_vals), 0, sizeof(gHighGAccelerometer.final_vals));
	memset((void *)(&(gHighGAccelerometer).offset_vals), 0, sizeof(gHighGAccelerometer.offset_vals));
	
	/* call initialize functions to prepare accelerometer (probably set offsets and add self test) */
	ADXL375_reset();

}

/**
 * @brief State machine for High G Accelerometer
 * 
 * This function is called repeatedly from SensorTask.c in tasks
 * When this is called it does its current states job and increments
 * this objects state to the next value.
 * Eventually this will return successful and the data will be pulled out of the
 * appropriate global buffers.
 */
sensor_status_t ADXL375_get_data() {

	/** 
	 *	1. Enqueue read request
	 *	2. read and convert data
	 */

	sensor_status_t return_status;

	return_status = SENSOR_BUSY;

	switch(gHighGAccelerometer.get_data_state) {
		
		case ENQUEUE:
			enqueue_helper();
			gHighGAccelerometer.get_data_state = XYZ_DATA_CONVERT;
			return_status = SENSOR_WAITING;
			break;	
			
		case XYZ_DATA_CONVERT:		
			if(gHighGAccelerometer.send_complete){
				xyz_read_helper();
				convert_helper();
			
				gHighGAccelerometer.get_data_state = ENQUEUE;
				return_status = SENSOR_COMPLETE;			
			}
			break;
	}
	
	return return_status;					   

}

/** 
 * @brief gets offset values from High G Accelerometer
 * 
 * reads offsets, possibly sets them? 
 * we're not really sure what to do with these yet
 */
void ADXL375_get_offset_values() {
	
}

/** 
 * @brief reset function for High G Accelerometer
 * 
 * sends reset request to accelerometer
 */
void ADXL375_reset() {
	
}

////////////////////////////////
//HELPERS BELOW HERE
///////////////////////////////

/** 
 * @brief enqueues read request for High G Accelerometer data
 *
 * enqueues read request for x, y, & z accelerometer data
 */
void enqueue_helper() {
	/* clear buffers */
	memset((void *)gHighGAccelerometer.spi_recv_buffer, 0, sizeof(gHighGAccelerometer.spi_recv_buffer));
	memset((void *)gHighGAccelerometer.spi_send_buffer, 0, sizeof(gHighGAccelerometer.spi_send_buffer));
	
	/* set send buffer with read code which will be some constant defined in header */
	gHighGAccelerometer.spi_send_buffer[0] = HIGH_G_ACCELEROMETER_READ_BIT | HIGH_G_ACCELEROMETER_READ_DATA;
	
	/* spimaster enqueue read request */
	spi_master_enqueue(gHighGAccelerometer.spi_master,
	    &(gHighGAccelerometer.cs_info),
	    gHighGAccelerometer.spi_send_buffer,
	    1,
	    gHighGAccelerometer.spi_recv_buffer,
	    7,
	    &(gHighGAccelerometer.send_complete));
}

/** 
 * @brief moves x,y, & z raw High G Accelerometer data from buffer to control struct
 *
 * reads each byte of raw data into msb or lsb of global control struct raw x, y, or z data
 */
void xyz_read_helper() {
	/* has data  gHighGAccelerometer.spi_recv_buffer */
	/* we want to put data into */
	gHighGAccelerometer.raw_vals.dig_x = (uint16_t)(((uint16_t)gHighGAccelerometer.spi_recv_buffer[2] << 8) | ((uint16_t)gHighGAccelerometer.spi_recv_buffer[1]));
	gHighGAccelerometer.raw_vals.dig_y = (uint16_t)(((uint16_t)gHighGAccelerometer.spi_recv_buffer[4] << 8) | ((uint16_t)gHighGAccelerometer.spi_recv_buffer[3]));
	gHighGAccelerometer.raw_vals.dig_z = (uint16_t)(((uint16_t)gHighGAccelerometer.spi_recv_buffer[6] << 8) | ((uint16_t)gHighGAccelerometer.spi_recv_buffer[5]));
}

/** 
 * @brief converts raw High G Accelerometer data to final values
 * 
 * multiplies raw data by conversion factor to get actual accelerations
 * current factor is 2, actual is 1.96 fix this when board is updated and floating point is viable
 */
void convert_helper(){
	
	gHighGAccelerometer.final_vals.x = 2*gHighGAccelerometer.raw_vals.dig_x;
	gHighGAccelerometer.final_vals.y = 2*gHighGAccelerometer.raw_vals.dig_y;
	gHighGAccelerometer.final_vals.z = 2*gHighGAccelerometer.raw_vals.dig_z;
	
}


