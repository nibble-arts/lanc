// LANC Remote Control
// Copyright: Thomas Winkler 2013-2016
// thomas.winkler@iggmp.net

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Dieses Programm wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License für weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/

// Video camera control via LANC on Aurduino micro
// with status output

// functions: rec/pause
//            zoom

// LANC data
// Display ON/OFF
//byte0 = 0x18;
//byte1 = 0xB4;

// VTR Record
//byte0 = 0x18;
//byte1 = 0x3A;

// VTR Playback
//byte0 = 0x18;
//byte1 = 0x34;

// VTR Stop
//byte0 = 0x18;
//byte1 = 0x30;

// VTR Pause
//byte0 = 0x18;
//byte1 = 0x32;

// Fast Forward >> continous
//byte0 = 0x18;
//byte1 = 0x38;

// Fast Rewind << continuous
//byte0 = 0x18;
//byte1 = 0x36;

// Fast Forward >>
//byte0 = 0x18;
//byte1 = 0x38;

// Fast Rewind <<
//byte0 = 0x18;
//byte1 = 0x36;

// Eject
//byte0 = 0x18;
//byte1 = 0x2C;

// Start/Stop
//byte0 = 0x18;
//byte1 = 0x33;

// Wide Zoom
//byte0 = 0x28;
//byte1 = 0x37;

// Tele Zoom
//byte0 = 0x28;
//byte1 = 0x35;


// port definitions

#define debugPort 10
#define actionPort 11

#define recPort 2
#define pausePort 3
#define notRecPort 4
#define notPausePort 5

#define inPort 6 // connected
#define outPort 7 // connected
#define button 12 // connected

#define zoomHor A0
#define zoomVer A1
#define maxSpeed A2
#define accell A3

int inTime = 60;
int outTime = 85;
int startBitTime = 95;

//========================================================
// init ports
void setup() {

  pinMode(inPort, INPUT);
  digitalWrite(inPort, LOW);
  
  pinMode(outPort, OUTPUT);
  digitalWrite(outPort, LOW);

  pinMode(button, INPUT);
  digitalWrite(button, HIGH);
  
// tally output
  pinMode(recPort, OUTPUT);
  digitalWrite(recPort, LOW);

  pinMode(pausePort, OUTPUT);
  digitalWrite(pausePort, LOW);
  
// error output
  pinMode(notRecPort, OUTPUT);
  digitalWrite(notRecPort, HIGH);
  
  pinMode(notPausePort, OUTPUT);
  digitalWrite(notPausePort, HIGH);
  
// debug output
  pinMode(debugPort, OUTPUT);
  digitalWrite(debugPort, LOW);

  pinMode(actionPort, OUTPUT);
  digitalWrite(actionPort, LOW);
}

//========================================================
// main loop
//========================================================
void loop() {
  int status;
  
//  digitalWrite(debugPort,!digitalRead(inPort));
  
  int x;
  int byte0 = 0x18;
  int byte1 = 0x33;
  
  if (!digitalRead(button)) {
    for (x=0;x<5;x++) {
      scan(byte0, byte1);
    }
  }

//TODO set 
// set zoom

// run scan and set tally lights
  status = scan(0,0);
  set_tally(status);
}



//========================================================
//========================================================
int scan(int byte0, int byte1) {
  long timeout;
  int values[8] = {0,0,0,0,0,0,0,0};
  int i;
  int j;
  int data;

  if (byte0) {
    values[0] = byte0;
    values[1] = byte1;
  }
  

//==============================
// wait for sequence start
  digitalWrite(actionPort, HIGH);


  timeout = millis();
  while (millis() < timeout + 6) {
    if (digitalRead(inPort) == LOW)
      timeout = millis();
  }

  while (digitalRead(inPort) == HIGH) {}

//==============================
// loop over 8 bytes

  digitalWrite(actionPort, LOW);

  for (j=0;j<=7;j++) {
// wait for end of startbit
    delayMicroseconds(startBitTime);

    data = values[j];
    
//===================================
// loop 8 bits
    for (i=0;i<=7;i++) {
      if (data) {
// write byte to data line
        digitalWrite(debugPort, HIGH);

        digitalWrite(outPort,bitRead(values[j],i));
        delayMicroseconds(outTime);

        digitalWrite(debugPort, LOW);
      } else {
// read byte from data line
        delayMicroseconds(inTime / 2);

        digitalWrite(debugPort, HIGH);
        bitWrite(values[j],i,!digitalRead(inPort));
        digitalWrite(debugPort, LOW);

        delayMicroseconds(inTime / 2);
      }

    }
    
//===================================

// end of byte -> wait 1 1/2 bits
    delayMicroseconds(startBitTime);
    digitalWrite(outPort,LOW);
    delayMicroseconds(startBitTime / 2);

// wait for next start bit
    if (j < 7) {
      while(digitalRead(inPort) == HIGH) { }
    }
  }
  
// sequence finished
  return (values[4]);
}



//========================================================
//========================================================
// set tally leds

void set_tally(int status) {
  clear_tally();
  
  switch (status) {
//====================================
// eject
    case 0x01:
      digitalWrite(recPort, LOW);
      digitalWrite(pausePort, LOW);
      break;
      
//====================================
// stop
    case 0x02:
      digitalWrite(recPort, LOW);
      digitalWrite(pausePort, HIGH);
      break;
      
//====================================
// rec/pause
    case 0x14:
      digitalWrite(recPort, HIGH);
      digitalWrite(pausePort, HIGH);
      break;
    
//====================================
//record
    case 0x04:
      digitalWrite(recPort, HIGH);
      digitalWrite(pausePort, LOW);
      break;
    
    default:
      break;
  }
}


void clear_tally() {
  digitalWrite(recPort, LOW);
}
