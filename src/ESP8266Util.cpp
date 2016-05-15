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
// @file ESP8266Util.cpp
// 
// @brief 
// A simple Library to perform common functions on ESP8266
//
// @version API 1.0.0
//
//
// @author boseji - salearj@hotmail.com
// ---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "ESP8266Util.h"
#include "FS.h"
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>


/* Sets Up debug Prints */
#define NoDebug 0

#ifdef ESP8266 /* Happy to Complied in ESP8266 ! */

extern "C" {
#include "user_interface.h"
}


///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////

bool Connect(const char *ssid, const char *pass, 
    uint32_t retry, bool persistent)
{
    // Perform Initial Configuration
    WiFi.disconnect(false);
    delay(100);
    WiFi.persistent(persistent);
    WiFi.setPhyMode(WIFI_PHY_MODE_11G);
    WiFi.mode(WIFI_STA);
    delay(100);
    
    if(Serial && !NoDebug)
    {
        Serial.print("Connecting to ");
        Serial.println(ssid);
    }
    WiFi.begin(ssid, pass);
    
    // Based on Retry select what to Do
    if(retry)
    {
        // Check for WiFi connection
        while (WiFi.status() != WL_CONNECTED && (--retry)) {
            delay(500);
            if(Serial && !NoDebug)
            Serial.print(".");
        }
        if(Serial && !NoDebug)
            Serial.println();
        // If some values is remaining then Connected
        if(retry)
        {
            if(Serial && !NoDebug)
            {
              Serial.print(" SUCCESS! Connected to ");
              Serial.println(ssid);
              Serial.print(" IP Address: ");
              Serial.println(WiFi.localIP());
              Serial.print(" Hostname: ");
              Serial.println(WiFi.hostname());
            }
            return true;
        }
        // All other cases it was not successful
        return false;
    }
    
    // Check for WiFi connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if(Serial && !NoDebug)
        Serial.print(".");
    }
    if(Serial && !NoDebug)
    {
      Serial.println();
      Serial.print(" SUCCESS! Connected to ");
      Serial.println(ssid);
      Serial.print(" IP Address: ");
      Serial.println(WiFi.localIP());
    }
    return true;
}

///////////////////////////////////////////////////////////////////////

bool AccessPoint(const char *ssid, const char *pass, int32_t chan, 
    IPAddress apip)
{
  // Check the Input Parameters
  if(ssid == NULL || pass == NULL)
  {
    if(Serial && !NoDebug)
      Serial.println(" ERROR! Input Parameters are Empty ");
    return false;
  }
  
  // Check Size
  if(strlen(ssid) < 2 || strlen(ssid) > 32)
  {
    if(Serial && !NoDebug)
      Serial.println(" ERROR! Input SSID is not correct ");
    return false;
  }
  
  // Check for password
  if(strlen(pass)>64)
  {
    if(Serial && !NoDebug)
      Serial.println(" ERROR! Input Password is not correct ");
    return false;
  }
  
  // Setup the AP
  delay(10);
  WiFi.disconnect(false);  
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apip, apip, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, pass, chan);
  delay(100);
  
  // No problem to continue now
  return true;
}

///////////////////////////////////////////////////////////////////////

int32_t FreeChannel(uint32_t cycle, WiFiPhyMode_t mode)
{
    int32_t retchan = 1;
    int32_t chan[15];
    
    const uint8_t valid_chan[4][4] = {
      {0,4,4,2,}, // Size Specifications for each Band of Wireless Lan
      {// As per Wireless Lan 802.11b standard Non-Overlapped DSSS bands
        1, 6, 11, 14 //WIFI_PHY_MODE_11B = 1
      },
      {// As per Wireless Lan 802.11g standard Non-Overlapped DSSS bands
        1, 5, 9, 13 //WIFI_PHY_MODE_11G = 2
      },
      {// As per Wireless Lan 802.11n standard
        3, 11, 0, 0 //WIFI_PHY_MODE_11N = 3
      },
    };
    uint32_t i;
    uint32_t nets;
    
    // Initialize the Channel Count
    for(i=0;i<15;i++)
        chan[i] = 0;
    
    // Initialize the WiFi for Scanning
    delay(100);
    WiFi.mode(WIFI_STA);
    delay(100);
    WiFi.disconnect();
    delay(100);
    if(Serial && !NoDebug)
        Serial.println(" Beginning Scanning....");
    // Run the Cycles
    do
    {
        nets = WiFi.scanNetworks(false, true);
        if(Serial && !NoDebug)
        {
            Serial.println();
            Serial.print(" Scan Cycle ....");
            Serial.println(cycle);
            Serial.println();
        }    
        // If some Networks were found
        if(nets > 0)
        {
            // Spin through all the Networks
            for(i=0;i<nets;i++)
            {
                retchan = WiFi.channel(i);
                if(Serial && !NoDebug)
                {
                    Serial.print(" ");
                    Serial.print(i + 1);
                    Serial.print(": ");
                    Serial.print(WiFi.SSID(i));
                    Serial.print(" (");
                    Serial.print(WiFi.RSSI(i));
                    Serial.print(")");
                    Serial.print(" Channel ");
                    Serial.println(retchan);
                }
                
                // Found the Valid Channel
                if(retchan < 15 && retchan > 0)
                {
                    chan[retchan] += 1;
                }
            }
        }
        WiFi.scanDelete();
    }while(--cycle);// End of Scan cycles
    if(Serial && !NoDebug)
    {    
        Serial.println(" .... Scan Complete");
        Serial.println();
        Serial.println(" Channel Capacity List: ");
    }
    // There is No Channel 0 in WiFi
    nets = chan[valid_chan[mode][0]];
    retchan = valid_chan[mode][0];
    // Scan through the Channels and check the Least populated Channel
    for(i=0;i<valid_chan[0][mode];i++)
    {
        if(Serial && !NoDebug)
        {
            Serial.print(" Channel ");
            Serial.print(valid_chan[mode][i]);
            Serial.print(" x ");              
            Serial.println(chan[valid_chan[mode][i]]);
        }
        // Lowest Sort
        if(chan[valid_chan[mode][i]] < nets)
        {
            nets = chan[valid_chan[mode][i]];
            retchan = valid_chan[mode][i];
        }
    }
    
    if(Serial && !NoDebug)
    {
        Serial.println();
        Serial.print(" Selected Channel = ");
        Serial.println(retchan);
        Serial.println();
    }
    // Finally return the Selection of Channel
    return retchan;
}
///////////////////////////////////////////////////////////////////////

bool IsConnected(void)
{
  return (WiFi.status() == WL_CONNECTED);
}

///////////////////////////////////////////////////////////////////////

bool IsAccessPoint(void)
{
  return ((WiFi.getMode() & WIFI_AP) == WIFI_AP);
}

///////////////////////////////////////////////////////////////////////

bool IsStation(void)
{
  return ((WiFi.getMode() & WIFI_STA) == WIFI_STA);
}

///////////////////////////////////////////////////////////////////////

bool RestRequest(String uri, String data, 
    char *buffer, uint32_t max_sz, int32_t *httpCode)
{
  HTTPClient http; // Pick The client
  int32_t code = -1; // HTTP Code
  bool ret = false; // Return Variable
  
  do{
    // Parameter Validation
    if(uri == NULL || (buffer!=NULL && max_sz==0)) break;
    
    // Check Request Type for URI Alignment
    if(data.length() != 0) // POST Request
    {
      // Check for Correct Terminator
      if(uri.lastIndexOf('/') != uri.charAt(uri.length()-1))
      {
        uri += '/';
      }
    }
         
    // Start the Requester
    if(!http.begin(uri))
    {
      if(Serial && !NoDebug)
      {    
          Serial.println(" Error ! Problem with URI ");
          Serial.println(uri);
      }
      break;
    }
    
    // Check Request Type
    if(data.length() != 0) // POST Request
    {
      // Post Request
      code = http.POST(data);
      if(Serial && !NoDebug)
        Serial.print(" HTTP POST Code: ");
    }
    else if(uri.indexOf('?') == uri.lastIndexOf('?')) // GET Request
    {
      // Get Request
      code = http.GET();
      if(Serial && !NoDebug)
        Serial.print(" HTTP GET Code: ");
    }
    else // Un-Recognized Request
    {
      break;
    }
    if(Serial && !NoDebug)
    {    
      Serial.println(code);
    }
    
    // Check for Success types
    if(code == HTTP_CODE_OK)
      ret = true;
      
  }while(0);
  
  // Perform the Copy
  if(httpCode != NULL && code>0)
  {
    *httpCode = code;
  }
  code = http.getString().length();
  if(buffer != NULL && code != 0)
  {
    if(max_sz > code)
      strncpy(buffer,http.getString().c_str(), code);
    else
      strncpy(buffer,http.getString().c_str(), max_sz);
    if(Serial && !NoDebug)
    {    
        Serial.println(" HTTP Response: ");
        Serial.println(http.getString());
    }
  }
  
  // In all cases Close HTTP Client Before leaving
  http.end();
  return ret; // All other Cases ignore
}

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////

//// Constants for Configuration
// DNS Server
const byte DNS_PORT = 53;
// HTML Pieces
const char HTTP_HEAD[] = {"<!DOCTYPE html><html><head><title>IOT Autoconfig</title>"
  "<style>body{font-family: Verdana, Geneva, Tahoma, sans-serif;"
  "margin: 50px;}.sal{color:#999999;font-style: oblique;font-size:10px;}</style>"
  "</head><body><h1>IoT Autoconfig portal</h1>"
  "<p class=\"sal\">Designed By <strong>boseji </strong> "
  "&lt; salearj [at] hotmail [dot] com &gt; </p>"
  "<h2> System Configuration: </h2>"
};
const char *HTTP_APIP ="<p>AP Ip Address: ";
const char *HTTP_APMAC ="<p>AP Mac Address: ";
const char *HTTP_STAMAC ="<p>Station MAC Address: ";
const char *HTTP_DOC_CLOSE ="</p>";
const char *HTTP_DOC_INFO =" <sub>(Needed for MAC ID Protection on WiFi Router)</sub></p>";
const char HTTP_FOOT1[] = {
  "<h2> WiFi Configuration: </h2><form action=\"wifi\" method=\"POST\">"
  "<p><input type=\"text\" name=\"APName\" placeholder=\"WiFi AP Name\" maxlength=\"32\"></p><p>"
  "<input type=\"password\" name=\"APPassword\" placeholder=\"WiFi AP Password\" maxlength=\"64\">"
  "</p><input type=\"submit\" value=\"Submit\">"
  "</form></body></html>"
};
const char *HTTP_FOOT2 = "<h2> WiFi Configuration Stored !</h2></body></html>";
// Accesspoint Configuration
const char *gpAP_ssid = "Weg";
// File Path
const char *dynFile = "/dynaconf.txt";

//// Globals for Fetching SSID and Password
// Webserver to host the configuration Page
ESP8266WebServer webServer(80);
// DNS Server
DNSServer dnsServer;

// Trigger the Exit from Loop
bool n_Done = false;
// Configuration Storage for WiFi Credentials
char gn_ssid[33] = "";
char gn_password[65] = "";

//// Function Prototypes

// HTML Main Page
void h_Main(void);
// WiFi Info Submission
void h_Wifi(void);
// No Correct Page Found
void h_NotFound(void);

///////////////////////////////////
/// Internal Initialize Function
void Weg::_Init(void)
{
  uint32_t i;
  // Setup all the Variables
  for(i=0;i<33;i++)
    this->_szAP_SSID[i] = 0;  
  for(i=0;i<65;i++)
    this->_szAP_PASSWORD[i] = 0;
  this->_bAP_infoValid = false;
  for(i=0;i<33;i++)
    this->_szST_SSID[i] = 0;
  for(i=0;i<65;i++)
    this->_szST_PASSWORD[i] = 0;
  this->_bST_infoValid = false;
  this->_bAutoWifiNeeded = true;
  this->_bStoreInfo = true;
  this->_xprefStore = STORE_FS;
  this->_xAP_ip = IPAddress(192, 168, 1, 1);
  this->_bIsRunning = false; // Intially we are not Running
  this->nRetry = 50;
  this->nReconnect = 10;
  
}
/// Constructor for the Class
Weg::Weg(void)
{
  this->_Init();
}
/// Start the System
bool Weg::begin(void)
{
  // Can't Accept when loop is running
  if(this->_bIsRunning) return false;
  
  // In case Initialization is not Done
  if(!this->_bAP_infoValid)
  {
    strcpy(this->_szAP_SSID, gpAP_ssid); // Default AP  
    strcat(this->_szAP_SSID, " - ");
    strcat(this->_szAP_SSID, WiFi.softAPmacAddress().c_str()); // Add the AP MAC ID
    this->_bAP_infoValid = true;    
    this->_szAP_PASSWORD[0] = 0; // Empty Password
    strcpy(this->_szHostName, gpAP_ssid); // Generate Unique Host Name
    strcat(this->_szHostName, "-");
    strcat(this->_szHostName, WiFi.softAPmacAddress().c_str());
  }
  
  // In case a valid Station info is not loaded then load for One - Calling Connect
  /*if(!this->_bST_infoValid)
    return this->load();
  */
  // Setup the Hostname
  WiFi.hostname(String(this->_szHostName));
  /*
  if(String(this->_szHostName) != WiFi.hostname())
  {
    if(Serial && !NoDebug)
    {
      Serial.println(" Changing Host Name!");
    }
    WiFi.hostname(String(this->_szHostName));
  }*/
  
  // We are All good
  return this->startConnection();
}
/// Enable / Disable Automatic WiFi AP enablement
void Weg::autoAP(bool DynamicWiFiAP)
{
  if(!this->_bIsRunning) // Set only if we are not Running
  {
    this->_bAutoWifiNeeded = DynamicWiFiAP;
  }
}
/// Manually Configure a Custom AP Name and Password
bool Weg::apInfo(const char *ssid, const char *pass)
{
  // Can't Accept when loop is running
  if(this->_bIsRunning) return false;
  
  // Parameters Check
  if(ssid == NULL || pass == NULL) return false;
  
  // Size Check
  if(strlen(ssid) < 2 || strlen(ssid) > 32 || strlen(pass) > 64) return false;
  
  // Every thing is fine so lets update
  this->_bAP_infoValid = false;
  strcpy(this->_szAP_SSID, ssid);
  this->_szAP_PASSWORD[0] = 0; // Just in case if there is No Password
  strcpy(this->_szAP_PASSWORD, pass);
  this->_bAP_infoValid = true;
  
  // Every thing is OK
  return true;
}
/// Setup a Specific IP address to be Assigned to the Network
void Weg::apIp(IPAddress apip)
{
  // Can't Accept when loop is running
  if(this->_bIsRunning) return;
  
  this->_xAP_ip = apip;
}
/// Manually Configure a Custom Station Name and Password
bool Weg::stationInfo(const char *ssid, const char *pass)
{
  // Can't Accept when loop is running
  if(this->_bIsRunning) return false;
  
  // Parameters Check
  if(ssid == NULL || pass == NULL) return false;
  
  // Size Check
  if(strlen(ssid) < 2 || strlen(ssid) > 32 || strlen(pass) > 64) return false;
  
  // Every thing is fine so lets update
  this->_bST_infoValid = false;
  strcpy(this->_szST_SSID, ssid);
  this->_szST_PASSWORD[0] = 0; // Just in case if there is No Password
  strcpy(this->_szST_PASSWORD, pass);
  this->_bST_infoValid = true;
  
  // Every thing is OK
  return true;
}
/// Change to Storage where the Connection info is stored and if its needed
void Weg::setStore(bool bStoreInfo, Store_t xpStore)
{
  // Can't Accept when loop is running
  if(this->_bIsRunning) return;
  
  this->_bStoreInfo = bStoreInfo;
  this->_xprefStore = xpStore;
}
/// Perform storage function in case Valid Station Info is available
bool Weg::store(void)
{
  uint16_t cksum = 0; // Checksum
  int32_t i=0;      // Counter
  char dt = 0;      // Temporary Storage
  String data = ""; // Store for Processed data and readout
  uint32_t ret = 0; // Status Variable
  
  // Can't Accept when loop is running
  if(this->_bIsRunning) return false;
  
  // In case the Current Info is not valid then Cant Write to Memory
  if(!this->_bST_infoValid) return false;
  
  switch(this->_xprefStore)
  {
    case STORE_FS:
      {
        File f;     // For File Control
        
        cksum = 0;
        ret = 0;
        // Start the Filesystem
        SPIFFS.begin();
        
        // Open File for Writing
        f = SPIFFS.open(dynFile, "w");
        if (!f) 
        {
          if(Serial && !NoDebug)
            Serial.println(" Error Station configuration file open failed !");
          return false;
        }
        
        // Enter the Catch loop
        do{
        // // [
        
        // Write the SSID
        if(f.write((const uint8_t *)this->_szST_SSID, strlen(this->_szST_SSID)) != 
          strlen(this->_szST_SSID))
        {
          if(Serial && !NoDebug)
            Serial.println(" Error Station configuration SSID Write failed !");
          ret = 3;
          break;
        }
        if(f.write('\r') != 1)
        {
          if(Serial && !NoDebug)
            Serial.println(" Error Station configuration SSID Write failed (Termination)!");
          ret = 3;
          break;
        }
        // Write Password Only if needed
        if(strlen(this->_szST_PASSWORD))
        {
          if(f.write((const uint8_t *)this->_szST_PASSWORD, strlen(this->_szST_PASSWORD)) != 
            strlen(this->_szST_PASSWORD))
          {
            if(Serial && !NoDebug)
              Serial.println(" Error Station configuration Password Write failed !");
            ret = 4;
            break;
          }
        }
        if(f.write('\r') != 1)
        {
          if(Serial && !NoDebug)
            Serial.println(" Error Station configuration Password Write failed (Termination)!");
          ret = 4;
          break;
        }
        
        // Calculate Checksum
        for(i=0;i<strlen(this->_szST_SSID);i++)
          cksum += this->_szST_SSID[i];
        for(i=0;i<strlen(this->_szST_PASSWORD);i++)
          cksum += this->_szST_PASSWORD[i];
        cksum = (0x10000 - (uint32_t)cksum)&0xFFFF;    
        if(Serial && !NoDebug)
        {
          Serial.print(" Calculated Checksum: 0x");
          Serial.println(cksum,HEX);
        }
        
        // Convert Checksum to String
        data = "";
        for(i=0;i<4;i++)
        {  
          dt = cksum&0x0F;
          data += (char)((dt < 10)?(dt + '0'):(dt - 10 + 'A'));
          cksum >>= 4;
        }
        if(Serial && !NoDebug)
        {
          Serial.print(" Calculated Checksum String: ");
          Serial.println(data);
        }
        
        // Write Checksum to File
        if(f.write((const uint8_t *)data.c_str(), data.length()) != data.length())
        {
          if(Serial && !NoDebug)
            Serial.println(" Error Station configuration Checksum Write failed!");
          ret = 5;
          break;
        }
        
        // Everything is Ok Now
        ret = 0;
        
        // // ]
        }while(0); // End of Catch loop
        
        // Finally Close file
        f.close();
        
        // In case of Error
        if(ret != 0) return false;
      }
      break;
    
    case STORE_EEPROM:
      {
        uint16_t addr;
        // Start the EEPROM
        EEPROM.begin(512);
        addr = 0;
        
        // Write the SSID Data
        for(i=0;i<strlen(this->_szST_SSID);i++)
        {
          EEPROM.write(addr++,this->_szST_SSID[i]);
          cksum += this->_szST_SSID[i];
        }
        // Termination
        EEPROM.write(addr++,'\r');
        
        // Write the Password Data
        for(i=0;i<strlen(this->_szST_PASSWORD);i++)
        {
          EEPROM.write(addr++,this->_szST_PASSWORD[i]);
          cksum += this->_szST_PASSWORD[i];
        }
        // Termination
        EEPROM.write(addr++,'\r');
        
        // Calculate Checksum
        cksum = (0x10000 - (uint32_t)cksum)&0xFFFF;
        if(Serial && !NoDebug)
        {
          Serial.print(" Calculated Checksum: 0x");
          Serial.println(cksum,HEX);      
        }
        
        // Write Checksum
        for(i=0;i<4;i++)
        {  
          dt = cksum&0x0F;
          dt = (char)((dt < 10)?(dt + '0'):(dt - 10 + 'A'));
          cksum >>= 4;
          EEPROM.write(addr++,dt);
        }
        // Termination
        EEPROM.write(addr++,'\0'); // NULL Termination
        
        // Everything Is OK now
        ret = 0;
        // Stop the EEPROM Functions
        EEPROM.end();
      }
      break;
      
    default:
      return false;
  }// End of Switch
  
  if(Serial && !NoDebug)
    Serial.println(" SUCCESS! Stored Station Information");
  return true;
}
/// Clear the Info in Storage & Memory
bool Weg::remove(bool bmemory)
{
  bool ret = false;
  // Can't Accept when loop is running
  if(this->_bIsRunning) return false;
  
  switch(this->_xprefStore)
  {
    case STORE_FS:
      {
        File f;     // For File Control
    
        // Start the Filesystem
        SPIFFS.begin();
        
        // Check Existiance of the File
        if(!SPIFFS.exists(dynFile))
        {
          if(Serial && !NoDebug)
            Serial.println(" Success the File Does not exist !");
          ret = true;
        }
        else // If the file exist the delete it
        {
          if(!SPIFFS.remove(dynFile))
          {
            if(Serial && !NoDebug)
              Serial.println(" Could not delete File !");
            ret = false;
          }
          else
          {
            ret = true;
          }
        }// End of Existince Check in File System
      }
      break;
     
    case STORE_EEPROM:
      {
        uint16_t addr;
        // Start the EEPROM
        EEPROM.begin(512);
        // Erase all the Records
        for(addr = 0; addr < 120; addr++)
          EEPROM.write(addr, 0xFF);
        // Close the EEPROM with this modification
        EEPROM.end();
        ret = true;
      }
      break;
     
    default:
      ret = false;
      break;
  }// End of Switch
  
  if(bmemory) // Clear Info in Memory if needed
  {
    uint32_t i;
    this->_bST_infoValid = false;
    for(i=0;i<33;i++)
      this->_szST_SSID[i] = 0;
    for(i=0;i<65;i++)
      this->_szST_PASSWORD[i] = 0;
  }
  return ret;
}

/// Check if there is some Info available in Storage or Already valid info in Memory
bool Weg::load(void)
{
  uint16_t cksum=0, ck=0; // Checksum
  int32_t i=0;    // Counter
  char dt = 0;    // Temporary Storage
  String data = ""; // Store for Processed data and readout
  uint32_t ret = 1; // Return check
  // Configuration Storage for WiFi Credentials
  char n_ssid[33] = "";
  char n_password[65] = "";
  // Can't Accept when loop is running
  if(this->_bIsRunning) return false;
  
  switch(this->_xprefStore)
  {
    case STORE_FS:
      {
        File f;     // For File Control
    
        // Start the Filesystem
        SPIFFS.begin();
        delay(10);
        
        // Enter the Catch loop
        do{
        // // [
        
          // Check Existiance of the File
          if(!SPIFFS.exists(dynFile))
          {
            if(Serial && !NoDebug)
              Serial.println(" Error Station configuration file does not exist !");
            ret = 1;
            break;
          }
          
          // Know that the File exist
          f = SPIFFS.open(dynFile, "r");
          if (!f) 
          {
            if(Serial && !NoDebug)
              Serial.println(" Error Station configuration file open failed !");
            ret = 2;
            break;
          }
          
          // Enter the Catch loop
          do{
          // // [
            i = 0;
            data.reserve(200);
            // Read the SSID first
            while(f.available() && i<33) // Limit SSID Size
            {
              dt = (char)f.read();
              // First Terminator Reached
              if(dt == '\r') 
              {
                break;
              }
              data += dt;
              ++i;
            }
            
            // Check for Termination
            if(i>=33 || i<2)
            {
              if(Serial && !NoDebug)
                Serial.println(" Error File is Corrupt did not find Station SSID (size)!");
              ret = 3;
              break;
            }
            
            // Calculate the Checksum & Load the SSID Buffer
            for(i=0; i<data.length(); i++)
            {
              cksum += data.charAt(i);
              n_ssid[i] = data.charAt(i);
            }
            n_ssid[i]=0; // Adding an Ending Null Termination
            if(Serial && !NoDebug)
            {
              Serial.print(" SSID Found: ");
              Serial.println(n_ssid);
            }
            // Clear the Data buffer
            data = "";
            
            i = 0;
            // Read the Password Next
            while(f.available() && i<65) // Limit Password Size
            {
              dt = (char)f.read();
              // First Terminator Reached
              if(dt == '\r') 
              {
                break;
              }
              data += dt;
              ++i;
            }
            
            // Check for Termination
            if(i>=65)
            {
              if(Serial && !NoDebug)
                Serial.println(" Error File is Corrupt did not find Station Password (size) !");
              ret = 4;
              break;
            }
            
            // Calculate the Checksum & Load the Password Buffer
            for(i=0; i<data.length(); i++)
            {
              cksum += data.charAt(i);
              n_password[i] = data.charAt(i);
            }
            n_password[i]=0; // Adding an Ending Null Termination
            if(Serial && !NoDebug)
            {
              Serial.print(" Password Found: ");
              Serial.println(n_password);
            }
            // Clear the Data buffer
            data = "";
            
            // Calculate Checksum
            cksum = (0x10000 - (uint32_t)cksum)&0xFFFF;
            if(Serial && !NoDebug)
            {
              Serial.print(" Calculated Checksum: 0x");
              Serial.println(cksum,HEX);
            }
            
            i = 0;
            // Read the Checksum Next
            while(f.available() && i<5) // Limit Checksum Size
            {
              dt = (char)f.read();
              data += dt;
              ++i;
            }
            
            // Check for Termination
            if(i>=5 || i<3 )
            {
              if(Serial && !NoDebug)
                Serial.println(" Error File is Corrupt did not find Checksum (size) !");
              ret = 5;
              break;
            }
            
            // Validate the Checksum
            data.toUpperCase();
            ck = 0;
            for(i=3;i>=0;i--)
            {
              ck <<= 4;
              dt = data.charAt(i);
              ck |= ((dt < 'A')?(dt - '0'):(dt - 'A' + 10));      
            }
            if(Serial && !NoDebug)
            {
              Serial.print(" Found Checksum: 0x");
              Serial.println(ck,HEX);
            }
            if( ck != cksum)
            {
              if(Serial && !NoDebug)
                Serial.println(" Error File is Corrupt at Checksum !");
              ret = 5;
              break;
            }
            
            // Everything is Ok Now
            ret = 0;
          
          // // ]
          }while(0); // End of Catch loop

          // After Reading Close the File
          f.close();
        
        // // ]
        }while(0); // End of Catch loop
      }
      break;
     
    case STORE_EEPROM:
      {
        uint16_t addr;
        // Start the EEPROM
        EEPROM.begin(512);
        addr = 0;
        // Enter the Catch loop
        do{
        // // [
        
          i = 0;
          data.reserve(200);
          // Read the SSID first from EEPROM
          while(addr < 33 && i<33) // Limit SSID Size
          {
            dt = EEPROM.read(addr++);
            // First Terminator Reached
            if(dt == '\r') 
            {
              break;
            }
            data += dt;
            ++i;
          }
          
          // Check for Termination in EEPROM
          if(i>=33 || i<2)
          {
            if(Serial && !NoDebug)
              Serial.println(" Error EEPROM is Corrupt did not find Station SSID (size)!");
            ret = 3;
            break;
          }
          
          // Calculate the Checksum & Load the SSID Buffer from EEPROM
          for(i=0; i<data.length(); i++)
          {
            cksum += data.charAt(i);
            n_ssid[i] = data.charAt(i);
          }
          n_ssid[i]=0; // Adding an Ending Null Termination
          if(Serial && !NoDebug)
          {
            Serial.print(" SSID Found: ");
            Serial.println(n_ssid);
          }
          // Clear the Data buffer
          data = "";
          
          i = 0;ck = addr;
          // Read the Password from EEPROM
          while(addr < (ck+65) && i<65) // Limit Password Size
          {
            dt = EEPROM.read(addr++);
            // First Terminator Reached
            if(dt == '\r') 
            {
              break;
            }
            data += dt;
            ++i;
          }
          
          // Check for Termination in EEPROM
          if(i>=65)
          {
            if(Serial && !NoDebug)
              Serial.println(" Error EEPROM is Corrupt did not find Station Password (size)!");
            ret = 3;
            break;
          }
          
          // Calculate the Checksum & Load the Password Buffer from EEPROM
          for(i=0; i<data.length(); i++)
          {
            cksum += data.charAt(i);
            n_password[i] = data.charAt(i);
          }
          n_password[i]=0; // Adding an Ending Null Termination
          if(Serial && !NoDebug)
          {
            Serial.print(" Password Found: ");
            Serial.println(n_password);
          }
          // Clear the Data buffer
          data = "";
          
          // Calculate EEPROM Checksum
          cksum = (0x10000 - (uint32_t)cksum)&0xFFFF;
          if(Serial && !NoDebug)
          {
            Serial.print(" Calculated Checksum: 0x");
            Serial.println(cksum,HEX);
          }
          
          i = 0;ck = addr;
          // Read the Checksum from EEPROM
          while(addr < (ck+4) && i<4) // Limit Checksum Size
          {
            dt = EEPROM.read(addr++);
            // First Terminator Reached
            if(dt == '\0') 
            {
              break;
            }
            data += dt;
            ++i;
          }
          
          // Check for Termination Checksum in EEPROM
          if(i>=5 || i<3 )
          {
            if(Serial && !NoDebug)
              Serial.println(" Error EEPROM is Corrupt did not find Checksum (size) !");
            ret = 5;
            break;
          }
          
          // Validate the Checksum
          data.toUpperCase();
          if(Serial && !NoDebug)
          {
            Serial.print(" Read Checksum: 0x");
            Serial.println(data);
          }
          ck = 0;
          for(i=3;i>=0;i--)
          {
            ck <<= 4;
            dt = data.charAt(i);
            ck |= ((dt < 'A')?(dt - '0'):(dt - 'A' + 10));      
          }
          if(Serial && !NoDebug)
          {
            Serial.print(" Found Checksum: 0x");
            Serial.println(ck,HEX);
          }
          if( ck != cksum)
          {
            if(Serial && !NoDebug)
              Serial.println(" Error EEPROM is Corrupt at Checksum !");
            ret = 5;
            break;
          } 

          // Everything Is OK now in EEPROM
          ret = 0;
          
        // // ]
        }while(0); // End of Catch loop
        
        // Stop the EEPROM Functions
        EEPROM.end();
      }
      break;
     
    default:
      ret = 1;
      break;
  }// End of Switch
  
  // Finally if Error is Brought out
  if(ret != 0)
    return false;
  
  if(Serial && !NoDebug)
    Serial.println(" SUCCESS! Found Station Information");
  // Transfer the Data from Local to Actual Memory
  this->_bST_infoValid = false;
  strcpy(this->_szST_SSID, n_ssid);
  this->_szST_PASSWORD[0] = 0; // Just in case if there is No Password
  strcpy(this->_szST_PASSWORD, n_password);
  this->_bST_infoValid = true;
  // If there was No Error in Fetching
  return true;
}
/// Internal Used to Try out another Connection attempt
bool Weg::_TryAP(void)
{
  // Can't Accept when loop is running
  if(this->_bIsRunning) return false;
  
  // Stop the Servers If they had Started
  webServer.stop();
  dnsServer.stop();
  
  if(!this->_bAP_infoValid) // If AP info is not correct - Fill Default
  {
    strcpy(this->_szAP_SSID, gpAP_ssid); // Default AP  
    strcat(this->_szAP_SSID, " - ");
    strcat(this->_szAP_SSID, WiFi.softAPmacAddress().c_str()); // Add the AP MAC ID
    this->_szAP_PASSWORD[0] = 0; // No Password
  }
  
  if(Serial && !NoDebug)
  {
    Serial.print("Creating AP Name: ");
    Serial.println(this->_szAP_SSID);
    Serial.print("With AP Password: ");
    Serial.println(this->_szAP_PASSWORD);
  }
  
  if(!AccessPoint(this->_szAP_SSID, this->_szAP_PASSWORD))
  { // if Connection Fails
    if(Serial && !NoDebug)
      Serial.println(" Error Could not start AP !");
    return false;
  }
  
  // SETUP of DNS Server
  // modify TTL associated  with the domain name (in seconds)
  // default is 60 seconds
  dnsServer.setTTL(300);
  // set which return code will be used for all other domains (e.g. sending
  // ServerFailure instead of NonExistentDomain will reduce number of queries
  // sent by clients)
  // default is DNSReplyCode::NonExistentDomain
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  // start DNS server for all Domains to same page
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  
  // SETUP of Web Server
  // Handle Main Server on Root
  webServer.on("/", h_Main);  
  // Handle Post Request on WiFi
  webServer.on("/wifi", h_Wifi);  
  // Redirector for Main Page
  webServer.onNotFound(h_NotFound);  
  // Start the Server
  webServer.begin();
  
  // Setup that We are in Loop
  this->_bIsRunning = true;
  n_Done = false; // External Flag Reset
  
  if(Serial && !NoDebug)
  {
    Serial.println("Ready! ... to Acquire WiFi Info");
    #ifdef DEBUG
    Serial.print(" Free HEAP: "); Serial.println( ESP.getFreeHeap() );
    #endif
  }
  
  // We have Started Up
  return true;
}
/// To Begin connection in case everything is Available Alright
bool Weg::startConnection(void)
{
  // Can't Accept when loop is running
  if(this->_bIsRunning) return false;
  
  // Stop the Servers If they had Started
  webServer.stop();
  dnsServer.stop();
  
  // Disconnect WiFi if Connected
  if(WiFi.status() == WL_CONNECTED)
  {
    WiFi.disconnect(false);
    delay(100);
  }
  
  if(!this->_bST_infoValid) // STation is not configured yet - Lets Try
    this->load();
    
  // If Station InFo is Validate
  if(this->_bST_infoValid)
  {
    // Attempt the Connection
    Connect(this->_szST_SSID, this->_szST_PASSWORD, this->nRetry);
    
    // If Connection is Success
    if(WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    
    if(this->nReconnect > 0) // We still have attempts to Reconnect then
      return true;
  }// End of Station Info Valid Check
 
  // Looks like the Connection failed or Info is not Available
  // We need to Start the AP
  return this->_TryAP();  
}

/// To Run the Internal State Machine for connectivity
bool Weg::status(void)
{
  static uint32_t reconnect = 0;
  /*
  // Setup the Hostname
  if(String(this->_szHostName) != WiFi.hostname())
  {
    if(Serial && !NoDebug)
    {
      Serial.println(" Changing Host Name!");
    }
    WiFi.hostname(String(this->_szHostName));
  }*/
  if(!this->_bIsRunning) // We are done with Running
  {
    // If WiFi connection is There
    if(WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    // Now we are Disconnected and Loop is also not running
    if(Serial && !NoDebug)
      Serial.println(" Error WiFi Disconnected!");
    
    // Automatic Retry Enable
    if(this->_bAutoWifiNeeded)
    {
      if(reconnect == 0) // First time Reconnection
      {
        // Assign the Reconnection Attempts
        reconnect = this->nReconnect;
      }
      
      // Looks like either Station Info not correct or not connected to the AP
      if(this->_bST_infoValid)
      {
        // Attempt Reconnection
        if(--reconnect)
        {
          Connect(this->_szST_SSID, this->_szST_PASSWORD, this->nRetry);
          return false; // We are Still trying
        }
        // Invalidate the Station Records
        //this->_bST_infoValid = false;
        //if(Serial && !NoDebug)
        //  Serial.println(" Info: Retrying Dynamic Connection");
        // Attempt the Restart the Dynamic Connection
        //this->_TryAP();
        
        //.... Instead just Reset
        ESP.reset();
        return false; // We are Still trying
      }
      
      // ST Info is Invalidated but new Info is also not Correct
      reconnect = 0;
      // Else If the Station Info is not Valid
      if(Serial && !NoDebug)
          Serial.println(" Error Launching Full connect as Station Info is not Valid!");
      // Attempt to Perform a Full scale Connect
      return this->startConnection();
    }
    //if(Serial && !NoDebug)
    //  Serial.println(" Error Can't do any thing WiFi not connected!");
    // Else We can't Do Any thing
    return false;    
  }// End of Non-Running Check
  else
  {// We Are RUNNING
    if(!n_Done) // Not Flagged
    {
      dnsServer.processNextRequest();
      webServer.handleClient();
    }
    else // We have Acquired the required Data
    {
      // We are done with running
      this->_bIsRunning = false; // Stop
      
      if(Serial && !NoDebug)
        Serial.println(" SUCCESS! We have Station Info for connection");
      // Disable WiFi
      WiFi.disconnect(false);
      
      // Stop the Servers If they had Started
      webServer.stop();
      dnsServer.stop();
      
      // InValidate the Station data and Load the New Data
      this->_bST_infoValid = false;
      strcpy(this->_szST_SSID, gn_ssid);
      this->_szST_PASSWORD[0] = 0;
      strcpy(this->_szST_PASSWORD, gn_password);
      this->_bST_infoValid = true;
      
      // Attempt the Store the Info If Needed
      if(this->_bStoreInfo)
        this->store();
      
      // Perform a WiFi connection this time again with Connect
      // Attempt the Connection
      return Connect(this->_szST_SSID, this->_szST_PASSWORD, this->nRetry);      
    }
  }// End of Running Check
  return false; // Normally False
}
///////////////////////////////////
void h_Main(void)
{
  // Main HTML Content
  String Content;
  // Prepare Main Content
  Content += HTTP_HEAD;
  Content += HTTP_APIP;
  Content += WiFi.softAPIP().toString();
  Content += HTTP_DOC_CLOSE;
  Content += HTTP_APMAC;
  Content += WiFi.softAPmacAddress();
  Content += HTTP_DOC_CLOSE;
  Content += HTTP_STAMAC;
  Content += WiFi.macAddress();
  Content += HTTP_DOC_INFO;
  Content += HTTP_FOOT1;

  // Send out the Page
  webServer.send(200, "text/html", Content);
}
void h_Wifi(void)
{  
  String srvpath;
  // Response HTML Content
  String RContent;
  
  // Get the Actual Server Path
  srvpath = String(F("http://")) + WiFi.softAPIP().toString();

  // Prepare Main Content
  RContent += HTTP_HEAD;
  RContent += HTTP_APIP;
  RContent += WiFi.softAPIP().toString();
  RContent += HTTP_DOC_CLOSE;
  RContent += HTTP_APMAC;
  RContent += WiFi.softAPmacAddress();
  RContent += HTTP_DOC_CLOSE;
  RContent += HTTP_STAMAC;
  RContent += WiFi.macAddress();
  RContent += HTTP_DOC_INFO;
  RContent += HTTP_FOOT2;
  
  // Only for POST with Correct Arguments
  if(webServer.hasArg("APName") && 
    webServer.hasArg("APPassword") &&
    webServer.method() == HTTP_POST)
  {
    // Get Argument Array
    webServer.arg("APName").toCharArray(gn_ssid, 32);
    gn_password[0] = 0;
    webServer.arg("APPassword").toCharArray(gn_password, 64);
    if(strlen(gn_ssid) != 0) // No Password is OK
    {
      if(Serial && !NoDebug)
      {
        Serial.print(" AP Name: "); Serial.println(gn_ssid);
        Serial.print(" AP Pass: "); Serial.println(gn_password);
      }
      webServer.send(200, "text/html", RContent);
      n_Done = true; // We have the Password and SSID
    }
  }
  // In all error Cases Redirect to Main Page
  webServer.sendHeader("Location", srvpath);
  webServer.send ( 302, "text/plain", "");
  webServer.client().stop();
}
void h_NotFound(void)
{
  String srvpath;  
  // Get the Actual Server Path
  srvpath = String("http://") + WiFi.softAPIP().toString();
  
  webServer.sendHeader("Location", srvpath);
  webServer.send ( 302, "text/plain", "");
  webServer.client().stop();
}

///////////////////////////////////////////////////////////////////////


#endif /* End of ESP8266 System Level Selection */
