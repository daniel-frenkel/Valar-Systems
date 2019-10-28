// Blynk Wireless settings: Used in Blynk-Mode
const char AUTH[] = "utZTGeGw1w7XrrYKQS_zZrCg9Or9adBN"; //Your Blynk Auth Token
const char SSID[] = "Tony"; //Your 2.4GHz WiFi ID
const char PASSWORD[] = "spockntony"; // Your WiFi Password

//MQTT Setup
#include <SimpleTimer.h>
SimpleTimer timer;
int counter = 0;
long int last_UP_change = millis();

//MQTT Settings
IPAddress mqtt_server(192,168,50,178); 
const char* mqtt_username = "user";
const char* mqtt_password = "pass";
char* InTopic = "room/curtain"; //subscribe to topic to be notified about
char* InTopic1 = "room";
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
  // If a message is received on the topic esp32/output, you check if the message is either "open" or "close". 
  // Changes the output state according to the message
  if (String(topic) == "room/curtain") {
    Serial.print("Changing output to ");
    if(messageTemp == "open"){
      Serial.println("open");
      command = MOVE_OPEN;
    }
    else if(messageTemp == "close"){
      Serial.println("close");
      command = MOVE_CLOSE;
    }
  }
}

void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
   Serial.print("Attempting MQTT connection...");
   // Attempt to connect
   if (client.connect("MorningRod",mqtt_username,mqtt_password)) {
     Serial.println("connected");
     counter = 0;
     // ... and resubscribe
     client.subscribe(InTopic);
     client.subscribe(InTopic1);
   } else {
     Serial.print("failed, rc=");
     Serial.print(client.state());
     Serial.println(" try again in 0.3 second");
     ++counter;
     if (counter > 180) ESP.restart();
     // Wait .3 seconds before retrying
     delay(300);
   }
 }
}
