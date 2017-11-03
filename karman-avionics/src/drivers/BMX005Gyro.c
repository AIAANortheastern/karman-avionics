#include "BMX005Gyro.h"

#define GYROSCOPE_CS_PORT *****
#define GYROSCOPE_CS_BM ******
#define DEG_PER_LSB 0.0610370189519944
BMX005_control_t gyroControl;

void bmx500Gyro_init(spi_master_t *spi_master)
{
	gyroControl.cs_info.csPort = &GYROSCOPE_CS_PORT;
	gyroControl.cs_info.pinBitMask = GYROSCOPE_CS_BM;
	gyroControl.cs_info.csPort->DIRSET = gyroControl.pinBitMask;
	gyroControl.spi_master = spi_master;
	memset((void*) gyroControl.spi_recv_buffer, 0, sizeof(gyroControl.spi_recv_buffer));
	memset((void*) gyroControl.spi_send_buffer, 0 ,sizeof(gyroControl.spi_send_buffer));
	gyroControl.send_complete = false;
	

	/* Call initial functions to prepare gyroscope. */
	/* - Set Power Mode "Normal". Line 162 of support 
	- bits 7 and 5 in 0x11 have to be 0 */
	/* - Check (rate_ok) bit: bit4 in GYR 0x3C (1 means ok) */
	/* - Run self test: in GYR 0x3C set bit0 -> 1 to trigger
	- bit 1 will stay 1 while running, when done bit2 will be set to 1 if failed, stay 0 if passed. */
	/* - Measure offset by running slow compensation (GYR 0x31<0:2> slow_offset_en_x/y/z bits to enable)
	   - After a time set the bits to 0s and the last calculated values should remain.*/
	/* - Set Bandwidth of input data. Line 172 of support */
	
}

void bmx500Gyro_Get_XYZ_Data(void)
{
	memset((void*)gyroControl.spi_send_buffer, 0, sizeof(gyroControl.spi_send_buffer));
	gyroControl.spi_send_buffer[0] = GYRO_SEND_READ_CODE; 

	spi_master_enqueue(gyroControl.spi_master,
                                    &(gyroControl.cs_info),
                                    gyroControl.spi_send_buffer,
                                    1,
                                    gyroControl.spi_recv_buffer,
                                    7,
                                    &(gyroControl.send_complete));
}

void gyro_get_data(void);
{
	/* 1. Send read code over SPI */
	/* 2. Wait the read to finish and receive buffer is filled */
	/* 3. Convert x,y,z rot data and store in buffer */


	switch(gyroControl.data_state)
	{
		case ENQUEUE_GYRO_XYZ_READ:
			bmx500Gyro_Get_XYZ_Data();
			gyroControl.data_state = WAIT_XYZ_READ;
			break;
		case WAIT_XYZ_READ:
			if(true == gyroControl.send_complete)
			{
				gyroControl.data_state = XYZ_DATA_CONVERT;
			}
			break;
		case XYZ_DATA_CONVERT:
			gyroControl.raw_data.x_rot_data = get_data_from_buffer(gyroControl.spi_recv_buffer,1);
			gyroControl.raw_data.y_rot_data = get_data_from_buffer(gyroControl.spi_recv_buffer,3);
			gyroControl.raw_data.z_rot_data = get_data_from_buffer(gyroControl.spi_recv_buffer,5);
			break;

    
	}
}

static inline int16_t get_data_from_buffer(volatile uint8_t *buff, int axis)
{
	/* convert to degrees
	/* Signed or Unsigned for send, receive buffer
	/* In andrew's code, use of gAltimeterControl.final_vals?'*/
	int16_t rawval = (int16_t) ( ((int16_t)buff[axis] << 8) | ((int16_t)buff[axis+1]));
	return (rawval*DEG_PER_LSB);
}