#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "secrets.h"

// Loop Interval
unsigned long lastFetch = 0;
const unsigned long fetchInterval = 10000;

// Wi-Fi credentials
const char* ssid = ARDUINO_ACCESS_POINT_SSID;
const char* password = ARDUINO_ACCESS_POINT_PASS;

// JSON endpoint
const char* jsonURL = "http://192.168.4.1/";

// Display config
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint16_t draw_buf[DRAW_BUF_SIZE / 2];

// UI elements
lv_obj_t* arc_temp;
lv_obj_t* label_temp;
lv_obj_t* arc_hum;
lv_obj_t* label_hum;
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
  if (WiFi.status() == WL_CONNECTED) Serial.println(" connected!");
  else Serial.println(" failed!");
}

void ensureWiFiConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    lv_label_set_text(label_status, "Status: Reconnecting WiFi...");
    connectWiFi();
  }
}

// Create UI
void create_display_ui() {
  // ========== Temperature Container ==========
  lv_obj_t* cont_temp = lv_obj_create(lv_scr_act());
  lv_obj_set_size(cont_temp, 175, 210);
  lv_obj_align(cont_temp, LV_ALIGN_LEFT_MID, 5, -5);
  lv_obj_set_style_pad_all(cont_temp, 5, 0);
  lv_obj_set_style_radius(cont_temp, 10, 0);
  lv_obj_set_style_bg_color(cont_temp, lv_color_hex(0xfefefe), 0);
  lv_obj_set_style_border_width(cont_temp, 0, 0);

  // Temp Title
  lv_obj_t* title_temp = lv_label_create(cont_temp);
  lv_label_set_text(title_temp, "Temperature");
  lv_obj_align(title_temp, LV_ALIGN_TOP_MID, 0, 0);
  static lv_style_t style_title_temp;
  lv_style_init(&style_title_temp);
  lv_style_set_text_font(&style_title_temp, &lv_font_montserrat_24);
  lv_obj_add_style(title_temp, &style_title_temp, 0);
  lv_obj_set_style_text_color(title_temp, lv_palette_main(LV_PALETTE_TEAL), 0);

  // Temperature Arc
  arc_temp = lv_arc_create(cont_temp);
  lv_obj_set_size(arc_temp, 150, 150);
  lv_obj_align(arc_temp, LV_ALIGN_CENTER, 0, 20);
  lv_arc_set_rotation(arc_temp, 135);
  lv_arc_set_bg_angles(arc_temp, 0, 270);
  lv_arc_set_range(arc_temp, -4, 104);
  lv_arc_set_value(arc_temp, 0);
  lv_obj_set_style_arc_color(arc_temp, lv_color_hex(0x666666), LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(arc_temp, lv_color_hex(0x333333), LV_PART_KNOB);
  lv_obj_remove_style(arc_temp, NULL, LV_PART_KNOB);

  // Temp Value Label (inside arc)
  label_temp = lv_label_create(cont_temp);
  lv_label_set_text(label_temp, "-- °F");
  lv_obj_align(label_temp, LV_ALIGN_CENTER, 0, 20);
  static lv_style_t style_arc_temp_text;
  lv_style_init(&style_arc_temp_text);
  lv_style_set_text_font(&style_arc_temp_text, &lv_font_montserrat_30);
  lv_obj_add_style(label_temp, &style_arc_temp_text, 0);

  // ========== Humidity Container ==========
  lv_obj_t* cont_hum = lv_obj_create(lv_scr_act());
  lv_obj_set_size(cont_hum, 130, 180);
  lv_obj_align(cont_hum, LV_ALIGN_RIGHT_MID, -5, -5);
  lv_obj_set_style_pad_all(cont_hum, 5, 0);
  lv_obj_set_style_radius(cont_hum, 10, 0);
  lv_obj_set_style_bg_color(cont_hum, lv_color_hex(0xfefefe), 0);
  lv_obj_set_style_border_width(cont_hum, 0, 0);

  // Humidity Title
  lv_obj_t* title_hum = lv_label_create(cont_hum);
  lv_label_set_text(title_hum, "Humidity");
  lv_obj_align(title_hum, LV_ALIGN_TOP_MID, 0, 0);
  static lv_style_t style_title_hum;
  lv_style_init(&style_title_hum);
  lv_style_set_text_font(&style_title_hum, &lv_font_montserrat_22);
  lv_obj_add_style(title_hum, &style_title_hum, 0);
  lv_obj_set_style_text_color(title_hum, lv_palette_main(LV_PALETTE_DEEP_PURPLE), 0);

  // Humidity Arc (smaller)
  arc_hum = lv_arc_create(cont_hum);
  lv_obj_set_size(arc_hum, 110, 110);  // Smaller arc
  lv_obj_align(arc_hum, LV_ALIGN_CENTER, 0, 15);
  lv_arc_set_rotation(arc_hum, 135);
  lv_arc_set_bg_angles(arc_hum, 0, 270);
  lv_arc_set_range(arc_hum, 0, 100);
  lv_arc_set_value(arc_hum, 0);
  lv_obj_set_style_arc_color(arc_hum, lv_color_hex(0x666666), LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(arc_hum, lv_color_hex(0x333333), LV_PART_KNOB);
  lv_obj_remove_style(arc_hum, NULL, LV_PART_KNOB);

  // Humidity Label
  label_hum = lv_label_create(cont_hum);
  lv_label_set_text(label_hum, "-- %");
  lv_obj_align(label_hum, LV_ALIGN_CENTER, 0, 15);
  static lv_style_t style_hum;
  lv_style_init(&style_hum);
  lv_style_set_text_font(&style_hum, &lv_font_montserrat_18);
  lv_obj_add_style(label_hum, &style_hum, 0);
  lv_obj_set_style_text_color(label_hum, lv_palette_main(LV_PALETTE_DEEP_PURPLE), 0);

  // Status label (outside of both containers)
  label_status = lv_label_create(lv_scr_act());
  lv_label_set_text(label_status, "Status: Ready");
  lv_obj_align(label_status, LV_ALIGN_BOTTOM_MID, 0, -5);
  static lv_style_t style_status;
  lv_style_init(&style_status);
  lv_style_set_text_font(&style_status, &lv_font_montserrat_10);
  lv_obj_add_style(label_status, &style_status, 0);
}

// Update UI with fetched JSON
void fetch_and_update_ui() {
  ensureWiFiConnected();
  lv_label_set_text(label_status, "Status: Fetching...");

  HTTPClient http;
  http.setTimeout(10000);
  http.begin(jsonURL);
  int code = http.GET();

  if (code == 200) {
    String payload = http.getString();
    Serial.println(payload);  // for debug

    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, payload);


    if (!err && doc.containsKey("temperature") && doc.containsKey("humidity")) {
      float temp = doc["temperature"];
      float hum = doc["humidity"];

      // Update Temperature
      lv_label_set_text_fmt(label_temp, "%.1f°F", temp);
      lv_arc_set_value(arc_temp, temp);

      // Arc color based on temp
      lv_color_t temp_color;
      int ROOM_TEMP = 72;
      if (temp <= ROOM_TEMP - 15)       temp_color = lv_palette_main(LV_PALETTE_PURPLE);
      else if (temp <= ROOM_TEMP - 12)  temp_color = lv_palette_main(LV_PALETTE_INDIGO);
      else if (temp <= ROOM_TEMP - 9)   temp_color = lv_palette_main(LV_PALETTE_BLUE);
      else if (temp <= ROOM_TEMP - 6)   temp_color = lv_palette_main(LV_PALETTE_LIGHT_BLUE);
      else if (temp <= ROOM_TEMP -3)    temp_color = lv_palette_main(LV_PALETTE_CYAN);
      else if (temp <= ROOM_TEMP)       temp_color = lv_palette_main(LV_PALETTE_GREEN);
      else if (temp <= ROOM_TEMP + 3)   temp_color = lv_palette_main(LV_PALETTE_LIGHT_GREEN);
      else if (temp <= ROOM_TEMP + 6)   temp_color = lv_palette_main(LV_PALETTE_LIME);
      else if (temp <= ROOM_TEMP + 9)   temp_color = lv_palette_main(LV_PALETTE_YELLOW);
      else if (temp <= ROOM_TEMP + 12)  temp_color = lv_palette_main(LV_PALETTE_AMBER);
      else if (temp <= ROOM_TEMP + 15)  temp_color = lv_palette_main(LV_PALETTE_ORANGE);
      else if (temp <= ROOM_TEMP + 18)  temp_color = lv_palette_main(LV_PALETTE_RED);
      else                              temp_color = lv_palette_main(LV_PALETTE_PINK);

      lv_obj_set_style_arc_color(arc_temp, temp_color, LV_PART_INDICATOR);
      lv_obj_set_style_text_color(label_temp, temp_color, 0);

      // Update Humidity
      lv_label_set_text_fmt(label_hum, "%.1f %%", hum);
      lv_arc_set_value(arc_hum, hum);

      // Arc color based on hum
      lv_color_t hum_color;
      if (hum <= 40)       hum_color = lv_palette_main(LV_PALETTE_PURPLE);
      else if (hum <= 60)  hum_color = lv_palette_main(LV_PALETTE_DEEP_PURPLE);
      else                  hum_color = lv_palette_main(LV_PALETTE_DEEP_ORANGE);

      lv_obj_set_style_arc_color(arc_hum, hum_color, LV_PART_INDICATOR);
      lv_obj_set_style_text_color(label_hum, hum_color, 0);

      // Update Status
      lv_label_set_text(label_status, "Status: OK");
    } else {
      lv_label_set_text_fmt(label_status, "Status: JSON error: %s", err.c_str());
    }
  } else {
    Serial.print("HTTP Error: ");
    Serial.println(http.errorToString(code));
    lv_label_set_text_fmt(label_status, "Status: HTTP error %d", code);
  }

  http.end();
}

// Setup
void setup() {
  Serial.begin(115200);
  connectWiFi();

  lv_init();

  // Display
  lv_display_t* disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);

  create_display_ui();
}

void loop() {
  lv_task_handler();
  lv_tick_inc(5);

  if (millis() - lastFetch >= fetchInterval) {
    lastFetch = millis();
    fetch_and_update_ui();
  }

  delay(5);
}
