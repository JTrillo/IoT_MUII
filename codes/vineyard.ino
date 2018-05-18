#include "DHT.t"
#include <SFE_BMP180.h>
#include <Wire.h>

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
SFE_BMP180 bmp180;

const int DHTPin = 5;     // what digital pin we're connected to
DHT dht(DHTPin, DHTTYPE);

//constantes y variables iluminacion
const long A = 1000;     //Resistencia en oscuridad en KΩ
const int B = 15;        //Resistencia a la luz (10 Lux) en KΩ
const int Rc = 10;       //Resistencia calibracion en KΩ
const int LDRPin = A0;   //Pin del LDR







void setup(){
  Serial.begin(9600);
  
  dht.begin();
  bmp180.begin();
  
  
}


void loop(){
  
  //Leer humedad, temperatura, presión e iluminación
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float p = getPressure();
  int ilum = getIllumination();  
  
   
  delay(5000);
  
}

//función devuelve el valor de presión
double getPressure(){
  char status;
  double T,P;
  
  status = bmp180.startTemperature(); //Inicio de lectura de temperatura
  if (status != 0)
  {   
    delay(status); //Pausa para que finalice la lectura
    status = bmp180.getTemperature(T); //Obtener la temperatura
    if (status != 0)
    {
      status = bmp180.startPressure(3); //Inicio lectura de presión
      if (status != 0)
      {        
        delay(status); //Pausa para que finalice la lectura        
        status = bmp180.getPressure(P,T); //Obtenemos la presión
        /*
        if (status != 0)
        {                  
          Serial.print("Presion: ");
          Serial.print(P,2);
          Serial.println(" mb");          
        } 
        */     
      }      
    }   
  }
  
  return P;
} 

//función devuelve valor de iluminación
int getIllumination(){
  int V = analogRead(LDRPin);
  int ilum = ((long)V*A*10)/((long)B*Rc*(1024-V));
  return ilum;
}


