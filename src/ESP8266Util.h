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
// A simple Library to perform common functions on ESP8266
// 
// @brief 
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

#define DYNACONNECT_OK          0
#define DYNACONNECT_SUCCESS     0
#define DYNACONNECT_ERROR_PARAM 1
#define DYNACONNECT_ERROR_FILE  2
#define DYNACONNECT_ERROR_SSID  3
#define DYNACONNECT_ERROR_PASS  4
#define DYNACONNECT_ERROR_CHECK 5

#ifndef ESP8266
//------------------------------<<<<

#define ESP8266Util_Store_t

#define ESP8266Util_Conn(P,X,Y,Z)
#define ESP8266Util_FreeChannel(X)

//------------------------------>>>>
#else /* Happy to Complied in ESP8266 ! */
//------------------------------<<<<

typedef enum{
    ESP8266UTIL_STORE_FS=0,
    ESP8266UTIL_STORE_EEPROM
}ESP8266Util_Store_t;

bool ESP8266Util_Conn(const char *ssid, const char *pass, 
    bool persistent = false, uint32_t retry = 0);

bool ESP8266Util_AP(const char *ssid, const char *pass, IPAddress apip = IPAddress(192, 168, 1, 1));

int32_t ESP8266Util_FreeChannel(uint32_t cycle = 4);

class ESP8266Weg{
  
  private:
  
  char _szAP_SSID[33];
  char _szAP_PASSWORD[65];
  char _szST_SSID[33];
  char _szST_PASSWORD[65];
  char _szHostName[33];
  bool _bAutoWifiNeeded;
  bool _bStoreInfo;
  ESP8266Util_Store_t _xprefStore;
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
  ESP8266Weg(void);
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
  void setStore(bool bStoreInfo = true, ESP8266Util_Store_t xpStore = ESP8266UTIL_STORE_FS);
  /// Perform storage function in case Valid Station Info is available
  bool store(void);
  /// Clear the Info in Storage & Memory
  bool remove(bool bmemory = true);
  /// Check if there is some Info available in Storage or Already valid info in Memory
  bool check(void);
  /// To Begin connection in case everthing is Available Alright
  bool connect(void);
  /// To Run the Internal State Machine for connectivity
  bool run(void);
};

//------------------------------>>>>
#endif /* End of ESP8266 System Level Selection */

#endif /* End of _ESP8266UTIL_H_ */