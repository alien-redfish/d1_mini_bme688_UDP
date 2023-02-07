#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define BME_SCK 5
#define BME_CS 4
#define SEALEVELPRESSURE_HPA (1012)
#define UDP_PORT 5669

Adafruit_BME680 bme; // I2C

const char *ssid = "SSID"; //WiFi SSID
const char *password = "PASSWORD"; //WiFi Password

char packet[200];
char pressure[10];
char temperature[10];
char humidity[10];
char resistance[10];
int packetSize;

WiFiUDP udp;
IPAddress SendIP(192,168,0,255);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println(F("BME680 test"));

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      Serial.println("Connecting to WiFi");
    }
  if (WiFi.status() == WL_CONNECTED)
    {
      digitalWrite(LED_BUILTIN, HIGH);
    }
  Serial.println("WiFi Connected");
  Serial.println("IP Address: ");
  Serial.print(WiFi.localIP());
  Serial.println("");

  // Set up sensor
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  packetSize = 0;
  udp.begin(UDP_PORT);
}

void loop() {
  
 if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
 }

  dtostrf(bme.temperature, sizeof(bme.temperature), 2, temperature);
  dtostrf(bme.pressure / 100.0, sizeof(bme.pressure), 0, pressure);
  dtostrf(bme.humidity, sizeof(bme.humidity), 2, humidity);
  dtostrf(bme.gas_resistance / 100, sizeof(bme.gas_resistance), 2, resistance);
  strcpy(packet,temperature);
  strcat(packet, ",");
  strcat(packet,pressure);
  strcat(packet, ",");
  strcat(packet,humidity);
  strcat(packet, ",");
  strcat(packet,resistance);
  strcat(packet,"\n");
  udp.beginPacket(SendIP,UDP_PORT);
  udp.write(packet);
  udp.endPacket();
  Serial.println(temperature);
  Serial.println(pressure);
  Serial.println(humidity);
  Serial.println(resistance);
  Serial.println("Packet Sent");
  delay(5000);
}
