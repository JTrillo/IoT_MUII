#define PIN_TEMP A0

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
      millivolts = (reading / 1023.0) * 5000;
      celsius = millivolts / 10;
      suma = suma + celsius;
    }
    temp->value = suma / (float)temp->cont;
    Serial.println(temp->value);
  }
  
}

unsigned long curr_ms, last_ms=0;
struct Temperature t1;

void setup() {
  Serial.begin(9600);

  setupTemperature(&t1, PIN_TEMP, 10000, 20, 0);
}

void loop() {
  curr_ms = millis();
  loopTemperature(&t1, curr_ms);
  

}
