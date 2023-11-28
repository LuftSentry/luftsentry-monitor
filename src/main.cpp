#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>

#include <pgmspace.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "HTTPClient.h"
#include "SPIFFS.h"
#include <Update.h>

WiFiUDP udp;

WiFiClientSecure espClient = WiFiClientSecure();
PubSubClient mqttClient(espClient);
AsyncWebServer server(80);

DynamicJsonDocument dataResponse(1024);

bool loadData = true;

#include <config.h>
#include "ESP32_Utils_OTA.hpp"
#include "MQTT.hpp"
#include "ESP32_Utils_WiFi.hpp"
#include "ESP32_Utils_MQTT.hpp"
#include "ESP32_Utils_Keys.hpp"

#define BUTTON_PIN 19
#define MATRIX_PIN 18

int lcdColumns = 16;
int lcdRows = 2;
int lastState = HIGH;
int currentState;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, MATRIX_PIN, NEO_GRB + NEO_KHZ800);

uint8_t aqi0[] = {10, 13, 18, 21, 33, 38, 42, 45, 51, 52};
uint8_t aqi1[] = {10, 13, 18, 21, 41, 46, 50, 51, 52, 53};
uint8_t aqi2[] = {10, 13, 18, 21, 41, 42, 43, 44, 45, 46};
uint8_t aqi3[] = {10, 13, 18, 21, 42, 43, 44, 45, 49, 54};
uint8_t aqi4[] = {10, 13, 18, 21, 35, 36, 42, 45, 49, 54};
uint8_t aqi5[] = {9, 14, 18, 21, 35, 36, 42, 45, 49, 54};

void powerOnLCD()
{
  int aqi = dataResponse["aqi"];
  char pmStatus[14] = "PM 2.5 = ";
  const char* pm25;
  if (dataResponse.containsKey("pm25"))
  {
    pm25 = dataResponse["pm25"];
  } else {
    pm25 = "N/A";
  }
  strcat(pmStatus, pm25);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(pmStatus);
  lcd.setCursor(0, 1);
  lcd.print("El estado es...");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  switch (aqi)
  {
  case 0:
    lcd.print("Bueno");
    break;
  case 1:
    lcd.print("Aceptable");
    break;
  case 2:
  case 3:
    lcd.print("Danino para");
    break;
  case 4:
    lcd.print("Muy danino para");
    break;
  case 5:
    lcd.print("Peligroso");
    break;
  }
  lcd.setCursor(0, 1);
  switch (aqi)
  {
  case 0:
  case 1:
    lcd.print("");
    break;
  case 2:
    lcd.print("grupos sensibles");
    break;
  case 3:
  case 4:
    lcd.print("la salud");
    break;
  }
  delay(5000);
  lcd.clear();
  lcd.noBacklight();
}

void drawMatrix(uint32_t color, uint8_t shape[], int size)
{
  for (byte i = 0; i < size; i = i + 1)
  {
    strip.setPixelColor(shape[i], color);
    delay(50);
  }
  strip.show();
}

void powerOffMatrix()
{
  for (byte i = 0; i < 64; i = i + 1)
  {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    delay(10);
  }
  strip.show();
}

void powerOnMatrix()
{
  int aqi = dataResponse["aqi"];
  strip.setBrightness(25);
  switch (aqi)
  {
  case 0:
    drawMatrix(strip.Color(0, 255, 0), aqi0, 10);
    break;
  case 1:
    drawMatrix(strip.Color(255, 255, 0), aqi1, 10);
    break;
  case 2:
    drawMatrix(strip.Color(255, 165, 0), aqi2, 10);
    break;
  case 3:
    drawMatrix(strip.Color(255, 0, 0), aqi3, 10);
    break;
  case 4:
    drawMatrix(strip.Color(128, 0, 128), aqi4, 10);
    break;
  case 5:
    drawMatrix(strip.Color(139, 69, 19), aqi5, 10);
    break;
  }
}

void setup()
{
  Serial.begin(115200);
  delay(100);

  StaticJsonDocument<200> defaultJson;
  defaultJson["pm25"] = "12.5";
  defaultJson["aqi"] = 1;

  deserializeJson(dataResponse, defaultJson.as<String>());

  lcd.init();
  strip.begin();
  strip.setBrightness(50);
  powerOffMatrix();
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  ConnectWiFi_STA(false);
  InitOTA();
  InitKeys();
  InitMqtt();
}

void loop()
{
  HandleMqtt();
  currentState = digitalRead(BUTTON_PIN);
  if (lastState == LOW && currentState == HIGH && loadData)
  {
    powerOnMatrix();
    powerOnLCD();
    powerOffMatrix();
  }
  lastState = currentState;
  delay(100);
}
