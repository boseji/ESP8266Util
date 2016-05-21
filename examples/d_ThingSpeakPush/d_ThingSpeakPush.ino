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
// @file d_ThingSpeakPush
//  
// @brief 
// Thingspeak Data push to Channel example by connecting to an Internet 
//  enabled WiFi network using the 'ESP8266Util' library
//
// @version API 1.0.0
//
//
// @author boseji - salearj@hotmail.com
// ---------------------------------------------------------------------------
#include <ESP8266Util.h>

const char *thingspeak_api_key = "................";
const char *thingspeak_field = "field1";
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
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Seed the Random Number Generator
  randomSeed(analogRead(0));
  
  Serial.println(" Ready!");
}

void loop() {
  
  // Check if the WiFi is connected
  if(IsConnected())
  {
    uint32_t rand;
    
    // Generate a Random Value - This can be an actual ADC data
    rand = random(300);
    Serial.print(" Sending Data: ");
    Serial.println(rand);
    if(ThingSpeak_Push(thingspeak_api_key, thingspeak_field, String(rand)))
      Serial.println(" Data Send Successfully !");
    delay(20000); // 20 Seconds delay in sending as needed by Thingspeak
  }
}
