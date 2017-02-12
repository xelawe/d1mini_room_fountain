#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN_WS2812  14
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS   12

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN_WS2812, NEO_GRB + NEO_KHZ800);

uint16_t gv_rainbow_state;



// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    if (wait != 0) {
      strip.show();
      delay(wait);
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

uint32_t Wheel_new(uint16_t iv_WheelPos) {

  if (iv_WheelPos < (255 + 1)) {
    return strip.Color(255, 0, iv_WheelPos);
  }
  if  (iv_WheelPos < (255 * 2 + 1)) {
    return strip.Color(255 - (iv_WheelPos - 255), 0, 255);
  }

}

void display_hour( int iv_hour ) {
  int lv_hour = iv_hour;

  if ( lv_hour > 11 ) {
    lv_hour = lv_hour - 12;
  }

  strip.setPixelColor(lv_hour, strip.Color(255, 255, 255));

}

void do_WS2812_step(  ) {

  if (relayState == relStateOFF) {
    colorWipe(strip.Color(0, 0, 0), 50); // Black
    return;
  }

  colorWipe(Wheel(gv_rainbow_state & 255), 0);

  display_hour( hour( ) );

  gv_rainbow_state++;
  //if (gv_rainbow_state > (255 * 6)) {
  if (gv_rainbow_state > 255) {
    gv_rainbow_state = 0;
  }
  
  strip.show();

}

void do_WS2812_col_test( ) {
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
}

void init_ws2812( ) {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  do_WS2812_col_test( );

  gv_rainbow_state = 0;
  do_WS2812_step(  );
}
