#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif

char ssid[] = "Keep Tryin";
char pass[] = "kyubtaupassword";

const int trigPin = 2;
const int echoPin = 3;

float duration, distance;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "mqtt-dashboard.com";
int port = 1883;
const char topic[] = "AshimaWave";

const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = (duration * 0.0343) / 2;

    if (distance < 10) {
      mqttClient.beginMessage(topic);
      mqttClient.print(" Wave delivered");
      mqttClient.endMessage();
      delay(1000);
    }

    Serial.println();

    count++;
  }
}

