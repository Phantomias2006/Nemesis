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
// Data Transfer to THINGSPEAK

void sendData() {

  if (client.connect(server,80)) { // "184.106.153.149" or api.thingspeak.com
   
    String postStr = apiKey;
    postStr +="&1=";
    postStr += String(temp[0],1);
    postStr +="&2=";
    postStr += String(temp[1],1);
    postStr +="&3=";
    postStr += String(BatteryPercentage);

    // Sendedauer: 1s  
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  }
  
  client.stop();
 }
 

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Data Communication with TELEGRAM

void Bot_ExecMessages() {
	
  for (int i = 1; i < bot.message[0][0].toInt() + 1; i++) {
    
	//bot.message[i][5]=bot.message[i][5].substring(1,bot.message[i][5].length());
    if (bot.message[i][5] == "/on") {
      digitalWrite(5, LOW);   // turn DAC on
      bot.sendMessage(bot.message[i][4], "DAC is ON", "");
    }
    
	if (bot.message[i][5] == "/off") {
      digitalWrite(5, HIGH);    // turn DAC off
      bot.sendMessage(bot.message[i][4], "DAC is OFF", "");
    }
    
	if (bot.message[i][5] == "/temp") {
      String postStr = "Kanal 1: ";
      postStr += String(temp[0],1);
      postStr += " °C";
      bot.sendMessage(bot.message[i][4], postStr, "");
      postStr = "Kanal 2: ";
      postStr += String(temp[1],1);
      postStr += " °C";
      bot.sendMessage(bot.message[i][4], postStr, "");
    }
    
    if (bot.message[i][5] == "/info") {
      String wellcome = "Welcome from NanoBot, your personal BBQ-Bot.";
      String wellcome1 = "/on : to switch the DAC ON";
      String wellcome2 = "/off : to switch the DAC OFF";
      String wellcome3 = "/temp : to get temperature";
      bot.sendMessage(bot.message[i][4], wellcome, "");
      bot.sendMessage(bot.message[i][4], wellcome1, "");
      bot.sendMessage(bot.message[i][4], wellcome2, "");
      bot.sendMessage(bot.message[i][4], wellcome3, "");
    
    }
  }
  bot.message[0][0] = "";   // All messages have been replied - reset new messages
}


//-------------------------------------------------------
// Testversion
/*
void Bot_update() {
  message m = bot.getUpdates(); // Read new messages 
  if (m.text.equals("On"))
        { 
    digitalWrite(5, HIGH); 
    Serial.println("message received"); 
    bot.sendMessage(m.chat_id, "The Led is now ON"); 
  } 
  else if (m.text.equals("Off"))
        { 
    digitalWrite(5, LOW); 
    Serial.println("message received"); 
    bot.sendMessage(m.chat_id, "The Led is now OFF"); 
  }
}
*/


