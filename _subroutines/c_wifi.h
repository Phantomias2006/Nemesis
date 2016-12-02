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
// Benutzerdaten

// WIFI
#define WIFISSID1 "xxx"
#define PASSWORD1 "xxx"

#define WIFISSID2 "xxx"
#define PASSWORD2 "xxx" 

// ACCESS POINT
#define APNAME "APOLLO"
#define APPASSWORT "12345678"

// TELEGRAM
#define BOTTOKEN "xxx" 
#define BOTNAME "xxx"
#define BOTUSERNAME "xxx"

// THINGSPEAK
#define THINGSPEAK_KEY "xxx"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialize WIFI

char apname[] = APNAME;
char appass[] = APPASSWORT;

IPAddress local_IP(192,168,66,1);
IPAddress gateway(192,168,66,1);
IPAddress subnet(255,255,255,0);

ESP8266WiFiMulti wifiMulti;
WiFiClient client;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialize Telegram BOT

TelegramBOT bot(BOTTOKEN, BOTNAME, BOTUSERNAME);


//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialize Thingspeak

const char* server = "api.thingspeak.com";
String apiKey = THINGSPEAK_KEY;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialize Variables

long rssi = 0;
byte isAP = 0;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Connect WiFi

void set_wifi() {

  WiFi.mode(WIFI_STA);
  Serial.print("Connecting");

  wifiMulti.addAP(WIFISSID1, PASSWORD1);
  wifiMulti.addAP(WIFISSID2, PASSWORD2);

  int counter = 0;
  while (wifiMulti.run() != WL_CONNECTED && counter < 20) {
    delay(500);
    Serial.print(".");
    drawConnect(3, counter % 3);
    counter++;
  }

  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected to: ");
	Serial.println(WiFi.SSID());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    isAP = false;
  }
  else {
    WiFi.mode(WIFI_AP);
    Serial.print("Configuring access point: ");
    Serial.print(APNAME);
    Serial.println(" ...");
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(apname, appass, 5);  // Channel 5
    Serial.print("AP IP address: ");
    //wifi.startLocalAPAndServer(apname,appass,"5","2121");
    Serial.println(WiFi.softAPIP());
    isAP = true;
  }

}


void get_rssi() {
  rssi = WiFi.RSSI();
  Serial.println(rssi);
}

