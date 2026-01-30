#include "pressureSensor.h"
#include <imu.h>
unsigned long previousMillis2 = 0;

float threshold1 = 0.5;
void Pressure::init() {
  hspi = new SPIClass(HSPI);
  hspi->begin(SCLK, MISO, MOSI, MCLK);  //see SPI library details on arduino.cc for details
  pinMode(hspi->pinSS(), OUTPUT); 
  hspi->setBitOrder(MSBFIRST);
  hspi->setClockDivider(SPI_CLOCK_DIV32);  //divide 16 MHz to communicate on 500 kHz
  ledcSetup(pwm_Channel, pwm_Freq, pwm_Resolution);
  ledcAttachPin(MCLK, pwm_Channel);
  imu_updateFull(threshold1);
  if (millis() - previousMillis2 > 500) {
    imu_updateFull(threshold1);
    previousMillis2 = millis();
  }
  delay(100);
  if (millis() - previousMillis2 > 500) {
    imu_updateFull(threshold1);
    previousMillis2 = millis();
  }
}

void Pressure::resetSensor() {
  hspi->setDataMode(SPI_MODE0);
  hspi->transfer(0x15);
  hspi->transfer(0x55);
  hspi->transfer(0x40);
}

void Pressure::update() {
  int dutyCycle = 128;
  delay(1);
  ledcWrite(pwm_Channel, dutyCycle);
  this->resetSensor();

  //Calibration word 1
  unsigned int word1 = 0;
  unsigned int word11 = 0;
  hspi->transfer(0x1D);           //send first byte of command to get calibration word 1
  hspi->transfer(0x50);           //send second byte of command to get calibration word 1
  hspi->setDataMode(SPI_MODE1);   //change mode in order to listen
  word1 = hspi->transfer(0x00);   //send dummy byte to read first byte of word
  word1 = word1 << 8;           //shift returned byte
  word11 = hspi->transfer(0x00);  //send dummy byte to read second byte of word
  word1 = word1 | word11;       //combine first and second byte of word

  this->resetSensor();  //resets the sensor

  //Calibration word 2; see comments on calibration word 1
  unsigned int word2 = 0;
  byte word22 = 0;
  hspi->transfer(0x1D);
  hspi->transfer(0x60);
  hspi->setDataMode(SPI_MODE1);
  word2 = hspi->transfer(0x00);
  word2 = word2 << 8;
  word22 = hspi->transfer(0x00);
  word2 = word2 | word22;

  this->resetSensor();  //resets the sensor

  //Calibration word 3; see comments on calibration word 1
  unsigned int word3 = 0;
  byte word33 = 0;
  hspi->transfer(0x1D);
  hspi->transfer(0x90);
  hspi->setDataMode(SPI_MODE1);
  word3 = hspi->transfer(0x00);
  word3 = word3 << 8;
  word33 = hspi->transfer(0x00);
  word3 = word3 | word33;

  this->resetSensor();  //resets the sensor

  //Calibration word 4; see comments on calibration word 1
  unsigned int word4 = 0;
  byte word44 = 0;
  hspi->transfer(0x1D);
  hspi->transfer(0xA0);
  hspi->setDataMode(SPI_MODE1);
  word4 = hspi->transfer(0x00);
  word4 = word4 << 8;
  word44 = hspi->transfer(0x00);
  word4 = word4 | word44;

  long c1 = word1 >> 1;
  long c2 = ((word3 & 0x3F) << 6) | ((word4 & 0x3F));
  long c3 = (word4 >> 6);
  long c4 = (word3 >> 6);
  long c5 = (word2 >> 6) | ((word1 & 0x1) << 10);
  long c6 = word2 & 0x3F;

  this->resetSensor();  //resets the sensor

  //Temperature:
  unsigned int tempMSB = 0;  //first byte of value
  unsigned int tempLSB = 0;  //last byte of value
  unsigned int D2 = 0;
  hspi->transfer(0x0F);            //send first byte of command to get temperature value
  hspi->transfer(0x20);            //send second byte of command to get temperature value
  if (millis() - previousMillis2 > 500) {
    imu_updateFull(threshold1);
    previousMillis2 = millis();
  }
  delay(35);                     //wait for conversion end
  if (millis() - previousMillis2 > 500) {
    imu_updateFull(threshold1);
    previousMillis2 = millis();
  }
  hspi->setDataMode(SPI_MODE1);    //change mode in order to listen
  tempMSB = hspi->transfer(0x00);  //send dummy byte to read first byte of value
  tempMSB = tempMSB << 8;        //shift first byte
  tempLSB = hspi->transfer(0x00);  //send dummy byte to read second byte of value
  D2 = tempMSB | tempLSB;        //combine first and second byte of value

  this->resetSensor();  //resets the sensor

  //Pressure:
  unsigned int presMSB = 0;  //first byte of value
  unsigned int presLSB = 0;  //last byte of value
  unsigned int D1 = 0;
  hspi->transfer(0x0F);            //send first byte of command to get pressure value
  hspi->transfer(0x40);          //send second byte of command to get pressure value
  if (millis() - previousMillis2 > 500) {
    imu_updateFull(threshold1);
    previousMillis2 = millis();
  }
  delay(35);                  //wait for conversion end
  if (millis() - previousMillis2 > 500) {
    imu_updateFull(threshold1);
    previousMillis2 = millis();
  }
  hspi->setDataMode(SPI_MODE1);    //change mode in order to listen
  presMSB = hspi->transfer(0x00);  //send dummy byte to read first byte of value
  presMSB = presMSB << 8;        //shift first byte
  presLSB = hspi->transfer(0x00);  //send dummy byte to read second byte of value
  D1 = presMSB | presLSB;

  const long UT1 = (c5 * 8) + 20224;
  const long dT = (D2 - UT1);
  const long TEMP = 200 + ((dT * (c6 + 50)) / 1024);
  const long OFF = (c2 * 4) + (((c4 - 512) * dT) / 4096);
  const long SENS = c1 + ((c3 * dT) / 1024) + 24576;

  long PCOMP = ((((SENS * (D1 - 7168)) / 16384) - OFF) / 32) + 270;
  const long dT2 = dT - ((dT >> 7 * dT >> 7) >> 3);
  const float TEMPCOMP = (200 + (dT2 * (c6 + 100) >> 11)) / 10;
  this->pressure = PCOMP;
  this->temp = TEMPCOMP;
}

void Pressure::display() {
  Serial.print("pressure =    ");
  Serial.print(this->pressure);
  Serial.println(" mbar");
  Serial.print("temperature = ");
  Serial.print(this->temp);
  Serial.println(" °C");
  Serial.println("************************************");
}

Pressure::Pressure(){
  this->pressure=0;
  this->temp=0;
}

int Pressure::getPressure(){
  return this->pressure;
}
int Pressure::getTemprature(){
  return this->temp;
}

