#pragma once
#define SNES_BUTTONS      12

class SnesController
{
  public:

  enum Button
  {
    B = 1,
    Y = 2,
    SELECT = 4,
    START = 6,
    UP = 16,
    DOWN = 32,
    LEFT = 64,
    RIGHT = 128,
    A = 256,
    X = 512,
    L = 1024,
    R = 2048,
  };

  int latchPin;
  int clockPin;
  int dataPin;
  uint16_t state;
  
  void init(int latch, int clock, int data)
  {
    latchPin = latch;
    clockPin = clock;
    pinMode(latchPin, OUTPUT);
    digitalWrite(latchPin, LOW);
    pinMode(clockPin, OUTPUT);
    digitalWrite(clockPin, HIGH);
    dataPin = data;
    pinMode(dataPin, INPUT_PULLUP);
    state = 0;
  }

  void poll()
  {
    uint16_t bit;
    uint8_t databit,controller;

    state = 0;
    digitalWrite(latchPin, HIGH);
    delayMicroseconds(12);
    digitalWrite(latchPin, LOW);  
    delayMicroseconds(6);

    // shift-read all bits in
    bit = 1;
    for(databit = 0; databit < SNES_BUTTONS; databit++)
    {
      if(digitalRead(dataPin)) state |= bit;
      bit = bit << 1;
      digitalWrite(clockPin, LOW);
      delayMicroseconds(6);
      digitalWrite(clockPin, HIGH);
      delayMicroseconds(6);
    }
  }
  
  void controllerToBitstring(char *str)
  {
    uint8_t i;
    char *tmp;
    uint16_t v = state;
  
  
    tmp = str + 15;
    for(i = 0; i < 16; i++)
    {
      *tmp = (v & 0x1) + 48;
      tmp--;
      v = v >> 1;
    }
    str[16] = 0;
  }
};
