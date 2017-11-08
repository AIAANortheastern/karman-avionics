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
	
	memset((void *)(&(gMagnetometer).final_vals), 0, sizeof(gMagnetometer.final_vals));
	
	/* set power control bit to 1  */
	set_init_value(BMM050_POWER_CONTROL ,BMX055_POWER_MODE_ON);
	/* set repetitions xy */
	set_init_value(BMM050_REP_XY ,BMX055_REPETITIONS_XY_MAX);
	/* set repetitions z */
	set_init_value(BMM050_REP_Z ,BMX055_REPETITIONS_Z_MAX);
	/* set data rate & set power mode to normal mode */
	set_init_value(BMM050_CONTROL ,BMX055_DATA_RATE_30HZ | BMX055_OPERATION_MODE_NORMAL);
	
}

// make three functions. one for set, one for verify, and one for write
void set_init_value(uint8_t reg, uint8_t value)
{
	
	do
	{
		
		write_init_value(reg, value);
		
	}while(!verify_init_write(reg, value));
	
}

Bool verify_init_write(uint8_t reg, uint8_t value)
{
	
		gMagnetometer.spi_send_buffer[0] = BMX055_READ | reg;
		
		spi_master_blocking_send_request(gMagnetometer.spi_master,
			&(gMagnetometer.cs_info),
			gMagnetometer.spi_send_buffer,
			1,
			gMagnetometer.spi_recv_buffer,
			1,
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

void write_init_value(uint8_t reg, uint8_t value)
{
	
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

//void set_power_mode(bmx055_mag_power_mode_t powerMode)
//{
	//
	//while() // write is complete
	//{
		//write_power_mode(powerMode);
	//}
	//
//}
//
//void write_power_mode(bmx055_mag_power_mode_t powerMode)
//{
	//
	//gMagnetometer.spi_send_buffer[0] = BMX055_WRITE | BMM050_POWER_CONTROL;
	//gMagnetometer.spi_send_buffer[1] = powerMode;
	//
	//spi_master_blocking_send_request(gMagnetometer.spi_master,
		//&(gMagnetometer.cs_info),
		//gMagnetometer.spi_send_buffer,
		//2,
		//gMagnetometer.spi_recv_buffer,
		//0,
		//&(gMagnetometer.send_complete));
	//
//}
//
//void set_repetitions_xy(uint8_t repetitions)
//{
	//
	//gMagnetometer.spi_send_buffer[0] = BMX055_WRITE | BMM050_REP_XY;
	//gMagnetometer.spi_send_buffer[1] = repetitions;
	//
	//spi_master_blocking_send_request(gMagnetometer.spi_master,
		//&(gMagnetometer.cs_info),
		//gMagnetometer.spi_send_buffer,
		//2,
		//gMagnetometer.spi_recv_buffer,
		//0,
		//&(gMagnetometer.send_complete));
	//
//}
//
//void set_repetitions_z(uint8_t repetitions)
//{
	//
	//gMagnetometer.spi_send_buffer[0] = BMX055_WRITE | BMM050_REP_Z;
	//gMagnetometer.spi_send_buffer[1] = repetitions;
	//
	//spi_master_blocking_send_request(gMagnetometer.spi_master,
		//&(gMagnetometer.cs_info),
		//gMagnetometer.spi_send_buffer,
		//2,
		//gMagnetometer.spi_recv_buffer,
		//0,
		//&(gMagnetometer.send_complete));
	//
//}
//
//void set_control(uint8_t data)
//{
	//
	//gMagnetometer.spi_send_buffer[0] = BMX055_WRITE | BMM050_CONTROL;
	//gMagnetometer.spi_send_buffer[1] = data;
	//
	//spi_master_blocking_send_request(gMagnetometer.spi_master,
		//&(gMagnetometer.cs_info),
		//gMagnetometer.spi_send_buffer,
		//2,
		//gMagnetometer.spi_recv_buffer,
		//0,
		//&(gMagnetometer.send_complete));
	//
//}

void bmx055_mag_reset(void)
{
	
	/* set soft reset bit to high in power control register */
	set_init_value(BMM050_POWER_CONTROL, BMX055_RESET);
	
}


sensor_status_t bmx055_mag_get_data(void)
{
	
		/*
			1. Enqueue x data
			2. Read x data
			3. Enqueue y data
			4. Read y data
			5. Enqueue z data
			6. Read z data
			7. Enqueue hall data
			8. Read hall data		
		*/
	
		sensor_status_t return_status;

		return_status = SENSOR_BUSY;

		switch(gMagnetometer.get_data_state) {
			
			case ENQUEUE_X:
				enqueue_helper(BMM050_DATA_X_LSB);
				gMagnetometer.get_data_state = READ_X_DATA;
				return_status = SENSOR_WAITING;
				break;
			
			case READ_X_DATA:
				if(gMagnetometer.send_complete){
					
					read_helper();
					
					gMagnetometer.get_data_state = ENQUEUE_Y;
					return_status = SENSOR_WAITING;
					
				}
				break;
			
			case ENQUEUE_Y:
				enqueue_helper(BMM050_DATA_Y_LSB);
				gMagnetometer.get_data_state = READ_Y_DATA;
				return_status = SENSOR_WAITING;
				break;
			
			case READ_Y_DATA:
				if(gMagnetometer.send_complete){
					
					read_helper();
					
					gMagnetometer.get_data_state = ENQUEUE_Z;
					return_status = SENSOR_WAITING;
					
				}
				break;
			
			case ENQUEUE_Z:
				enqueue_helper(BMM050_DATA_Z_LSB);
				gMagnetometer.get_data_state = READ_Z_DATA;
				return_status = SENSOR_WAITING;
				break;
			
			case READ_Z_DATA:
				if(gMagnetometer.send_complete){
					
					read_helper();
					
					gMagnetometer.get_data_state = ENQUEUE_HALL;
					return_status = SENSOR_WAITING;
					
				}
				break;
				
			case ENQUEUE_HALL:
				enqueue_helper(BMM050_DATA_R_LSB);
				gMagnetometer.get_data_state = READ_HALL_DATA;
				return_status = SENSOR_WAITING;
				break;
			
			case READ_HALL_DATA:
				if(gMagnetometer.send_complete){
					
					read_helper();
					
					gMagnetometer.get_data_state = ENQUEUE_X;
					return_status = SENSOR_WAITING;
					
				}
				break;
				
		}
		
		return return_status;
	
}

void enqueue_helper(uint8_t reg)
{
	
	memset((void *)gMagnetometer.spi_recv_buffer, 0, sizeof(gMagnetometer.spi_recv_buffer));
	memset((void *)gMagnetometer.spi_send_buffer, 0, sizeof(gMagnetometer.spi_send_buffer));
	
	// set send buffer with read code which will be some constant defined in header
	gMagnetometer.spi_send_buffer[0] = BMX055_READ | reg;
	
	// spimaster enqueue
	spi_master_enqueue(gMagnetometer.spi_master,
		&(gMagnetometer.cs_info),
		gMagnetometer.spi_send_buffer,
		1,
		gMagnetometer.spi_recv_buffer,
		2,
		&(gMagnetometer.send_complete));
	
}

// need one for each x-y, z and hall
void read_helper(void)
{
	
}