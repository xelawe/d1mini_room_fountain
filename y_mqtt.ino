void callback_mqtt(char* topic, byte* payload, unsigned int length) {
  DebugPrint("Message arrived [");
  DebugPrint(topic);
  String message_string = "";

  for (int i = 0; i < length; i++) {
    DebugPrint((char)payload[i]);
    //fill up the message string
    message_string.concat((char)payload[i]);
  }
  DebugPrintln();

  if (message_string.equalsIgnoreCase(F("ON"))) {
    turnOn();
  }

  if (message_string.equalsIgnoreCase(F("OFF"))) {
    turnOff();
  }
}

void init_mqtt_local() {
  init_mqtt(gv_clientname);

  
  add_subtopic(mqtt_GetTopic_P(gv_stopic_0, 0, gv_clientname, get_stopic_ix(0)), callback_mqtt);
}

void pub_mqtt_toggle() {
  //client.publish(mqtt_pubtopic, "2");
}

void pub_power() {
  if ( gv_power == gv_power_old) {
    return;
  }

  char *lv_power_c = "OFF";
  if (gv_power == 0) {
    lv_power_c = "OFF";
  } else {
    lv_power_c = "ON";
  }

  if (!client.publish(mqtt_GetTopic_P(gv_ptopic, mqtt_pre_stat, gv_clientname, (PGM_P)F("POWER")), lv_power_c, true)) {
    DebugPrintln("pub failed!");
  } else {
    gv_power_old = gv_power;
    DebugPrintln("pub ok!");
  }
}

void pub_sens() {

  char buffer[256];
  //dtostrf(fire.NUMFirePIXELS, 0, 0, buffer);

  // Allocate the JSON document
  //
  // Inside the brackets, 200 is the RAM allocated to this document.
  // Don't forget to change this value to match your requirement.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<200> jsondoc;

  // Add values in the document

  JsonObject lv_ldr = jsondoc.createNestedObject("Level");
  lv_ldr["Water"] = gv_waterlevel;

  int n  = serializeJson(jsondoc, buffer);

  //client.publish(mqtt_pubtopic_sensor, buffer, true);
  client.publish(mqtt_GetTopic_P(gv_ptopic, mqtt_pre_tele, gv_clientname, (PGM_P)F("SENSOR")), buffer, true);
}

char *get_stopic_ix( int ix ) {
  strcpy_P(gv_sbuffer, (char*)pgm_read_dword(&(gt_stopic[ix])));
  //strcpy_P(gv_buffer, );
  return gv_sbuffer;
}
