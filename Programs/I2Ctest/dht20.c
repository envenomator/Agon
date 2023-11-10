#include "dht20.h"
#include "agontimer.h"
#include "mos-interface.h"
#include <stdio.h>

float    _humidity;
float    _temperature;
float    _humOffset;
float    _tempOffset;

UINT8  _status;
UINT32 _lastRequest;
UINT32 _lastRead;
UINT8  _bits[7];

//  set DHT20_WIRE_TIME_OUT to 0 to disable.
//  note this timeout is commented in code below.
#define DHT20_WIRE_TIME_OUT         250000    //  microseconds

const UINT8 DHT20_ADDRESS = 0x38;

//  See datasheet 7.4 Sensor Reading Process, point 1
//  use with care.
UINT8 DHT20_resetSensor()
{
  UINT8 status;
  UINT8 count = 255;
  status = DHT20_readStatus();
  //printf("ResetSensor status: 0x%02X\r\n", status);
  if ((status & 0x18) != 0x18)
  {
    count++;
    if (DHT20_resetRegister(0x1B)) count++;
    if (DHT20_resetRegister(0x1C)) count++;
    if (DHT20_resetRegister(0x1E)) count++;
    delayms(10);
  }
  return count;
}

////////////////////////////////////////////////
//
//  READ THE SENSOR
//
int DHT20_read()
{
  int status;
	
  status = DHT20_requestData();
  //printf("DHT20 requestdata complete\r\n");
  if (status < 0) return status;
  //  wait for measurement ready
  delayms(80);
  while (DHT20_isMeasuring());
  //  read the measurement
  //printf("DHT20 measurement complete\r\n");
  status = DHT20_readData();
  if (status < 0) return status;
  //printf("DHT20 data read complete\r\n");
  //  convert it to meaningful data
  return DHT20_convert();
}

int DHT20_requestData()
{
  UINT8 data[] = {0x70,0xAC,0x33,0x00};
  //  reset sensor if needed.
  DHT20_resetSensor();
  
  //mos_i2c_write(DHT20_ADDRESS, 1, &data[0]);
  //mos_i2c_write(DHT20_ADDRESS, 1, &data[1]);
  //mos_i2c_write(DHT20_ADDRESS, 1, &data[2]);
  //mos_i2c_write(DHT20_ADDRESS, 1, &data[3]);
  mos_i2c_write(DHT20_ADDRESS, sizeof(data), data);
  _lastRequest = timer1;
  return 1;
}

int DHT20_readData()
{
  int i;
  int errorcode;
  BOOL allZero;
  //  GET DATA
  const UINT8 length = 7;

  errorcode = mos_i2c_read(DHT20_ADDRESS, length, _bits);

  if (errorcode != 0)     return DHT20_ERROR_CONNECT;

  allZero = 1;
  for (i = 0; i < length; i++)
  {
    allZero = allZero && (_bits[i] == 0);
  }
  //  Serial.println();
  if (allZero) return DHT20_ERROR_BYTES_ALL_ZERO;

  _lastRead = timer1;
  return length;
}


int DHT20_convert()
{
  UINT32 raw;
  UINT8 _crc;
	
  //  CONVERT AND STORE
  _status      = _bits[0];
  raw = _bits[1];
  raw <<= 8;
  raw += _bits[2];
  raw <<= 4;
  raw += (_bits[3] >> 4);
  _humidity = raw * 9.5367431640625e-5;   // ==> / 1048576.0 * 100%;

  raw = (_bits[3] & 0x0F);
  raw <<= 8;
  raw += _bits[4];
  raw <<= 8;
  raw += _bits[5];
  _temperature = raw * 1.9073486328125e-4 - 50;  //  ==> / 1048576.0 * 200 - 50;

  //  TEST CHECKSUM
  _crc = DHT20_crc8(_bits, 6);
  //  Serial.print(_crc, HEX);
  //  Serial.print("\t");
  //  Serial.println(_bits[6], HEX);
  if (_crc != _bits[6]) return DHT20_ERROR_CHECKSUM;

  return DHT20_OK;
}


////////////////////////////////////////////////
//
//  TEMPERATURE & HUMIDITY & OFFSET
//
float DHT20_getHumidity()
{
  return _humidity + _humOffset;
}


float DHT20_getTemperature()
{
  return _temperature + _tempOffset;
}


void DHT20_setHumOffset(float offset)
{
  _humOffset  = offset;
}


void DHT20_setTempOffset(float offset)
{
  _tempOffset = offset;
}


float DHT20_getHumOffset()
{
  return _humOffset;
}


float DHT20_getTempOffset()
{
  return _tempOffset;
}


////////////////////////////////////////////////
//
//  STATUS
//
UINT8 DHT20_readStatus()
{
  UINT8 r;
  mos_i2c_read(DHT20_ADDRESS, 1, &r);
  delayms(2);  //  needed to stabilize timing
  return r;
}


BOOL DHT20_isCalibrated()
{
  return (DHT20_readStatus() & 0x08) == 0x08;
}


BOOL DHT20_isMeasuring()
{
  UINT8 ret = DHT20_readStatus();
  //if(ret == 0x18) DHT20_resetSensor();
  //ret = DHT20_readStatus();
  //printf("Measuring status: 0x%02X\r\n", ret);
  //return (DHT20_readStatus() & 0x80) == 0x80;
  return (ret & 0x80) == 0x80;
}


BOOL DHT20_isIdle()
{
  return (DHT20_readStatus() & 0x80) == 0x00;
}


int DHT20_internalStatus()
{
  return _status;
}


////////////////////////////////////////////////
//
//  TIMING
//
UINT32 DHT20_lastRead()
{
  return _lastRead;
}


UINT32 DHT20_lastRequest()
{
  return _lastRequest;
}


////////////////////////////////////////////////
//
//  PRIVATE
//
UINT8 DHT20_crc8(UINT8 *ptr, UINT8 len)
{
  UINT i;
  UINT8 crc = 0xFF;
  while(len--)
  {
    crc ^= *ptr++;
    for (i = 0; i < 8; i++)
    {
      if (crc & 0x80)
      {
        crc <<= 1;
        crc ^= 0x31;
      }
      else
      {
        crc <<= 1;
      }
    }
  }
  return crc;
}


//  Code based on demo code sent by www.aosong.com
//  no further documentation.
//  0x1B returned 18, 0, 4
//  0x1C returned 18, 65, 0
//  0x1E returned 18, 8, 0
//    18 seems to be status register
//    other values unknown.
BOOL DHT20_resetRegister(UINT8 reg)
{
  UINT8 value[3] = {0,0,0};
  UINT8 command;

  value[0] = 0x00;
  command = 0x70;
  mos_i2c_write(DHT20_ADDRESS, 1, &command);
  mos_i2c_write(DHT20_ADDRESS, 1, &reg);
  mos_i2c_write(DHT20_ADDRESS, 1, value);
  mos_i2c_write(DHT20_ADDRESS, 1, value);
  delayms(5);

  command = 0x71;
  mos_i2c_write(DHT20_ADDRESS, 1, &command);
  mos_i2c_read(DHT20_ADDRESS, 3, value);
  delayms(10);

  value[0] = 0xB0 | reg;
  command = 0x70;
  mos_i2c_write(DHT20_ADDRESS, 1, &command);
  mos_i2c_write(DHT20_ADDRESS, 3, value);
   
  delayms(5);
  return 1;
}


// -- END OF FILE --
