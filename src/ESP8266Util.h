// ---------------------------------------------------------------------------
// Created by Abhijit Bose (boseji) on 28/04/16.
// Copyright 2016 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
//
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// Thread Safe: No
// Extendable: Yes
//
// @file ESP8266Util.h
// 
// @brief 
// A simple Library to perform common functions on ESP8266
//
// @version API 1.0.0
//
//
// @author boseji - salearj@hotmail.com
// ---------------------------------------------------------------------------
#ifndef _ESP8266UTIL_H_
#define _ESP8266UTIL_H_

#if (ARDUINO <  100)
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <inttypes.h>
#include <Print.h>
#include <ESP8266WiFi.h>

#ifndef ESP8266
//------------------------------<<<<

#define ESP8266Util_Store_t

#define Connect(P,X,Y,Z)
#define FreeChannel(X)
#define AccessPoint(P,Q,R,S)
#define IsConnected() 0
#define IsAccessPoint() 0
#define IsStation() 0
#define RestRequest(P,Q,R,S,T)
#define ThingSpeeak_Push(X,Y,Z)

//------------------------------>>>>
#else /* Happy to Complied in ESP8266 ! */
//------------------------------<<<<

typedef enum{
    STORE_FS=0,
    STORE_EEPROM
}Store_t;

bool Connect(const char *ssid, const char *pass, 
     uint32_t retry = 0, bool persistent = false);

bool AccessPoint(const char *ssid, const char *pass, int32_t chan = 7, 
    IPAddress apip = IPAddress(192, 168, 1, 1));

int32_t FreeChannel(uint32_t cycle = 4, WiFiPhyMode_t mode = WIFI_PHY_MODE_11G);

bool IsConnected(void);

bool IsAccessPoint(void);

bool IsStation(void);

bool RestRequest(String uri, String data, 
    char *buffer = NULL, uint32_t max_sz=0, int32_t *httpCode = NULL);

bool ThingSpeak_Push(const char *key, const char* fieldname, String Value);

bool ThingSpeak_Push(const char *key, const char* fieldname1, String Value1,
  const char* fieldname2, String Value2);

bool ThingSpeak_Execute(const char *tb_id, const char *tb_key, char *cmd_buf, size_t max_sz);

bool ThingSpeak_Command(const char *tb_id, const char *tb_key, const char *cmd_buf);

/// Wireless Automatic Configuration Class
class Weg{
  
  private:
  
  char _szAP_SSID[33];
  char _szAP_PASSWORD[65];
  char _szST_SSID[33];
  char _szST_PASSWORD[65];
  char _szHostName[33];
  bool _bAutoWifiNeeded;
  bool _bStoreInfo;
  Store_t _xprefStore;
  bool _bAP_infoValid;
  bool _bST_infoValid;
  IPAddress _xAP_ip;
  bool _bIsRunning;
  
  /// Internal Initialization Function
  void _Init(void);
  
  /// Internal Used to Try out another Connection attempt
  bool _TryAP(void);
  
  public:
  
  /// Variable Controlling the Number of Retry while connecting to WiFi Network
  uint32_t nRetry;
  /// Variable Controlling the Number of Reconnection attempts before going to AP mode
  uint32_t nReconnect;
  /// Constructor for the Class
  Weg(void);
  /// Start the System
  bool begin(void);
  /// Enable / Disable Automatic WiFi AP enablement
  void autoAP(bool DynamicWiFiAP = true);
  /// Manually Configure a Custom AP Name and Password
  bool apInfo(const char *ssid, const char *pass);
  /// Setup a Specific IP address to be Assigned to the Network
  void apIp(IPAddress apip);
  /// Manually Configure a Custom Station Name and Password
  bool stationInfo(const char *ssid, const char *pass);
  /// Change to Storage where the Connection info is stored and if its needed
  void setStore(bool bStoreInfo = true, Store_t xpStore = STORE_FS);
  /// Perform storage function in case Valid Station Info is available
  bool store(void);
  /// Clear the Info in Storage & Memory
  bool remove(bool bmemory = true);
  /// Load if there is some Info available in Storage or Already valid info in Memory
  bool load(void);
  /// To Begin connection in case everything is Available Alright
  bool startConnection(void);
  /// To Run the Internal State Machine for connectivity
  bool status(void);
};

//------------------------------>>>>
#endif /* End of ESP8266 System Level Selection */

#endif /* End of _ESP8266UTIL_H_ */