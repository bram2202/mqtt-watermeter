#include "MQTTPublisher.h"
#include "Settings.h"

WiFiClient espClient;
PubSubClient client(espClient);

MQTTPublisher::MQTTPublisher(bool inDebugMode)
{
  randomSeed(micros());
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
    Serial.print("MQTT) Attempting connection to server: ");
    Serial.print(MQTT_HOST_NAME);
    Serial.println("...");
  }

  // Create a random client ID
  String clientId = "watermeter-";
  clientId += String(random(0xffff), HEX);

  // Attempt to connect
  bool clientConnected;
  if (String(MQTT_USER_NAME).length())
  {
    Serial.println("MQTT) Connecting with credientials");
    clientConnected = client.connect(clientId.c_str(), MQTT_USER_NAME, MQTT_PASSWORD);
  }
  else
  {
    Serial.println("MQTT) Connecting without credentials.");
    clientConnected = client.connect(clientId.c_str());
  }

  if (clientConnected)
  {
    if (debugMode) {
      Serial.println("MQTT) connected");
    }

    // Once connected, publish an announcement...
    client.publish("watermeter", "online");

    return true;
  }
  else {
    if (debugMode)
    {
      Serial.print("MQTT) failed, rc=");
      Serial.print(client.state());
    }
  }

  return false;
}


void MQTTPublisher::start()
{
  if (String(MQTT_HOST_NAME).length() == 0 || MQTT_PORT == 0)
  {
    Serial.println("MQTT) disabled. No hostname or port set.");
    return; //not configured
  }
  if(debugMode){
    Serial.println("MQTT) enabled. Connecting.");
  }
  client.setServer(MQTT_HOST_NAME, MQTT_PORT);
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

  bool sendRegular = millis() - lastSentRegularUpdate > MQTT_REGULAR_UPDATE_INTERVAL;
  bool sendQuick = millis() - lastSentQuickUpdate > MQTT_QUICK_UPDATE_INTERVAL;
  bool sendOk = true; //if a mqtt message fails, wait for retransmit at a later time
  if (sendRegular || sendQuick)
  {

    auto mqttTopic = MQTT_TOPIC;
    if (sendQuick)
    {

      uint32_t currentTime = millis();
      if (currentTime - lastSend > SEND_FREQUENCY ) {
        lastSend = currentTime;

        // send flow
        if (flow != oldflow) {
          oldflow = flow;
          if(debugMode){
            Serial.print("MQTT) l/min:");
            Serial.println(flow);
          }
          if (flow < ((uint32_t)MAX_FLOW)) {
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
          if(debugMode){
            Serial.print("MQTT) pulsecount:");
            Serial.println(pulseCount);
          }
          if (sendOk) sendOk = publishOnMQTT(mqttTopic, "/puls", String(pulseCount));

          double volume = ((double)pulseCount / (double)PULSE_FACTOR);
          if (volume != oldvolume) {
            oldvolume = volume;
            if(debugMode){
              Serial.print("MQTT) volume:");
              Serial.println(volume, 3);
            }
            if (sendOk) sendOk = publishOnMQTT(mqttTopic, "/vol", String(volume, 4));
          }
        }

      }

    }
    client.loop();
  }

  if (sendQuick)
    lastSentQuickUpdate = millis();
  if (sendRegular)
    lastSentRegularUpdate = millis();
}

bool MQTTPublisher::publishOnMQTT(String prepend, String topic, String value)
{
  auto retVal =  client.publish((prepend.c_str() + topic).c_str(), value.c_str());
  yield();
  return retVal;
}
