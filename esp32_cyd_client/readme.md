# 🌤️ ESP32 Temperature & Humidity Display Client with LVGL

## Dependencies

| Library        | Notes                                  |  More Info                                  |
|----------------|----------------------------------------|---------------------------------------------|
| `WiFi`         | Built-in with ESP32 Arduino core       |                                             |
| `HTTPClient`   | Also part of the ESP32 Arduino core    |                                             |
| `ArduinoJson`  | Install via Library Manager            | [arduinojson.org](https://arduinojson.org/) |
| `LVGL`         | Install via Library Manager            | [GitHub](https://github.com/Bodmer/TFT_eSPI)|
| `TFT_eSPI`     | Install via Library Manager            | [LVGL](https://lvgl.io/)                    |

Note: Add [`lv_conf.h`](esp32_cyd_client/LVGL/lv_conf.h) manually to your Arduino libraries folder. 

Note: Replace or edit [`User_Setup.h`](esp32_cyd_client/TFT_eSPI/User_Setup.h) to match your display configuration. 
