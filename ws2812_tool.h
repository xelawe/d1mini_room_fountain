/**
  Arduino Uno - NeoPixel Fire
  v. 1.0
  Copyright (C) 2015 Robert Ulbricht

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN_WS2812  14
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS   12

int gv_ws2812;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN_WS2812, NEO_GRB + NEO_KHZ800);

uint16_t gv_rainbow_state;
//uint32_t fire_color   = strip.Color ( 80,  35,  0);
uint32_t fire_color   = strip.Color ( 255,  80,  0);
uint32_t off_color    = strip.Color (  0,  0,  0);
//uint32_t ice_color   = strip.Color ( 0,  216,  255);
uint32_t ice_color   = strip.Color ( 0,  80,  94);

int fader_pos;
#define fader_steps 60

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Fire simulator
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class NeoFire
{
    Adafruit_NeoPixel &strip;
  public:

    NeoFire(Adafruit_NeoPixel&);
    uint32_t fire_colors[NUMPIXELS];
    int fire_ticks[NUMPIXELS];
    void Draw();
    void Clear();
    void AddColor(uint8_t position, uint32_t color);
    void SubstractColor(uint8_t position, uint32_t color);
    uint32_t Blend(uint32_t color1, uint32_t color2);
    uint32_t Substract(uint32_t color1, uint32_t color2);
    uint32_t Fade(uint32_t color1, uint32_t color2, uint8_t position, int led);
    int16_t fade_calc_new( uint8_t old_col, uint8_t new_col, int divisor );
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Constructor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
NeoFire::NeoFire(Adafruit_NeoPixel& n_strip) : strip (n_strip)
{
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Set all colors
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void NeoFire::Draw()
{
  Clear();

  for (int i = 0; i < NUMPIXELS; i++)
  {
    AddColor(i, fire_color);
    //int r = random(255);
    int16_t r = random(355);
    if (r > 255) {
      r = 255;
    }
    uint32_t diff_color = strip.Color ( r, r / 2, r / 2);
    SubstractColor(i, diff_color);
  }

  //strip.show();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Set color of LED
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void NeoFire::AddColor(uint8_t position, uint32_t color)
{
  //  uint32_t blended_color = Blend(strip.getPixelColor(position), color);
  uint32_t blended_color = Blend(fire_colors[position], color);
  //  strip.setPixelColor(position, blended_color);
  fire_colors[position] = blended_color;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Set color of LED
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void NeoFire::SubstractColor(uint8_t position, uint32_t color)
{
  // uint32_t blended_color = Substract(strip.getPixelColor(position), color);
  uint32_t blended_color = Substract(fire_colors[position], color);
  //strip.setPixelColor(position, blended_color);
  fire_colors[position] = blended_color;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Color blending
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t NeoFire::Blend(uint32_t color1, uint32_t color2)
{
  uint8_t r1, g1, b1;
  uint8_t r2, g2, b2;
  uint8_t r3, g3, b3;

  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);

  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);

  return strip.Color(constrain(r1 + r2, 0, 255), constrain(g1 + g2, 0, 255), constrain(b1 + b2, 0, 255));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Color blending
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t NeoFire::Substract(uint32_t color1, uint32_t color2)
{
  uint8_t r1, g1, b1;
  uint8_t r2, g2, b2;
  uint8_t r3, g3, b3;
  int16_t r, g, b;

  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);

  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);

  r = (int16_t)r1 - (int16_t)r2;
  g = (int16_t)g1 - (int16_t)g2;
  b = (int16_t)b1 - (int16_t)b2;
  if (r < 0) r = 0;
  if (g < 0) g = 0;
  if (b < 0) b = 0;

  return strip.Color(r, g, b);
}

int16_t NeoFire::fade_calc_new( uint8_t old_col, uint8_t new_col, int divisor )
{
  int16_t col;

  if (old_col < new_col) {
    uint8_t diff = new_col - old_col;
    col = (int16_t)old_col + (int16_t)round((diff / (divisor)));
  } else {
    uint8_t diff = old_col - new_col;
    col = (int16_t)old_col - (int16_t)round((diff / (divisor)));
  }

  return col;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// fade from color1 to color2 with certain fader position
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t NeoFire::Fade(uint32_t color1, uint32_t color2, uint8_t position, int led)
{
  uint8_t r1, g1, b1;
  uint8_t r2, g2, b2;
  uint8_t r3, g3, b3;
  int16_t r, g, b;
  uint8_t diff;
  float incf_r, incf_g, incf_b;
  int16_t inci_r, inci_g, inci_b;

  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);

  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);

  if ( position == 1) {
    fire_ticks[led] = 0;
  }

  //  if ( led < 5 ) {
  //    Serial.print("LED: ");
  //    Serial.print(led);
  //    Serial.print(" ");


  //  Serial.print("Rot:");
  //  Serial.print(r1);
  //  Serial.print(" -> ");
  //  Serial.print(r2);
  //  Serial.print("Grün:");
  //  Serial.print(g1);
  //  Serial.print(" -> ");
  //  Serial.print(g2);
  //  Serial.print("Blau:");
  //  Serial.print(b1);
  //  Serial.print(" -> ");
  //  Serial.println(b2);
  // }


  // int divisor = fader_steps - position;
  int divisor = fader_steps - position + fire_ticks[led];
  if (divisor <= 0) {
    divisor = 1;
  }

  if (r1 < r2) {
    uint8_t diff = r2 - r1;
    incf_r = (diff   * ( fire_ticks[led] + 1 ) ) / (divisor);
    inci_r = (int16_t)round( incf_r );
    r = (int16_t)r1 + inci_r;
  } else {
    uint8_t diff = r1 - r2;
    incf_r = (diff   * ( fire_ticks[led] + 1 ) ) / (divisor);
    inci_r = (int16_t)round( incf_r );
    r = (int16_t)r1 - inci_r;
  }

  if (g1 < g2) {
    uint8_t diff = g2 - g1;
    incf_g = (diff   * ( fire_ticks[led] + 1 ) ) / (divisor);
    inci_g = (int16_t)round( incf_g );
    g = (int16_t)g1 + inci_g;
  } else {
    uint8_t diff = g1 - g2;
    incf_g = (diff   * ( fire_ticks[led] + 1 ) ) / (divisor);
    inci_g = (int16_t)round( incf_g );
    g = (int16_t)g1 - inci_g;
  }

  if (b1 < b2) {
    uint8_t diff = b2 - b1;
    incf_b = (diff   * ( fire_ticks[led] + 1 ) ) / (divisor);
    inci_b = (int16_t)round( incf_b );
    b = (int16_t)b1 + inci_b;
  } else {
    uint8_t diff = b1 - b2;
    incf_b = (diff   * ( fire_ticks[led] + 1 ) ) / (divisor);
    inci_b = (int16_t)round( incf_b );
    b = (int16_t)b1 - inci_b;
  }

  if ( inci_r == 0 && inci_g == 0 && inci_b == 0 ) {
    fire_ticks[led]++;
  } else {
    fire_ticks[led] = 0;
  }

  //  if ( led < 5 ) {
  //    Serial.print("Tick: ");
  //    Serial.print(fire_ticks[led] );
  //    Serial.print(", ");
  //
  //    Serial.print("R: ");
  //    Serial.print(inci_r);
  //    Serial.print(" ");
  //    Serial.print("G: ");
  //    Serial.print(inci_g);
  //    Serial.print(" ");
  //    Serial.print("B: ");
  //    Serial.print(inci_b);
  //    Serial.print(" ");
  //  }

  return strip.Color(r, g, b);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Every LED to black
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void NeoFire::Clear()
{
  for (uint16_t i = 0; i < strip.numPixels (); i++)
    //strip.setPixelColor(i, off_color);
    fire_colors[i] = off_color;
}

NeoFire fire(strip);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Fill the dots one after the other with a color
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    if (wait != 0) {
      strip.show();
      delay(wait);
    }
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint32_t Wheel_new(uint16_t iv_WheelPos) {

  if (iv_WheelPos < (255 + 1)) {
    return strip.Color(255, 0, iv_WheelPos);
  }
  if  (iv_WheelPos < (255 * 2 + 1)) {
    return strip.Color(255 - (iv_WheelPos - 255), 0, 255);
  }

}
//
//// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//void display_hour( int iv_hour ) {
//  int lv_hour = iv_hour;
//
//  if ( lv_hour > 11 ) {
//    lv_hour = lv_hour - 12;
//  }
//
//  // circle runs counte clockwise
//  lv_hour = 11 - lv_hour;
//
//  strip.setPixelColor(lv_hour, strip.Color(255, 255, 255));
//
//}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void do_WS2812_newcol(  ) {
  fader_pos = 0;
  fire.Draw();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void fireshow() {

  fader_pos++;
  //Serial.print("Fader: ");
  //Serial.println(fader_pos );
  for (int i = 0; i < NUMPIXELS; i++)
  {
    strip.setPixelColor(i, fire.Fade(strip.getPixelColor(i), fire.fire_colors[i], fader_pos, i) );
  }

  strip.show();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void do_WS2812_step(  ) {

  if (relayState == relStateOFF) {
    if (gv_ws2812 == 1 ) {
      gv_ws2812 = 0;

      //colorWipe(strip.Color(0, 0, 0), 50); // Black
      colorWipe(off_color, 50);
    }
    return;
  } else {
    if (gv_ws2812 == 0 ) {
      gv_ws2812 = 1;

      // colorWipe(strip.Color(255, 80, 0), 50);
      colorWipe(fire_color, 50);
    }
    fireshow();


    return;
  }

  colorWipe(Wheel(gv_rainbow_state & 255), 0);

  //display_hour( hour( ) );

  gv_rainbow_state++;
  //if (gv_rainbow_state > (255 * 6)) {
  if (gv_rainbow_state > 255) {
    gv_rainbow_state = 0;
  }

  strip.show();

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void do_WS2812_col_test( ) {
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  //Alarm.delay(500);
  delay(500);
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  //Alarm.delay(500);
  delay(500);
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  //Alarm.delay(500);
  delay(500);

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void init_ws2812( ) {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  do_WS2812_col_test( );

  do_WS2812_newcol( );

  gv_rainbow_state = 0;
  gv_ws2812 = 1;

  do_WS2812_step(  );
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void do_WS2812_tick() {

  if (fader_pos >= fader_steps ) {
    do_WS2812_newcol();
  }

  do_WS2812_step(  );

  //Serial.println();
}
