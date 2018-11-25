#include <WiFi.h>
#include <PubSubClient.h>
#define GW_ID "gateway" //CHANGE IT

/* Wifi router */
const char* ssid     = "POCOPHONE"; //CHANGE IT
const char* password = "holahola"; //CHANGE IT

/* mqtt server */
const char* mqtt_server = "192.168.43.51"; //CHANGE IT
const char* mqtt_user = "gw1";
const char* mqtt_pass = "topsecret";

/* topics */
#define TEMPERATURE_TOPIC    "node1/temperature"

WiFiClient wifi;
PubSubClient client(wifi);

void callbackOnReceive(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);

  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    if (client.connect(GW_ID, mqtt_user, mqtt_pass)) {
      Serial.println("Connected to MQTT Server");
      /* subscribe topic with default QoS 0*/
      client.subscribe(TEMPERATURE_TOPIC);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void setupMQTT(){
  client.setServer(mqtt_server, 1883);
  client.setCallback(callbackOnReceive);
}

void setupWifi(){
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("...");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
    delay(10);
    setupWifi();
    delay(10);
    setupMQTT();
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }

  client.loop();

}
