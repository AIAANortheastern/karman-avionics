#include "BMX005Gyro.h"

#define DEG_PER_LSB 0.0610370189519944 
gyroscope_control_t gyroControl;


/******    Static Functions		*******/
static void writeGyroReg(const uint8_t addr, uint8_t val, gyroscope_control_t *gyro)
{
	volatile uint8_t sendBuf[2];
	volatile uint8_t recvBuf[2];
	volatile Bool writeComplete = 0;
	
	sendBuf[0] = (addr & ~((uint8_t)GYRO_SPI_READ_BIT));
	sendBuf[1] = val;
	spi_master_blocking_send_request(gyro->spi_master,
									&gyro->cs_info,
									sendBuf,
									2,
									recvBuf,
									2,
									&writeComplete);
}

static uint8_t readGyroReg(const uint8_t addr, gyroscope_control_t *gyro)
{
	volatile uint8_t sendBuf[2];
	volatile uint8_t recvBuf[2];
	volatile Bool readComplete = 0;
	
	sendBuf[0] = (addr | (uint8_t)GYRO_SPI_READ_BIT);
	
	spi_master_blocking_send_request(gyro->spi_master,
									&gyro->cs_info,
									sendBuf,
									2,
									recvBuf,
									2,
									&readComplete);
	return recvBuf[1];
}

static inline int16_t get_data_from_buffer(volatile uint8_t *buff, int axis)
{
	/* convert to degrees
	   Signed or Unsigned for send, receive buffer
	   In andrew's code, use of gAltimeterControl.final_vals?'*/
	int16_t rawval = (int16_t) ( ((int16_t)buff[axis] << 8) | ((int16_t)buff[axis+1]));
	
	/* This conversion:   realval = rawval * 1/16 
	   The above conversion has an error of + 2.5% from the accurate conversion of:
				realval = rawval * 2000 / 32767, but I think we might need some tricks to
				do this computation (or FPU) */ 
	rawval = rawval >> 4; 
	return rawval;
}


static uint8_t isBandwidthSet(gyroscope_control_t *gyroControl)
{
	uint8_t val;
	writeGyroReg(GYRO_BANDWIDTH_REG, (uint8_t)GYRO_BANDWIDTH, gyroControl);
	/*writeGyroReg(GYRO_BANDWIDTH_REG, (uint8_t)GYRO_BANDWIDTH, gyroControl); */
	val = readGyroReg(GYRO_BANDWIDTH_REG, gyroControl);
	return ((GYRO_BANDWIDTH_MASK & val) == GYRO_BANDWIDTH);
	// Read bandwidth from sensor w/ blocking send request, then check if result is correct

}


//TODO don't use blocking request here
static void bmx500Gyro_Get_XYZ_Data(void)
{
	memset((void*)gyroControl.spi_send_buffer, 0, sizeof(gyroControl.spi_send_buffer));
	gyroControl.spi_send_buffer[0] = GYRO_SEND_READ_CODE;
gyroControl.send_complete = 1;
	spi_master_enqueue(gyroControl.spi_master,
	&(gyroControl.cs_info),
	gyroControl.spi_send_buffer,
	1,
	gyroControl.spi_recv_buffer,
	7,
	&(gyroControl.send_complete));
}


/********	Non-static Functions	*********/


sensor_status_t gyro_state_machine(void)
{

	sensor_status_t returnStatus = SENSOR_BUSY;

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
			gyroControl.data_state = ENQUEUE_GYRO_XYZ_READ;
			returnStatus = SENSOR_COMPLETE;
			break;
	}
	return returnStatus;
}

void gyro_get_data(gyro_data_raw_t *outdata)
{
	outdata->x_rot_data = gyroControl.raw_data.x_rot_data;
	outdata->y_rot_data = gyroControl.raw_data.y_rot_data;
	outdata->z_rot_data = gyroControl.raw_data.z_rot_data;
}

void bmx500Gyro_init(spi_master_t *spi_master)
{
	gyroControl.cs_info.csPort = &IMU_GYRO1_PORT;
	gyroControl.cs_info.pinBitMask = IMU_GYRO1_CS;
	gyroControl.cs_info.csPort->DIRSET = gyroControl.cs_info.pinBitMask;
	gyroControl.spi_master = spi_master;
	memset((void*) gyroControl.spi_recv_buffer, 0, sizeof(gyroControl.spi_recv_buffer));
	memset((void*) gyroControl.spi_send_buffer, 0, sizeof(gyroControl.spi_send_buffer));
	gyroControl.send_complete = false;


	 while(!isBandwidthSet(&gyroControl));  

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
