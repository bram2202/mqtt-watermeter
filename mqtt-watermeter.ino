#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include "MQTTPublisher.h"
#include "WifiConnector.h"
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
bool hasWIFI = false;

MQTTPublisher mqqtPublisher(DEBUGE_MODE);
WifiConnector wifiConnector(DEBUGE_MODE);
WiFiUDP ntpUDP;

void setup() {

  ppl = ((double)PULSE_FACTOR) / 1000;

  // Init
  if(DEBUGE_MODE){
    Serial.begin(115200);
    Serial.println("Booting");
  }

  // Setup Pin and interrupt
  pinMode(DATA_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(DATA_PIN), pinTrigger, FALLING);

  // Setup Wifi
  wifiConnector.start();

  // Setup MQTT
  mqqtPublisher.start();
}

void loop() {
  wifiConnector.handle();
  yield();
  mqqtPublisher.handle();
  yield();
}

void pinTrigger() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 1000) //1000ms
  {
    Serial.println("Trigger");
    onPulse();
  }
  last_interrupt_time = interrupt_time;
}

void onPulse()
{
  uint32_t newBlink = micros();
  uint32_t interval = newBlink - lastBlink;
  if (interval != 0) {
    lastPulse = millis();
    if (interval < 500000L) {
      return;
    }
    flow = (60000000.0 / interval) / ppl;
  }
  lastBlink = newBlink;
  pulseCount++;
}
