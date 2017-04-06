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
    
    HISTORY:
    0.1.00 - 2016-12-30 initial version: Example ESP8266WebServer/FSBrowser
    
 ****************************************************/

//#include <ESP8266WebServer.h>   // https://github.com/esp8266/Arduino
//ESP8266WebServer server(80);    // declare webserver to listen on port 80
//File fsUploadFile;              // holds the current upload

#include <ESP8266mDNS.h>        
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>    // https://github.com/me-no-dev/ESPAsyncWebServer/issues/60
AsyncWebServer server(80);        // https://github.com/me-no-dev/ESPAsyncWebServer


const char* www_username = "admin";
const char* www_password = "esp8266";

String getContentType(String filename, AsyncWebServerRequest *request) {
  if (request->hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".json")) return "application/json";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path, AsyncWebServerRequest *request){
  
  if(path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path, request);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
      Serial.println(path);
      AsyncWebServerResponse *response = request->beginResponse(SPIFFS, path, contentType);
      if (path.endsWith(".gz"))
        response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    
    return true;
  }
  return false;
}

void buildDatajson(char *buffer, int len) {
  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  JsonObject& system = root.createNestedObject("system");

  system["time"] = String(now());
  system["utc"] = timeZone;
  system["soc"] = BatteryPercentage;
  system["charge"] = false;
  system["rssi"] = rssi;
  system["unit"] = temp_unit;

  JsonArray& channel = root.createNestedArray("channel");

  for (int i = 0; i < CHANNELS; i++) {
  JsonObject& data = channel.createNestedObject();
    data["number"]= i+1;
    data["name"]  = ch[i].name;
    data["typ"]   = ch[i].typ;
    data["temp"]  = ch[i].temp;
    data["min"]   = ch[i].min;
    data["max"]   = ch[i].max;
    data["alarm"] = ch[i].alarm;
    data["color"] = ch[i].color;
  }
  
  JsonObject& master = root.createNestedObject("pitmaster");

  master["channel"] = pitmaster.channel;
  master["typ"] = pitmaster.typ;
  master["value"] = pitmaster.value;
  master["set"] = pitmaster.set;
  master["active"] = pitmaster.active;

  size_t size = root.measureLength() + 1;
  //Serial.println(size);
  
  if (size < len) {
    root.printTo(buffer, size);
  } else Serial.println("Buffer zu klein");
  
}

void buildSettingjson(char *buffer, int len) {

  String host = HOSTNAME;
  host += String(ESP.getChipId(), HEX);

  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  JsonObject& _system = root.createNestedObject("system");

  _system["time"] = String(now());
  _system["utc"] = timeZone;
  _system["ap"] = APNAME;
  _system["host"] = host;
  _system["language"] = "de";
  _system["unit"] = temp_unit;
  _system["version"] = FIRMWAREVERSION;
  
  JsonArray& _typ = root.createNestedArray("sensors");
  for (int i = 0; i < SENSORTYPEN; i++) {
    _typ.add(ttypname[i]);
  }

  JsonArray& _pit = root.createNestedArray("pitmaster");
  for (int j = 0; j < pidsize; j++) {
    _pit.add(pid[j].name);
  }

  JsonObject& _chart = root.createNestedObject("charts");
  _chart["thingspeak"] = THINGSPEAK_KEY;
    
  size_t size = root.measureLength() + 1;
  //Serial.println(size);
  
  if (size < len) {
    root.printTo(buffer, size);
  } else Serial.println("Buffer zu klein");
  
}

void handleData(AsyncWebServerRequest *request) {
  
  static char sendbuffer[1200];
  buildDatajson(sendbuffer, 1200);
  request->send(200, "application/json", sendbuffer);
}


void handleSettings(AsyncWebServerRequest *request) {

  static char sendbuffer[300];
  buildSettingjson(sendbuffer, 300);
  request->send(200, "application/json", sendbuffer);
}


bool handleSetChannels(AsyncWebServerRequest *request) {
  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(request->arg("plain"));   //https://github.com/esp8266/Arduino/issues/1321
  if (!json.success()) return 0;

  for (int i = 0; i < CHANNELS; i++) {
    JsonObject& _cha = json["channel"][i];
    String _name = _cha["name"].asString();                       // KANALNAME
    if (_name.length() < 11)  ch[i].name = _name;
    byte _typ = _cha["typ"];                                      // FÜHLERTYP
    if (_typ > -1 && _typ < SENSORTYPEN) ch[i].typ = _typ;  
    float _limit = _cha["min"];                                   // LIMITS
    if (_limit > LIMITUNTERGRENZE && _limit < LIMITOBERGRENZE) ch[i].min = _limit;
    _limit = _cha["max"];
    if (_limit > LIMITUNTERGRENZE && _limit < LIMITOBERGRENZE) ch[i].max = _limit;
    ch[i].alarm = _cha["alarm"];                                  // ALARM
    ch[i].color = _cha["color"].asString();                       // COLOR
  }

  modifyconfig(eCHANNEL,{});                                      // SPEICHERN
  return 1;
}

bool handleSetPitmaster(AsyncWebServerRequest *request) {
  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& _pitmaster = jsonBuffer.parseObject(request->arg("plain"));   //https://github.com/esp8266/Arduino/issues/1321
  if (!_pitmaster.success()) return 0;
  
  pitmaster.channel = _pitmaster["channel"]; // 0
  pitmaster.typ = _pitmaster["typ"]; // ""
  pitmaster.set = _pitmaster["value"]; // 100
  pitmaster.active = _pitmaster["active"];
  return 1;
}

bool handleSetNetwork(AsyncWebServerRequest *request) {
  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& _network = jsonBuffer.parseObject(request->arg("plain"));   //https://github.com/esp8266/Arduino/issues/1321
  if (!_network.success()) return 0;
  
  const char* data[2];
  data[0] = _network["ssid"];
  data[1] = _network["password"];

  if (!modifyconfig(eWIFI,data)) {
    #ifdef DEBUG
      Serial.println("[INFO]\tFailed to save wifi config");
    #endif
    return 0;
  } else {
    #ifdef DEBUG
      Serial.println("[INFO]\tWifi config saved");
    #endif
    return 1;
  }
}

bool handleSetSystem(AsyncWebServerRequest *request) {
  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& _system = jsonBuffer.parseObject(request->arg("plain"));   //https://github.com/esp8266/Arduino/issues/1321
  if (!_system.success()) return 0;
  
  // = _system["hwalarm"];
  // = _system["host"];
  // = _system["utc"];
  // = _system["language"];
  // = _system["unit"];
  
  return 1;
}

void buildWifiScanjson(char *buffer, int len) {

  // https://github.com/me-no-dev/ESPAsyncWebServer/issues/85
  int n = WiFi.scanComplete();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  if (n > 0) {
  
    if (WiFi.status() == WL_CONNECTED)  {
      json["Connect"] = true;
      json["Scantime"] = millis()-scantime;
      json["SSID"] = WiFi.SSID();
      json["IP"] = WiFi.localIP().toString();
      json["Mask"] = WiFi.subnetMask().toString();  
      json["Gate"] = WiFi.gatewayIP().toString();
    }
    else {
      json["Connect"] = false;
      json["SSID"] = APNAME;
      json["IP"] = WiFi.softAPIP().toString();
    }
  
    JsonArray& _scan = json.createNestedArray("Scan");
    for (int i = 0; i < n; i++) {
      JsonObject& _wifi = _scan.createNestedObject();
      _wifi["SSID"] = WiFi.SSID(i);
      _wifi["RSSI"] = WiFi.RSSI(i);
      _wifi["Enc"] = WiFi.encryptionType(i);
      //_wifi["Hid"] = WiFi.isHidden(i);
    }
  }
  
  size_t size = json.measureLength() + 1;
  
  if (size < len) {
    json.printTo(buffer, size);
  } else Serial.println("Buffer zu klein");
}

void handleWifiResult(AsyncWebServerRequest *request) {
  static char sendbuffer[1200];
  buildWifiScanjson(sendbuffer, 1200);  
  request->send(200, "application/json", sendbuffer);
}

void handleWifiScan(AsyncWebServerRequest *request) {

  //dumpClients();

  WiFi.scanDelete();
  if(WiFi.scanComplete() == -2){
    WiFi.scanNetworks(true);
    scantime = millis();
    request->send(200, "text/json", "OK");
  }   
}

String getMacAddress()  {
  uint8_t mac[6];
  char macStr[18] = { 0 };
  WiFi.macAddress(mac);
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return  String(macStr);
}



void server_setup() {

    String host = HOSTNAME;
    host += String(ESP.getChipId(), HEX);
    MDNS.begin(host.c_str());  // siehe Beispiel: WiFi.hostname(host); WiFi.softAP(host);
    Serial.print("[INFO]\tOpen http://");
    Serial.print(host);
    Serial.println("/data to see the current temperature");
  

    //list Temperature Data
    server.on("/data", HTTP_GET, handleData);
    
    //list Setting Data
    server.on("/settings", HTTP_GET, handleSettings);
    
    //list Wifi Scan
    server.on("/networkscan", HTTP_GET, handleWifiScan);

    server.on("/networklist", HTTP_GET, handleWifiResult);

    /*
    server.on("/index.html",HTTP_GET, [](AsyncWebServerRequest *request) {
      if (!handleFileRead("/index.html", request))
        request->send(404, "text/plain", "FileNotFound");
    });
    */

    // to avoid multiple requests to ESP
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html"); // gibt alles im Ordner frei
    
    // serves all SPIFFS Web file with 24hr max-age control
    //server.serveStatic("/font", SPIFFS, "/font","max-age=86400");
    //server.serveStatic("/js",   SPIFFS, "/js"  ,"max-age=86400");
    //server.serveStatic("/css",  SPIFFS, "/css" ,"max-age=86400");
    //server.serveStatic("/png",  SPIFFS, "/png" ,"max-age=86400");

    /*
    server.on("/", [](AsyncWebServerRequest *request){
      if(!handleFileRead("/", request))
        request->send(404, "text/plain", "FileNotFound");
    });
    */
    

    server.begin();
    #ifdef DEBUG
    Serial.println("[INFO]\tHTTP server started");
    #endif
    MDNS.addService("http", "tcp", 80);

}

