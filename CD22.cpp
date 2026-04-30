#include "CD22.h"

CD22::CD22()
{

}

CD22_SensorType CD22::begin(
      CD22_SerialType SerialType,
      CD22_SerialBaud SerialBaud,
	  uint8_t sensor_timeout_ms
    )
    {
	  _sensor_timeout_ms = sensor_timeout_ms;
      if (!bool(SerialType))
	  {
	    Serial1.begin(SerialBaud);
		uint8_t type, data1, data2;
		
		writeData(CD22_READ_COMMAND, CD22_REG_MODEL_TYPE_U, CD22_REG_MODEL_TYPE_L);
		if (readData(&type, &data1, &data2))
		{
			switch(~data1&data2)
			{
				case CD22_SENSOR_15_485:
					_least_count = CD22_SENSOR_LEAST_COUNT_1_MICRON;
					_sensor_type = CD22_SENSOR_15_485;
					return CD22_SENSOR_15_485;
					break;
				case CD22_SENSOR_35_485:
					_least_count = CD22_SENSOR_LEAST_COUNT_10_MICRON;
					_sensor_type = CD22_SENSOR_35_485;
					return CD22_SENSOR_35_485;
					break;
				case CD22_SENSOR_100_485:
					_least_count = CD22_SENSOR_LEAST_COUNT_10_MICRON;
					_sensor_type = CD22_SENSOR_100_485;
					return CD22_SENSOR_100_485;
					break;
				default:
					return CD22_SENSOR_NO_SENSOR;
					break;
			}
		}
		else
			return CD22_SENSOR_NO_SENSOR;
	  }
      else
        return CD22_SENSOR_NO_SENSOR;
    }

bool CD22::readData(uint8_t* type, uint8_t* data1, uint8_t* data2)
{
  uint8_t inByte[6];
  unsigned long startTime = millis();
  bool timeout = false;
  while((Serial1.available() < 6) && ((millis() - startTime) <= _sensor_timeout_ms));
  if ((millis() - startTime) > _sensor_timeout_ms)
	  timeout = true;
  for (uint8_t i = 0; i < 6 ; i++)
  {
    inByte[i] = Serial1.read();
  }
  if ((inByte[0]==CD22_VAL_STX) && (inByte[4]==CD22_VAL_ETX) && (inByte[1]==CD22_VAL_ACK) && !timeout)
  {
    if(inByte[5] == (inByte[1] ^ inByte[2] ^ inByte[3]) )
    {
      *type = inByte[1];
      *data1 = inByte[2];
      *data2 = inByte[3];
      return true;
    }
    else
      return false;
  }
  else
    return false;
}

bool CD22::writeData(uint8_t type, uint8_t data1, uint8_t data2)
{
  Serial1.flush();
  uint8_t outByte[6] = {CD22_VAL_STX, type, data1, data2, CD22_VAL_ETX, (type^data1^data2)};
  for (uint8_t i = 0; i < 6 ; i++)
  {
    Serial1.write(outByte[i]);
  }
}

bool CD22::LaserON(void)
{
  uint8_t type, data1, data2;
  writeData(CD22_TABLE_PARAM, CD22_REG_LASER_ON_U, CD22_REG_LASER_ON_L);
  delay(10);
  if (readData(&type, &data1, &data2))
    return true;
  else
    return false;
}

bool CD22::LaserOFF(void)
{
  uint8_t type, data1, data2;
  writeData(CD22_TABLE_PARAM, CD22_REG_LASER_OFF_U, CD22_REG_LASER_OFF_L);
  if (readData(&type, &data1, &data2))
    return true;
  else
    return false;
}

float CD22::readDistance(void)
{
  uint8_t type, data1, data2;
  writeData(CD22_TABLE_PARAM, CD22_REG_MEAS_U, CD22_REG_MEAS_L);
  if (readData(&type, &data1, &data2))
  {
    float dat = int16_t((data1<<8) | (data2))*float(_least_count/1000.0);
    return dat;
  }
  else
    return 0;
}

int16_t readDistanceRaw(void)
{
  uint8_t type, data1, data2;
  writeData(CD22_TABLE_PARAM, CD22_REG_MEAS_U, CD22_REG_MEAS_L);
  if (readData(&type, &data1, &data2))
  {
    return int16_t((data1<<8) | (data2));
  }
  else
    return 0;
}

CD22_SensorSampleTime CD22::getSampleTime(void)
{
	uint8_t type, data1, data2;
	writeData(CD22_READ_COMMAND, CD22_REG_SAMPLE_TIME_U, CD22_REG_SAMPLE_TIME_L);
	if (readData(&type, &data1, &data2))
	{
		switch(data2)
		{
			case CD22_SENSOR_SAMPLE_TIME_500US:
				return CD22_SENSOR_SAMPLE_TIME_500US;
				break;
			case CD22_SENSOR_SAMPLE_TIME_1000US:
				return CD22_SENSOR_SAMPLE_TIME_1000US;
				break;
			case CD22_SENSOR_SAMPLE_TIME_2000US:
				return CD22_SENSOR_SAMPLE_TIME_2000US;
				break;
			case CD22_SENSOR_SAMPLE_TIME_4000US:
				return CD22_SENSOR_SAMPLE_TIME_4000US;
				break;
			case CD22_SENSOR_SAMPLE_TIME_AUTO:
				return CD22_SENSOR_SAMPLE_TIME_AUTO;
				break;
		}
	}
}

bool CD22::setSampleTime(CD22_SensorSampleTime SampleTime)
{
  uint8_t type, data1, data2;
  writeData(CD22_READ_COMMAND, CD22_REG_SAMPLE_TIME_U, CD22_REG_SAMPLE_TIME_L);
  if (readData(&type, &data1, &data2))
  {
    writeData(CD22_WRITE_COMMAND, 0x00, SampleTime);
    if (readData(&type, &data1, &data2))
    {
      if (getSampleTime() == SampleTime)
        return true;
      else
        return false;
    }
    else
      return false;
  }
  else
    return false;
}

CD22_SensorAvgNum CD22::getAveraging(void)
{
	uint8_t type, data1, data2;
	writeData(CD22_READ_COMMAND, CD22_REG_AVG_NUM_U, CD22_REG_AVG_NUM_L);
	if (readData(&type, &data1, &data2))
	{
		switch(data2)
		{
			case CD22_SENSOR_AVG_NUM_1:
				return CD22_SENSOR_AVG_NUM_1;
				break;
			case CD22_SENSOR_AVG_NUM_8:
				return CD22_SENSOR_AVG_NUM_8;
				break;
			case CD22_SENSOR_AVG_NUM_64:
				return CD22_SENSOR_AVG_NUM_64;
				break;
			case CD22_SENSOR_AVG_NUM_512:
				return CD22_SENSOR_AVG_NUM_512;
				break;
		}
	}
}

bool CD22::setAveraging(CD22_SensorAvgNum AvgNum)
{
  uint8_t type, data1, data2;
  writeData(CD22_READ_COMMAND, CD22_REG_AVG_NUM_U, CD22_REG_AVG_NUM_L);
  if (readData(&type, &data1, &data2))
  {
    writeData(CD22_WRITE_COMMAND, 0x00, AvgNum);
    if (readData(&type, &data1, &data2))
    {
      if (getAveraging() == AvgNum)
        return true;
      else
        return false;
    }
    else
      return false;
  }
  else
    return false;
}

bool CD22::executeZeroReset(void)
{
  uint8_t type, data1, data2;
  writeData(CD22_TABLE_PARAM, CD22_REG_SET_ZERO_U, CD22_REG_SET_ZERO_L);
  if (readData(&type, &data1, &data2))
    return true;
  else
    return false;
}

bool CD22::releaseZeroReset(void)
{
  uint8_t type, data1, data2;
  writeData(CD22_TABLE_PARAM, CD22_REG_RESET_ZERO_U, CD22_REG_RESET_ZERO_L);
  if (readData(&type, &data1, &data2))
    return true;
  else
    return false;
}