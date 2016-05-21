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
// @file a_WiFiConnect
//  
// @brief 
// Simple Example to Show an easy way to create WiFi Connection 
//  in Station Mode using the 'ESP8266Util' library
//
// @version API 1.0.0
//
//
// @author boseji - salearj@hotmail.com
// ---------------------------------------------------------------------------
#include <ESP8266Util.h>
#include <ESP8266WebServer.h>

const char *ssid = "..........";
const char *password = "...........";

ESP8266WebServer server(80);

void handle_root();

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

  // Setup WebServer
  server.on("/", handle_root);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handle_root() {
  server.send(200, "text/plain", "hello from esp8266!");
}