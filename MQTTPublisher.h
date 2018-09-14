#define MQTT_SOCKET_TIMEOUT 5
#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <vector>
#include "PubSubClient.h"
#include "WiFiClient.h"

#define RECONNECT_TIMEOUT 15000


//test

extern volatile uint32_t pulseCount;
extern volatile uint32_t lastBlink;
extern volatile double flow;
extern uint32_t oldPulseCount;
extern uint32_t newBlink;
extern double oldflow;
extern double volume;
extern double oldvolume;
extern uint32_t lastSend;
extern uint32_t lastPulse;
extern double ppl;

class MQTTPublisher
{
  private:
    bool debugMode;
    bool isStarted;

    unsigned long lastConnectionAttempt = 0;		//last reconnect
    unsigned long lastSentQuickUpdate = 0;			//last update of the fast changing info
    unsigned long lastSentRegularUpdate = 0;		//last update of the regular update info

    bool publishOnMQTT(String prepend, String topic, String value);
    bool reconnect();
  public:
    MQTTPublisher(bool inDebugMode = false);
    ~MQTTPublisher();

    void start();
    void stop();

    void handle();
};
