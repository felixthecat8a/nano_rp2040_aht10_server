# nano_rp2040_aht10_server
JSON WiFi server for Nano RP2040 + AHT10/AHT20 that shares sensor data over AP using WiFiNINA.

## 📚 Required Libraries

To compile and run this project on the Arduino Nano RP2040 Connect, you need the following libraries:

| Library             | Description                                         | Install From               |
|---------------------|-----------------------------------------------------|----------------------------|
| **WiFiNINA**        | Handles WiFi and Access Point functionality         | Arduino Library Manager    |
| **ArduinoJson**     | Formats the JSON response                           | Arduino Library Manager    |
| **Adafruit AHTX0**  | Interface for AHT10 and AHT20 temperature sensors   | Arduino Library Manager    |

> ⚠️ The **Adafruit AHTX0** library will also automatically install **Adafruit BusIO**, a required dependency.

### How to Install

1. Open the Arduino IDE.
2. Go to **Tools > Manage Libraries…**
3. Search for and install:
   - `WiFiNINA`
   - `ArduinoJson`
   - `Adafruit AHTX0`
