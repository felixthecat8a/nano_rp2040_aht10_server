#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include <Adafruit_AHTX0.h>
#include "arduino_secrets.h"

#define DEVICE_NAME "Nano RP2040"
#define SENSOR_NAME "AHT10"

#define RED 0 // Wifi Failed
#define YELLOW 1 // Sensor Failed
#define GREEN 2 // Connected
#define CYAN 3 // Initiating Wifi
#define BLUE 4 // Awaiting Connection
#define MAGENTA 5 // Initiating Sensor

#define MONITOR 0 // set 1 to monitor

#if MONITOR == 1
#define monitor(x) Serial.print(x)
#define monitorln(x) Serial.println(x)
#else
#define monitor(x)
#define monitorln(x)
#endif

Adafruit_AHTX0 aht;

WiFiServer server(80);

int status = WL_IDLE_STATUS;

/* Monitoring Prototypes */
void initiateMonitoring(bool monitoringActivated);

/* Sensor Prototypes */
void initialiateTemperatureSensor();

/* Access Point Prototypes */
void initiateWiFiAccessPoint();
void checkAccessPointConnectionStatus();
void printWifiStatusWhenReadyToConnect();

/* Prototypes */
void handleClientRequests(float temperatureCelsius, float humidity);

/* Status LED Prototypes */
void initiateLED();
void setStatusLED(int status);

void setup() {
  initiateLED();
  initiateMonitoring(MONITOR);
  monitorln("Arduino JSON Server");
  initiateTemperatureSensor();
  initiateWiFiAccessPoint();
  server.begin();
  printWifiStatusWhenReadyToConnect();
}

void loop() {
  checkAccessPointConnectionStatus();
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  handleClientRequests(temp.temperature, humidity.relative_humidity); 
}

void printWifiStatusWhenReadyToConnect() {
  monitor("SSID: "); monitorln(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  monitor("IP Address: "); monitorln(ip);
  long rssi = WiFi.RSSI();
  monitor("Signal strength (RSSI): "); monitor(rssi); monitorln(" dBm");
}

void handleClientRequests(float temperatureCelsius, float humidity) {
  float tempFahrenheit = temperatureCelsius * 1.8 + 32;
  WiFiClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        // Read and discard full request
        while (client.available()) client.read();
        // Allocate a temporary JsonDocument
        JsonDocument doc;
        doc["device"] = DEVICE_NAME;
        doc["sensor"] = SENSOR_NAME;
        doc["temperature"] = tempFahrenheit;
        doc["humidity"] = humidity;
        // Write response headers
        client.println(F("HTTP/1.0 200 OK"));
        client.println(F("Content-Type: application/json"));
        client.println("Access-Control-Allow-Origin: *");
        client.println(F("Connection: close"));
        client.print(F("Content-Length: "));
        client.println(measureJsonPretty(doc));
        client.println();
        // Write JSON document
        serializeJsonPretty(doc, client);
        // Log readings in the serial monitor
        monitor("Temperature: "); monitor(tempFahrenheit); monitorln(" F");
        monitor("Humidity: "); monitor(humidity); monitorln(" %");
      }
    }
    delay(100);
    client.stop();
  }
}

void initiateMonitoring(bool monitoringActivated) {
  if (monitoringActivated == 1) {
    Serial.begin(9600);
    while (!Serial) setStatusLED(RED);
  }
}

void initiateTemperatureSensor() {
  setStatusLED(YELLOW); monitorln("Initializing AHT10/AHT20...");
  if (! aht.begin()) {
    setStatusLED(MAGENTA); monitorln("AHT10/AHT20 not found");
    while (1) delay(10);
  }
  setStatusLED(CYAN); monitorln("AHT10/AHT20 found");
}

// Access Point Functions
void initiateWiFiAccessPoint() {
  char ap_ssid[] = ARDUINO_ACCESS_POINT_SSID;
  char ap_pass[] = ARDUINO_ACCESS_POINT_PASS;
  status = WiFi.beginAP(ap_ssid, ap_pass);
  if (status != WL_AP_LISTENING) {
    setStatusLED(RED); monitorln("Access Point failed to initialize!");
    while (true); // access point failed
  }
  setStatusLED(BLUE); monitorln("Access Point awaiting connection!");
}

void checkAccessPointConnectionStatus() {
  if (status != WiFi.status()) {
    status = WiFi.status();
    if (status == WL_AP_CONNECTED) {
      setStatusLED(GREEN); monitorln("Device connected to AP");
    } else {
      setStatusLED(BLUE); monitorln("Device disconnected from AP");
    }
  }
}

void initiateLED() {
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
}

void setStatusLED(int status) {
  switch (status) {
    case RED:
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDB, LOW);
      break;
    case YELLOW:
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, LOW);
      break;
    case GREEN:
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, LOW);
      break;
    case CYAN:
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, HIGH);
      break;
    case BLUE:
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDB, HIGH);
      break;
    case MAGENTA:
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDB, HIGH);
      break;
  }
}
