#include <Wire.h>

int LED = 16;
int x = 0;
int y = 0;
byte buffer[32];

void setup() {
  Serial.begin(115200);
  // Define the LED pin as Output
  pinMode (LED, OUTPUT);
  // Start the I2C Bus as Slave on address 9
  Wire.begin(9); 
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
}
void receiveEvent(int bytes) {
  //x = Wire.read();    // read one character from the I2C
  Wire.readBytes(buffer,2);

  //Serial.println(x);
  //Serial.printf("%d %d\n",buffer[0],buffer[1]);
}
void loop() {
  if(buffer[0] == 0)
  {
    digitalWrite(LED, LOW);
  }
  if(buffer[0] == 1)
  {
    digitalWrite(LED, HIGH);
  }
}