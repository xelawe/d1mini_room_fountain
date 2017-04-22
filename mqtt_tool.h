#ifndef MQTT_TOOL_H
#define MQTT_TOOL_H

#include <PubSubClient.h>

const char* mqtt_server = "mqtt.cytron.tiscali.at";
const char* mqtt_subtopic_rl = "ATSH28/UG/Z1/RL/1/set";
const char* mqtt_pubtopic_rl = "ATSH28/UG/Z1/RL/1/state";
//const char* mqtt_subtopic = "ATSH28/OG/Z1/SW/+";
const char* mqtt_pubtopic = "ATSH28/UG/Z1/SW/1/set";
const char* mqtt_pubtopic_wl = "ATSH28/UG/Z1/WL/1/state";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void reconnect_mqtt() {
  // Loop until we're reconnected
  while (!client.connected()) {
    DebugPrint("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("D1miniRF")) {
      DebugPrintln("connected");
      // Once connected, publish an announcement, retained
      //client.publish(mqtt_pubtopic, "hello world");
      
      // ... and resubscribe
      //client.subscribe(mqtt_subtopic);
      client.subscribe(mqtt_subtopic_rl);
      } else {
      DebugPrint("failed, rc=");
      DebugPrint(client.state());
      DebugPrintln(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void init_mqtt(MQTT_CALLBACK_SIGNATURE) {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void check_mqtt() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();
}

void pub_mqtt_toggle(){
  client.publish(mqtt_pubtopic, "2");
}

#endif
