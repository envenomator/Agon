## Title:       send.py
## Author:      Jeroen Venema
## Created:     25/10/2022
## Last update: 25/10/2022

## Modinfo:
## 25/10/2022 initial version

import sys
import serial
import time
import os.path

## syntax
## send.py FILENAME <SERIAL PORT>
## 
## SERIAL PORT defaults to COM3

if len(sys.argv) == 1 or len(sys.argv) >3:
  sys.exit('Usage: send.py FILENAME <SERIAL_PORT>')

if not os.path.isfile(sys.argv[1]):
  sys.exit(f'Error: file \'{sys.argv[1]}\' not found')

if len(sys.argv) == 2:
  serialport = 'COM3'

if len(sys.argv) == 3:
  serialport = sys.argv[2]

print(f'Sending {sys.argv[1]}')
print(f'Using port {serialport}')

f = open(sys.argv[1], "r")

content = f.readlines()

try:
  with serial.Serial(serialport, 115200,rtscts=False,dsrdtr=False) as ser:
    ##ser.setDTR(None)
    print('Opening serial port...')
    time.sleep(1)
    print('Writing textfile to serial port')

    for line in content:
        ser.write(str(line).encode('ascii'))

    f.close()
    print('Done')
except serial.SerialException:
  print('Error: serial port unavailable')