 /*************************************************** 
    Copyright (C) 2016  Steffen Ochs, Phantomias2006

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
    
    HISTORY: Please refer Github History
    
 ****************************************************/


//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Initialize Buttons
void set_button() {
  
  for (int i=0;i<NUMBUTTONS;i++) pinMode(buttonPins[i],INPUTMODE);
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Dedect Button Input
static inline boolean button_input() {
// Rückgabewert false ==> Prellzeit läuft, Taster wurden nicht abgefragt
// Rückgabewert true ==> Taster wurden abgefragt und Status gesetzt

  static unsigned long lastRunTime;
  //static unsigned long buttonDownTime[NUMBUTTONS];
  unsigned long now=millis();
  
  if (now-lastRunTime<PRELLZEIT) return false; // Prellzeit läuft noch
  
  lastRunTime=now;
  
  for (int i=0;i<NUMBUTTONS;i++)
  {
    byte curState=digitalRead(buttonPins[i]);
    if (INPUTMODE==INPUT_PULLUP) curState=!curState; // Vertauschte Logik bei INPUT_PULLUP
    if (buttonResult[i]>=SHORTCLICK) buttonResult[i]=NONE; // Letztes buttonResult löschen
    if (curState!=buttonState[i]) // Flankenwechsel am Button festgestellt
    {
      if (curState)   // Taster wird gedrückt, Zeit merken
      {
        if (buttonResult[i]==FIRSTUP && now-buttonDownTime[i]<DOUBLECLICKTIME)
          buttonResult[i]=DOUBLECLICK;
        else
        { 
          buttonDownTime[i]=now;
          buttonResult[i]=FIRSTDOWN;
        }
      }
      else  // Taster wird losgelassen
      {
        if (buttonResult[i]==FIRSTDOWN) buttonResult[i]=FIRSTUP;
        if (now-buttonDownTime[i]>=LONGCLICKTIME) buttonResult[i]=LONGCLICK;
      }
    }
    else // kein Flankenwechsel, Up/Down Status ist unverändert
    {
      if (buttonResult[i]==FIRSTUP && now-buttonDownTime[i]>DOUBLECLICKTIME)
        buttonResult[i]=SHORTCLICK;
    }
    buttonState[i]=curState;
  } // for
  return true;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Response Button Status
static inline void button_event() {

  static unsigned long lastMupiTime;    // Zeitpunkt letztes schnelles Zeppen
  int mupi = 0;
  bool event[3] = {0, 0, 0};
  int b_counter;
  
  /*
  // Reset der Config erwuenscht
  if (buttonResult[1]==DOUBLECLICK && inMenu == TEMPSUB) {
      displayblocked = true;
      question = CONFIGRESET;
      drawQuestion();
      return;
  }

  // Anzeigemodus wechseln
  if (buttonResult[0]==DOUBLECLICK && inMenu == TEMPSUB) {
    INACTIVESHOW = !INACTIVESHOW;

    #ifdef DEBUG
      Serial.println("[INFO]\tAnzeigewechsel");
    #endif
    return;
  }
  */

  // Bearbeitungsmodus aktivieren/deaktivieren
  if (buttonResult[0]==DOUBLECLICK) {
    
    if (inWork) {
      inWork = false;                     // Bearbeitung verlassen
      flashinwork = true;                 // Dauerhafte Symbolanzeige
      event[0] = 1;
      event[1] = 0;
      event[2] = 1;                       // Endwert setzen
    } else {
      switch (inMenu) {
      
        case TEMPKONTEXT:                 // Temperaturwerte bearbeiten
          inWork = true;
          break;

        case PITSUB:                      // Pitmasterwerte bearbeiten
          inWork = true;
          break;

        case SYSTEMSUB:                   // Systemeinstellungen bearbeiten
          inWork = true;
          break;

      }
      event[0] = 1;
      event[1] = 1;         // Startwert setzen
      event[2] = 0;
      
    }
  }

  // Tiefer im Menü
  if (buttonResult[1]==DOUBLECLICK) {}
  


  // Bei LONGCLICK rechts großer Zahlensprung jedoch gebremst
  if (buttonResult[0] == FIRSTDOWN && (millis()-buttonDownTime[0]>400)) {
    
    if (inWork) {
      mupi = 10;
      if (millis()-lastMupiTime > 200) {
        event[0] = 1;
        lastMupiTime = millis();
      }
    } else {
      buttonResult[0] = NONE;     // Manuelles Zurücksetzen um Überspringen der Menus zu verhindern
      switch (inMenu) {

        case MAINMENU:                    // Menu aufrufen
          inMenu = menu_count;
          displayblocked = false;
          b_counter = framepos[menu_count];
          ui.switchToFrame(b_counter);
          return;
      
        case TEMPSUB:                     // Main aufrufen
          displayblocked = true;
          drawMenu();
          inMenu = MAINMENU;
          return;

        case PITSUB:                      // Main aufrufen
          if (isback) {
            displayblocked = true;
            drawMenu();
            inMenu = MAINMENU;
            //modifyconfig(ePIT,{});
            isback = 0;
          }
          return;

        case SYSTEMSUB:                   // Main aufrufen
          if (isback) {
            displayblocked = true;
            drawMenu();
            inMenu = MAINMENU;
            modifyconfig(eSYSTEM,{});
            isback = 0;
          }
          return;

        case TEMPKONTEXT:                 // Temperaturmenu aufrufen
          if (isback) {
            b_counter = 0;
            modifyconfig(eCHANNEL,{});      // Am Ende des Kontextmenu Config speichern
            inMenu = TEMPSUB;
            ui.switchToFrame(b_counter);
            isback = 0;
          }
          return;
          

      }
    }
  }


  // Bei LONGCLICK links großer negativer Zahlensprung jedoch gebremst
  if (buttonResult[1] == FIRSTDOWN && (millis()-buttonDownTime[1]>400)) {

    if (inWork) {
      mupi = -10;
      if (millis()-lastMupiTime > 200) {
        event[0] = 1;
        lastMupiTime = millis();
      }
    } else {
      
      buttonResult[1] = NONE;     // Manuelles Zurücksetzen um Überspringen der Menus zu verhindern
      switch (inMenu) {
        case TEMPSUB:                     // Temperaturkontextmenu aufgerufen
          inMenu = TEMPKONTEXT;
          ui.switchToFrame(framepos[0]+1);
          return;

        case MAINMENU:                    // Menu aufrufen
          b_counter = 0;
          displayblocked = false;
          inMenu = TEMPSUB;
          ui.switchToFrame(b_counter);
          return;

        case PITSUB:                      // Main aufrufen
          displayblocked = true;
          drawMenu();
          inMenu = MAINMENU;
          //modifyconfig(ePIT,{});
          return;

        case SYSTEMSUB:                   // Main aufrufen
          displayblocked = true;
          drawMenu();
          inMenu = MAINMENU;
          modifyconfig(eSYSTEM,{});
          return;

        case TEMPKONTEXT:                 // Temperaturmenu aufrufen
          b_counter = 0;
          modifyconfig(eCHANNEL,{});      // Am Ende des Kontextmenu Config speichern
          inMenu = TEMPSUB;
          ui.switchToFrame(b_counter);
          return;

      }
    }
  }


  // Button rechts Shortclick: Vorwärts / hochzählen / Frage mit Ja beantwortet
  if (buttonResult[0] == SHORTCLICK) {

    if (question.typ > 0) {
      // Frage wurde mit YES bestätigt
      switch (question.typ) {
        case CONFIGRESET:
          setconfig(eCHANNEL,{});
          loadconfig(eCHANNEL);
          set_Channels();
          break;

        case HARDWAREALARM:
          ch[question.con].showalarm = false;
          break;

      }
      question.typ = NO;
      displayblocked = false;
      return;
    }

    if (inWork) {

      // Bei SHORTCLICK kleiner Zahlensprung
      mupi = 1;
      event[0] = 1;
      
    } else {

      b_counter = ui.getCurrentFrameCount();
      int i = 0;
    
      switch (inMenu) {
      
        case MAINMENU:                     // Menu durchwandern
          if (menu_count < 2) menu_count++;
          else menu_count = 0;
          drawMenu();
          break;

        case TEMPSUB:                     // Temperaturen durchwandern
          if (INACTIVESHOW) {
            current_ch++;
            if (current_ch > MAXCOUNTER) current_ch = MINCOUNTER;
          }
          else {
            do {
              current_ch++;
              i++;
              if (current_ch > MAXCOUNTER) current_ch = MINCOUNTER;
            } while ((ch[current_ch].temp==INACTIVEVALUE) && (i<CHANNELS)); 
          }
          ui.setFrameAnimation(SLIDE_LEFT);
          ui.transitionToFrame(0);      // Refresh
          break;

        case TEMPKONTEXT:
          if (b_counter < framepos[1]-1) b_counter++;
          else b_counter = framepos[0]+1;
          if (b_counter == framepos[1]-1) isback = 1;
          else isback = 0;
          ui.switchToFrame(b_counter);
          break;

        case PITSUB:
          if (b_counter < framepos[2]-1) b_counter++;
          else  b_counter = framepos[1];
          if (b_counter == framepos[2]-1) isback = 1;
          else isback = 0;
          ui.switchToFrame(b_counter);
          break;

        case SYSTEMSUB:
          if (b_counter < frameCount-1) b_counter++;
          else b_counter = framepos[2];
          if (b_counter == frameCount-1) isback = 1;
          else isback = 0;
          ui.switchToFrame(b_counter);
          break;
      }
      return;
    }
  }
  
  
  
  // Button links gedrückt: Rückwärts / runterzählen / Frage mit Nein beantwortet
  if (buttonResult[1] == SHORTCLICK) {

    // Frage wurde verneint -> alles bleibt beim Alten
    if (question.typ > 0) {

      switch (question.typ) {

        case HARDWAREALARM:
          return;

      }
      
      question.typ = NO;
      displayblocked = false;
      return;
    }

    if (inWork) {

      mupi = -1;
      event[0] = 1;
    
    } else {
    
      b_counter = ui.getCurrentFrameCount();
      int j = CHANNELS;
    
      switch (inMenu) {

        case MAINMENU:                     
          if (menu_count > 0) menu_count--;
          else menu_count = 2;
          drawMenu();
          break;
        
        case TEMPSUB: 
          if (INACTIVESHOW) {
            current_ch--;
            if (current_ch < MINCOUNTER) current_ch = MAXCOUNTER;
          } else {
            do {
              current_ch--;
              j--;
              if (current_ch < MINCOUNTER) current_ch = MAXCOUNTER;
            } while ((ch[current_ch].temp==INACTIVEVALUE) && (j > 0)); 
          }
          ui.setFrameAnimation(SLIDE_RIGHT);
          ui.transitionToFrame(0);      // Refresh
          break;

        case TEMPKONTEXT:
          if (b_counter > framepos[0]+1) {
            b_counter--;
            isback = 0;
          }
          else {
            b_counter = framepos[1]-1;
            isback = 1;
          }
          ui.switchToFrame(b_counter);
          break;

        case PITSUB:
          if (b_counter > framepos[1]) {
            b_counter--;
            isback = 0;
          } else {
            b_counter = framepos[2]-1;
            isback = 1;
          }
          ui.switchToFrame(b_counter);
          break;

        case SYSTEMSUB:
          if (b_counter > framepos[2]) {
            b_counter--;
            isback = 0;
          } else {
            b_counter = frameCount-1;
            isback = 1;
          }
          ui.switchToFrame(b_counter);
          break;
      }
      return;
    }
  }

  
    
  if (event[0]) {  
    switch (ui.getCurrentFrameCount()) {
        
      case 1:  // Upper Limit
        if (event[1]) tempor = ch[current_ch].max;
        tempor += (0.1*mupi);
        if (temp_unit == "C") {
          if (tempor > OLIMITMAX) tempor = OLIMITMIN;
          else if (tempor < OLIMITMIN) tempor = OLIMITMAX;
        } else {
          if (tempor > OLIMITMAXF) tempor = OLIMITMINF;
          else if (tempor < OLIMITMINF) tempor = OLIMITMAXF;
        }
        if (event[2]) ch[current_ch].max = tempor;
        break;
          
      case 2:  // Lower Limit
        if (event[1]) tempor = ch[current_ch].min;
        tempor += (0.1*mupi);
        if (temp_unit == "C") {
          if (tempor > ULIMITMAX) tempor = ULIMITMIN;
          else if (tempor < ULIMITMIN) tempor = ULIMITMAX;
        } else {
          if (tempor > ULIMITMAXF) tempor = ULIMITMINF;
          else if (tempor < ULIMITMINF) tempor = ULIMITMAXF;
        }
        if (event[2]) ch[current_ch].min = tempor;
        break;
          
      case 3:  // Typ
        if (mupi == 10) mupi = 1;
        if (event[1]) tempor = ch[current_ch].typ;
        tempor += mupi;
        if (tempor > (SENSORTYPEN-2)) tempor = 0;
        else if (tempor < 0) tempor = SENSORTYPEN-2;
        if (event[2]) ch[current_ch].typ = tempor;
        break;
        
      case 4:  // Alarm
        if (event[1]) tempor = ch[current_ch].alarm;
        if (mupi) tempor = !tempor; 
        if (event[2]) ch[current_ch].alarm = tempor;
        break;
        
      case 6:  // Pitmaster Typ
        if (mupi == 10) mupi = 1;
        if (event[1]) tempor = pitmaster.typ; 
        tempor += mupi;
        if (tempor > pidsize-1) tempor = 0;
        else if (tempor < 0) tempor = pidsize-1;
        if (event[2]) pitmaster.typ = tempor;
        break;
        
      case 7:  // Pitmaster Channel
        if (mupi == 10) mupi = 1;
        if (event[1]) tempor = pitmaster.channel;
        tempor += mupi;
        if (tempor > CHANNELS-1) tempor = 0;
        else if (tempor < 0) tempor = CHANNELS-1;
        if (event[2]) pitmaster.channel = tempor;
        break;
        
      case 8:  // Pitmaster Set
        if (event[1]) tempor = pitmaster.set;
        tempor += (0.1*mupi);
        if (tempor > ch[pitmaster.channel].max) tempor = ch[pitmaster.channel].min;
        else if (tempor < ch[pitmaster.channel].min) tempor = ch[pitmaster.channel].max;
        if (event[2]) pitmaster.set = tempor;
        break;
        
      case 9:  // Pitmaster Active
        if (event[1]) tempor = pitmaster.active;
        if (mupi) tempor = !tempor;
        if (event[2]) pitmaster.active = tempor;
        break;
        
      case 14:  // Unit Change
        if (event[1]) {
          if (temp_unit == "F") tempor = 1;
        }
        if (mupi) tempor = !tempor;
        if (event[2]) {
          String unit;
          if (tempor) unit = "F";
          else unit = "C";
          if (unit != temp_unit) {
            temp_unit = unit;
            transform_limits();                             // Transform Limits
            modifyconfig(eCHANNEL,{});                      // Save Config
            get_Temperature();                              // Update Temperature
            #ifdef DEBUG
              Serial.println("[INFO]\tEinheitenwechsel");
            #endif
          }
        }
        break;
        
      case 15:  // Hardware Alarm
        if (event[1]) tempor = doAlarm;
        if (mupi) tempor = !tempor;
        if (event[2]) doAlarm = tempor;
        break;
      
      case 17:  // Fastmode
        if (event[1]) tempor = INACTIVESHOW;
        if (mupi) tempor = !tempor;
        if (event[2]) INACTIVESHOW = tempor;
        break;
     
    }
  }
  
}





