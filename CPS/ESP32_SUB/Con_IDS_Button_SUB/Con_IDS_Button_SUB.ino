#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "Arduino.h"
#define GW_ID "gateway" //CHANGE IT
#define TEMPERATURE_TOPIC "node1/temperature" //CHANGE IT
#define PIN_LED_R A6 //CHANGE IT
#define PIN_LED_G A8 //CHANGE IT
#define PIN_LED_B A10 //CHANGE IT
#define PIN_LED_ANOMALY 4 //CHANGE IT
#define PIN_BUZZER 12 //CHANGE IT
#define PIN_BUTTON A1 //CHANGE IT

//RGB LED
struct RGB{
  byte red;
  byte green;
  byte blue;  
};
void setupRGB(struct RGB * rgb, byte pinRed, byte pinGreen, byte pinBlue){
  rgb->red = pinRed;
  rgb->green = pinGreen;
  rgb->blue = pinBlue;
  pinMode(rgb->red, OUTPUT);
  pinMode(rgb->green, OUTPUT);
  pinMode(rgb->blue, OUTPUT);
  digitalWrite(rgb->red, LOW);
  digitalWrite(rgb->green, LOW);
  digitalWrite(rgb->blue, LOW);
}
void setColorRGB(struct RGB * rgb, byte R, byte G, byte B){
  digitalWrite(rgb->red, R);
  digitalWrite(rgb->green, G);
  digitalWrite(rgb->blue, B);
}

//ANOMALY LED
struct AnomalyLed {
  byte pin;
  unsigned long per;
  unsigned long last_ms;
  boolean active;
  byte state;
};
void setupAnomalyLed(struct AnomalyLed * l, byte pin, unsigned long per, unsigned long ms){
  l->pin = pin;
  pinMode(l->pin, OUTPUT);
  l->per = per;
  l->last_ms = ms;
  l->active = false;
  l->state = LOW;
  digitalWrite(l->pin, l->state);
}
void loopAnomalyLed(struct AnomalyLed * l, unsigned long ms){
  if(l->active){
    if(ms-l->last_ms>=l->per){
      l->last_ms = ms;
      l->state = !l->state;
      digitalWrite(l->pin, l->state);
    }
  }
}

//PASSIVE BUZZER
struct PassiveBuzzer {
  byte pin;
  unsigned long period_ms;
  unsigned long last_ms;
  boolean active;
  int intensity;
  boolean sound;
  
  int channel;
  int freq;
  int resolution;
  int dutyCycle;
};
void setupPassiveBuzzer(struct PassiveBuzzer * pb, byte pin, unsigned long per, unsigned long ms, boolean active){
  pb->pin = pin;
  pb->period_ms = per;
  pb->last_ms = ms;
  pb->active = active;
  pb->intensity = 0;
  pb->sound = true;
  
  //We found this configuration
  pb->channel = 0; 
  pb->freq = 2000;
  pb->resolution = 8;
  pb->dutyCycle = 128;
  
  ledcSetup(pb->channel, pb->freq, pb->resolution);
  ledcAttachPin(pb->pin, pb->channel);
}
void loopPassiveBuzzer(struct PassiveBuzzer * pb, unsigned long ms){
  if(!pb->active){
    //noTone(pb->pin);
    ledcWrite(pb->channel, 0);
  }else{
    if(ms-pb->last_ms>=pb->period_ms){
      pb->last_ms = ms;
      if(!pb->sound){
        //noTone(pb->pin);
        ledcWrite(pb->channel, 0);
      }else{
        //tone(pb->pin, 1000);
        ledcWrite(pb->channel, pb->dutyCycle);
      }
      pb->sound = !pb->sound;
    }
  }
}

//BUTTON
struct Button {
  byte pin;
  boolean no_send;
  unsigned long pressing_time;
  unsigned long last_ms;
};
void setupButton(struct Button * b, byte pin, unsigned long pressing_time){
  b->pin = pin;
  pinMode(b->pin, INPUT);
  b->no_send = false;
  b->pressing_time = pressing_time;
  b->last_ms=10000;
}
void loopButton(struct Button * b, unsigned long ms){
  byte reading = digitalRead(b->pin);
  //Serial.println(digitalRead(b->pin));
  if(reading==LOW){
    b->last_ms = ms;
  }else{
    if(ms-b->last_ms>=b->pressing_time){
      b->last_ms = ms;
      Serial.println("Botón pulsado más de 1 segundo. El próximo dato recibido no se enviará al servidor");
      b->no_send = true;
    }
  }
}


/* Wifi router */
const char* ssid     = "POCOPHONE"; //CHANGE IT
const char* password = "holahola"; //CHANGE IT

/* Temperature server (Python) */
const uint16_t port = 8090; //CHANGE IT
const char * host = "192.168.43.65"; //CHANGE IT

/* mqtt server */
const char* mqtt_server = "192.168.43.51"; //CHANGE IT
const char* mqtt_user = "gw1"; //CHANGE IT
const char* mqtt_pass = "topsecret"; //CHANGE IT

/* values */
struct RGB rgb1;
struct AnomalyLed led1;
struct PassiveBuzzer pb1;
struct Button b1;
unsigned long curr_ms, last_ms=0;
int anomaly_cont = 0;
const String anomaly_str = "anomaly";

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

//cliente 1
const char* test_client_key = "-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpAIBAAKCAQEAk5aXuIIrnaa2pAWK/hvQGy6kxAZCG+Tjq7b1ZXfkf89lKlqr\n" \
"TmyYLss68+BBSLexjsCZBqOQabh/fkYWRVg90ON3cbkdFqVu1k/fyyPvoYxtIB9I\n" \
"lmR/qnKjPlQ2p2ZYEhuqL8b+Mfzth3z3GXlzpO1GSCx4RwTxH7hggKnhe6liBEXj\n" \
"PFKmJsBdH+WXZLVo6xWPuBI72ETbdTcw7Q9ZibDTba7xVoJZUimhRIKa27E9C0ob\n" \
"aT5m4SOmNR+dXUI86ohzYQS4IYCOR1XT/nih/BV3ehfjO/6bRmHM23OVyJUsFpmT\n" \
"+MoadQ5y7LLH6xbQN/FHf8sVg/flTLtVvgTRrQIDAQABAoIBAQCD3WpDvXIw0nKf\n" \
"DCDKt5jxoHQwfJL2j0Rn+Pk6pqtvEqOF+yrR0C8MqCJNIZHS4eVv8V8IUn3GJ8Up\n" \
"0n81iAj7KTFgV4/I2xizvPvsaXhCc9BQggL99F8osHJwTxfh64xG6ZbeR0J48U5c\n" \
"3ud8SEpuEl3YQHtaoOy7daYrbx16uVnz/oKILKjHNNeS2va5XxFh70FUk/sukyY1\n" \
"aapouHlpvM4MPqlHAnRoz0cAPXetsQpBPe47iXZeLXRBlPKHUxHoUkLJQfwKbi+6\n" \
"Jl1+CSliO32yvZhjQY8Y2eDaKUzHt2vte+/hTbJyYApFfFDUXuSDQbAKcvAQpDJp\n" \
"qF5cBLmBAoGBAMNJYQ4nK8I/u2EzbNPXPztw+WK2wElxAuaA2vvDu9dBO6yYDHFQ\n" \
"VpRrmo3xbPose+1ZG5a6xNvxor3PXYgQUnUuHOLNqfO8xg7neHZ4tv5fmUlHQqBx\n" \
"nntagq7GHqvFBOhl4iDjJPWgs59inYArjoA48KrbSPf9aIitqlrNoLbNAoGBAMF4\n" \
"9VasV5XXeXVX0qPau4bW/VE+u0yL27TZtZNcwQn7upMwW8knwM6vFIIrPaPja13O\n" \
"tlk8P/2dFG3LZYLJSj1Ie94IBFLRInQKd4Ns9K8rQ9kcSjpKxpfVabScEySD2Dp4\n" \
"Ym9rd+4fdWIVtBKhWGaUu3oQbzwnLQTMUAuvscZhAoGAaO7x/tffdIYoWod2b7IW\n" \
"YQtIhdlrI0tdYpxUjrnTIkwEE66z6ONhUmkraCAVDax/fr7Fy01aYFDLwLA/MtCK\n" \
"bjqz/M7EBpsl1XZdTVN6CN83j5+B1pKWtPOiiuUcL/Iyu+mbA07qWi3atscXmHpP\n" \
"HOj7sOMZp198Hw0qH5akR/UCgYEAvzAD6dm3V6iuKupVinxtkitD+xRtzI/s1oPJ\n" \
"rk0bqfzre+Zu3ZJ+MwDK2HNYn2ruP641fmtOpRWkCXfU4XB+/55wYZI0i46FaHyr\n" \
"a+GwzqWHj7YoGACqzYIkykjtlckAVY9/XrGpu1O1EO93mm18ySozNcWl1qC0GSi5\n" \
"9UJzACECgYB22iaRdSdPTE3Oh7vQgfyIdDkpaxhsGgi/nnLSjWEJj5dlQQ46wp/l\n" \
"lqi9V2Ow1jiy4CqycUybPd1a1QTVuvugDeTZa9RHZO0+kU9ucE3GYwyIpYAi+URG\n" \
"VXdwPWvpIoU00Vm90k1ISYLROkG9OIIZdkJ8G/zYZvkebXR7ubMqQw==\n" \
"-----END RSA PRIVATE KEY-----\n";


const char* test_client_cert = "-----BEGIN CERTIFICATE-----\n" \
"MIIDkDCCAnigAwIBAgIJAOrIGiEjYwa4MA0GCSqGSIb3DQEBCwUAMEUxCzAJBgNV\n" \
"BAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYDVQQKDBhJbnRlcm5ldCBX\n" \
"aWRnaXRzIFB0eSBMdGQwHhcNMTgxMTIxMTczNzE4WhcNMjMxMTIwMTczNzE4WjB6\n" \
"MQswCQYDVQQGEwJFUzEOMAwGA1UECAwFU3BhaW4xDzANBgNVBAcMBk1hbGFnYTEM\n" \
"MAoGA1UECgwDVU1BMQ0wCwYDVQQLDARNVUlJMQ8wDQYDVQQDDAZjbGllbnQxHDAa\n" \
"BgkqhkiG9w0BCQEWDXRyaWxsb0B1bWEuZXMwggEiMA0GCSqGSIb3DQEBAQUAA4IB\n" \
"DwAwggEKAoIBAQCTlpe4giudprakBYr+G9AbLqTEBkIb5OOrtvVld+R/z2UqWqtO\n" \
"bJguyzrz4EFIt7GOwJkGo5BpuH9+RhZFWD3Q43dxuR0WpW7WT9/LI++hjG0gH0iW\n" \
"ZH+qcqM+VDanZlgSG6ovxv4x/O2HfPcZeXOk7UZILHhHBPEfuGCAqeF7qWIEReM8\n" \
"UqYmwF0f5ZdktWjrFY+4EjvYRNt1NzDtD1mJsNNtrvFWgllSKaFEgprbsT0LShtp\n" \
"PmbhI6Y1H51dQjzqiHNhBLghgI5HVdP+eKH8FXd6F+M7/ptGYczbc5XIlSwWmZP4\n" \
"yhp1DnLsssfrFtA38Ud/yxWD9+VMu1W+BNGtAgMBAAGjTjBMMB8GA1UdIwQYMBaA\n" \
"FKDTNwwnm9Sv4xlVse0VcDZhEp5IMAkGA1UdEwQCMAAwCwYDVR0PBAQDAgTwMBEG\n" \
"A1UdEQQKMAiCBmNsaWVudDANBgkqhkiG9w0BAQsFAAOCAQEAQtcEEu+9eF7Io3Lk\n" \
"QY/KOnuKoX9UZHCI97scW8tDwzmLdd3PIL1mxNHE8qGMdefAmO5x7OSEuO+0EIVV\n" \
"GaFsQcVlWNbeYnFrSuEfKh+TwC3VkYXnTVzNW0pCOk11k4Ll5uvo5uhflXKrP0xC\n" \
"cl0AbQgbmtQzXXgc9eYB23RTPsgBpMRgS172iUMsxt+JoYsO6hcoLjg7hkEroCte\n" \
"Q7IMJGfaBcbSN/jD6Wt53eqfS1iZWQJK+Bo2fV1FV6DWJToIsqkZL4i+jIPyH8Yb\n" \
"ge01Bu97WlbRVlLQBp2GbVOcHf1CK1h9ThKW4BJNy2+44V+rqksJGNixysEI5JNi\n" \
"j+QILA==\n" \
"-----END CERTIFICATE-----\n";

//cliente2
//const char* test_client_key = "-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEogIBAAKCAQEArwyb3rS5a8fVgOYn5XVbzrGt+mRMJmtn7ONVn+5CKHkRIA+A\n" \
"XXp/CgRQKt8NTQn1sy7GqLLEKFM6TpGvdTeQbVQ/+UFmzwUYQdU3XTWJn+aN8hJ9\n" \
"OtuxO56hHct57AQZRUigVQgpImMTCgNnwnU2qUCELN4GvIDChU5dkibh/qJ+Jdz9\n" \
"Az6tf8GwRmHacGcB5DzRmCbVnpN7majpmH8j/CJTGAQXQTiMVCeKE9qGmIoF1IQM\n" \
"7zf8bIKmFTTXNxFfgujLwVkdmgvtaL3R5frdc09CBKaJ9UFLxvTYwM09UtmzXd0G\n" \
"fEolWRCPEYWxWk4GYh4QHjIw9GfgHN5sAXB9hwIDAQABAoIBACDLZfBFP2+30oIB\n" \
"nHvajsacdxGv9KxF9wRkWshJpM9cTx8IiG364w4HXNWLUDycnxFgtAsq4qEDZqsN\n" \
"lCeLr10sKYgS/qXOOIss3OBzhUMVuIYffKKoQf3/bAPe+QqEgg9+2brUepBcq1Z2\n" \
"6Nt/hiwTUC/vsKdkIA1FbzZQXoqLq075fRFTXvpaSBXXB5SoIOqDq3Dm+JCihb5q\n" \
"8O/ugj8XV11zb30x192zyQHOkY2IKKjyg+u9n8I526Bu2j124yvkynC2SLU+mw2L\n" \
"RDUrstAB6alJM3LP8RJ2atlBnCBJcmIG8U3fPULO7p4SC4U4h/RdT3GmNpoirC33\n" \
"10d4ZTECgYEA4ETCp8eGAvnVeXURZa53KQyyOBCmF0r6TGIXneLiYGDf/r/tRnAB\n" \
"3DdD2cGH9N42sfYu+felnYOr0iMcq+ig5Fz8YKWkCmEU+QDaY0jn6xZR4qPZ8TJ6\n" \
"1YHpOBDlQOK/CyfpSHPhqjIRul55cyd4+zBM09/K23lhKR8d2GtPqasCgYEAx9ES\n" \
"vhfkMfjyyy+TeyTwsKERdd0FsjMsoT+kqIRKxui/Vp8BUf4Y7uxmHQsmATEC7MLz\n" \
"xLs9j+kuRvMrK6zTdwuRy8fVElCfi0aFcbC3ihN/hu1P8DTsKWg1rR7nVO3Bx3SZ\n" \
"uYMfJdT3YOA8yn6kkCi97pPFnR1san+7i+LdN5UCgYAM8Srij8sWBODi33Sfj4/k\n" \
"2Iy4W42aLoN/QPHn9J3nve9vPhgtRm11DeNmf5qe8OvrHS/c0QPHsT+LIueWrBgM\n" \
"1aji0uxChRO2wuvXau+nS2RyVc5P8v/Prm2NBLyWH/+yXx1BRx6wua74OFd947tB\n" \
"k6n7WCDWxYxr2RXZ1WeFfQKBgG5Ogc1E/SmFJ+t5KS57FxA66pupVyKn04R00N5i\n" \
"obEq/a3A+4abAoKrKDfyVpmjx9ySRuYdA4A1luF+Dh//OYHndiYL/BkvPtQxKyu3\n" \
"BauI1gbTbSTK4nB9YdxfSg99bVNBkDd0JjbJWYM2ZvjWbtxSUX+qPhZdDuO//fXc\n" \
"WIj9AoGALEHuZtAYa0j8onQhlrhPF6TgD44/31wCrIRqnbrF1ZjgDrov3S/KdorJ\n" \
"z2vc10CLdhN+R5hhdjI6U/Iipmgm+ALS5bgtBu7fnnCc6kUMLNM1UTjCgJmjpqJE\n" \
"T1oVOc9ykr5v5rluijrAQzpqUcUMORDwPst9kI5/yVDq367eyyM=\n" \
"-----END RSA PRIVATE KEY-----\n";

//const char* test_client_cert ="-----BEGIN CERTIFICATE-----\n" \
"MIIDlDCCAnygAwIBAgIJAOrIGiEjYwa5MA0GCSqGSIb3DQEBCwUAMEUxCzAJBgNV\n" \
"BAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYDVQQKDBhJbnRlcm5ldCBX\n" \
"aWRnaXRzIFB0eSBMdGQwHhcNMTgxMTIxMTc0ODUzWhcNMjMxMTIwMTc0ODUzWjB9\n" \
"MQswCQYDVQQGEwJFUzEOMAwGA1UECAwFU3BhaW4xDzANBgNVBAcMBk1hbGFnYTEM\n" \
"MAoGA1UECgwDVU1BMQ0wCwYDVQQLDARNVUlJMRAwDgYDVQQDDAdjbGllbnQyMR4w\n" \
"HAYJKoZIhvcNAQkBFg9pdmFuYWxiYUB1bWEuZXMwggEiMA0GCSqGSIb3DQEBAQUA\n" \
"A4IBDwAwggEKAoIBAQCvDJvetLlrx9WA5ifldVvOsa36ZEwma2fs41Wf7kIoeREg\n" \
"D4Bden8KBFAq3w1NCfWzLsaossQoUzpOka91N5BtVD/5QWbPBRhB1TddNYmf5o3y\n" \
"En0627E7nqEdy3nsBBlFSKBVCCkiYxMKA2fCdTapQIQs3ga8gMKFTl2SJuH+on4l\n" \
"3P0DPq1/wbBGYdpwZwHkPNGYJtWek3uZqOmYfyP8IlMYBBdBOIxUJ4oT2oaYigXU\n" \
"hAzvN/xsgqYVNNc3EV+C6MvBWR2aC+1ovdHl+t1zT0IEpon1QUvG9NjAzT1S2bNd\n" \
"3QZ8SiVZEI8RhbFaTgZiHhAeMjD0Z+Ac3mwBcH2HAgMBAAGjTzBNMB8GA1UdIwQY\n" \
"MBaAFKDTNwwnm9Sv4xlVse0VcDZhEp5IMAkGA1UdEwQCMAAwCwYDVR0PBAQDAgTw\n" \
"MBIGA1UdEQQLMAmCB2NsaWVudDIwDQYJKoZIhvcNAQELBQADggEBAAT9w3ODYwei\n" \
"B/1SmQ71ss2Jo5psdoJnEjSZxRhvRwVS7nmjfEkO6OHYJ0tKSgmZWzq1KEi+5TAZ\n" \
"CYiPFKMD2uERKosCX/FNZxQd5x4Iiy6Y1LTNrUYTHxtqI4x4alu0BXz3nAiXFTw4\n" \
"kN/Yh/sT94c2O6zRxXHIgpX/+XqX4U9K0tSTY7ALz56IqWt3Ee5+Bm4AAVA6CZ8C\n" \
"W8JKez/QXon1YKYc1bQZml0i1juISClMqiQmacqvQMN2Nh+D3fpfsYVk3kemVigh\n" \
"wT0CqJxG79H2TlTqrYBMPlyoSbJxhlnoR3aR2D+vpm/74qHd8+MW2itssey+sYQ2\n" \
"67EQ+KCjWgA=\n" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure wifi;
PubSubClient client(wifi);


void callbackOnReceive(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);

  String aux = "";
  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    aux += (char)payload[i];
    Serial.print((char)payload[i]);
  }
  Serial.println();

  //CHECK IF RECEIVED STRING IS 'anomaly'
  if(aux==anomaly_str){
    anomaly_cont++;
    switch(anomaly_cont){
      case 1:
        Serial.println("Anomalia nivel 1");
        led1.active=true;
        break;
      case 2:
        Serial.println("Anomalia nivel 2");
        led1.per=500;
        break;
      case 3:
        Serial.println("Anomalia nivel 3");
        led1.per=250;
        pb1.active=true;
        break;
    }
  }
  
  if(anomaly_cont<3 && !b1.no_send){
    client.disconnect();
    Serial.println("Disconnected from MQTT Server");
  
    if(!wifi.connect(host, port)){
      char err_buf[100];
      if(wifi.lastError(err_buf,100)<0){
        Serial.print("Last WiFiClientSecure Error: ");
        Serial.println(err_buf);
      }
    }else{
      Serial.println("Connected to server!");
      Serial.print("Sending temperature: ");
      Serial.println(aux);
      wifi.println(aux);
    }
    boolean no_exit=true;
    while(no_exit){
      if(wifi.available()){
        char reading = wifi.read();
        Serial.print("Leido: ");
        Serial.println(reading);
        switch(reading){
          case 'R':
            setColorRGB(&rgb1, HIGH, LOW, LOW);
            break;
          case 'B':
            setColorRGB(&rgb1, LOW, LOW, HIGH);
            break;
          case 'G':
            setColorRGB(&rgb1, LOW, HIGH, LOW);
            break;
        }
        no_exit = false;
      }
    } 
  }else if(b1.no_send){
    Serial.println("Dato no enviado al servidor");
    b1.no_send = false;
  }
  
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
  client.setServer(mqtt_server, 8883);
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

  wifi.setCACert(test_root_ca);
  wifi.setCertificate(test_client_cert);
  wifi.setPrivateKey(test_client_key);
}
void setup() {
  Serial.begin(115200);
  delay(10);
  setupWifi();
  delay(10);
  setupMQTT();
  delay(10);

  //Setup leds and buzzer
  setupRGB(&rgb1, PIN_LED_R, PIN_LED_G, PIN_LED_B);
  setupAnomalyLed(&led1, PIN_LED_ANOMALY, 1000, last_ms);
  setupPassiveBuzzer(&pb1, PIN_BUZZER, 250, last_ms, false);
  setupButton(&b1, PIN_BUTTON, 1000);
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }

  curr_ms = millis();
  client.loop();
  loopAnomalyLed(&led1, curr_ms);
  loopPassiveBuzzer(&pb1, curr_ms);
  loopButton(&b1, curr_ms);
}
