# I2C test utility

This utility provides example code to
- Scan the I2C bus for slave devices (no option on the cmdline)
- Query the Humidity/Temperature of a DHT20 sensor (option: dht)
- Send a test string to a 1602 I2C connected LCD (option: lcd)

## Requirements
A MOS with support for the I2C API

The utility needs to be placed in the /mos/ directory