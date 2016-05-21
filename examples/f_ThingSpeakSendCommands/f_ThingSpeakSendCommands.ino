// ---------------------------------------------------------------------------
// Created by Abhijit Bose (boseji) on 21/05/16.
// Copyright 2016 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
//
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// Thread Safe: No
// Extendable: Yes
//
// @file f_ThingSpeakSendCommands
//  
// @brief 
// Thingspeak Sending Control Command example by connecting to an Internet 
//  enabled WiFi network using the 'ESP8266Util' library
//
// @version API 1.0.0
//
//
// @author boseji - salearj@hotmail.com
// ---------------------------------------------------------------------------
#include <ESP8266Util.h>

const char *thingspeak_api_key = "................";
const char *thingspeak_talkback_id = "......";
const char *ssid = "..........";
const char *password = "...........";
const char *command_on = "ON";
const char *command_off = "OFF";

void setup() {
  // Boot-up Wait
  delay(1000);
  // Configure Serial
  Serial.begin(115200);
  Serial.println();  
  Serial.println();
  
  Serial.println(" BEGIN");
  // Connect to the WiFi network
  //  Perform 50 Retries
  if(Connect(ssid,password,50))
    Serial.println(" Connected");
  else
  {
    Serial.println(" Error Unable to connect");
    while(1);
  }
  
  // Print the IP Address
  Serial.print(" IP address: ");
  Serial.println(WiFi.localIP());
  
  pinMode(16, OUTPUT);
  digitalWrite(16, HIGH);
  randomSeed(analogRead(0));
  
  Serial.println(" Ready!");
}

char buffer[100];
void loop() {
  
  // Check if the WiFi is connected
  if(IsConnected())
  {
    uint32_t rand;
    rand = random(2);
    
    if(rand == 1)
    {
      if(ThingSpeak_Command(thingspeak_talkback_id, 
          thingspeak_api_key, command_on))
        Serial.println(" Command to Turn ON Sent");
      digitalWrite(16, LOW);
    }
    else
    {
      if(ThingSpeak_Command(thingspeak_talkback_id, 
          thingspeak_api_key, command_off))
        Serial.println(" Command to Turn OFF Sent");
      digitalWrite(16, HIGH);
    }
    
    delay(20000); // 20 Seconds delay in Sending as needed by Thingspeak
  }
}
