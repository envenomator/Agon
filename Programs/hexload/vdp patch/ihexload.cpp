//
// Title:         iHexLoad
// Author:        Jeroen Venema
// Contributors:  Kimmo Kulovesi (kk_ihex library https://github.com/arkku/ihex)
// Created:       05/10/2022
// Last Updated:  08/10/2022
//
// Modinfo:
//
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <HardwareSerial.h>
#include "ihexload.h"
#include "agon.h"

extern void vdu(uint8_t c);
extern void send_packet(uint8_t code, uint8_t len, uint8_t data[]);
extern void printFmt(const char *format, ...);
extern uint8_t readByte();

// helper method to send a uint8_t transparently as a KEYCODE
/*
void send_byte(uint8_t b)
{
  uint8_t packet[2] = {0,0};

  switch(b)
  {
    case 0: // escape with 0x01/0x02 - meaning 0x00
      packet[0] = 1;
      send_packet(PACKET_KEYCODE, sizeof packet, packet);                       
      packet[0] = 2;
      send_packet(PACKET_KEYCODE, sizeof packet, packet);                       
      break;
    case 1: // escape with 0x01/0x01 - meaning 0x01
      packet[0] = 1;
      send_packet(PACKET_KEYCODE, sizeof packet, packet);                       
      packet[0] = 1;
      send_packet(PACKET_KEYCODE, sizeof packet, packet);                           
      break;
    default:
      packet[0] = b;
      send_packet(PACKET_KEYCODE, sizeof packet, packet);                           
      break;
  }
}
*/
void send_byte(uint8_t b)
{
  uint8_t packet[2] = {0,0};

  switch(b)
  {
    case 0: // escape with 0x01/0x02 - meaning 0x00
      packet[0] = 1;
      packet[1] = 1;
      send_packet(PACKET_KEYCODE, sizeof packet, packet);                       
      break;
    case 1: // escape with 0x01/0x01 - meaning 0x01
      packet[0] = 1;
      packet[1] = 0;
      send_packet(PACKET_KEYCODE, sizeof packet, packet);                       
      break;
    default:
      packet[0] = b;
      send_packet(PACKET_KEYCODE, sizeof packet, packet);                           
      break;
  }
}

//
// CRC32 functions
//
uint32_t crc32(const char *s, uint32_t n, bool firstrun)
{
  static uint32_t crc;
  static uint32_t crc32_table[256];
   
  if(firstrun)
  {
    for(uint32_t i = 0; i < 256; i++)
    {
      uint32_t ch = i;
      crc = 0;
      for(uint32_t j = 0; j < 8; j++)
      {
        uint32_t b=(ch^crc)&1;
        crc>>=1;
        if(b) crc=crc^0xEDB88320;
        ch>>=1;
      }
      crc32_table[i] = crc;
    }        
    crc = 0xFFFFFFFF;
  }
  
  for(uint32_t i=0;i<n;i++)
  {
    char ch=s[i];
    uint32_t t=(ch^crc)&0xFF;
    crc=(crc>>8)^crc32_table[t];
  }

  return ~crc;
}
//
// ihex callbackup function
bool ihex_data_read (struct ihex_state *ihex, ihex_record_type_t type, bool checksum_error)
{
    unsigned long address_delta;
    char o[128];
    uint8_t count,xsb;
    
    if (type == IHEX_DATA_RECORD) {

      // Mark errors
      ihex->checksum_error |= checksum_error;
      ihex->length_error |= (ihex->length < ihex->line_length);

      // first write?
      if(ihex->next_address == 0)
      {
        ihex->next_address = ihex->address; // start first record at first given address, so everything becomes relative
        printFmt("Start address 0x%lx\r\n",ihex->address);
        // send 24bit start address
        xsb = (uint8_t)(ihex->address & 0xFF); // LSB
        send_byte(xsb);
        xsb = (uint8_t)((ihex->address & 0xFF00) >> 8);
        send_byte(xsb);
        xsb = (uint8_t)((ihex->address & 0xFF0000) >> 16); // MSB
        send_byte(xsb);
      }

      // first check if this record aligns with the previous, or if we do need to pad 0x00's between them
      address_delta = ihex->address - ihex->next_address;
      if(address_delta)
      {
        //printFmt("Addr 0x%lx len 0x%04x padding\r\n", ihex->next_address, address_delta);
        send_byte(address_delta);
        char zero = 0; // need this as a source for crc32
        while(address_delta) // send padding bytes
        {
          send_byte(0);
          crc32(&zero,1,false);
          ihex->total += 1;
          address_delta--;
        }        
      }

      // prepare anticipated address for next record
      ihex->next_address = ihex->address + ihex->length;

      //printFmt("Addr 0x%lx len 0x%02x\r\n",ihex->address, ihex->length);
      printFmt(".");      
      //
      // now start sending the actual bytes in this record
      //
      ihex->total += ihex->length;

      // start receiving ihex->length bytes
      send_byte(ihex->length);
      for(count = 0; count < ihex->length; count++) send_byte(ihex->data[count]);
      // calculate crc for this record
      crc32((const char *)ihex->data, ihex->length, false); // update
    }
    return true;
}

// Ihex engine
//
void vdu_sys_hexload(void)
{
  uint32_t crc_sent,crc_rec;
  struct ihex_state ihex;
  char o[128];
  unsigned int count = 0;
  bool serialdone = false;
  bool error = false;
  char buffer[256];
  char *ptr;
  uint8_t c;
  uint8_t bytecount,recordtype;
  uint8_t state = 0;

  ihex_begin_read(&ihex);
  crc32(0,0,true); // init crc table
  recordtype = 0; // initial state
  
  printFmt("Receiving Intel HEX records\r\n");
  ptr = buffer;
  while(!serialdone)
  {
    if(Serial.available())
    {
      c = Serial.read();
      switch(state)
      {
        case 0: // nothing or end-of-line received previously
          if(c == ':') state = 1;
          break;
        case 1: // start code received previously
          bytecount = (c - '0') << 4; // high nibble
          state = 2;
          break;
        case 2: // first uint8_t of bytecount received previously
          bytecount = bytecount | (c - '0'); // low nibble  
          state = 3;
          break;
        case 3 ... 6:
          state++; // next - skip 4 nibbles of address
          break;
        case 7:
          recordtype = (c - '0') << 4; // high nibble
          state = 8;
          break;
        case 8:
          recordtype = recordtype | (c - '0'); // low nibble
          state = 9;
          break;
        default:
          if(c == ':')
          {
            serialdone = (recordtype == 1);
            state = 1;
          }
          else
          {
            if((c == 0x0D) || (c == 0x0A))
            {
              serialdone = (recordtype == 1);
              state = 0;
            }
            else state++;             
          }
        break;
      }
      *ptr = c; // store in buffer
      ptr++;
      count++;      

      if(count == 128) // Process once per this number of characters received
      {
        ihex_read_bytes(&ihex, buffer, count);
        count = 0;
        ptr = buffer;
      }
    }
  }

  ihex_read_bytes(&ihex, buffer, count); // read remaining count characters
  ihex_end_read(&ihex); // flush remaining processed buffer bytes to ez80
  printFmt("\r\n");
  if(ihex.checksum_error || ihex.length_error)
  {
    if(ihex.checksum_error) printFmt("Input checksum error(s)\r\n");
    if(ihex.length_error)   printFmt("Input line(s) too long\r\n");    
  }
  else printFmt("%d bytes\r\n", ihex.total);

  send_byte(0x0); // signal termination of transmission to ez80

  // calculate crc and match receiving crc
  crc_sent = crc32(0,0,false);
  crc_rec = readByte(); // LSB
  crc_rec |= (readByte() << 8);
  crc_rec |= (readByte() << 16);
  crc_rec |= (readByte() << 24);
  if(crc_sent == crc_rec) printFmt("CRC32 OK\r\n");
  else printFmt("CRC32 ERROR\r\n");
}

#define IHEX_START ':'
#define ADDRESS_HIGH_MASK ((ihex_address_t) 0xFFFF0000U)
enum ihex_read_state {
    READ_WAIT_FOR_START = 0,
    READ_COUNT_HIGH = 1,
    READ_COUNT_LOW,
    READ_ADDRESS_MSB_HIGH,
    READ_ADDRESS_MSB_LOW,
    READ_ADDRESS_LSB_HIGH,
    READ_ADDRESS_LSB_LOW,
    READ_RECORD_TYPE_HIGH,
    READ_RECORD_TYPE_LOW,
    READ_DATA_HIGH,
    READ_DATA_LOW
};
#define IHEX_READ_RECORD_TYPE_MASK 0x07
#define IHEX_READ_STATE_MASK 0x78
#define IHEX_READ_STATE_OFFSET 3


void
ihex_begin_read (struct ihex_state * const ihex) {
    ihex->address = 0;
#ifndef IHEX_DISABLE_SEGMENTS
    ihex->segment = 0;
#endif
    ihex->flags = 0;
    ihex->line_length = 0;
    ihex->length = 0;
    ihex->total = 0;  // modified by JV
    ihex->next_address = 0;  // modified by JV
    ihex->checksum_error = 0;
    ihex->length_error = 0;
}

#ifndef IHEX_DISABLE_SEGMENTS
void
ihex_read_at_segment (struct ihex_state * const ihex, ihex_segment_t segment) {
    ihex_begin_read(ihex);
    ihex->segment = segment;
}
#endif

void
ihex_end_read (struct ihex_state * const ihex) {
    uint_fast8_t type = ihex->flags & IHEX_READ_RECORD_TYPE_MASK;
    uint_fast8_t sum;
    if ((sum = ihex->length) == 0 && type == IHEX_DATA_RECORD) {
        return;
    }
    {
        // compute and validate checksum
        const uint8_t * const eptr = ihex->data + sum;
        const uint8_t *r = ihex->data;
        sum += type + (ihex->address & 0xFFU) + ((ihex->address >> 8) & 0xFFU);
        while (r != eptr) {
            sum += *r++;
        }
        sum = (~sum + 1U) ^ *eptr; // *eptr is the received checksum
    }
    if (ihex_data_read(ihex, type, (uint8_t) sum)) {
        if (type == IHEX_EXTENDED_LINEAR_ADDRESS_RECORD) {
            ihex->address &= 0xFFFFU;
            ihex->address |= (((ihex_address_t) ihex->data[0]) << 24) |
                             (((ihex_address_t) ihex->data[1]) << 16);
#ifndef IHEX_DISABLE_SEGMENTS
        } else if (type == IHEX_EXTENDED_SEGMENT_ADDRESS_RECORD) {
            ihex->segment = (ihex_segment_t) ((ihex->data[0] << 8) | ihex->data[1]);
#endif
        }
    }
    ihex->length = 0;
    ihex->flags = 0;
}

void
ihex_read_byte (struct ihex_state * const ihex, const char byte) {
    uint_fast8_t b = (uint_fast8_t) byte;
    uint_fast8_t len = ihex->length;
    uint_fast8_t state = (ihex->flags & IHEX_READ_STATE_MASK);
    ihex->flags ^= state; // turn off the old state
    state >>= IHEX_READ_STATE_OFFSET;

    if (b >= '0' && b <= '9') {
        b -= '0';
    } else if (b >= 'A' && b <= 'F') {
        b -= 'A' - 10;
    } else if (b >= 'a' && b <= 'f') {
        b -= 'a' - 10;
    } else if (b == IHEX_START) {
        // sync to a new record at any state
        state = READ_COUNT_HIGH;
        goto end_read;
    } else {
        // ignore unknown characters (e.g., extra whitespace)
        goto save_read_state;
    }

    if (!(++state & 1)) {
        // high nybble, store temporarily at end of data:
        b <<= 4;
        ihex->data[len] = b;
    } else {
        // low nybble, combine with stored high nybble:
        b = (ihex->data[len] |= b);
        // We already know the lowest bit of `state`, dropping it may produce
        // smaller code, hence the `>> 1` in switch and its cases.
        switch (state >> 1) {
        default:
            // remain in initial state while waiting for :
            return;
        case (READ_COUNT_LOW >> 1):
            // data length
            ihex->line_length = b;
#if IHEX_LINE_MAX_LENGTH < 255
            if (b > IHEX_LINE_MAX_LENGTH) {
                ihex_end_read(ihex);
                return;
            }
#endif
            break;
        case (READ_ADDRESS_MSB_LOW >> 1):
            // high byte of 16-bit address
            ihex->address &= ADDRESS_HIGH_MASK; // clear the 16-bit address
            ihex->address |= ((ihex_address_t) b) << 8U;
            break;
        case (READ_ADDRESS_LSB_LOW >> 1):
            // low byte of 16-bit address
            ihex->address |= (ihex_address_t) b;
            break;
        case (READ_RECORD_TYPE_LOW >> 1):
            // record type
            if (b & ~IHEX_READ_RECORD_TYPE_MASK) {
                // skip unknown record types silently
                return;
            }
            ihex->flags = (ihex->flags & ~IHEX_READ_RECORD_TYPE_MASK) | b;
            break;
        case (READ_DATA_LOW >> 1):
            if (len < ihex->line_length) {
                // data byte
                ihex->length = len + 1;
                state = READ_DATA_HIGH;
                goto save_read_state;
            }
            // end of line (last "data" byte is checksum)
            state = READ_WAIT_FOR_START;
        end_read:
            ihex_end_read(ihex);
        }
    }
save_read_state:
    ihex->flags |= state << IHEX_READ_STATE_OFFSET;
}

void
ihex_read_bytes (struct ihex_state * ihex,
                 const char * data,
                 ihex_count_t count) {
    while (count > 0) {
        ihex_read_byte(ihex, *data++);
        --count;
    }
}
