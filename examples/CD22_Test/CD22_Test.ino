#include "CD22.h"

CD22 mySensor;
CD22_SensorType senType = CD22_SENSOR_NO_SENSOR;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting Device");

  // Start device communication
  senType = mySensor.begin(CD22_SERIAL_TYPE_HW, CD22_SERIAL_BAUD_115200, 60); // Last number is time waited for response before erroring out (0ms - 255ms)
    Serial.println(uint8_t(senType)); // Returns ideal center point (15mm, 35mm, 100mm) based on the sensor connected

  // Turn Laser Off
  if(mySensor.LaserOFF())
    Serial.println("Laser turned OFF");
  else
    Serial.println("Unable to turn Laser OFF");

  // Print currently set Sample Rate
  Serial.println(mySensor.getSampleTime()); // 0=500us, 1=1000us, 2=2000us, 3=4000us, 4=auto

  // Change Sample Rate
  if(!mySensor.setSampleTime(CD22_SENSOR_SAMPLE_TIME_500US)) // CD22_SENSOR_SAMPLE_TIME_500US, CD22_SENSOR_SAMPLE_TIME_1000US, CD22_SENSOR_SAMPLE_TIME_2000US, CD22_SENSOR_SAMPLE_TIME_4000US, CD22_SENSOR_SAMPLE_TIME_AUTO
    Serial.println("Can't communicate with CD22, please check connection");

  // Print Currently set Averaging Setting
  Serial.println(mySensor.getAveraging());

  // Change Averaging Setting
  if(!mySensor.setAveraging(CD22_SENSOR_AVG_NUM_8)) // CD22_SENSOR_AVG_NUM_1, CD22_SENSOR_AVG_NUM_8, CD22_SENSOR_AVG_NUM_64, CD22_SENSOR_AVG_NUM_512
    Serial.println("Can't communicate with CD22, please check connection");
  
  delay(5000);
  // Turn Laser On
  if(mySensor.LaserON())
    Serial.println("Laser turned ON");
  else
    Serial.println("Unable to turn Laser ON");  

  delay(5000);
  // Set Zero
  if(mySensor.executeZeroReset())
    Serial.println("Zero set");
  else
    Serial.println("Unable to set zero");

  // Print 1000 readings, 10ms apart
  for(uint16_t i = 0; i<1000; i++)
  {
    Serial.println(mySensor.readDistance());
    delay(10);
  }

  // Turn Laser Off
  if(mySensor.LaserOFF())
    Serial.println("Laser turned OFF");
  else
    Serial.println("Unable to turn Laser OFF");

  delay(5000);

  // release Zero Reset
  if(mySensor.releaseZeroReset())
    Serial.println("Zero released");
  else
    Serial.println("Unable to release zero");  
}

void loop()
{
  
}