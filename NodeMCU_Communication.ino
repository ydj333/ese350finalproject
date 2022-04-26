#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include<Wire.h>
String receiver;

const char *ssid = "AirPennNet-Device";
const char *password = "penn1740wifi";

int readingInt;
char readingString[10];
ESP8266WebServer server(80);


void handleSentVar() {
  if (server.hasArg("mag")) { // this is the variable sent from the client
    readingInt = server.arg("mag").toInt();
    server.send(200, "text/html", "Data received");
    Serial.print("Sensor reading: ");
    Serial.println(readingInt);
    sprintf(readingString, "%d\n", readingInt);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(4,5);
  delay(1000);
  
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  WiFi.begin(ssid, password);
  Serial.println("Trying Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  Serial.println("Connected");
  Serial.println(WiFi.localIP());
  server.on("/data/", HTTP_GET, handleSentVar); // when the server receives a request with /data/ in the string then run the handleSentVar function
  server.begin();
  Serial.println("begin receiving");
  
  
}

void loop() {
  server.handleClient();

  Wire.beginTransmission(8);//0x08 = 8;
  Wire.write(readingString);
  Wire.endTransmission();

//  Wire.requestFrom(8, 10);//0x08 = 8;
//
//  String receiver;
//  while (0 < Wire.available()){
//    char c = Wire.read();
//    receiver += c; 
//  }
//  Serial.println(receiver);
  
  
}
