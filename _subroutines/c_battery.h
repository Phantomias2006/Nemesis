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
// Initialize Variables

int   BatteryPercentage = 0;
float BattMin = 3.4;          // bei 3V ist schluss
float BattMax = 4.19;

RunningMedian sam = RunningMedian(8);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Reading Battery Voltage

void get_Vbat() 
{
  int   vbatADC   = 0;              // The raw ADC value off the voltage div
  float vbatFloat = 0.0F;           // The ADC equivalent in millivolts
  float vbatLSB   = 1.0156F/1000;   // mV per LSB  // 1/1024
 
  // Read the analog in value:
  vbatADC = analogRead(0);
  vbatADC = analogRead(0);

 
  // Multiply the ADC by mV per LSB, and then
  // compensate the 200K+750K voltage divider
  vbatFloat = ((float)vbatADC * vbatLSB) * 4.82143F; //950/200;

  sam.add((vbatFloat-BattMin)/(BattMax - BattMin) * 100);
  BatteryPercentage = sam.getMedian();

  if (BatteryPercentage > 100) BatteryPercentage = 100;

 
  //oled.setBattery(vbatFloat/1000);
}
