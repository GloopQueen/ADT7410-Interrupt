
/**************************************************************************/
/*!
  This is a demo for the Adafruit ADT7410 breakout
  ----> http://www.adafruit.com/products/4089
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
*/
/**************************************************************************/

/*
   INTERRUPT NOTES:

   This is done assuming your sensor is defined as 'tempsensor',
   if you defined it as something else change those parts.
   You'll need to re-run this function every time your interrupt fires
   to reset it (assuming your ADT7410's still in the default modes.)
   I'm not sure why, I likely missed something, but it works.

   For this sketch, I connected the ADT's int pin to the Arduino's digital 3.
   All other wiring follows the Adafruit ADT article (it's just a regular i2c hookup)

*/



#include <Wire.h>
#include "Adafruit_ADT7410.h"

// Create the ADT7410 temperature sensor object
Adafruit_ADT7410 tempsensor = Adafruit_ADT7410();

const int interruptPin = 3;
bool tempInterrupt = false;

int roomTemp;

void setup() {

  //This sets up the interrupt code, which will run when pin 3 drops low.
  pinMode(interruptPin, INPUT);
  attachInterrupt(1, my_ISR, FALLING);

  Serial.begin(115200);
  Serial.println("ADT7410 demo");

  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x49) for example
  if (!tempsensor.begin()) {
    Serial.println("Couldn't find ADT7410!");
    while (1);
  }

  // sensor takes 250 ms to get first readings
  delay(250);


  //Reads the current temperature and sets the interrupts just above/below it.
  //Try putting your fingertip on the chip to fire the interrupt!
  roomTemp = tempsensor.readTempC();
  tempInterruptSet(roomTemp - 2, roomTemp + 2);
}

void my_ISR() {
  tempInterrupt = true;
}

void loop() {
  
  delay(1200);
  
  // Read and print out the temperature, then convert to *F
  float c = tempsensor.readTempC();
  float f = c * 9.0 / 5.0 + 32;
  Serial.print(digitalRead(interruptPin));
  Serial.print(" Temp: "); Serial.print(c); Serial.print("*C\t");
  Serial.print(f); Serial.println("*F");


  //Do something about the interrupt, if there was one
  if (tempInterrupt == true) {
    tempInterrupt = false;
    Serial.println("Interrupt!");
  }


  //Manually reset the interrupt by sending anything on serial
  if (Serial.available() > 0) {

    //This while loop is just to clear out the serial buffer
    while (Serial.available() > 0){
          char incoming = Serial.read();
    }
    tempInterruptSet(roomTemp - 2, roomTemp + 2);
    Serial.println("Interrupt set again.");
  }

}


void tempInterruptSet(int coldTemp, int hotTemp) {
  //For some reason you gotta call begin() again. You probably actually don't and I'm missing something
  tempsensor.begin();
  delay(250);

  //Temperatures need to be scaled up due to the accuracy of this sensor
  hotTemp = hotTemp * 128;
  coldTemp = coldTemp * 128;

  //Register addresses here are from the datasheet
  tempsensor.write8(0x04, highByte(hotTemp));
  tempsensor.write8(0x05, lowByte(hotTemp));
  tempsensor.write8(0x06, highByte(coldTemp));
  tempsensor.write8(0x07, lowByte(coldTemp));
}
