#ifndef DHT20_H
#define DHT20_H

#include <ez80.h>
#include <defines.h>
#include <float.h>

#define DHT20_LIB_VERSION                    "0.2.1"

#define DHT20_OK                             0
#define DHT20_ERROR_CHECKSUM                -10
#define DHT20_ERROR_CONNECT                 -11
#define DHT20_MISSING_BYTES                 -12
#define DHT20_ERROR_BYTES_ALL_ZERO          -13
#define DHT20_ERROR_READ_TIMEOUT            -14
#define DHT20_ERROR_LASTREAD                -15

int DHT20_requestData();
int DHT20_readData();
int DHT20_convert();
UINT8 DHT20_crc8(UINT8 *ptr, UINT8 len);

BOOL     DHT20_isConnected();
UINT8  DHT20_getAddress();

//  SYNCHRONUOUS CALL
//  blocking read call to read + convert data
int      DHT20_read();
//  access the converted temperature & humidity
float    DHT20_getHumidity();
float    DHT20_getTemperature();


//  OFFSET  1st order adjustments
void     DHT20_setHumOffset(float offset);
void     DHT20_setTempOffset(float offset);
float    DHT20_getHumOffset();
float    DHT20_getTempOffset();


//  READ STATUS
UINT8  DHT20_readStatus();
//  3 wrapper functions around readStatus()
BOOL     DHT20_isCalibrated();
BOOL     DHT20_isMeasuring();
BOOL     DHT20_isIdle();
//  status from last read()
int      DHT20_internalStatus();


//  TIMING
UINT32 DHT20_lastRead();
UINT32 DHT20_lastRequest();


//  RESET  (new since 0.1.4)
//  use with care 
//  returns number of registers reset => must be 3
//  3     = OK
//  0,1,2 = error.
//  255   = no reset needed.
//  See datasheet 7.4 Sensor Reading Process, point 1
//  use with care 
UINT8  DHT20_resetSensor();

UINT8  _crc8(UINT8 *ptr, UINT8 len);

//  use with care
BOOL     DHT20_resetRegister(UINT8 reg);

#endif DHT20_H
