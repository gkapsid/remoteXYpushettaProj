/*
   -- NodeMCUIot --
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 2.3.3 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.1.1 or later version;
     - for iOS 1.2.1 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.   

    Code for pushetta API from https://github.com/Astu04/Pushetta_ESP8266 
*/
/* 

The code tries to connect to two servers.
One server is the remoteXY which provides a remote control and the other is Pushetta http://www.pushetta.com/ 
Pushetta is used for pushing an alarm message when the value of a sensor is over a specific value
Can not connect at the same time to both servers

step2: It reads the D5 input and then sends an alarm if it is different than normal
It sets a flag to TRUE to avoid resending many times the same alarm
It resets the flag if the D5 input returns to normal and an amount of time has passed to avoid bounce
After sending the alarm ends the pushetta client an connects to remoteXY

step3: Wifi configuration added to easily configure SSID and password of the router
testedQ OK 22/5/2019

*/
//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP8266WIFI_LIB_CLOUD

#include <ESP8266WiFi.h>

#include <RemoteXY.h>

// RemoteXY connection settings 
#define REMOTEXY_WIFI_SSID "yourSSID"
#define REMOTEXY_WIFI_PASSWORD "YourPass"
#define REMOTEXY_CLOUD_SERVER "cloud.remotexy.com"
#define REMOTEXY_CLOUD_PORT 6376
#define REMOTEXY_CLOUD_TOKEN "YourTOKEN"

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

# define bounce 10000

// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
  { 255,1,0,2,0,34,0,8,13,0,
  2,0,6,3,22,11,2,26,31,31,
  79,78,0,79,70,70,0,65,4,10,
  22,9,9,66,1,12,37,7,16,2,
  26 };
  
// this structure defines all the variables of your control interface 
struct {

    // input variable
  uint8_t switch_1; // =1 if switch ON and =0 if OFF 

    // output variable
  uint8_t led_1_r; // =0..255 LED Red brightness 
  int8_t level_1; // =0..100 level position 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

#define PIN_SWITCH_1 D7
#define inPin D5

bool flag = false;
bool flag2 = false;

//pushetta related
char APIKEY[] = "YourAPIKey"; // Put here your API key. It's in the dashboard
char CHANNEL[] = "Your Channel";                               // and here your channel name

char serverName[] = "api.pushetta.com";
boolean lastConnected = false;

int counter = 0;

float alarmTime = 0;

void setup() 
{
  
  Serial.begin(115200);
  pinMode (PIN_SWITCH_1, OUTPUT);
  digitalWrite(PIN_SWITCH_1, LOW);
  pinMode(inPin, INPUT);
  // TODO you setup code
 
 //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;  

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("AutoConnectAP");
  RemoteXY_Init (); 
    
}

void loop() 
{ 
  RemoteXY_Handler ();
  
  digitalWrite(PIN_SWITCH_1, (RemoteXY.switch_1==0)?LOW:HIGH);
     RemoteXY.led_1_r = (digitalRead(PIN_SWITCH_1)==HIGH)?255:0;   

  // TODO you loop code
  // use the RemoteXY structure for data transfer

  if (digitalRead(inPin) == HIGH && flag==false) {
    alarmTime = millis();
  digitalWrite(PIN_SWITCH_1, HIGH);
  Serial.println("Connecting to Pushetta");

  // Use WiFiClient class to create TCP connections
  WiFiClient client2;
  const int httpPort = 80;


  if (!client2.connect("api.pushetta.com", httpPort)) {
    Serial.println("Connection failed");
    return;
  }
  sendToPushetta(CHANNEL, "Hello REMOTE!");
  delay(100);
  flag = true;

  client2.stop();
  client2.flush(); 

RemoteXY_Init (); 
   //RemoteXY.led_1_r = (digitalRead(inPin)==HIGH)?255:0;   
   RemoteXY.led_1_r = 255; //lights the led in the RemoteXY remote control
   RemoteXY.switch_1 = 255; //poses the switch at RemoteXY remote control to ON position!

 }
  
  if (digitalRead(inPin)==LOW && bounce < millis()-alarmTime && flag==true) {
    flag = false;

       RemoteXY.led_1_r = 0;   
       RemoteXY.switch_1 = 0;
       digitalWrite(PIN_SWITCH_1,LOW);

    }
  }



//Function for sending the request to Pushetta
void sendToPushetta(char channel[], String text) {
  WiFiClient client2;
  client2.stop();

  if (client2.connect(serverName, 80))
  {
    client2.print("POST /api/pushes/");
    client2.print(channel);
    client2.println("/ HTTP/1.1");
    client2.print("Host: ");
    client2.println(serverName);
    client2.print("Authorization: Token ");
    client2.println(APIKEY);
    client2.println("Content-Type: application/json");
    client2.print("Content-Length: ");
    client2.println(text.length() + 46);
    client2.println();
    client2.print("{ \"body\" : \"");
    client2.print(text);
    client2.println("\", \"message_type\" : \"text/plain\" }");
    client2.println();
  }
}

