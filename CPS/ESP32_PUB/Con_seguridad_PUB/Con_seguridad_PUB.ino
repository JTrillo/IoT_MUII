#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#define CLIENT_ID "node1" //CHANGE IT
#define PIN_TEMP A2 //CHANGE IT

// TEMPERATURE SENSOR
struct Temperature {
  byte pin;
  unsigned long period_ms;
  unsigned long last_ms;
  float value;
  int cont;
};
void setupTemperature(struct Temperature * temp, byte pin, unsigned long per, int cont, unsigned long ms){
  temp->pin = pin;
  temp->period_ms = per;
  temp->last_ms = ms;
  temp->cont = cont;
}
void loopTemperature(struct Temperature * temp, unsigned long ms){
  if(ms-temp->last_ms >= temp->period_ms){
    temp->last_ms = ms;
    int reading;
    float millivolts, celsius;
    float suma = 0.0;
    for(int i=0; i<temp->cont; i++){
      reading = analogRead(temp->pin);
      millivolts = (reading / 2048.0) * 3300;
      celsius = millivolts / 10;
      suma = suma + celsius;
    }
    temp->value = suma / (float)temp->cont;
    //Serial.println(temp->value);
  }
}

/* Wifi router */
const char* ssid     = "POCOPHONE"; //CHANGE IT
const char* password = "holahola"; //CHANGE IT

/* mqtt server */
const char* mqtt_server = "192.168.43.51"; //CHANGE IT
const char* mqtt_user = "node1";
const char* mqtt_pass = "password"; 

/* topics */
#define TEMPERATURE_TOPIC    "node1/temperature"

/* values */
unsigned long lastMsg = 0;
char msg[100];

const char* test_root_ca = "-----BEGIN CERTIFICATE-----\n" \
"MIIDYDCCAkigAwIBAgIJANPgk7YKxcSGMA0GCSqGSIb3DQEBCwUAMEUxCzAJBgNV\n" \
"BAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYDVQQKDBhJbnRlcm5ldCBX\n" \
"aWRnaXRzIFB0eSBMdGQwHhcNMTgxMTIwMTkwMDEzWhcNMjMxMTE5MTkwMDEzWjBF\n" \
"MQswCQYDVQQGEwJBVTETMBEGA1UECAwKU29tZS1TdGF0ZTEhMB8GA1UECgwYSW50\n" \
"ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB\n" \
"CgKCAQEAp/NUU0U2CJOskS2J8XMW1KdJU8KujYfEDDWeZf3xi4iVUtvv774KDHiX\n" \
"PBfpZRyrpLUalh00tYT9U6Te3BcWYQEWAp7wIO9DwsCLdBu2uZy6/ZtjHNYQpCna\n" \
"o2wZFW1jTe0pX1eF4b2IazbqCYehXo4tKGm7fk6cfSIWkXqobVNHI+TcrKRogmXE\n" \
"pmk6GDkKiNk1Gk+qYP+6gMha0fgAPWDP5xNcm+uM6YyPWscJK3BJR6PZYPDGqVfq\n" \
"gsNkfksfwEY7WkFbdhKQw/s4Rr1uoXLA1X/H3Ld9c8OcdHPKO/aLX78RvuEjtRum\n" \
"M7Xz7K2YVPqr+BYOV6k1oj4sFF/PZwIDAQABo1MwUTAdBgNVHQ4EFgQUoNM3DCeb\n" \
"1K/jGVWx7RVwNmESnkgwHwYDVR0jBBgwFoAUoNM3DCeb1K/jGVWx7RVwNmESnkgw\n" \
"DwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEARNkvQ8vgHrIlsI/R\n" \
"T+TrqNem3Y01ENT54fHQ1rzW+zdZqBCkIdzbmB5OicjJb+fUbmWvuR8h257B4JRQ\n" \
"qy93XyHtAXOgN+kUrRyPuicapHl+MfXL2Hx4B0y8ihAJmkQo14idPYUYFQu9aJUD\n" \
"xhOwC2izMEM5Grof1YVYA9BmzkdB3NxR0GDY1MtZy0NAfn6DW/ytGYOIdIp6PiSf\n" \
"FGsEWrK8uea1hksxtgEm5cl9hpebPerQMgNSDsPNpuL/p1j4eC8LY2HopF8uwJs/\n" \
"x4z3Ve/8wa/FeKZeqHnV6yeBWA3hXatHw96ZhuJ+1WGNuCrCgTmx/fU9wanYPmVt\n" \
"ECDo8A==\n" \
"-----END CERTIFICATE-----\n";

struct Temperature t1;

WiFiClientSecure wifi;
PubSubClient client(wifi);

void connectMQTT(){
  while(!client.connected()){
    Serial.print("MQTT connecting ...");
    if(client.connect(CLIENT_ID, mqtt_user, mqtt_pass)){
      Serial.println("Connected to MQTT Server");
      //client.subscribe(TEMPERATURE_TOPIC);
    }else{
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void setupMQTT(){
  client.setServer(mqtt_server, 8883);
  //client.setCallback(callbackOnReceive);
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

  wifi.setCACert(test_root_ca);
}

void setup() {
  Serial.begin(115200);
  delay(10);
  setupWifi();
  delay(10);
  setupMQTT();
  delay(10);

  //Setup temperature sensor
  setupTemperature(&t1, PIN_TEMP, 5000, 20, 0);
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }

  //client.loop();
  long now = millis();
  loopTemperature(&t1, now);
  if (now - lastMsg > 10000) {
    lastMsg = now;

    Serial.println(t1.value);
    Serial.println("Sending temperature update...");
    snprintf(msg, 100, "%lf", t1.value);
    client.publish(TEMPERATURE_TOPIC, msg);
  }
}
