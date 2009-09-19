/******************************************************************************

  Description:	Header file for QPID
  
  This file contains various configuration data for QPID including network settings,
  Twitter account information, and hardware pin assignments.

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


#ifndef QPID_H_
#define QPID_H_


// Twitter account info (user:password)
#define TWITTER_AUTH "foo:bar"

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {192,168,1,2};	// IP address of WiShield
unsigned char gateway_ip[] = {192,168,1,1};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = {"linksys"};		// max 32 bytes

unsigned char security_type = 0;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"12345678"};	// max 64 characters

// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = {};

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = 1;

unsigned char ssid_len;
unsigned char security_passphrase_len;
// End of wireless configuration parameters ----------------------------------------



/* Hardware pin assignments (these must match the QPID Shield) */
/* Output pins */
#define FAN_PIN 6  // PWM output for the fan 6
#define LED_POWER 5 // Power Indicator 5
#define LED_COLD 4 // Low Temperature Indicator 4
#define LED_NORMAL 3 // Normal Temperature Indicator 3
#define LED_HOT 1  // High Temperature Indicator 1
#define LED_RX 7 // WiFi Rx Indicator 7
#define LED_TX 8 // WiFi Tx Indicator 8
#define LED_WIFI 9 // WiFi Connection Indicator 9

/* Analog Inputs */ 
#define SMOKER_PIN 2 // Smoker probe
#define MEAT_PIN 3 // Meat probe
#define VOLTS_PIN 0 // Vin sensing (with 22k - 10k divider, factor = 10 / 32))


/* Miscellaneous configuration parameters */
#define TWEET_INTERVAL 1800 // Twitter status notification interval, in seconds


#endif /* QPID_H_ */
