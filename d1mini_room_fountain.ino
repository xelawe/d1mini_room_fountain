/*
  Room Fountain control
*/
#include <cy_serdebug.h>
#include <cy_serial.h>

#include "cy_wifi.h"
#include "cy_ota.h"


#define PIN_BUTTON 13
#define PIN_RELAY 4
#define PIN_LED BUILTIN_LED
#define PIN_INPUT 12
#define PIN_WS2812 14

#define relStateOFF LOW
#define relStateON HIGH
#define LEDStateOFF HIGH
#define LEDStateON LOW
#define butStateOFF HIGH
#define butStateON LOW
#define inpStateLow LOW // Low Water state
int relayState = relStateOFF;

const char *gc_hostname = "D1miniRF";

//#include "time_tool.h"
#include "ws2812_tool.h"

//for LED status
#include <Ticker.h>
Ticker ticker;
Ticker WS2812_ticker;

const int CMD_WAIT = 0;
const int CMD_BUTTON_CHANGE = 1;
int cmd = CMD_WAIT;

const int CMD_INPUT_CHANGE = 1;
int cmd_inp = CMD_WAIT;

boolean gv_tick_sens = true;

//inverted button state
int buttonState = HIGH;
int InputState = inpStateLow;

int gv_waterlevel = 2;

static long startPress = 0;

void setup() {

  cy_serial::start(__FILE__);

  pinMode(PIN_LED, OUTPUT);  // initialize onboard LED as output
  digitalWrite(PIN_LED, LEDStateOFF);   // turn off LED with voltage LOW
  pinMode(PIN_RELAY, OUTPUT);  // initialize onboard LED as output
  digitalWrite(PIN_RELAY, relStateOFF);   // turn off LED with voltage LOW
  pinMode(PIN_BUTTON, INPUT_PULLUP);  // initialize onboard LED as output
  pinMode(PIN_INPUT, INPUT_PULLUP);
  InputState = digitalRead(PIN_INPUT);
  attachInterrupt(PIN_INPUT, toggleInput, CHANGE);

  wifi_init(gc_hostname);


  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.2, tick);
  delay(500);

  init_ota(gv_clientname);

  attachInterrupt(PIN_BUTTON, toggleState, CHANGE);

  init_ws2812( );

  //init_time();

  ticker.detach();

  turnOff();

  init_ws2812( );

  init_mqtt_local();

  // Steuerung in Zukunft über MQTT
  // Alarm.alarmRepeat(6, 0, 0, AlarmOn);
  // Alarm.alarmRepeat(20, 15, 0, AlarmOff);

  WS2812_ticker.attach(0.5, do_WS2812_tick);

  ticker.attach(60, tick_sens);

  DebugPrintln("done setup");
}
