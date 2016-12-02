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
// Initialize Sensor

MAX11613 Sensor;
Adafruit_MAX31855 thermocouple(15);


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialize Variables

float temp[3] = {0.0, 0.0, 0.0};


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Calculate Temperature from ADC-Bytes

float calcT(uint32_t r, uint32_t typ){ 

  float Rmess = 47;
  float a, b, c, Rn;

  switch (typ) {
  case 1: { // Maverik
    Rn = 1000; a = 0.003358; b = 0.0002242; c = 0.00000261;
    break; }
  case 2: { // Fantast-Neu
    Rn = 220; a = 0.00334519; b = 0.000243825; c = 0.00000261726;
    break; }
  case 3: { // Maverik selbst gemessen
    Rn = 1000; a = 0.0033552819; b = 0.0002248386; c = 0.0000025964;
    break; }
  case 4: { // NTC 100K6A1B (lila Kopf)
    Rn = 100; a = 0.00335639; b = 0.000241116; c = 0.00000243362; 
    break; }
  case 5: { // NTC 100K (braun/schwarz/gelb/gold)
    Rn = 100; a = 0.003354016; b = 0.0002460380; c = 0.00000340538; 
    break; }
  default: { // NTC 5K3A1B (orange Kopf)
    Rn = 5; a = 0.0033555; b = 0.0002570; c = 0.00000243; } 
  }
  
  float Rt = Rmess*((4096.0/(4096-r)) - 1);
  float v = log(Rt/Rn);
  float erg = (1/(a + b*v + c*v*v)) - 273;
  return (erg>-10)?erg:0x00;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Reading Temperature ADC

void get_Temperature() {
  
  temp[0] = calcT(Sensor.get_Average(0),4);
  temp[1] = calcT(Sensor.get_Average(1),4);  
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Reading Temperature K-TYPE

void get_KTYPE() {

  //Serial.print("Internal Temp = ");
  //Serial.println(thermocouple.readInternal());
  
  float c = thermocouple.readCelsius();
  if (isnan(c)) {
     Serial.println("Something wrong with thermocouple!");
  } else {
  temp[2] = c;
  }
}
