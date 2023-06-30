
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <arduino_homekit_server.h>
#include "wifi_info.h"

#define LED_PIN 2
#define LED_COUNT 60

#define LOG_D(fmt, ...) printf_P(PSTR(fmt "\n"), ##__VA_ARGS__);

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

bool on;
int rgb = 0;

void setup() {
  Serial.begin(115200);
  wifi_connect();           // in wifi_info.h
                            //homekit_storage_reset(); // to remove the previous HomeKit pairing storage when you first run this new HomeKit example
  strip.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();             // Turn OFF all pixels ASAP
  strip.setBrightness(50);  // Set BRIGHTNESS to about 1/5 (max = 255)

  my_homekit_setup();
}

void loop() {
  my_homekit_loop();
  delay(10);
}

//==============================
// HomeKit setup and loop
//==============================

// access your HomeKit characteristics defined in my_accessory.c
extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t cha_switch_on;

static uint32_t next_heap_millis = 0;




//Called when the switch value is changed by iOS Home APP
void cha_switch_on_setter(const homekit_value_t value) {
  on = value.bool_value;
  cha_switch_on.value.bool_value = on;  //sync the value
  LOG_D("Switch: %s", on ? "ON" : "OFF");

  if (on) {
    // Re-initialise RGB value.
    rgb = 0;
  } else {
    Serial.println("Off");
    strip.clear();
    strip.show();
  }
}



void my_homekit_setup() {
  cha_switch_on.setter = cha_switch_on_setter;
  arduino_homekit_setup(&config);
}

void my_homekit_loop() {
  arduino_homekit_loop();
  const uint32_t t = millis();
  if (t > next_heap_millis) {
    // show heap info every 5 seconds
    next_heap_millis = t + 5 * 1000;
    LOG_D("Free heap: %d, HomeKit clients: %d",
          ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
  }

  // Do the rainbow ting.
  if (on) {
    rainbow(rgb);
    // Change this value to change how quickly the rgb values cycle.
    rgb += 1280;
  }

}

void rainbow(int colour) {
  strip.rainbow(colour % 65536);
  strip.show();
  }
