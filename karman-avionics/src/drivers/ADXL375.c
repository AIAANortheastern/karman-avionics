 //#include "ADXL375.h"
 //
 //#define  HIGH_G_ACCELEROMETER_READ_DATA (0x32)
 ////high g accelerometer
 //
 //ADXL375_control_t gHighGAccelerometer;
 //
 //#define x_bit_mask = 0x0000FF;
 //#define y_bit_mask = 0x00FF00
 //#define z
 //
//void ADXL375_init(spi_master_t *spi_master) {
	////assign the spi_master to master struct for high g accelerometer
	//// clear buffers with memset
	//ADXL375_reset();
//
//}
//
//void ADXL375_get_data() {
////x, y, z
//
//
	//switch(gHighGAccelerometer.get_data_state) {
		//case ENQUEUE:
			//enqueue_helper();
			//gHighGAccelerometer.get_data_state = XYZ_DATA_CONVERT;
			//break;	
		//case XYZ_DATA_CONVERT:
			//
			//if(gHighGAccelerometer.send_complete){
				////do some logic
				//
				//gHighGAccelerometer.get_data_state = ENQUEUE;			
			//}
			//break;
	//}
//
//
//
					   //
//
//}
//
//void ADXL375_get_offset_values() {
	//
//}
//
//
//////////////////////////////////
////HELPERS BELOW HERE
/////////////////////////////////
//void enqueue_helper() {
	//// clear buffers
	//memset((void *)gHighGAccelerometer.spi_recv_buffer, 0, sizeof(gHighGAccelerometer.spi_recv_buffer));
	//memset((void *)gHighGAccelerometer.spi_send_buffer, 0, sizeof(gHighGAccelerometer.spi_send_buffer));
	//
	//// set send buffer with read code which will be some constant defined in header
	//gHighGAccelerometer.spi_send_buffer[0] = HIGH_G_ACCELEROMETER_READ_DATA | 0xC0;
	//
	//// spimaster enqueue
	//spi_master_enqueue(gHighGAccelerometer.spi_master,
	    //&(gHighGAccelerometer.cs_info),
	    //gHighGAccelerometer.spi_send_buffer,
	    //1,
	    //gHighGAccelerometer.spi_recv_buffer,
	    //7,
	    //&(gHighGAccelerometer.send_complete));
//}
//
//void xyz_read_helper() {
	//// has data  gHighGAccelerometer.spi_recv_buffer
	//// we want to put data into 
	//gHighGAccelerometer.raw_vals.dig_x = (uint16_t)(((uint16_t)gHighGAccelerometer.spi_recv_buffer[2] << 8) | ((uint16_t)gHighGAccelerometer.spi_recv_buffer[1]));
	//gHighGAccelerometer.raw_vals.dig_y = (uint16_t)(((uint16_t)gHighGAccelerometer.spi_recv_buffer[4] << 8) | ((uint16_t)gHighGAccelerometer.spi_recv_buffer[3]));
	//gHighGAccelerometer.raw_vals.dig_z = (uint16_t)(((uint16_t)gHighGAccelerometer.spi_recv_buffer[6] << 8) | ((uint16_t)gHighGAccelerometer.spi_recv_buffer[5]));
//}
//
//
//
