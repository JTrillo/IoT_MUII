#define pinRed A0
#define pinGreen A1
#define pinBlue A2

struct RGB{
  byte red;
  byte green;
  byte blue;  
};

void setupRGB(struct RGB * rgb, byte pinRed, byte pinGreen, byte pinBlue){
  rgb->red = pinRed;
  rgb->green = pinGreen;
  rgb->blue = pinBlue;
}

void setColorRGB(struct RGB * rgb, int R, int G, int B){
  analogWrite(rgb->red, R);
  analogWrite(rgb->green, G);
  analogWrite(rgb->blue, B);
}

RGB rgb1;

void setup() {
  setupRGB(&rgb1, pinRed, pinGreen, pinBlue);
}

void loop() {
  //RED
  setColorRGB(&rgb1, 255, 0, 0);
  delay(2000);
  //GREEN
  setColorRGB(&rgb1, 0, 255, 0);
  delay(2000);
  //BLUE
  setColorRGB(&rgb1, 0, 0, 255);
  delay(2000);
}
