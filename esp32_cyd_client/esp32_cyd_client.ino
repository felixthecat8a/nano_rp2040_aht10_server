#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "secrets.h"

// Main Loop Interval
unsigned long lastFetch = 0;
const unsigned long fetchInterval = 10000;

// Wi-Fi credentials (connect to Nano RP2040 AP)
const char* ssid = ARDUINO_ACCESS_POINT_SSID;
const char* password = ARDUINO_ACCESS_POINT_PASS;

// Weather JSON endpoint
const char* jsonURL = "http://192.168.4.1/";

// Display dimensions
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

// UI elements
lv_obj_t* disp_title;
lv_obj_t* label_temp;
lv_obj_t* label_humidity;
lv_obj_t* label_status;

// Connect to WiFi
void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" connected!");
  } else {
    Serial.println(" failed!");
  }
}

// Reconnect if needed
void ensureWiFiConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    lv_label_set_text(label_status, "Status: Reconnecting WiFi...");
    connectWiFi();
  }
}

// Setup UI with LVGL
void create_display_ui() {
  // Title
  disp_title = lv_label_create(lv_scr_act());
  lv_label_set_text(disp_title, "device / sensor");
  lv_obj_align(disp_title, LV_ALIGN_TOP_MID, 0, 10);
  static lv_style_t style_title;
  lv_style_init(&style_title);
  lv_style_set_text_font(&style_title, &lv_font_montserrat_24);
  lv_obj_add_style(disp_title, &style_title, 0);
  lv_obj_set_style_text_color(disp_title, lv_palette_main(LV_PALETTE_TEAL), 0);

  // Temperature Label
  label_temp = lv_label_create(lv_scr_act());
  lv_label_set_text(label_temp, "Temp: -- °F");
  lv_obj_align(label_temp, LV_ALIGN_CENTER, 0, -30);
  static lv_style_t style_temp;
  lv_style_init(&style_temp);
  lv_style_set_text_font(&style_temp, &lv_font_montserrat_22);
  lv_obj_add_style(label_temp, &style_temp, 0);

  // Humidity Label
  label_humidity = lv_label_create(lv_scr_act());
  lv_label_set_text(label_humidity, "Humidity: -- %");
  lv_obj_align(label_humidity, LV_ALIGN_CENTER, 0, 30);
  static lv_style_t style_hum;
  lv_style_init(&style_hum);
  lv_style_set_text_font(&style_hum, &lv_font_montserrat_22);
  lv_obj_add_style(label_humidity, &style_hum, 0);
  lv_obj_set_style_text_color(label_humidity, lv_palette_main(LV_PALETTE_BLUE_GREY), 0);

  // Status Label
  label_status = lv_label_create(lv_scr_act());
  lv_label_set_text(label_status, "Status: Ready");
  lv_obj_align(label_status, LV_ALIGN_BOTTOM_MID, 0, -10);
  static lv_style_t style_status;
  lv_style_init(&style_status);
  lv_style_set_text_font(&style_status, &lv_font_montserrat_12);
  lv_obj_add_style(label_status, &style_status, 0);
  lv_obj_set_style_text_color(label_status, lv_palette_main(LV_PALETTE_GREY), 0);
}

// Fetch JSON and update UI
void fetch_and_update_ui() {
  ensureWiFiConnected();
  lv_label_set_text(label_status, "Status: Fetching...");

  HTTPClient http;
  http.setTimeout(10000);
  http.begin(jsonURL);
  int code = http.GET();

  if (code == 200) {
    String payload = http.getString();
    Serial.println("Response: " + payload);

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload);
    
    if (!err && doc["temperature"].is<float>() && doc["humidity"].is<float>()) {
      const char* device = doc["device"] | "Unknown";
      const char* sensor = doc["sensor"] | "Unknown";

      lv_label_set_text_fmt(disp_title, "%s / %s", device, sensor);

      float temp = doc["temperature"];
      float hum = doc["humidity"];
      Serial.printf("Fetched: %.1f°F, %.1f%%\n", temp, hum);

      lv_label_set_text_fmt(label_temp, "Temp: %.1f °F", temp);
      lv_label_set_text_fmt(label_humidity, "Humidity: %.1f %%", hum);
      lv_label_set_text(label_status, "Status: OK");

      lv_color_t color;
      int ROOM_TEMP = 72;
      if (temp <= ROOM_TEMP - 15)       color = lv_palette_main(LV_PALETTE_DEEP_PURPLE);
      else if (temp <= ROOM_TEMP - 12)   color = lv_palette_main(LV_PALETTE_INDIGO);
      else if (temp <= ROOM_TEMP - 9)   color = lv_palette_main(LV_PALETTE_BLUE);
      else if (temp <= ROOM_TEMP - 6)   color = lv_palette_main(LV_PALETTE_LIGHT_BLUE);
      else if (temp <= ROOM_TEMP -3)    color = lv_palette_main(LV_PALETTE_CYAN);
      else if (temp <= ROOM_TEMP)       color = lv_palette_main(LV_PALETTE_GREEN);
      else if (temp <= ROOM_TEMP + 3)   color = lv_palette_main(LV_PALETTE_LIGHT_GREEN);
      else if (temp <= ROOM_TEMP + 6)   color = lv_palette_main(LV_PALETTE_LIME);
      else if (temp <= ROOM_TEMP + 9)   color = lv_palette_main(LV_PALETTE_YELLOW);
      else if (temp <= ROOM_TEMP + 12)  color = lv_palette_main(LV_PALETTE_AMBER);
      else if (temp <= ROOM_TEMP + 15)  color = lv_palette_main(LV_PALETTE_ORANGE);
      else if (temp <= ROOM_TEMP + 18)  color = lv_palette_main(LV_PALETTE_RED);
      else                              color = lv_palette_main(LV_PALETTE_PINK);

      lv_obj_set_style_text_color(label_temp, color, 0);
    } else {
      Serial.println("JSON parse error or missing keys");
      lv_label_set_text_fmt(label_status, "Status: JSON error: %s", err.c_str());
    }
  } else {
    Serial.printf("HTTP error: %d\n", http.errorToString(code));
    lv_label_set_text_fmt(label_status, "Status: HTTP error %d", code);
  }

  http.end();
}

// Arduino setup
void setup() {
  Serial.begin(115200);
  connectWiFi();

  lv_init();

  // Display setup
  lv_display_t* disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);  // Adjust as needed

  create_display_ui();
}

// Main loop
void loop() {
  lv_task_handler();
  lv_tick_inc(5);

  if (millis() - lastFetch >= fetchInterval) {
    lastFetch = millis();
    fetch_and_update_ui();
  }

  delay(5);
}
