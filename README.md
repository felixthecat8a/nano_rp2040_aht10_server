# nano_rp2040_aht10_server
JSON WiFi server for Nano RP2040 + AHT10/AHT20 that shares sensor data over AP using WiFiNINA.

## Required Libraries

To compile and run this project on the Arduino Nano RP2040 Connect, you need the following libraries:

| Library             | Description                                         | Install From               |
|---------------------|-----------------------------------------------------|----------------------------|
| **WiFiNINA**        | Handles WiFi and Access Point functionality         | Arduino Library Manager    |
| **ArduinoJson**     | Formats the JSON response                           | Arduino Library Manager    |
| **Adafruit AHTX0**  | Interface for AHT10 and AHT20 temperature sensors   | Arduino Library Manager    |

> The **Adafruit AHTX0** library will also automatically install **Adafruit BusIO**, a required dependency.

### How to Install

1. Open the Arduino IDE.
2. Go to **Tools > Manage Libraries…**
3. Search for and install:
   - `WiFiNINA`
   - `ArduinoJson`
   - `Adafruit AHTX0`

## Hardware Used

| Component                          | Notes                                                                |
|------------------------------------|----------------------------------------------------------------------|
| **Arduino Nano RP2040 Connect**    | Built-in WiFi; acts as a wireless JSON server                        |
| **AHT10 or AHT20 Sensor**          | I2C temperature and humidity sensor                                  |
| **20 Gauge Silicone Wire**         | Flexible, durable wire for reliable sensor connections               |
| **Nano Screw Terminal Adapter**    | Breaks out Nano pins into screw terminals for secure wiring          |
| **USB-Micro Cable**                | Used to program and power the Nano RP2040 Connect                    |

> Optional: You can also use a breadboard or headers depending on your project enclosure or prototyping needs.

### Sensor Wiring (AHT10/AHT20 to Nano RP2040)

| AHT Sensor Pin | Connects To Nano RP2040 |
|----------------|--------------------------|
| VIN / VCC      | 3.3V                     |
| GND            | GND                      |
| SDA            | SDA (Pin 24 / D4)        |
| SCL            | SCL (Pin 25 / D5)        |

> **Note:** The AHT10/AHT20 operates at 3.3V logic.

