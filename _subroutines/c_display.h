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
 
// OLED

#define SDA 0
#define SCL 2
#define I2C 0x3C


//++++++++++++++++++++++++++++++++++++++++++++++++++++++

SSD1306 display(I2C, SDA, SCL);
OLEDDisplayUi ui     ( &display );


//++++++++++++++++++++++++++++++++++++++++++++++++++++++

      
void drawConnect(int count, int active) {
    display.clear();
    display.drawXbm(34, 25, 60, 36, WiFi_Logo_bits);
    display.setColor(WHITE);
    
    for (int i = 0; i < count; i++) {
      const char *xbm;
      if (active == i) {
        xbm = active_bits;
      } else {
        xbm = inactive_bits;
      }
      display.drawXbm(64 - (12 * count / 2) + 12 * i, 5, 8, 8, xbm);
    }
    display.display();

}

