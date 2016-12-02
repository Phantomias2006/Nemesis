 /*************************************************** 
    Copyright (C) 2016  Steffen Ochs

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************/
 

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialising Variables

byte flash = 0;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Status Row

void gBattery(OLEDDisplay *display, OLEDDisplayUiState* state) {

  int MaxBatteryBar = 13;
  int battPixel = (BatteryPercentage*MaxBatteryBar)/100;  
  if(flash == 0){flash = 1;}
  else {flash = 0;} //Toggle flash flag for icon blinking later
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(Noto_Sans_8);
  //display->drawString(128, 0, String(BatteryPercentage)+"%");
  display->drawString(24,0,String(BatteryPercentage));
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  if (isAP)  display->drawString(128,0,"AP");
  else display->drawString(128,0,String(rssi)+" dBm");
  
  if (flash && BatteryPercentage < 10) {}
  else {
  display->fillRect(18,3,2,4); //Draw battery end button
  display->fillRect(16,8,1,1); //Untere Ecke
  display->drawRect(0,1,16,7); //Draw Outline
  display->fillRect(2,3,battPixel,4);  // Draw Battery Status
  }
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Main Frames

void drawTemp1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(114 + x, 20 + y, "Kanal 1");
  display->drawXbm(x, y+8, 60, 60, xbmtemp);
  display->setFont(ArialMT_Plain_16);
  display->drawString(114 + x, 36 + y, String(temp[0],1) + " °C");
}

void drawTemp2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(114 + x, 20 + y, "Kanal 2");
  display->drawXbm(x, y+8, 60, 60, xbmtemp);
  display->setFont(ArialMT_Plain_16);
  display->drawString(114 + x, 36 + y, String(temp[1],1) + " °C");
}

void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  //display.drawXbm(x + 7, y + 7, 50, 50, getIconFromString(weather.getIconTomorrow()));
  display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display->setFont(ArialMT_Plain_10);
  //display->drawString(90 + x, 20 + y, "");
  if (isAP) {
    display->drawString(DISPLAY_WIDTH/2 +x, DISPLAY_HEIGHT/3 +y, "IP Adresse: \n" + WiFi.softAPIP().toString()+ "\n SSID:" + APNAME);
  }
  else {
     display->drawString(DISPLAY_WIDTH/2 +x, DISPLAY_HEIGHT/2 +y, "IP Adresse: \n" + WiFi.localIP().toString());
  }
}





// ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialising Frames

// this array keeps function pointers to all frames
// frames are the single views that slide from right to left
FrameCallback frames[] = { drawTemp1, drawTemp2 };  // drawFrame3

// how many frames are there?
int frameCount = 2;   // 3

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { gBattery };
int overlaysCount = 1;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Configuration OLEDDisplay

void set_OLED() {
  
  // The ESP is capable of rendering 60fps in 80Mhz mode
  // but that won't give you much time for anything else
  // run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(30);

  // Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(TOP);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Add overlays
  ui.setOverlays(overlays, overlaysCount);

  ui.setTimePerFrame(10000);

  // Initialising the UI will init the display too.
  ui.init();

  display.flipScreenVertically();

  display.clear();
  display.display();

}



