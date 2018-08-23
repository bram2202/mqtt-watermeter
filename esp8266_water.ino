// -- MQTT puls-watermeter --

//#include <TimeLib.h>
//#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "SettingsManager.h"
#include "MQTTPublisher.h"
#include "ESP8266mDNS.h"
#include "Settings.h"

volatile uint32_t pulseCount = 0;
volatile uint32_t lastBlink = 0;
volatile double flow = 0;
uint32_t oldPulseCount = 0;
uint32_t newBlink = 0;
double oldflow = 0;
double volume = 0;
double oldvolume = 0;
uint32_t lastSend = 0;
uint32_t lastPulse = 0;
double ppl = 0;  // Pulses per liter

SettingsManager settingsManager;
MQTTPublisher mqqtPublisher(&settingsManager, true);
WiFiUDP ntpUDP;

void setup() {

  // Settings
  auto settings = settingsManager.GetSettings();
  // Set settings from heade file
  settings->mqttHostName = MQTT_HOST_NAME;
  settings->mqttPort = MQTT_PORT;
  settings->mqttUserName = MQTT_USER_NAME;
  settings->mqttPassword = MQTT_PASSWORD;
  settings->mqttQuickUpdateInterval = MQTT_QUICK_UPDATE_INTERVAL;
  settings->mqttRegularUpdateInterval = MQTT_REGULAR_UPDATE_INTERVAL;
  settings->mqttTopic = MQTT_TOPIC;
  settings->wifiHostname = WIFI_HOSTNAME;
  settings->wifiSSID = WIFI_SSID;
  settings->wifiPassword = WIFI_PASSWORD;
  settings->dataPin = DATA_PIN;
  settings->pulsFactor = PULSE_FACTOR;
  settings->maxFlow = MAX_FLOW;
  settings->sendFrequency = SEND_FREQUENCY;

  ppl = ((double)PULSE_FACTOR) / 1000;

  // Init
  Serial.begin(115200);
  Serial.println("Booting");

  // Setup Pin and interrupt
  pinMode(settings->dataPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(settings->dataPin), onPulse, FALLING);

  // Setup Wifi
  WiFi.mode(WIFI_STA);
  WiFi.hostname(settings->wifiHostname.c_str());
  WiFi.begin(settings->wifiSSID.c_str(), settings->wifiPassword.c_str());

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected!");

  ArduinoOTA.setHostname("WaterPuls");

  ArduinoOTA.onStart([]() {
    Serial.println("Start Ota");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd Ota");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.println("IP address: ");

  Serial.println(WiFi.localIP());

  mqqtPublisher.start();
}

void loop() {

  ArduinoOTA.handle();
  yield();
  mqqtPublisher.handle();
  yield();

}


void onPulse()
{

  uint32_t newBlink = micros();
  uint32_t interval = newBlink - lastBlink;

  if (interval != 0) {
    lastPulse = millis();
    if (interval < 500000L) {
      // Sometimes we get interrupt on RISING,  500000 = 0.5 second debounce ( max 120 l/min)
      return;
    }
    flow = (60000000.0 / interval) / ppl;
  }
  lastBlink = newBlink;

  pulseCount++;

}

