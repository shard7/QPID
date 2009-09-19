/******************************************************************************
  table.c
 
  Description: Implements a simple thermistor look-up table for converting A/D converter values 
  to degrees farenheit.
 
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


#include <avr/pgmspace.h>


// Temperature corresponding to the last table entry
#define MIN_TEMP 50

// Temperature corresponding to the first table entry
#define MAX_TEMP 370

// Number of entries in the table
#define TABLE_SIZE 33

// Temperature span between two entries in the table
#define SPAN ((MAX_TEMP - MIN_TEMP) / (TABLE_SIZE - 1))


// Lookup table data for a Maverick Industries ET73 probe pulled up to 5 volts with a 22k resistor
const prog_uint16_t table[] PROGMEM = {968, 952, 936, 919, 898, 873, 845, 817, 786, 751, 713, 675, 638, 599, 560,
						516, 477, 436, 403, 371, 338, 310, 283, 257, 233, 212, 190, 172, 156, 141,
						128, 116, 105};


/**
 * Gets the value of the specified table entry
 * @param index Table index
 * @return table value for the given index
 */
int tableEntry(int index) {
	return pgm_read_word(table + index);
}


/**
 * Gets the temperature for the given raw A/D value using the provided table
 * @param rawValue the raw A/D converter value (0 - 1023)
 @ @param tableData pointer to the lookup table data
 * @return the corresponding temperature in tenths of degrees F (or -1 or 9999 if out of range)
 */
int convert(int rawValue) {

  // Check for a value that is outside the range of the lookup table and
  // return the special value (also makes sure the subsequent while loop
  // doesn't go off into the weeds)
  if (rawValue >= tableEntry(0)) {
    return -1;
  }
  if (rawValue <= tableEntry(TABLE_SIZE - 1)) {
    return 9999;
  }

  // Locate pair of table entries that the raw value lies between
  int i = 0;
  while (rawValue < tableEntry(i + 1)) {
    i++;
  }

  // Interpolate between the values in the lookup table
  // Fractional portion of the value between the two table entries (percent)
  int fraction = (100 * (tableEntry(i) - rawValue)) / (tableEntry(i) - tableEntry(i+1));

  // Compute and return the temperature value
  return (MIN_TEMP * 10) + (SPAN * ((i * 100) + fraction) / 10);

}

