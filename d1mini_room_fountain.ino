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
//#define PIN_WS2812 14

#define relStateOFF LOW
#define relStateON HIGH
#define LEDStateOFF HIGH
#define LEDStateON LOW
#define butStateOFF HIGH
#define butStateON LOW
#define inpStateLow LOW // Low Water state
int relayState = relStateOFF;

const char *gc_hostname = "D1miniRF";

//#include "tools_wifi.h"
//#include "ota_tool.h"
#include "time_tool.h"
#include "ws2812_tool.h"
#include "mqtt_tool.h"

//for LED status
#include <Ticker.h>
Ticker ticker;

const int CMD_WAIT = 0;
const int CMD_BUTTON_CHANGE = 1;
int cmd = CMD_WAIT;

const int CMD_INPUT_CHANGE = 1;
int cmd_inp = CMD_WAIT;


//inverted button state
int buttonState = HIGH;
int InputState = inpStateLow;

static long startPress = 0;

void tick()
{
  //toggle state
  int state = digitalRead(PIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(PIN_LED, !state);     // set pin to the opposite state
}

void setState(int s) {

  relayState = s;

  // When there is no water -> turn relay off!
  if ( relayState == relStateON && InputState == inpStateLow ) {
    relayState = relStateOFF;
  }

  DebugPrintln(relayState);

  digitalWrite(PIN_RELAY, relayState);
  if (relayState == relStateOFF) {
    digitalWrite(PIN_LED, LEDStateOFF);
    client.publish(mqtt_pubtopic_rl, "0", true);
  }
  else {
    digitalWrite(PIN_LED, LEDStateON);
    client.publish(mqtt_pubtopic_rl, "1", true);
  }

}

void AlarmOff( ) {
  turnOff( );
}

void AlarmOn( ) {
  turnOn( );
}

void turnOn() {
  setState(relStateON);
}

void turnOff() {
  setState(relStateOFF);
}

void toggleState() {
  cmd = CMD_BUTTON_CHANGE;
}

void toggleInput() {
  cmd_inp = CMD_INPUT_CHANGE;
}


void toggle() {

  DebugPrintln("toggle state");
  int lv_s = relayState == relStateOFF ? relStateON : relStateOFF;
  setState(lv_s);
}

void restart() {
  ESP.reset();
  delay(1000);
}

void reset() {
  //reset settings to defaults
  /*
    WMSettings defaults;
    settings = defaults;
    EEPROM.begin(512);
    EEPROM.put(0, settings);
    EEPROM.end();
  */
  //reset wifi credentials
  WiFi.disconnect();
  delay(1000);
  ESP.reset();
  delay(1000);
}

void callback_mqtt(char* topic, byte* payload, unsigned int length) {
  DebugPrint("Message arrived [");
  DebugPrint(topic);
  DebugPrint("] ");
  for (int i = 0; i < length; i++) {
    DebugPrint((char)payload[i]);
  }
  DebugPrintln();

  // Switch on the LED if an 1 was received as first character
  switch ((char)payload[0]) {
    case '0':
      turnOff();
      break;
    case '1':
      turnOn();
      break;
    case '2':
      toggle();
      break;
  }
}

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

  init_time();

  ticker.detach();

  turnOff();

  init_ws2812( );

  //init_mqtt(callback_mqtt);
  //init_mqtt(gv_clientname);
  init_mqtt_local();



  Alarm.alarmRepeat(6, 0, 0, AlarmOn);
  Alarm.alarmRepeat(20, 15, 0, AlarmOff);

  Alarm.timerRepeat(fader_steps, do_WS2812_newcol);

  Alarm.timerRepeat(1, do_WS2812_step);

  DebugPrintln("done setup");
}

void loop() {

  check_ota();

  check_mqtt();

  switch (cmd_inp) {
    case CMD_WAIT:
      break;
    case CMD_INPUT_CHANGE:
      {
        int currentStateInp = digitalRead(PIN_INPUT);
        if (currentStateInp != InputState) {
          if (currentStateInp == inpStateLow) {
            turnOff();
          }
          InputState = currentStateInp;

          if (InputState == inpStateLow) {
            client.publish(mqtt_pubtopic_wl, "0", true);
          }
          else {
            client.publish(mqtt_pubtopic_wl, "1", true);
          }
        }
        cmd_inp = CMD_WAIT;
        break;
      }
  }

  switch (cmd) {
    case CMD_WAIT:
      break;
    case CMD_BUTTON_CHANGE:
      {
        int currentState = digitalRead(PIN_BUTTON);
        if (currentState != buttonState) {
          if (buttonState == butStateON && currentState == butStateOFF) {
            long duration = millis() - startPress;
            if (duration < 10) {
              DebugPrintln("too short press - no action");
            } else if (duration < 5000) {
              DebugPrintln("short press - toggle relay");
              toggle();
              pub_mqtt_toggle();
            } else if (duration < 10000) {
              DebugPrintln("medium press - reset");
              restart();
            } else if (duration < 60000) {
              DebugPrintln("long press - reset settings");
              reset();
            }
          } else if (buttonState == butStateOFF && currentState == butStateON) {
            startPress = millis();
          }
          buttonState = currentState;
        }
        cmd = CMD_WAIT;
        break;
      }
  }

  check_time();

  Alarm.delay( 0 );

}
