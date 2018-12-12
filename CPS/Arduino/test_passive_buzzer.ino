#define PIN_BUZZER A0

// PASSIVE BUZZER
struct PassiveBuzzer {
  byte pin;
  unsigned long period_ms;
  unsigned long last_ms;
  boolean turn_off;
  int intensity;
  boolean sound;
  //byte state;
};
void setupPassiveBuzzer(struct PassiveBuzzer * pb, byte pin, unsigned long per, unsigned long ms, boolean turn_off){
  pb->pin = pin;
  pb->period_ms = per;
  pb->last_ms = ms;
  pb->turn_off = turn_off;
  pb->intensity = 0;
  pb->sound = true;
}
void loopPassiveBuzzer(struct PassiveBuzzer * pb, unsigned long ms){
  if(!pb->turn_off){
    noTone(pb->pin);
  }else{
    if(ms-pb->last_ms>=pb->period_ms){
      pb->last_ms = ms;
      if(!pb->sound){
        noTone(pb->pin);
      }else{
        tone(pb->pin, 1000);
      }
      pb->sound = !pb->sound;
    }
  }
}

unsigned long curr_ms;
PassiveBuzzer pb1;

void setup() {
  Serial.begin(9600);
  setupPassiveBuzzer(&pb1, PIN_BUZZER, 100, 0, true);
}

void loop() {
  curr_ms = millis();
  loopPassiveBuzzer(&pb1, curr_ms);
}
