//NodeMCU server, receive packets from 3DS to detect keys pressed

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "strucdefs.h"

#define port 8889

#define DEBUG 0

//WiFi stuff, keep a static ip so you don't need to change it on 3ds
IPAddress ip(192, 168, 1, 79); //set static ip
IPAddress gateway(192, 168, 1, 1); //set gateway
IPAddress subnet(255, 255, 255, 0);//set subnet

//Variables/Constants from the python sketch
unsigned int currentKeys = 0;
unsigned int oldKeys = 0;
struct sticks {
  uint8_t circleX, circleY, stickX, stickY;
};
struct sticks currentStick;
struct sticks oldStick;

USB_JoystickReport_Input_t controllerData; //struct that is sent via serial to the arduino

const int cmd_connect = 0;
const int cmd_keys = 1;
const int cmd_screenshot = 2;

const char* keyNames[33] = {
  "A", "B", "Select", "Start", "Right", "Left", "Up", "Down",
  "R", "L", "X", "Y", "0", "0", "ZL", "ZR",
  "0", "0", "0", "0", "Tap", "0", "0", "0",
  "CSRight", "CSLeft", "CSUp", "CSDown", "CRight", "CLeft", "CUp", "CDown",
};

const char* switchKeyNames[16] = {
  "A", "B", "CAPTURE", "HOME", "RIGHT", "LEFT", "TOP", "BOTTOM",
  "R", "L", "X", "Y", "0", "0", "ZL", "ZR", //Two missing keys are the plus and minus keys on the joy-cons
};

const char* directions[2] = {
  "MAX", "MIN"
};

const JoystickButtons_t switch_keys[16] = {
  SWITCH_A,SWITCH_B,SWITCH_CAPTURE,SWITCH_HOME,SWITCH_RELEASE,SWITCH_RELEASE,SWITCH_RELEASE,SWITCH_RELEASE,
  SWITCH_R,SWITCH_L,SWITCH_X,SWITCH_Y,SWITCH_MINUS,SWITCH_PLUS,SWITCH_ZL,SWITCH_ZR,
};

/*Keys
#define A 1<<0
#define B        1<<1
#define Select   1<<2
#define Start    1<<3
#define Right    1<<4
#define Left     1<<5
#define Up       1<<6
#define Down     1<<7
#define R        1<<8
#define L        1<<9
#define X        1<<10
#define Y        1<<11
#define ZL       1<<14 // (new 3DS only)
#define ZR       1<<15 // (new 3DS only)
#define Tap      1<<20 // Not actually provided by HID
#define CSRight  1<<24 // c-stick (new 3DS only)
#define CSLeft   1<<25 // c-stick (new 3DS only)
#define CSUp     1<<26 // c-stick (new 3DS only)
#define CSDown   1<<27 // c-stick (new 3DS only)
#define CRight   1<<28 // circle pad
#define CLeft    1<<29 // circle pad
#define CUp      1<<30 // circle pad
#define CDown    1<<31 // circle pad
*/
//End of python sketch variables

WiFiUDP Udp;
char incomingPacket[255];
struct packet dataIn;

void setup() {
  Serial.begin(9600);
  setupWifi();

}

void setupWifi() {
  WiFi.mode(WIFI_STA);

  WiFi.config(ip, gateway, subnet);
  WiFi.begin("Sardegna", "ader2009");
#if DEBUG
  Serial.print("Connecting");
  #endif
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    #if DEBUG
    Serial.print(".");
    #endif
  }
  #if DEBUG
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  #endif

  //begin UDP
  Udp.begin(port);
}

// first one,
int receivePacket() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = '\0';
    }
  //Serial.printf("UDP packet contents:");
  //Serial.printf("%n",incomingPacket);

  memcpy(&dataIn, &incomingPacket, sizeof(struct packet));
  //Serial.println(dataIn.packetHeader.command);

  //(...)
  return 1;
  }
  return 0;
}
/*
//Alternative packet interpretation function - sends a struct containing data to hori emulator
void packData() {
  int command = dataIn.packetHeader.command;
  if(command == 0) {
    //This means it has recognized us
    //If we get this, it may be a good idea to reset all our key values
    //Serial.println("Connected to 3DS");
  } else if (command == 1) {
    //reset everything
    controllerData.Button = SWITCH_RELEASE;
    controllerData.LX = 128;
    controllerData.LY = 128;
    controllerData.RX = 128;
    controllerData.RY = 128;
    controllerData.HAT = 0x08;
    //memcpy(&controllerData.keys, &dataIn.myunion.keysPacket.keys, 4);

    for(int i = 0; i <= 16; i++) {
      if(dataIn.myunion.keysPacket.keys & 1<<i) {//If it is pressed down
       controllerData.Button |= switch_keys[i];
      }
    }

    updateSticks();

    controllerData.LX = currentStick.circleX;
    controllerData.LY = currentStick.circleY;

    controllerData.RX = currentStick.stickX;
    controllerData.RY = currentStick.stickY;

    //possibly add some kind of waiting for a response from the arduino?

    Serial.write('\n');//Indicates beginning of transmission. Originally went after \r
    Serial.write((char*)&controllerData, sizeof(controllerData));
    Serial.write('\r');
  } else {
    //something's wrong if you get here
  }
}
*/


void interpretPacket(){
  int command = dataIn.packetHeader.command;
  if(command == 0) {
    //This means it has recognized us. If we get this, it may be a good idea to reset all our key values
  } else if (command == 1) {
    //This means it is sending us keys

    memcpy(&oldKeys, &currentKeys, 4); // copy current keys to old keys
    memcpy(&currentKeys, &dataIn.myunion.keysPacket.keys, 4); // update current keys

    for(int i = 0; i <= /*31 with the stick direction buttons and TAP*/ 16; i++) {
      if((keyNames[i] != "0")) {
        if(currentKeys & 1<<i) {
          if(!(oldKeys & 1<<i)) pressKey(i);
        } else {
          if(oldKeys & 1<<i) releaseKey(i);
        }
      }
    }

    updateSticks();

/* Old example code before it was put in a for loop above
    if((currentKeys & A)) {
      if(!(oldKeys & A))  Serial.println("A pressed");
    } else {
      if((oldKeys & A))  Serial.println("A released");
    }
    if(currentKeys & 1<<9) {
      Serial.println("L pressed");
    }
    */


  } else if (command ==2) {
    //This is screenshot mode; not in use, if you get this then something is VERY wrong or the 3DS app has been modified
  }
}

void updateSticks() {
  //test circle pad and c stick
    memcpy(&oldStick, &currentStick, sizeof(currentStick));
    short temp;

    memcpy(&temp, &dataIn.myunion.keysPacket.circlePad.x, 2);
    currentStick.circleX = map(temp, -160, 160, 0, 255);
    //Serial.printf("Circle pad X: %i \n", temp);

    memcpy(&temp, &dataIn.myunion.keysPacket.circlePad.y, 2);
    currentStick.circleY = map(temp, 160, -160, 0, 255);
    //Serial.printf("Circle pad Y: %i \n", temp);

    memcpy(&temp, &dataIn.myunion.keysPacket.cStick.x, 2);
    currentStick.stickX = map(temp, -160, 160, 0, 255);
    //Serial.printf("C-Stick X: %i \n", temp);

    memcpy(&temp, &dataIn.myunion.keysPacket.cStick.y, 2);
    currentStick.stickY = map(temp, 160, -160, 0, 255);
    //Serial.printf("C-Stick Y: %i \n", temp);

    char buf[3];

    if(currentStick.circleX != oldStick.circleX) {
      sprintf(buf, "%03i", currentStick.circleX);
      Serial.write("LX ");
      Serial.write(buf);
      Serial.write("\r\n");
    }

    if(currentStick.circleY != oldStick.circleY) {
      sprintf(buf, "%03i", currentStick.circleY);
      Serial.write("LY ");
      Serial.write(buf);
      Serial.write("\r\n");
    }

    if(currentStick.stickX != oldStick.stickX) {
      sprintf(buf, "%03i", currentStick.stickX);
      Serial.write("RX ");
      Serial.write(buf);
      Serial.write("\r\n");
    }

    if(currentStick.stickY != oldStick.stickY) {
      sprintf(buf, "%03i", currentStick.stickY);
      Serial.write("RY ");
      Serial.write(buf);
      Serial.write("\r\n");
    }
}

void pressKey(int keyI) {
  if(keyI >=4 && keyI <= 7) {
    Serial.write("H "); 
    Serial.write(switchKeyNames[keyI]);
    Serial.write('\r');
    Serial.println();
  } else if(keyI < 16) {
    //If they are regular buttons
    Serial.write("B ");
    Serial.write(switchKeyNames[keyI]);
    Serial.write('\r');
    Serial.println();
  } else {
    if(keyI >= 24 && keyI <= 25) {
      Serial.write("RX ");
      Serial.write(directions[keyI-24]);
      Serial.write('\r');
      Serial.println();

    } else if (keyI ==26) {
      Serial.write("RY ");
      Serial.write(directions[1]);
      Serial.write('\r');
      Serial.println();

    } else if (keyI ==27) {
      Serial.write("RY ");
      Serial.write(directions[0]);
      Serial.write('\r');
      Serial.println();

    } else if(keyI >= 28 && keyI <= 29) {
      Serial.write("LX ");
      Serial.write(directions[keyI-28]);
      Serial.write('\r');
      Serial.println();

    } else if (keyI >=30 && keyI <= 31) {
      Serial.write("LY ");
      Serial.write(directions[keyI-30]);
      Serial.write('\r');
      Serial.println();
    }
  }

  //Serial.printf("%s pressed \n",keyNames[keyI]);
}

void releaseKey(int keyI) {
  if(keyI >=4 &&  keyI <= 7) {
    Serial.write("H "); 
    Serial.write("CENTER");
    Serial.write('\r');
    Serial.println();
  } else if(keyI < 16) {
    Serial.write("RE ");
    Serial.write(switchKeyNames[keyI]);
    Serial.write('\r');
    Serial.println();
  } else {
    if(keyI >= 24 && keyI <= 25) {
      Serial.write("RX CENTER");
      Serial.write('\r');
      Serial.println();
    } else if (keyI >=26 && keyI <= 27) {
      Serial.write("RY CENTER");
      Serial.write('\r');
      Serial.println();
    } else if(keyI >= 28 && keyI <= 29) {
      Serial.write("LX CENTER");
      Serial.write('\r');
      Serial.println();
    } else if (keyI >=30 && keyI <= 31) {
      Serial.write("LY CENTER");
      Serial.write('\r');
      Serial.println();
    }
  #if DEBUG
  Serial.printf("%s released \n", keyNames[keyI]);
  #endif
  }
}

void loop() {
  while (WiFi.status() != WL_CONNECTED)   {
    delay(500);
    #if DEBUG
    Serial.print(".");
    #endif
  }

  if(receivePacket()) {
    interpretPacket();
  }
}
