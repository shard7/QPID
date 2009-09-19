/******************************************************************************

  Description:	Main Arduino sketch for QPID

 ******************************************************************************

  Copyright(c) 2009 Mark A. Patel  All rights reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 59
  Temple Place - Suite 330, Boston, MA  02111-1307, USA.


 *****************************************************************************/

// Libraries
#include <WiServer.h>
#include <EEPROM.h>

// QPID-specific includes
#include "QPID.h"
#include "favicon.h"
#include "strings.h"
#include "TempProbe.h"

// Twitter auth string (Base64 encoded)
char* twitterAuth = WiServer.base64encode(TWITTER_AUTH);

// Twitter notification counter
int tweetCount = 0;

// Smoker probe to maintain the smoker temperature
SmokerProbe smoker(SMOKER_PIN, FAN_PIN, LED_COLD, LED_NORMAL, LED_HOT);

// Meat probe to monitor the meat temperature
TempProbe meat(MEAT_PIN);

// Indicates if QPID is powered via the USB cable (configuration mode)
boolean configMode = false;

typedef struct configData {
   U16 targetTemp; 
};

configData config;

void readConfig() {

  int i = sizeof(configData) - 1;
  U8* p = i + (U8*)&config;
  while (i >=0) {
      *(p--) = EEPROM.read(i--);
  } 
}

void writeConfig() {

  int i = sizeof(configData) - 1;
  U8* p = i + (U8*)&config;
  while (i >=0) {
      EEPROM.write(i--, *(p--)); 
  } 
}

/*
 * This function is called by the WebServer whenever a resource needs to be sent
 * @param URL null-terminated URL of the resource to send
 */
boolean sendPage(char* URL) { 
  
    // Check if the request is to set the temperature 
    if (strncmp(URL, "/set?temp=", 10) == 0) {
      
      // Start the confirmation page
      WiServer.print_P(startHtml);
      WiServer.print_P(notification);
      
      // Check for local access
      if (WiServer.clientIsLocal()) {
        // Parse the new temperature
        config.targetTemp = atoi(URL + 10);
        smoker.targetTemp = config.targetTemp;
        smoker.warmUpMode = true;
        writeConfig();
        // Send the confirmation page with the new temperature 
        WiServer.print_P(setting_temp);
        WiServer.print(config.targetTemp);
        WiServer.print_P(degF);
      } else {
        // Access Denied
        WiServer.print_P(not_authorized);
      }
      WiServer.print_P(endHtml);
      return true;
    }

    // Check if request is for the favicon
    if (strcmp(URL, "/favicon.ico") == 0) {
      // Write the bytes of the favicon in flash
      WiServer.write_P((char*)favicon, favicon_size);
      return true;
    }
    
    // Get raw data snapshot for logging purposes
    // Comma delimited row of data containing:
    // time, target temo, current temp, STA, LTA, fan speed, fan speed LTA, meat temp
    if (strcmp(URL, "/data") == 0) {
      WiServer.print(millis() / 1000);
      WiServer.print(',');
      WiServer.print(config.targetTemp);
      WiServer.print(',');
      WiServer.print(smoker.getTemp() / 10);
      WiServer.print(',');
      WiServer.print(smoker.shortTermAvg / 10);
      WiServer.print(',');
      WiServer.print(smoker.longTermAvg / 10);
      WiServer.print(',');
      WiServer.print(smoker.fanPct);
      WiServer.print(',');
      WiServer.print(smoker.fanAvg);
      WiServer.print(',');
      WiServer.print(meat.getTemp() / 10);
      
       WiServer.println();
      return true;
    }
    
    
    // Main page
    if (strcmp(URL, "/") == 0) {

        WiServer.print_P(startHtml);
        WiServer.print_P(startHead);
        WiServer.print_P(startTitle);
        WiServer.print("QPID ");
        WiServer.print_P(endTitle);
        //WiServer.print_P(refresh);
        WiServer.print_P(endHead);
        
        WiServer.print("Meat Temp: ");
        WiServer.print(meat.getTemp() / 10);
        WiServer.print_P(degF);
        WiServer.print_P(br);
        
        WiServer.print("Smoker Temp: ");
        WiServer.print(smoker.getTemp() / 10);
        WiServer.print_P(degF);
        WiServer.print_P(br);
        
        if (WiServer.clientIsLocal()) {
          // Show form for local clients only
          WiServer.print_P(formStart);
          WiServer.print_P(fieldA);
          WiServer.print(config.targetTemp);
          WiServer.print_P(fieldB);
          WiServer.print_P(button);
          WiServer.print_P(formEnd);
        } else {
          // Rest of the world just sees the data
          WiServer.print_P(target);
          WiServer.print(config.targetTemp);
          WiServer.print_P(degF);
          WiServer.print_P(br);
        }
        
        // Display the fan speed
        WiServer.print("Fan Speed : ");
        WiServer.print(smoker.fanPct);
        WiServer.print('%');
        WiServer.print_P(br);
        
        WiServer.print("Fan Utilization : ");
        WiServer.print((int)smoker.fanAvg);
        WiServer.print('%');
        WiServer.print_P(br);
        

        float v = (float)analogRead(VOLTS_PIN) * (32 * 5) / (10 * 1024);
        WiServer.print("Supply Voltage : ");
        WiServer.print(v);
        
        WiServer.print("v <br>Updated at ");
        WiServer.printTime(millis());
        WiServer.print_P(br);
          
        
        WiServer.print_P(endHtml);
        return true;
    }
    
    // URL not recognized
    return false;
}


void rebootMsg() {
  WiServer.println("Subject:Boot Msg");
  WiServer.println();
  WiServer.print("QPID is now running");
}

TWEETrequest rebootTweet(twitterAuth, rebootMsg);

void lowFireAlertMsg() {
  WiServer.print("WARNING: Low fire level detected at ");
  WiServer.printTime(millis());
  WiServer.print(", please check fuel level.  Current temperature is ");
  WiServer.print(smoker.getTemp() / 10);
  WiServer.print_P(degF);
}

TWEETrequest lowFireTweet(twitterAuth, lowFireAlertMsg);
//uint8 tmo[] = {76,96,30,117};

//uint8* ipAddr, char* hostName, char* to, char* from, bodyFunction message)
//SMTPrequest bootMail(tmo, "comcast.net", "8476686952@tmomail.net", "shard7@yahoo.com", rebootMsg);


void setup() {
  
  // Power LED on
  pinMode(LED_POWER, OUTPUT);  
  digitalWrite(LED_POWER, HIGH);
  
  // Check for config mode (powered by USB)
  //configMode = analogRead(VOLTS_PIN) < (1024 * 22 / 32);
  
  if (configMode) {
    Serial.begin(57600);
    Serial.println("Config mode");
    
    
    
  } else {
    // Set the output pin modes
    pinMode(FAN_PIN, OUTPUT);
    pinMode(LED_COLD, OUTPUT);
    pinMode(LED_NORMAL, OUTPUT);
    pinMode(LED_HOT, OUTPUT);
    pinMode(LED_TX, OUTPUT);
    pinMode(LED_RX, OUTPUT);
    pinMode(LED_WIFI, OUTPUT);

    // Power-On test (toggle the LEDs and fan)
    int n = 2;
    while (--n >= 0) {
      char val = n & 1 ? HIGH : LOW;
    
      digitalWrite(LED_POWER, val);
      digitalWrite(LED_COLD, val);
      digitalWrite(LED_NORMAL, val);
      digitalWrite(LED_HOT, val);
      digitalWrite(LED_RX, val);
      digitalWrite(LED_TX, val);
      digitalWrite(LED_WIFI, val);
      digitalWrite(FAN_PIN, val);
      delay(1000);
    }
  
    // Power LED on
    digitalWrite(LED_POWER, HIGH);
  
    // Init the WiShield and server with our sendPage function
    WiServer.init(sendPage);

    // Set the LED indicator pins
    WiServer.setIndicatorPins(LED_TX, LED_RX);

    // Read config data  
    readConfig();
    smoker.targetTemp = config.targetTemp;
    
  }
}
  



// Main loop
void loop(){
    
    if (configMode) {
       
      
      
      
      
    } else {
      // Normal operation
      
      // Run the meat probe
      meat.probe_task();
      
      // Run the smoker probe and control algorithm
      if (smoker.probe_task2() == LOW_FIRE) {
        lowFireTweet.submit();
      }
      
      // Run WiServer
      WiServer.server_task(); 
   }
}


