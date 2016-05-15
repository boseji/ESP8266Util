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
// @file c_WiFiChannels
//  
// @brief 
// Simple Example to Show how to perform quick Channel assessment on various 
//  WiFi bands using the 'ESP8266Util' library
//
// @version API 1.0.0
//
//
// @author boseji - salearj@hotmail.com
// ---------------------------------------------------------------------------
#include <ESP8266Util.h>

void setup() {
  // Boot-up Wait
  delay(1000);
  // Configure Serial
  Serial.begin(115200);
  Serial.println();  
  Serial.println();
  
  Serial.println(" BEGIN");  
}

void loop() {
  
  // Assessment for WiFi 802.11b
  WiFi.setPhyMode(WIFI_PHY_MODE_11B);
  FreeChannel(1, WIFI_PHY_MODE_11B);
  Serial.println();
  delay(2000);
  
  // Assessment for WiFi 802.11g
  WiFi.setPhyMode(WIFI_PHY_MODE_11G);
  FreeChannel(1);
  Serial.println();
  delay(2000);
  
  // Assessment for WiFi 802.11n
  WiFi.setPhyMode(WIFI_PHY_MODE_11N);
  FreeChannel(1, WIFI_PHY_MODE_11N);
  Serial.println();
  delay(2000);
}
