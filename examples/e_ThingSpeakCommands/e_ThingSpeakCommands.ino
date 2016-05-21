// ---------------------------------------------------------------------------
// Created by Abhijit Bose (boseji) on 15/05/16.
// Copyright 2016 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
//
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// Thread Safe: No
// Extendable: Yes
//
// @file e_ThingSpeakCommands
//  
// @brief 
// Thingspeak Control Command example by connecting to an Internet 
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
  
  Serial.println(" Ready!");
}

char buffer[100];
void loop() {
  
  // Check if the WiFi is connected
  if(IsConnected())
  {
    if(ThingSpeak_Execute(thingspeak_talkback_id,thingspeak_api_key,
      buffer, 100))
    {
      // Check if there is some command received
      if(strlen(buffer) > 0)
      {
        Serial.print(" Command Received : ");
        Serial.println(buffer);
        // Process the Command
        if(strcmp(buffer, "ON") == 0)
          digitalWrite(16, LOW);
        else if(strcmp(buffer, "OFF") == 0)
          digitalWrite(16, HIGH);
        else
          Serial.println(" Error Invalid Command!");
      }
    }
    delay(5000); // 5 Seconds delay in Reading as needed by Thingspeak
  }
}
