#include "DHT.h"

//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
const int DHTPin = 5;     // what digital pin we're connected to
 
DHT dht(DHTPin, DHTTYPE);

void setup() {
   Serial.begin(9600);
   Serial.println("DHTxx test!");
 
   dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
   delay(2000);
 
   // Reading temperature or humidity takes about 250 milliseconds!
   float h = dht.readHumidity();
   float t = dht.readTemperature();
 
   if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
   }
 
 
   Serial.print("Humidity: ");
   Serial.print(h);
   Serial.println(" %");
   Serial.print("Temperature: ");
   Serial.print(t);
   Serial.println(" *C ");
  
}


