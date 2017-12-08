/*
 * BMX055Mag.c
 *
 * Created: 10/26/2017 8:04:39 PM
 *  Author: Adam Mirza
 */ 

#include "BMX055Mag.h"
#include "Timer.h"
/* hash defines located in above header. We tried something new this time */

/* initialize global magnetometer control structure */
bmx055_mag_control_t gMagnetometer;

/**
 * @brief Initialize the Magnetometer
 *
 * resets buffers and control data
 * sends configuration data to Magnetometer, and sets it into standard operation mode
 */
void bmx055_mag_init(spi_master_t *spi_master)
{

	gMagnetometer.cs_info.csPort = &IMU_MAG1_PORT;
	gMagnetometer.cs_info.pinBitMask = IMU_MAG1_CS;
	gMagnetometer.spi_master = spi_master;
	
	/* clear buffers */
	memset((void *)gMagnetometer.spi_send_buffer, 0, sizeof(gMagnetometer.spi_send_buffer));
	memset((void *)gMagnetometer.spi_recv_buffer, 0, sizeof(gMagnetometer.spi_recv_buffer));
	gMagnetometer.send_complete = false;
	
	memset((void *)(&(gMagnetometer).final_vals), 0, sizeof(gMagnetometer.final_vals));
	
	
	/* set power control bit to 1  */
	set_init_value(BMM050_POWER_CONTROL, BMX055_POWER_MODE_ON);
    /* settling time */
    timer_delay_ms(3);
	/* set repetitions xy */
	set_init_value(BMM050_REP_XY, BMX055_REPETITIONS_XY_MAX);
	/* set repetitions z */
	set_init_value(BMM050_REP_Z, BMX055_REPETITIONS_Z_MAX);
	/* set data rate & set power mode to normal mode */
	set_init_value(BMM050_CONTROL, BMX055_DATA_RATE_30HZ | BMX055_OPERATION_MODE_NORMAL);

    /* debug */
    verify_init_write(BMM050_CHIP_ID, 0x32);
}

/** 
 * @brief writes the desired value to the selected register
 * 
 * @param reg address of the selected register
 * @param value desired value to be contained in the register
 *
 * writes data to a register until the data is confirmed to be written
 */
void set_init_value(uint8_t reg, uint8_t value)
{
	
	do
	{
		write_init_value(reg, value);
		timer_delay_ms(1);
		
	}while(!verify_init_write(reg, value));
	
}

/** 
 * @brief checks to see if the desired value is set in the selected register
 * 
 * @param reg address of the selected register
 * @param value desired value to be contained in the register
 *
 * checks the contents of a register and returns true if it matches the expected value
 */
Bool verify_init_write(uint8_t reg, uint8_t value)
{
	
	gMagnetometer.spi_send_buffer[0] = BMX055_READ | reg;
	
    spi_master_blocking_send_request(gMagnetometer.spi_master,
			&(gMagnetometer.cs_info),
			gMagnetometer.spi_send_buffer,
			1,
			gMagnetometer.spi_recv_buffer,
			2,
			&(gMagnetometer.send_complete));
		
		if (gMagnetometer.spi_recv_buffer[1] == value)
		{
			return true;
		} 
		else
		{
			return false;
		}
	
}

/** 
 * @brief writes the desired value to the selected register
 * 
 * @param reg address of the selected register
 * @param value desired value to be written to the register
 *
 * sends the request to write a value to the selected register
 */
void write_init_value(uint8_t reg, uint8_t value)
{
	
		/* set send buffer with write code, register address and value to be written */	
		gMagnetometer.spi_send_buffer[0] = BMX055_WRITE | reg;
		gMagnetometer.spi_send_buffer[1] = value;
		
		spi_master_blocking_send_request(gMagnetometer.spi_master,
			&(gMagnetometer.cs_info),
			gMagnetometer.spi_send_buffer,
			2,
			gMagnetometer.spi_recv_buffer,
			0,
			&(gMagnetometer.send_complete));
		
}

/**
 * @brief reset function for magnetometer
 *
 * sets the soft reset bit (bit 2) in the Power Control register to high, resetting the chip
 */
void bmx055_mag_reset(void)
{
	
	/* set soft reset bit to high in power control register */
	set_init_value(BMM050_POWER_CONTROL, BMX055_RESET);
	
}

static inline uint16_t read_helper_x(void)
{
	return (gMagnetometer.spi_recv_buffer[2] << 5)|(gMagnetometer.spi_recv_buffer[1] >> 3);
}

static inline uint16_t read_helper_y(void)
{
	return (gMagnetometer.spi_recv_buffer[4] << 5)|(gMagnetometer.spi_recv_buffer[3] >> 3);
}

static inline uint16_t read_helper_z(void)
{
	return (gMagnetometer.spi_recv_buffer[6] << 7)|(gMagnetometer.spi_recv_buffer[5] >> 1);
}

static inline uint16_t read_helper_rhall(void)
{
	return (gMagnetometer.spi_recv_buffer[8] << 6)|(gMagnetometer.spi_recv_buffer[7] >> 2);
}

void read_helper(void)
{
	gMagnetometer.final_vals.x = read_helper_x();
	gMagnetometer.final_vals.y = read_helper_y();
	gMagnetometer.final_vals.z = read_helper_z();
	gMagnetometer.final_vals.rhall = read_helper_rhall();
}

/**
 * @brief State machine for Magnetometer
 * 
 * This function is called repeatedly from SensorTask.c in tasks 
 * When this is called it does its current states job and increments 
 * this objects state to the next value. 
 * Eventually this will return successful and the data will be pulled out of the
 * appropriate global buffers.
 */
sensor_status_t bmx055_mag_run(void)
{
	
		/*
			1. Enqueue data read request
			2. Read data	
		*/
	
		sensor_status_t return_status;

		return_status = SENSOR_BUSY;

		switch(gMagnetometer.get_data_state) {
			
			case ENQUEUE:
				enqueue_helper(BMM050_DATA_X_LSB);
				gMagnetometer.get_data_state = READ_DATA;
				return_status = SENSOR_WAITING;
				break;
			
			case READ_DATA:
				if(gMagnetometer.send_complete){
					
					read_helper();
					/* convert values using constants from bosch driver */
					gMagnetometer.get_data_state = ENQUEUE;
					return_status = SENSOR_COMPLETE;
					
				}
				break;
		}
		
		return return_status;
	
}

/** 
 * @brief enqueues the read request for the register address of the parameter
 *
 * @param reg the register address you want to read data from
 *
 * this enqueues a non-blocking SPI request to read 2 bytes of data, the SPI driver handles the rest
 * 
 */
void enqueue_helper(uint8_t reg)
{
	
	memset((void *)gMagnetometer.spi_recv_buffer, 0, sizeof(gMagnetometer.spi_recv_buffer));
	memset((void *)gMagnetometer.spi_send_buffer, 0, sizeof(gMagnetometer.spi_send_buffer));
	
	/* set send buffer with read code and register address */
	gMagnetometer.spi_send_buffer[0] = BMX055_READ | reg;
	
	spi_master_enqueue(gMagnetometer.spi_master,
		&(gMagnetometer.cs_info),
		gMagnetometer.spi_send_buffer,
		1,
		gMagnetometer.spi_recv_buffer,
		9,
		&(gMagnetometer.send_complete));
}

void bmx055_mag_get_data(bmx055_mag_data_t *out_data)
{
	out_data->x = gMagnetometer.final_vals.x;
	out_data->y = gMagnetometer.final_vals.y;
	out_data->z = gMagnetometer.final_vals.z;
	out_data->rhall = gMagnetometer.final_vals.rhall;
}