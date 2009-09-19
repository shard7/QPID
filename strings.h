/******************************************************************************

  Description:	Common strings used by the main QPID sketch to generate web pages

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



// Common strings used in the web page (stored in flash to reduce RAM usage)

// HTML tags
const prog_char startHtml[] PROGMEM = {"<html>"};
const prog_char endHtml[] PROGMEM = {"</html>"};
const prog_char startHead[] PROGMEM = {"<head>"};
const prog_char endHead[] PROGMEM = {"</head>"};
const prog_char startTitle[] PROGMEM = {"<title>"};
const prog_char endTitle[] PROGMEM = {"</title>"};
const prog_char br[] PROGMEM = {"<br>"};

// Meta tag to refresh the page every 60 seconds
const prog_char refresh[] PROGMEM = {"<meta http-equiv=\"refresh\"content=\"60\">"};

// QPID-specific strings
const prog_char degF[] PROGMEM = {" deg F"};
const prog_char target[] PROGMEM = {"Target: "};

const prog_char warmUp[] PROGMEM = {"Warm Up"};
const prog_char overheat[] PROGMEM = {"Overheat"};
const prog_char normal[] PROGMEM = {"Normal PID"};
const prog_char* modeText[] = {warmUp, overheat, normal};

const prog_char formStart[] PROGMEM = {"<form action=\"/set\">Target: "};
const prog_char fieldA[] PROGMEM = {"<input TYPE=\"text\" name=\"temp\" value=\""};
// This is where the current target temp is printed 
const prog_char fieldB[] PROGMEM = {"\" size=\"3\"> deg F "};
const prog_char button[] PROGMEM = {"<input TYPE=\"SUBMIT\" VALUE=\"SET\">"};
const prog_char formEnd[] PROGMEM = {"</form>"};


// Confirmation page head tag with 3 second redirect to main page
const prog_char notification[] PROGMEM = {"<head><title>QPID</title><meta HTTP-EQUIV=\"REFRESH\" content=\"3; url=/\"></head>"};

// Temp change confirmation
const prog_char setting_temp[] PROGMEM = {"Temperature set to "};
// Access denial message
const prog_char not_authorized[] PROGMEM = {"Access Not Authorized"};
