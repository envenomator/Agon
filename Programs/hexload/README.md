# HEXLOAD patch
## Purpose
Playing microSD jockey during assemble/compile/transfer/test/debug cycles is no fun at all. I'd like to shorten this cycle significantly by removing the need to physically bring over new binaries to the Agon.

This code patches Quark VDP, so the user can send Intel HEX files from the USB-Serial interface, directly to ez80 memory.
After compile/assemble, you can use whatever you like to send the contents of the resulting .hex file to the USB-Serial port from your PC.

![image](https://github.com/envenomator/Agon/blob/master/Programs/hexload/example.jpg?raw=true)

## The sequence of iHex file transfer:
0. The first time after a cold reboot, an extra push to the reset button is needed. Otherwise the ESP32 is set to firmware update mode during opening of the serial interface. 
1. Enter command 'hexload' in MOS, which requests a serial-receive function in VDP. The VDP then accepts Intel HEX files, sent as text to the serial-over-USB interface, decodes the content, marks line CRC faults and sends a contiguous bytestream back to the ez80 CPU in chunks of max 255 bytes. The VDP gives an overview of each line received over the serial port and the status
2. Send the actual file in text format to the serial-over-USB interface. You can use the example send.py script for this. You need to edit this to match the serial port you are using
3. Both VDP and CPU calculate a full CRC32 over the sent/received datastream. Success/failure is reported by the VDP.
4. On success, you might expect to be able to JMP or RUN to the appropriate memory location. Default is 0x40000, unless your iHex file specifies a different start address. 

## Patch details
A supplied unix script clones Quark VPD from the Git repo, copies over the new sourcefiles and includes the necessary #include statements in the appropriate locations.
In video.ino, the DBGSerial is instanciated to (0), but that creates problems referencing this externally, whereas the default Arduino Serial is already mapped. So DBGSerial is renamed to Serial. The speed is reduced from 500000 to 115200; the MOS currently has no inbound hardware flowcontrol and is overrun at higher speeds than this. In order to achieve this speed at all, some code was written in z80 assembly.

For people using this from Windows, consider installing the Windows Subsystem for Linux (WSL) and use the script from there.

## Hexload command
Download the hexload.bin file and place it in the /mos/ directory on the microSD card, so it is available as a MOS command.

## Code details
- The MOS requests transfer using VDU 23,28 currently. I might need to move this in the future, but for now, this sequence seems unused
- Because of serial speed differences and lack of flowcontrol on both serial interfaces (inbound serial-usb and VDP->CPU), the VDP having more power to decode the iHex stream, the z80 CPU just receives the inbound stream in chunks. At the end of chunk transmissions, both sides calculate CRC32 and the CPU reports the value to the VDP which then matches the results
- To avoid a complete overhaul and additional code, just to transfer bytes differently than in the current serial packet protocol, the PACKET_KEYCODE is used for transfer of all byte values. In order to transfer '0' as a value and still be able to differentiate a receipt at the CPU, the keyboard modifier byte is used as an escape sequence.