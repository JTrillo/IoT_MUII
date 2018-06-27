//librerias
#include <SoftwareSerial.h>
#include "DHT.h" //sensor temperatura y humedad aire
#include <SFE_BMP180.h> //sensor presión y temperatura
#include <Wire.h>

#define DHTTYPE DHT11


//pines
int pinIluminacion = A0;
int pinDHT = 5;
int pinHumedadSuelo = A1;

//variables sensores
int iluminacion;
float humedadAire;
int humedadSuelo;
double temperatura,presion;

SFE_BMP180 bmp180;
DHT dht(pinDHT, DHTTYPE);
SoftwareSerial loraSerial(10, 11);

//temporizador
unsigned long tiempoInicio;  
unsigned long tiemopActual;
const unsigned long periodo = 10000;

String strLora;

void setup()
{
	Serial.begin(57600);
	dht.begin();
	bmp180.begin();

	tiempoInicio = millis();

  //Lora
  loraSerial.begin(9600);
  loraSerial.setTimeout(1000);
  lora_autobaud();

  Serial.println("Inicio LoRa");
  
  loraSerial.listen();
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  loraSerial.println("sys get ver"); //devuelve la versión actual de firmware de la placa.
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  
  loraSerial.println("mac pause"); //número decimal que representa la cantidad de milisegundos que se puede pausar
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  
  loraSerial.println("radio set mod lora"); //configura en modo lora. Modos lora or fsk.
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  
  loraSerial.println("radio set freq 869100000"); //Este comando cambia la frecuencia de comunicación. Decimal que representa la frecuencia, de 433000000 a 434800000 o de 863000000 a 870000000, en Hz.
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  
  loraSerial.println("radio set pwr 14"); //Modificar la potencia de salida. De -3 a 15 dBm
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  
  loraSerial.println("radio set sf sf7"); //Este comando establece el factor de dispersión utilizado durante la transmisión. Los valores pueden ser: sf7, sf8, sf9, sf10, sf11 o sf12.
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  
  loraSerial.println("radio set afcbw 41.7"); //Este comando modifica el ancho de banda de corrección de frecuencia automática para recibir / transmitir. Valores (kHz):250, 125, 62.5, 31.3, 15.6, 7.8, 3.9, 200, 100, 50, 25, 12.5, 6.3, 3.1, 166.7, 83.3, 41.7, 20.8, 10.4, 5.2, 2.6.
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  
  loraSerial.println("radio set rxbw 125"); //Este comando establece el ancho de banda de la señal al recibir. Valores (kHz): 250, 125, 62.5, 31.3, 15.6, 7.8, 3.9, 200, 100, 50, 25, 12.5, 6.3, 3.1, 166.7, 83.3, 41.7, 20.8, 10.4, 5.2, 2.6.
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  
  loraSerial.println("radio set prlen 8"); //Este comando establece la longitud del preámbulo para transmitir / recibir. De 0 a 65535
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  
  loraSerial.println("radio set crc on"); //Este comando habilita el encabezado CRC para las comunicaciones
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  
  loraSerial.println("radio set iqi off"); //Este comando habilita o deshabilita  Invert IQ  para las comunicaciones.
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  
  loraSerial.println("radio set cr 4/5"); //Este comando modifica la velocidad de codificación que está siendo utilizada actualmente por la radio. Valores: 4/5, 4/6, 4/7, 4/8
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  
  loraSerial.println("radio set wdt 60000"); //Este comando actualiza la duración del tiempo de espera, en milisegundos. Si esta funcionalidad está habilitada, entonces el temporizador de vigilancia se inicia para cada recepción o transmisión del transceptor. El temporizador de vigilancia se detiene cuando finaliza la operación en curso.
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  
  loraSerial.println("radio set sync 12"); //Este comando configura la palabra de sincronización utilizada durante la comunicación. Hexadecimal
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);
  
  loraSerial.println("radio set bw 125"); //Este comando establece el ancho de banda de radio operativo para la operación LoRa. Valors(kHz): 125, 250, 500.
  strLora = loraSerial.readStringUntil('\n');
  Serial.println(strLora);

  Serial.println("Empieza loop");

  
}

void loop()
{
	tiemopActual = millis();

  //Con el siguiente if, se controla que se recojan datos de los sensores y se envíe mediante Lora cada cierto periodo. En este caso 10 segundos.
	if (tiemopActual - tiempoInicio >= periodo)
  	{
      tiempoInicio = tiemopActual;  

   		actualizarValorSensores();
      String cadenaEnviar = getStringLora();
      Serial.println(cadenaEnviar);
      
      loraSerial.println(cadenaEnviar);
      strLora = loraSerial.readStringUntil('\n');
      Serial.println(strLora);

 	}


}

//Esta función consiste en captar los nuevos valores de los sensores: iluminacion, humedad del aire, humedad del suelo, presión y temperatura
void actualizarValorSensores(){
	iluminacion = analogRead(pinIluminacion);
	humedadAire = dht.readHumidity();
	humedadSuelo = analogRead(A1);

	char estadoTempHum;
  	estadoTempHum = bmp180.startTemperature(); //Inicio de lectura de temperatura
  	if (estadoTempHum != 0)
  	{   
    	delay(estadoTempHum); //Pausa para que finalice la lectura
    	estadoTempHum = bmp180.getTemperature(temperatura); //Obtener la temperatura //devuelve 1 o 0 si la lectura se ha realizado correctamente
    	if (estadoTempHum != 0)
    	{
      		estadoTempHum = bmp180.startPressure(3); //Inicio lectura de presión // hay que indicar la cantidad de muestras adicionales  (de 0 a 3) que el sensor debe tomar para la lectura de la presión y nos retorna el tiempo en milisegundos que necesitamos esperar antes de obtener la lectura. Si nos retorna un 0, es porque ha fallado el inicio de la medición de presión
      		if (estadoTempHum != 0)
      		{        
        		delay(estadoTempHum); //Pausa para que finalice la lectura        
        		estadoTempHum = bmp180.getPressure(presion,temperatura); //Obtenemos la presión
        		    
      		}      
    	}   
  	} 
}

//Esta función convierte todo los valores de los sensores en un json. Posteriormente lo convierte todo a hexadecimal para poder enviarlo mediante el modulo Lora.
String getStringLora(){
  String res = "{id:1,t:"+String(temperatura)+",hA:"+String(humedadAire)+",i:"+String(iluminacion)+",hS:"+String(humedadSuelo)+",p:"+String(presion)+"}";
  //Serial.println(res);
  String hex = "";
  for(int i=0;i<res.length();i++){
    hex = hex + String(res[i],HEX);
  }
  hex = "radio tx " + hex;
  return hex;
}

void lora_autobaud()
{
  String response = "";
  while (response=="")
  {
    delay(1000);
    loraSerial.write((byte)0x00);
    loraSerial.write(0x55);
    loraSerial.println();
    loraSerial.println("sys get ver");
    response = loraSerial.readStringUntil('\n');
  }
}




