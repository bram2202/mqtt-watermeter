# mqtt-watermeter

Combining `Mysensors Water Meter Pulse Sensor`[[link]](https://www.mysensors.org/build/pulse_water) with `GoodWeLogger`[[link]](https://github.com/jantenhove/GoodWeLogger) to create a watermeter that sends its data over MQTT.

For example to use with `Hass.io`[[link]](https://www.home-assistant.io/)


## Requirements 
* ESP8266 (Wemos/LOLIN D1 mini)
* TCRT5000 IR module
* Basic soldering and wiring skills

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
| MQTT_TOPIC | MQTT topic|
| DATA_PIN | Pin of IR  |
| PULSE_FACTOR | Number of blinks per m3 of your meter (One rotation/liter)  |
| MAX_FLOW | Max flow (l/min) value to report. This filters outliers |
| SEND_FREQUENCY | Time (ms) between posts |

## MQTT
Subscribe to the `watermeter/` topic in your MQTT client. can be changes in the settings.

| Field | Description| Unit  |
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
