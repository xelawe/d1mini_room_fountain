//#ifndef MQTT_TOOL_H
//#define MQTT_TOOL_H

#include "cy_mqtt.h"

const char* mqtt_subtopic_rl = "ATSH28/UG/Z1/RL/1/set";
const char* mqtt_pubtopic_rl = "ATSH28/UG/Z1/RL/1/state";
//const char* mqtt_subtopic = "ATSH28/OG/Z1/SW/+";
const char* mqtt_pubtopic = "ATSH28/UG/Z1/SW/1/set";
const char* mqtt_pubtopic_wl = "ATSH28/UG/Z1/WL/1/state";


const char* mqtt_pubtopic_status_i_suff = "tele/status";
String mqtt_pubtopic_status_i_s;
const char* mqtt_pubtopic_status_i;

void init_mqtt_local() {

  mqtt_pubtopic_status_i_s += gv_clientname;
  mqtt_pubtopic_status_i_s += '/';
  mqtt_pubtopic_status_i_s += mqtt_pubtopic_status_i_suff;
  mqtt_pubtopic_status_i = (char*) mqtt_pubtopic_status_i_s.c_str();
  DebugPrintln(mqtt_pubtopic_status_i);


  //init_mqtt(callback_mqtt);
  init_mqtt(gv_clientname);
}

void pub_mqtt_toggle() {
  //client.publish(mqtt_pubtopic, "2");
}

//#endif
