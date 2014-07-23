#include <SPI.h>
#include <boards.h>
#include <RBL_nRF8001.h>
#include "Boards.h"
#include <Adafruit_NeoPixel.h>
#include <Time.h>

#define CLOCK_PIN 2

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = leave as is
Adafruit_NeoPixel clock = Adafruit_NeoPixel(12, CLOCK_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  clock.begin();
  clock.show(); //  Initialize all pixels to 'off'
  ble_begin();
  Serial.begin(57600);
}

unsigned char buf[16] = {
  0};
unsigned char len = 0;
int bleInput = 0;
int hr = hour();
int lastHr;
int mn = minute();
int lastMn;
int counter;

void loop()
{
//  turnOffLights();
  clock.setPixelColor(hr, clock.Color(20,20,20));
  clock.setPixelColor(mn / 5, clock.Color(10,10,30));
  clock.show();
  delay(50);
  bleCallbacks();
}

void turnOffLights() {
  for(uint16_t i=0; i<clock.numPixels(); i++) {
    clock.setPixelColor(i, clock.Color(0,0,0));
    clock.show();
  }
}

void bleCallbacks() {
  if ( ble_available() ) {
    int i = 0;
    bleInput = 0;
    while ( ble_available() ) {
      if (i == 1) {
        bleInput = bleInput*10;
      }
      bleInput += asciiToInt(ble_read());
      i++;
    } 
    counter++; 
  }

  if ( ble_connected() )
  {    
    if (counter % 2 != 0) {
      clock.setPixelColor(hr, clock.Color(0,0,0));
      hr = bleInput;
      ble_write((unsigned char)* "A");
    } 
    else {
      clock.setPixelColor(mn, clock.Color(0,0,0));
      mn = bleInput;
      ble_write((unsigned char)* "B");
    }
  }
  ble_do_events();
}

int asciiToInt(int a) {
  int x = 0;
  for (uint16_t i = 0; i < floor(log10(abs(a))); i++) {
    x += ((a % 100) - 48)*((int) pow((double) 10, i));
    a = a/100;
  }
  return x;
}



