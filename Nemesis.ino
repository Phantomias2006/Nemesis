#include <Wire.h>                 // I2C
#include <SPI.h>                  // SPI
#include <Ticker.h>               // TIMER
#include <SSD1306.h>              // TFT - ESP8266 OLED Driver for SSD1306: https://github.com/squix78/esp8266-oled-ssd1306
#include <ESP8266WiFi.h>          // WIFI
#include <ESP8266WiFiMulti.h>     // WIFI
#include <OLEDDisplayUi.h>        // TFT- ESP8266 OLED Driver for SSD1306: https://github.com/squix78/esp8266-oled-ssd1306
#include <MAX11613.h>             // NTC
#include <Adafruit_MAX31855.h>    // K-TYPE: https://github.com/adafruit/Adafruit-MAX31855-library
#include <RunningMedian.h>        // SAMPLES SMOOTH
#include <TelegramBot.h>          // TELEGRAM: https://github.com/Lstt2005/ESP8266_I.O.Broker
#include <ESP8266mDNS.h>          // OTA
#include <ArduinoOTA.h>           // OTA


// Include custom data (Reihenfolge ist wichtig)
#include "_subroutines/c_init.h"
#include "_subroutines/c_battery.h"
#include "_subroutines/c_temp.h"
#include "_icons/icons.h"
#include "_subroutines/c_display.h"
#include "_subroutines/c_wifi.h"
#include "_subroutines/c_frames.h"
#include "_subroutines/c_bot.h"
#include "_subroutines/c_ota.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++

void setup() {  

  // Initialize Debug 
  set_serial();
  
  // Initialize OLED
  set_OLED();
  
  // Initialize Wifi
  set_wifi();

  // Initialize OTA  
  set_ota();
  ArduinoOTA.begin();
  ota_hinweis();

  // Initialize ADC
  Sensor.begin(0xA0);

  // Initialize TELEGRAM
  bot.begin();      

  // Current Battery Voltage
  get_Vbat();
  get_rssi();
  
  // Initialize Timer
  set_timer();

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++

void loop() {
  
  ArduinoOTA.handle();
  
  int remainingTimeBudget = ui.update();
  if (remainingTimeBudget > 0) {
    // Don't do stuff if you are below your
    // time budget.
    
    if (readyForUpdate) {
      get_Vbat();
      get_rssi();
      
      if (!isAP) {
        sendData();
        
        bot.getUpdates(bot.message[0][1]);
        Bot_ExecMessages();
      }
      readyForUpdate = false;
    }
    
    if (readyForTemp) {
      get_Temperature();
      
      readyForTemp = false;
    }
    
    delay(remainingTimeBudget);
  }
}






