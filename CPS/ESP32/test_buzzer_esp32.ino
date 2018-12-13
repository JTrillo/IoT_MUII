const byte buzzerPin = 12;

int channel = 0;
int freq = 2000;
int resolution = 8;
int dutyCycle = 128;

void setup() {
  Serial.begin(115200);

  ledcSetup(channel, freq, resolution);
  ledcAttachPin(buzzerPin, channel);
}

void loop() {
  ledcWrite(channel, dutyCycle);
  delay(250);
  ledcWrite(channel, 0);
  delay(250);
}
