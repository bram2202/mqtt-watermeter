//Hostname of ESP8266
#define WIFI_HOSTNAME "WaterMeterPuls"

//Wifi SSID to connect to
#define WIFI_SSID "<wifi ssid>"

//Passowrd for WIFI
#define WIFI_PASSWORD "<wifi password>"

//set the mqqt host name or ip address to your mqqt host. Leave empty to disable mqtt.
#define MQTT_HOST_NAME  "<mqtt host>"

//mqtt port for the above host
#define MQTT_PORT       1883

//if authentication is enabled for mqtt, set the username below. Leave empty to disable authentication
#define MQTT_USER_NAME  "<mqtt user>"

//password for above user
#define MQTT_PASSWORD   "<mqtt password>"

//update interval for fast changing values in milliseconds for mqtt
#define MQTT_QUICK_UPDATE_INTERVAL  1000

//update interval for slow changing values in milliseconds for mqtt
#define MQTT_REGULAR_UPDATE_INTERVAL  10000

#define MQTT_TOPIC "watermeter"

#define DATA_PIN 3

#define PULSE_FACTOR 1000

#define MAX_FLOW 40

#define SEND_FREQUENCY 30000

#define DEBUGE_MODE true
