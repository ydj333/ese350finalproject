
#include <ESP8266WiFi.h>
#include <string.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "AirPennNet-Device";
char pass[] = "penn1740wifi";

// Basic demo for accelerometer readings from Adafruit LIS3DH, ported to NodeMCU

#include <Wire.h>
#include <Math.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

void setup(void) {
  Serial.begin(115200);
  delay(100);     // will pause Zero, Leonardo, etc until serial console opens
  //Serial.println();
  Serial.println("LIS3DH test!");
  
  if (!lis.begin(0x19)) {
    Serial.println("Couldnt start");
    lis.begin(0x18);
   //while (1) yield();
  }
  Serial.println("LIS3DH found!");

  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = "); Serial.print(2 << lis.getRange());
  Serial.println("G");

  // Explicitly set the ESP8266 to be a WiFi-client
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.println("Trying Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Connected");
}

void loop() {
  // Read the acceleration from the sensor
  lis.read(); 
  sensors_event_t event;
  lis.getEvent(&event);

  float mag = sqrt(event.acceleration.x * event.acceleration.x + event.acceleration.y * event.acceleration.y + event.acceleration.z * event.acceleration.z);
  int mag_int = (int) (mag * 100.0);
  //Serial.println(mag_int);

  Serial.print(" \tMag: "); Serial.print(mag);
  Serial.print(" \tMag_int:"); Serial.print(mag_int);
  Serial.println();
  
 // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const char * host = "10.103.213.215";
  const int httpPort = 80;
  client.connect(host, httpPort);
  /*if (!client.connect(host, httpPort)) {
    Serial.println("Client not connected.");
    return;
  }*/

  // We now create a URI for the request. Something like /data/?sensor_reading=123
  String url = "/data/";
  url += "?mag=";
  char mag_string [4];
  sprintf(mag_string, "%d", mag_int);
  url += mag_string;

  // Serial.print("URL:"); Serial.println(url);
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      //Serial.println("Timed out.");
      client.stop();
      return;
    }
  }
}
