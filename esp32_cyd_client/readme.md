# 🌤️ ESP32 Temperature & Humidity Display Client with LVGL

## Dependencies
- WiFi
- HTTPClient
- ArduinoJson
- LVGL (Add lv_conf.h to Arduino Library Folder)
- TFT_eSPI (Update or Replace User_Setup.h)

## Dependencies

| Library        | Notes                                                                           |
|----------------|---------------------------------------------------------------------------------|
| `WiFi`         | Built-in with ESP32 Arduino core                                                |
| `HTTPClient`   | Also part of the ESP32 Arduino core                                             |
| `ArduinoJson`  | Install via Library Manager or from [arduinojson.org](https://arduinojson.org/) |
| `LVGL`         | Install via Library Manager or from [GitHub](https://github.com/Bodmer/TFT_eSPI)|
| `TFT_eSPI`     | Install via Library Manager or from [LVGL](https://lvgl.io/)                    |

Note: Add [`lv_conf.h`](esp32_cyd_client/LVGL/lv_conf.h) manually to your Arduino libraries folder. 

Note: Replace or edit [`User_Setup.h`](esp32_cyd_client/TFT_eSPI/User_Setup.h) to match your display configuration. 
