#include "MQTTPublisher.h"

WiFiClient espClient;
PubSubClient client(espClient);

MQTTPublisher::MQTTPublisher(SettingsManager * settingsManager, bool inDebugMode)
{
  randomSeed(micros());
  mqttSettingsManager = settingsManager;
  debugMode = inDebugMode;
}

MQTTPublisher::~MQTTPublisher()
{
  client.publish("watermeter", "offline");
  client.disconnect();
}


bool MQTTPublisher::reconnect()
{
  lastConnectionAttempt = millis();
  if (debugMode)
  {
    Serial.print("Attempting MQTT connection to server: ");
    Serial.print(mqttSettings->mqttHostName);
    Serial.println("...");
  }

  // Create a random client ID
  String clientId = "watermeter-";
  clientId += String(random(0xffff), HEX);

  // Attempt to connect
  bool clientConnected;
  if (mqttSettings->mqttUserName.length())
  {
    Serial.println("Using user credientials for authentication.");
    clientConnected = client.connect(clientId.c_str(), mqttSettings->mqttUserName.c_str(), mqttSettings->mqttPassword.c_str());
  }
  else
  {
    Serial.println("Connecting without user credentials.");
    clientConnected = client.connect(clientId.c_str());
  }

  if (clientConnected)
  {
    if (debugMode){
      Serial.println("connected");
    }
      
    // Once connected, publish an announcement...
    client.publish("watermeter", "online");

    return true;
  }
  else {
    if (debugMode)
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }

  return false;
}


void MQTTPublisher::start()
{
  mqttSettings = mqttSettingsManager->GetSettings();
  if (mqttSettings->mqttHostName.length() == 0 || mqttSettings->mqttPort == 0)
  {
    Serial.println("MQTT disabled. No hostname or port set.");
    return; //not configured
  }
  Serial.println("MQTT enabled. Connecting.");
  client.setServer(mqttSettings->mqttHostName.c_str(), mqttSettings->mqttPort);
  reconnect(); //connect right away
  isStarted = true;
}

void MQTTPublisher::stop()
{
  isStarted = false;
}

void MQTTPublisher::handle()
{
  if (!isStarted)
    return;

  if (!client.connected() && millis() - lastConnectionAttempt > RECONNECT_TIMEOUT) {
    if (!reconnect()) return;
  }

  //got a valid mqtt connection. Loop through the inverts and send out the data if needed
  client.loop();

  bool sendRegular = millis() - lastSentRegularUpdate > mqttSettings->mqttRegularUpdateInterval;
  bool sendQuick = millis() - lastSentQuickUpdate > mqttSettings->mqttQuickUpdateInterval;
  bool sendOk = true; //if a mqtt message fails, wait for retransmit at a later time
  if (sendRegular || sendQuick)
  {

    auto mqttTopic = mqttSettings->mqttTopic;
    if (debugMode)
    {
      Serial.print("Publishing prepend topic for the pulsmeter is: ");
      Serial.println(mqttTopic);
    }

    if (sendQuick)
    {

      uint32_t currentTime = millis();
      if (currentTime - lastSend > mqttSettings->sendFrequency ) {
        lastSend = currentTime;

        // send flow
        if (flow != oldflow) {
          oldflow = flow;
          Serial.print("l/min:");
          Serial.println(flow);
          if (flow < ((uint32_t)mqttSettings->maxFlow)) {
            if (sendOk) sendOk = publishOnMQTT(mqttTopic, "/flow", String(flow, 4));
          }
        }

        // No Pulse count received in 2min
        if (currentTime - lastPulse > 120000) {
          flow = 0;
        }

        // Pulse count has changed
        if (pulseCount != oldPulseCount) {

          oldPulseCount = pulseCount;
          Serial.print("pulsecount:");
          Serial.println(pulseCount);
          if (sendOk) sendOk = publishOnMQTT(mqttTopic, "/puls", String(pulseCount));

          double volume = ((double)pulseCount / (double)mqttSettings->pulsFactor);
          if (volume != oldvolume) {
            oldvolume = volume;
            Serial.print("volume:");
            Serial.println(volume, 3);
            if (sendOk) sendOk = publishOnMQTT(mqttTopic, "/vol", String(volume, 4));
          }
        }

      }
      
    }
    else
    {
      //TODO: Rest of data
    }

    //On the github page of the pubsubclient it was suggested to add extra client.loop().
    client.loop();
  }

  if (sendQuick)
    lastSentQuickUpdate = millis();
  if (sendRegular)
    lastSentRegularUpdate = millis();

  if (debugMode)
  {
    Serial.print("MQTT send status: ");
    Serial.println(sendOk);
  }
}


bool MQTTPublisher::publishOnMQTT(String prepend, String topic, String value)
{
  auto retVal =  client.publish((prepend.c_str() + topic).c_str(), value.c_str());
  yield();
  return retVal;
}
