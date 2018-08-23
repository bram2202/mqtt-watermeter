#pragma once
#include <ESP8266WiFi.h>

class SettingsManager
{
  public:
    struct Settings
    {
      public:
        //mqtt settings
        String mqttHostName;
        int mqttPort;
        String mqttUserName;
        String mqttPassword;
        int mqttQuickUpdateInterval;
        int mqttRegularUpdateInterval;
        String mqttTopic;

        //wifi settings
        String wifiHostname;		//Set hostname for ESP8266
        String wifiSSID;
        String wifiPassword;

        //general settings
        int dataPin;
        int pulsFactor;
        double maxFlow;
        int sendFrequency;

    };
    SettingsManager();
    ~SettingsManager();

    Settings * GetSettings();
};

