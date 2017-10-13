//#ifndef ADXL375_H_
//#define ADXL375_H_
//
//#define HIGH_G_ACCELEROMETER_BUFF_SIZE = 7;
//
//typedef struct high_g_accel_control_s
//{
    //spi_master_t        *spi_master; /**< Pointer to the task's SPI Master */
    //chip_select_info_t  cs_info;     /**< Chip select info for the device */
    //volatile uint8_t    spi_send_buffer[HIGH_G_ACCELEROMETER_BUFF_SIZE]; /**< Data to be sent out */
    //volatile uint8_t    spi_recv_buffer[HIGH_G_ACCELEROMETER_BUFF_SIZE]; /**< Data received by the device */
    //volatile Bool       send_complete; /**< Boolean to know when transaction complete */
    //ADXL375_offset_t offset_vals; /**< PROM calibration values */
    //ADXL375_raw_t raw_vals;         /**< Raw ADC Values */
    //ADXL375_data_t final_vals;      /**< Usable values */
    //ADXL375_state_t get_data_state; /**< state machine */
    //uint32_t            time_start;       /**< For keeping track of time */
//} ADXL375_control_t;
//
//typedef struct high_g_accel_raw_s
//{
    //uint16_t dig_x;     /**< d1 */
    //uint16_t dig_y;      /**< d2 */
    //uint16_t dig_z;         /**< dt */
//} ADXL375_raw_t;
//
//typedef struct high_g_accel_data_s
//{
    //uint16_t x;     /**< d1 */
    //uint16_t y;      /**< d2 */
    //uint16_t z;         /**< dt */
//} ADXL375_data_t;
//
//typedef enum
//{
    //ENQUEUE, /**< Send SPI Request */
    //XYZ_DATA_CONVERT        /**< Convert the data and return the final result */
//} ADXL375_state_t;
//
//typedef struct high_g_accel_offset_s
//{
	//// Need to represent fractional values 
	//// struct to do this is TBD
     //x;     /**< d1 */
     //y;      /**< d2 */
     //z;         /**< dt */
//} ADXL375_data_t;
//
//void ADXL375_get_offset_values();
//
//#endif