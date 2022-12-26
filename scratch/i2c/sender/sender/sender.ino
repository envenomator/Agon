// Include the required Wire library for I2C
#include <Wire.h>

int x = 0;
void setup() {
  Serial.begin(115200);
  // Start the I2C Bus as Master
  Wire.begin();
  Serial.println("\nI2C sender\n"); 
  pinMode(16, OUTPUT);
}

void loop() {
  if(x) digitalWrite(16, HIGH);
  else digitalWrite(16, LOW);
  Serial.println("Next");
  Serial.println(x);
  Wire.beginTransmission(9); // transmit to device #9
  Wire.write(x);              // sends x 
  Wire.endTransmission();    // stop transmitting
  x = (x == 0)?1:0;
  delay(500);
}