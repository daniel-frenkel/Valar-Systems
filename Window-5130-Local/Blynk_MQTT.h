//MQTT Setup

BlynkTimer timer;

int counter = 0;
long int last_UP_change = millis();
WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* message, unsigned int length) { //Used for MQTT
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT
  // If a message is received on the topic esp32/output, you check if the message is either "OPEN" or "CLOSE". 
  // Changes the output state according to the message

  if (String(topic) == mqtt_set_topic) {
    Serial.print("Message Received: ");

    int mqtt_location = messageTemp.toInt();
    Serial.println("MQTT moving to");

    if(mqtt_location>0 && mqtt_location<=100){
    Serial.println(mqtt_location);
    move_to_position=(max_steps/100)*mqtt_location;
    command = CUSTOM_MOVE;
    }else if(messageTemp == "CLOSE"){
    move_to_position = 0;
    command = CUSTOM_MOVE;
    }else if(messageTemp == "OPEN_CLOSE"){
      Serial.println("MQTT OPENING THEN CLOSING");
      command = OPEN_CLOSE;
    }
  }
}

void reconnect() {
 // Loop until we're reconnected
 if (!client.connected()) {
   Serial.print("Attempting MQTT connection...");
   // Attempt to connect
   if (client.connect(mqtt_name.c_str(),mqtt_username.c_str(),mqtt_password.c_str())) {
     Serial.println("connecting");
     Blynk.virtualWrite(V44, "Connecting...");
     Blynk.virtualWrite(V48, mqtt_set_topic);
     Blynk.virtualWrite(V49, mqtt_state_topic);
     //counter = 0;
     client.subscribe(mqtt_set_topic.c_str());

      const size_t capacity = JSON_OBJECT_SIZE(512);
      DynamicJsonDocument doc(capacity);
      
      doc["name"] = mqtt_name;
      doc["device_class"] = "curtain";
      doc["state_topic"] = disc_prefix + "/cover/" + mqtt_name + "/state";
      doc["command_topic"] = disc_prefix + "/cover/" + mqtt_name + "/set";
      //doc["supported_features"] = "OPEN, CLOSE";
      doc["state_open"] = "open";
      doc["state_closed"] = "closed";
      
      char docbuffer[512];
      size_t n = serializeJson(doc, docbuffer);
      client.publish(mqtt_discovery_topic.c_str(), docbuffer, true);
      Blynk.virtualWrite(V44, "Connected 2");

    }else{
     Serial.print("FAILED MQTT CONNECTION");
     Blynk.virtualWrite(V44, "Failed");
     Blynk.virtualWrite(V15, 0);
     MQTT_ON = false;
     MQTT_SETUP = false;
     preferences_local.putBool("mqtt_on", MQTT_ON);
     Serial.print(client.state());
     }
   }
}
