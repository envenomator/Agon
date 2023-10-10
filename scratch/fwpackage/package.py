
import sys
import os
import binascii

vdpfilename = 'video.ino.bin'
mosfilename = 'mos.bin'
outputfilename = 'agon-firmware.pkg'

#
# OUTPUT FILE FORMAT
#
# 20-byte HEADER
# Byte  0 -  5 (6 bytes) : 'AGONFW' / ASCII values 0x41, 0x47, 0x4f, 0x4e, 0x46, 0x57
# Byte  6 -  8 (3 bytes) : 24-bit little-endian length of VDP firmware (0 if no data present)
# Byte  9 - 11 (3 bytes) : 24-bit little-endian length of MOS firmware (0 if no data present)
# Byte 12 - 15 (4 bytes) : 32-bit little-endian crc32 of VDP firmware (0 if no data present)
# Byte 16 - 19 (4 bytes) : 32-bit little-endian crc32 of MOS firmware (0 if no data present)
#
# DATA SECTION
#                        : VDP firmware data (0 bytes if no data present)
#                        : MOS firmware data (0 bytes if no data present)
#
# VDP data is positioned first because VDP is updated first
#
vdp_crc32 = 0
mos_crc32 = 0
vdp_len = 0
mos_len = 0

if os.path.isfile(vdpfilename):
  with open(vdpfilename, 'rb') as f:
    file=list(f.read())
    vdp_crc32 = binascii.crc32(bytes(file))
    vdp_len = len(file)
    f.close()

if os.path.isfile(mosfilename):
  with open(mosfilename, 'rb') as f:
    file=list(f.read())
    mos_crc32 = binascii.crc32(bytes(file))
    mos_len = len(file)
    f.close()

with open(outputfilename, 'wb') as f_out:
  # Write out Header to output file
  f_out.write(b'AGONFW')
  f_out.write(vdp_len.to_bytes(3, byteorder = 'little'))
  f_out.write(mos_len.to_bytes(3, byteorder = 'little'))
  f_out.write(vdp_crc32.to_bytes(4, byteorder = 'little'))
  f_out.write(mos_crc32.to_bytes(4, byteorder = 'little'))

  # Write VDP data, if any
  if os.path.isfile(vdpfilename):
    with open(vdpfilename, 'rb') as f:
      file=list(f.read())
      for byte in file:
        f_out.write(byte.to_bytes(1, byteorder = 'little'))
      f.close()

  # Write MOS data, if any
  if os.path.isfile(mosfilename):
    with open(mosfilename, 'rb') as f:
      file=list(f.read())
      for byte in file:
        f_out.write(byte.to_bytes(1, byteorder = 'little'))
      f.close()

f_out.close()
