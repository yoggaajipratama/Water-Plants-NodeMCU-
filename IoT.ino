#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

HTTPClient http;

const int buzzer = 5;
int posisi=0;
int sensorPin = A0;
int sensorValue = 0;
int humidity = 0;
String url = "http://192.168.43.53/kirim.php?hmd=";

ESP8266WebServer server(80);
const char* ssid = "xyz";
const char* password = "098765432100";
String homePage = "<!DOCTYPE html><meta charset=\"utf-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\"><html><head><title>Remote</title><style type=\"text/css\">body{padding: 10% 10%;background-color: black;}</style></head><body><center><table><tr><th><button style=\"border-width: 20px;border-color: white;background-color: red; border-radius: 40%;padding: 20px;font-weight: bold;margin: 15px;\"><a href=\"/on\" style=\"font-size: 15pt;text-decoration: none;color: black;\">ON/OFF</a></button></th></tr></table></center></body></html>";
String onPage = "<!DOCTYPE html><meta charset=\"utf-8\" http-equiv=\"refresh\" content=\"2; http://192.168.43.88/sucess\"><meta charset=\"utf-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\"><html><head><title>Remote</title><style type=\"text/css\">body{padding: 10% 10%;background-color: black;}</style></head><body><center><table><tr><th><button style=\"border-width: 20px;border-color: white;background-color: red; border-radius: 40%;padding: 20px;font-weight: bold;margin: 15px;\"><a href=\"\" style=\"font-size: 15pt;text-decoration: none;color: black;\"><marquee>Running process...</marquee></a></button></table></center></body></html>";
String offPage = "<!DOCTYPE html><meta charset=\"utf-8\" http-equiv=\"refresh\" content=\"2; http://192.168.43.88/\"><meta charset=\"utf-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\"><html><head><title>Remote</title><style type=\"text/css\">body{padding: 10% 10%;background-color: black;}</style></head><body><center><table><tr><th><button style=\"border-width: 20px;border-color: white;background-color: green; border-radius: 40%;padding: 20px;font-weight: bold;margin: 15px;\"><a href=\"\" style=\"font-size: 15pt;text-decoration: none;color: black;\">Finish</a></button></table></center></body></html>";
Servo servoku;
 
void setup(){
  pinMode(buzzer,OUTPUT);
  servoku.attach(2);
  servoku.write(0);
  delay(1000);
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  Serial.println("");
  while(WiFi.status() !=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Connected to :");
  Serial.println(WiFi.localIP());
  
  server.on("/",[](){
    server.send(200, "text/html",homePage);
  });
  /*server.on("/on",[](){
    servoku.write(140);
    server.send(200,"text/html",onPage);
    delay(5000);
    cekKelembaban();
  });*/

  server.on("/on1m",[](){
    servoku.write(140);
    server.send(200,"text/html",onPage);
    delay(10000);
    cekKelembaban();
  });

  server.on("/on5m",[](){
    servoku.write(140);
    server.send(200,"text/html",onPage);
    delay(50000);
    cekKelembaban();
  });

  server.on("/off",[](){
    servoku.write(140);
    server.send(200,"text/html",onPage);
    delay(500);
    server.send(200, "text/html",homePage);
  });

  server.on("/sucess",[](){
    server.send(200,"text/html",offPage);
  });

  server.begin();
  Serial.println("Webserver started");
}
 
void loop(void){
  server.handleClient();
  server.on("/on",[](){
    servoku.write(140);
    server.send(200,"text/html",onPage);
    delay(5000);
    cekKelembaban();
  });
  sensorValue = analogRead(sensorPin);
  humidity = convertToPercent(sensorValue);
  Serial.print("Connected to :");
  Serial.println(WiFi.localIP());
  Serial.print("Nilai Sensor : ");
  Serial.println(sensorValue);
  Serial.print("Kelembaban: ");
  Serial.print(humidity);
  Serial.println("%");
  Serial.println(" ");
  cekKelembaban();
  
  
}

void cekKelembaban(){

  http.begin(url + humidity);
  int httpCode = http.GET();
  String payload = http.getString();
  Serial.println(payload);
  http.end();
  
  if(humidity>=30){
     for (posisi = 0; posisi <= 140; posisi += 1){
      servoku.attach(2);
      servoku.write(posisi);
     }
    delay(5000);
  }else if(humidity<=30){
      tone(buzzer, 10000000);
      delay(1000);
      noTone(buzzer);
      delay(500);
      tone(buzzer, 10000000);
      delay(1000);
      noTone(buzzer);
      delay(500);
     for (posisi = 140; posisi >= 0; posisi -= 1){
        servoku.attach(2);
        servoku.write(posisi);                 
      }
      tone(buzzer, 10000000);
      delay(1000);
      noTone(buzzer);
      delay(500);
      tone(buzzer, 10000000);
      delay(1000);
      noTone(buzzer);
      delay(500);
  }
}

int convertToPercent(int value)
{
  int percentValue = 0;
  percentValue = map(value, 1023, 465, 0, 100);
  return percentValue;
}
