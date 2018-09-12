# mqtt-watermeter

Combining `Mysensors Water Meter Pulse Sensor`[[link]](https://www.mysensors.org/build/pulse_water) with `GoodWeLogger`[[link]](https://github.com/jantenhove/GoodWeLogger) to create a watermeter that sends its data over MQTT.

For example to use with `Hass.io` [[link]](https://www.home-assistant.io/)


## Requirements 
* ESP8266 (Wemos/LOLIN D1 mini)
* TCRT5000 IR module
* Basic soldering and wiring skills
* (For Wemos d1 mini) CH340G driver [[link]](https://wiki.wemos.cc/downloads)

## Arduino IDE
* Add boards `Preferences` -> `Additional Board Manager URLs` Add  ```http://arduino.esp8266.com/stable/package_esp8266com_index.json```
  Than install wemos boards (esp8266 by ESP8266 Community)  `tools` -> `board` -> `boards manager`
* Install lib `pubsubclient`[[link]](https://github.com/knolleary/pubsubclient)


## Settings
Copy `Settings.example.h` to `Settings.h` and fill in the correct data.

| Setting | Description|  
| ------------- |:-------------:| 
| WIFI_HOSTNAME | device name on network |
| WIFI_SSID | Wifi name to connect to |
| WIFI_PASSWORD | Wifi password |
| MQTT_HOST_NAME | MQTT broker address |
| MQTT_PORT | MQTT broker port |
| MQTT_USER_NAME| MQTT user name |
| MQTT_PASS | MQTT password |
| MQTT_QUICK_UPDATE_INTERVAL | ? |
| MQTT_REGULAR_UPDATE_INT | ? |
| MQTT_TOPIC | MQTT topic prefix |
| DATA_PIN | Pin of IR  |
| PULSE_FACTOR | Number of blinks per m3 of your meter (One rotation/liter)  |
| MAX_FLOW | Max flow (l/min) value to report. This filters outliers |
| SEND_FREQUENCY | Time (ms) between posts |

## MQTT
Subscribe to `watermeter/<topic>` in your MQTT client. prefix can be changes in the settings.

| Topic | Description| Unit  |
| ------------- |:-------------:| -----:|
| vol     | ?????? | m3 |
| puls    | pulses between posts | int |
| flow    | water flow last X min| l/min |

Example: 
```
watermeter/vol 6
watermeter/puls 500
watermeter/flow 5
```

## TODO
* Testing
* Expand readme
