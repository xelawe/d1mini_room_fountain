
void tick()
{
  //toggle state
  int state = digitalRead(PIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(PIN_LED, !state);     // set pin to the opposite state
}

void tick_sens() {
  gv_tick_sens = true;
}

void setState(int s) {

  relayState = s;

  // When there is no water -> turn relay off!
  if ( relayState == relStateON && InputState == inpStateLow ) {
    relayState = relStateOFF;
  }

  DebugPrintln(relayState);

  digitalWrite(PIN_RELAY, relayState);

  char *lv_power_c = "OFF";

  if (relayState == relStateOFF) {
    digitalWrite(PIN_LED, LEDStateOFF);
    lv_power_c = "OFF";
  }
  else {
    digitalWrite(PIN_LED, LEDStateON);
    lv_power_c = "ON";
  }

  client.publish(mqtt_GetTopic_P(gv_ptopic, mqtt_pre_stat, gv_clientname, (PGM_P)F("POWER")), lv_power_c, true);
}
//
//void AlarmOff( ) {
//  turnOff( );
//}
//
//void AlarmOn( ) {
//  turnOn( );
//}

void turnOn() {
  setState(relStateON);
}

void turnOff() {
  setState(relStateOFF);
}

ICACHE_RAM_ATTR void toggleState() {
  cmd = CMD_BUTTON_CHANGE;
}

ICACHE_RAM_ATTR void toggleInput() {
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

void check_waterlevel( ) {
  int currentStateInp = digitalRead(PIN_INPUT);

  if (currentStateInp == inpStateLow) {
    //client.publish(mqtt_pubtopic_wl, "0", true);
    gv_waterlevel = 0;
  }
  else {
    //client.publish(mqtt_pubtopic_wl, "1", true);
    gv_waterlevel = 1;
  }
  pub_sens();

  if (currentStateInp != InputState) {
    if (currentStateInp == inpStateLow) {
      turnOff();
    }
    InputState = currentStateInp;
  }
}


void loop() {

  check_ota();

  check_mqtt_reset();

  switch (cmd_inp) {
    case CMD_WAIT:
      break;
    case CMD_INPUT_CHANGE:
      {
        check_waterlevel( );
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

  if (gv_tick_sens) {
    check_waterlevel( );
    gv_tick_sens = false;
  }

  //  check_time();

  //  Alarm.delay( 0 );

}
