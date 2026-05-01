#ifndef _CD22_H
#define _CD22_H

#include <Serial.h>

#define CD22_REG_MODEL_TYPE_U	0x01
#define CD22_REG_MODEL_TYPE_L	0x00

#define CD22_TABLE_PARAM        0x43    ///< CD22 Parameter Table
#define CD22_REG_MEAS_U         0xb0    ///< CD22 Measurement Register Upper
#define CD22_REG_MEAS_L         0x01    ///< CD22 Measurement Register Lower

#define CD22_REG_LASER_ON_U     0xa0
#define CD22_REG_LASER_ON_L     0x03

#define CD22_REG_LASER_OFF_U    0xa0
#define CD22_REG_LASER_OFF_L    0x02

#define CD22_READ_COMMAND       0x52
#define CD22_WRITE_COMMAND      0x57

#define CD22_REG_SAMPLE_TIME_U  0x40
#define CD22_REG_SAMPLE_TIME_L  0x06

#define CD22_REG_AVG_NUM_U      0x40
#define CD22_REG_AVG_NUM_L      0x0a

#define CD22_VAL_STX            0x02    ///< CD22 Start Transmission Byte
#define CD22_VAL_ETX            0x03    ///< CD22 End Transmission Byte
#define CD22_VAL_ACK            0x06    ///< CD22 Acknowledgement
#define CD22_VAL_NACK           0x15    ///< CD22 NOT Acknowledgement

#define CD22_REG_SET_ZERO_U     0xa1
#define CD22_REG_SET_ZERO_L     0x00
#define CD22_REG_RESET_ZERO_U   0xa1
#define CD22_REG_RESET_ZERO_L   0x01

typedef enum _serial_type {
  CD22_SERIAL_TYPE_HW = false,     /**< Alert will latch until Mask/Enable
                                           register is read **/
  CD22_SERIAL_TYPE_SW = true,     /**< Alert will reset when fault is
                                           cleared **/
} CD22_SerialType;

typedef enum _sensor_type {
  CD22_SENSOR_NO_SENSOR = 0x00,
  CD22_SENSOR_15_485 = 0x0f,
  CD22_SENSOR_35_485 = 0x23,
  CD22_SENSOR_100_485 = 0x64
} CD22_SensorType;

typedef enum _sensor_least_count	{
	CD22_SENSOR_LEAST_COUNT_1_MICRON = 1,
	CD22_SENSOR_LEAST_COUNT_10_MICRON = 10
} CD22_SensorLeastCount;

typedef enum _serial_baud {
  CD22_SERIAL_BAUD_9600 = 9600,
  CD22_SERIAL_BAUD_19200 = 19200,
  CD22_SERIAL_BAUD_38400 = 38400,
  CD22_SERIAL_BAUD_57600 = 57600,
  CD22_SERIAL_BAUD_115200 = 115200,
  CD22_SERIAL_BAUD_230400 = 230400,
  CD22_SERIAL_BAUD_312500 = 312500,
  CD22_SERIAL_BAUD_468750 = 468750,
  CD22_SERIAL_BAUD_500000 = 500000,
  CD22_SERIAL_BAUD_625000 = 625000,
  CD22_SERIAL_BAUD_833333 = 833333,
  CD22_SERIAL_BAUD_937500 = 937500,
  CD22_SERIAL_BAUD_1250000 = 1250000
} CD22_SerialBaud;

typedef enum _sensor_sampletime {
  CD22_SENSOR_SAMPLE_TIME_500US = 0x00,
  CD22_SENSOR_SAMPLE_TIME_1000US = 0x01,
  CD22_SENSOR_SAMPLE_TIME_2000US = 0x02,
  CD22_SENSOR_SAMPLE_TIME_4000US = 0x03,
  CD22_SENSOR_SAMPLE_TIME_AUTO = 0x04
} CD22_SensorSampleTime;

typedef enum _sensor_avg_num {
  CD22_SENSOR_AVG_NUM_1 = 0x00,
  CD22_SENSOR_AVG_NUM_8 = 0x01,
  CD22_SENSOR_AVG_NUM_64 = 0x02,
  CD22_SENSOR_AVG_NUM_512 = 0x03
} CD22_SensorAvgNum;

class CD22
{
	public:
		CD22();

		CD22_SensorType begin(
		  CD22_SerialType SerialType = CD22_SERIAL_TYPE_HW,
		  CD22_SerialBaud SerialBaud = CD22_SERIAL_BAUD_9600,
		  uint8_t sensor_timeout_ms = 60
		);
		
		CD22_SensorType CheckSensorType(void);

		bool readData(uint8_t* type, uint8_t* data1, uint8_t* data2);

		bool writeData(uint8_t type, uint8_t data1, uint8_t data2);

		bool LaserON(void);

		bool LaserOFF(void);

		float readDistance(void);
		
		int16_t readDistanceRaw(void);

		CD22_SensorSampleTime getSampleTime(void);

		bool setSampleTime(CD22_SensorSampleTime SampleTime = CD22_SENSOR_SAMPLE_TIME_AUTO);

		CD22_SensorAvgNum getAveraging(void);

		bool setAveraging(CD22_SensorAvgNum AvgNum = CD22_SENSOR_AVG_NUM_1);

		bool executeZeroReset(void);

		bool releaseZeroReset(void);
		
		CD22_SensorLeastCount least_count = CD22_SENSOR_LEAST_COUNT_1_MICRON;
		
		void flush(void);

	private:
		CD22_SensorType _sensor_type = CD22_SENSOR_15_485;
		uint8_t _sensor_timeout_ms;
	  
};

#endif //_CD22_H