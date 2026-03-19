#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>

const char* ssid     = "DIGI-mqN9";
const char* password = "faJbuWtn7P";


WiFiUDP udp;
const char* laptop_ip = "192.168.1.134"; 
const int port = 5005; 


SoftwareSerial mySerial(D2, D3); 

void setup() {
  Serial.begin(115200); // Pentru debug pe USB
  
  delay(1000); // Așteptăm o secundă să se inițializeze Serialul
  Serial.println("--- TEST: ESP8266 A PORNIT ---");
  mySerial.begin(9600); // Pentru comunicare cu Arduino

  WiFi.begin(ssid, password);
  Serial.print("Conectare la Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectat!");
  Serial.print("IP ESP8266: ");
  Serial.println(WiFi.localIP());
  
  udp.begin(port);
}

void loop() {

  int packetSize = udp.parsePacket();
  if (packetSize) {
    char packetBuffer[255];
    int len = udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
      mySerial.print(packetBuffer[0]); // Trimite comanda ('F', 'S', etc.)
    }
  }


  if (mySerial.available()) {
    String dataFromArduino = mySerial.readStringUntil('\n');
    
    if (dataFromArduino.startsWith("T|")) {
      String json = formatToJson(dataFromArduino);
      
      udp.beginPacket(laptop_ip, port);
      udp.print(json);
      udp.endPacket();
    }
  }
}

String formatToJson(String raw) {
  raw.trim(); // Scoate caracterele invizibile (\r \n)
  raw = raw.substring(2);
  
  int firstPipe = raw.indexOf('|');
  int secondPipe = raw.lastIndexOf('|');
  
  if(firstPipe == -1 || secondPipe == -1) return "{\"error\":\"invalid_data\"}";

  String d = raw.substring(0, firstPipe);
  String l = raw.substring(firstPipe + 1, secondPipe);
  String r = raw.substring(secondPipe + 1);
  
  return "{\"dist\":" + d + ", \"L\":" + l + ", \"R\":" + r + "}";
}