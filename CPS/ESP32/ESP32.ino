#include <WiFi.h>
#define NLEDS 3
#define PIN_TEMP //CHANGE IT
#define PIN_LEDS //CHANGE IT

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
    float volt;
    float sum = 0.0;
    for(int i=0; i<temp->cont; i++){
      reading = analogRead(temp->pin);;
      volt = (reading / 1024.0) * 5.0;
      suma = suma + ((volt - 0.5) * 100.0);
    }
    temp->value = suma / (float)temp->cont;
  }
}

// LEDS
struct Leds {
  byte pin;
  byte state [ NLEDS ]; //Led0 - Red; Led1 - Blue; Led2 - Green
};
void setupLeds(struct Leds * l, byte pin){
  l->pin = pin;
  for(int i=0; i<NLEDS; i++){
    pinMode(pin+i, OUTPUT);
    l->state[i]=LOW;
    digitalWrite(pin+i, l->state[i]);
  }
}
void loopLeds(struct Leds * l, char received){
  switch(received){
    case 'R':
      l->state[0]=HIGH;
      l->state[1]=LOW;
      l->state[2]=LOW;
      break;
    case 'B':
      l->state[0]=LOW;
      l->state[1]=HIGH;
      l->state[2]=LOW;
      break;
    case 'G':
      l->state[0]=LOW;
      l->state[1]=LOW;
      l->state[2]=HIGH;
      break;
    default:
      l->state[0]=LOW;
      l->state[1]=LOW;
      l->state[2]=LOW;
      break;
  }
  for(int i=0; i<NLEDS; i++){
    digitalWrite(l->pin+i, l->state[i]);
  }
}

//----------------------------------------------------
unsigned long curr_ms, last_ms=0;
struct Temperature t1;
struct Leds leds1;

const char* ssid = "yourNetworkName"; //CHANGE IT
const char* password =  "yourNetworkPass"; //CHANGE IT
 
const uint16_t port = 8090; //CHANGE IT
const char * host = "192.168.1.83"; //CHANGE IT

WifiClient client;
//----------------------------------------------------
void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("...");
  }

  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());

  client.connect(host, port);

  //Setup temperature sensor and leds
  setupTemperature(&t1, PIN_TEMP, 10000, 20, 0);
  setupLeds(&leds1, PIN_LEDS);
}

void loop() {
  curr_ms = millis();
  loopTemperature(&t1, curr_ms);
  if(client.available()){
    loopLeds(&leds1, client.read());
  }
  if(curr_ms-last_ms>=30000){ //Each 30 seconds we send the temperature value
    client.println(t1->value);
  }
}
