/******************************************************************************
  TempProbe.cpp
 
  Description: The TempProbe and SmokerProbe classes simplify the process of measuring temperatures using a thermistor probe.
  In addition to handling the sampling and conversion to temperature units, the class can also
  monitor trends over time.
 
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


#include "TempProbe.h"
#include "WProgram.h"

//#include <avr/pgmspace.h>

extern "C" {
	int convert(int rawValue);
}


// Minimum analog pin value at which the fan will run
#define FAN_OFFSET 40
#define FAN_RANGE (255 - FAN_OFFSET)

// Temp error limit for switching on the hot and cold LEDs
#define TEMP_LED_LIMIT_1 25

// Temp error limit for switching off the normal LED
#define TEMP_LED_LIMIT_2 50

// Control algorithm interval, in milliseconds
#define PROBE_INTERVAL 5000

// Number of A/D readings to average
#define SAMPLE_COUNT 50

int TempProbe::getTemp() {
	return this->value;
}

char TempProbe::probe_task() {

	// Get the samples
	long sum = 0;
	for (int i = SAMPLE_COUNT; i > 0; i--) {
		sum += analogRead(pin);
	}	
	this->value = convert(sum / SAMPLE_COUNT);
	return 0;
}

TempProbe::TempProbe(char pin) {

	// Default the pin number to undefined
	this->pin = pin;

	// Default the value to -1
	this->value = -1;
}

/*
 * Sets the fan speed at the specified percentage
 * @param pct Percent of full speed (0 - 100)
 */
void setFanSpeed(int pct) {
	// Clamp pct at 0 - 100
	
}

#define LTA 0.99f
#define STA 0.90f

#define P_FACTOR 10
#define D_FACTOR 10

// Minimum cooling rate that signifies opening the smoker
// in tenths of a degree per sample period
#define OPEN_COOL_RATE 40

void updateAverage(float* avg, int value, float factor) {
	if (*avg == -1) {
		*avg = value; 
	} else {
		*avg = (*avg * factor) + ((1.0f - factor) * value);
	}
}

uint16_t probeMeasureCount = 0;

smokerState SmokerProbe::probe_task2() {
	
	smokerState ret = NORMAL;
	
	// Call superclass task to read the temperature
	this->TempProbe::probe_task();

	
	// Cycle LEDs in warm up mode
	if (this->warmUpMode) {
		int cycle = (millis() / 500) % 4;
		digitalWrite(this->coldIndicatorPin, cycle > 0 ? HIGH : LOW);
		digitalWrite(this->normalIndicatorPin, cycle > 1 ? HIGH : LOW);
		digitalWrite(this->hotIndicatorPin, cycle > 2 ? HIGH : LOW);
	}
	
	// Check if we're due for some number crunching
	if (millis() / PROBE_INTERVAL > probeMeasureCount) {
        probeMeasureCount++;    

		// Update the running averages
		updateAverage(&(this->longTermAvg), this->value, LTA);
		updateAverage(&(this->shortTermAvg), this->value, STA);
		
		// Warm-up mode
		if (this->warmUpMode) {
			// Run the fan at 100% to expedite heating
			this->fanPct = 100;	
			ret = HEATING;
			// Check if we're done with warm up mode (within 10 degrees of the target)
			if ((this->targetTemp * 10) - this->value < 10) {
				this->warmUpMode = false;
				ret = NORMAL;
			}
		}
		
		if (!this->warmUpMode) {
			// Compute the proportional error
			int P = this->value - (this->targetTemp * 10);
			// Compute the derivative error
			int D = this->value - this->shortTermAvg;
			
			// Check if smoker was opened (rapid cooling rate)
			if (D < -OPEN_COOL_RATE) {
				this->warmUpMode = true;
			}
			
			// Compute the appropriate fan speed
			this->fanPct = -((P * P_FACTOR) + (D * D_FACTOR));			  
			this->fanPct = min(100, max(0, this->fanPct));

			updateAverage(&(this->fanAvg), this->fanPct, LTA);
			
			// Check if the fan average exceeded 80%
			if (this->fanAvg > 80) {
				// Reset the average
				this->fanAvg = 0;
				// Indicate low fire state
				ret = LOW_FIRE;
			}
			
			// Set indicators to reflect current P error
			digitalWrite(this->normalIndicatorPin, abs(P) < TEMP_LED_LIMIT_2 ? HIGH : LOW);
			digitalWrite(this->hotIndicatorPin, P > TEMP_LED_LIMIT_1 ? HIGH : LOW);
			digitalWrite(this->coldIndicatorPin, P < -TEMP_LED_LIMIT_1 ? HIGH : LOW);
			
		}
		// Set the fan output
		analogWrite(this->fan_pin, this->fanPct == 0 ? 0 : FAN_OFFSET + ((FAN_RANGE * this->fanPct) / 100));   
	}
	return ret;
}

SmokerProbe::SmokerProbe(char pin, char fan_pin, char coldPin, char normalPin, char hotPin) : TempProbe (pin) {
	
	this->fan_pin = fan_pin;
	this->longTermAvg = -1;
	this->shortTermAvg = -1;
	this->fanAvg = 0;
	this->warmUpMode = true;
	this->coldIndicatorPin = coldPin;
	this->normalIndicatorPin = normalPin;
	this->hotIndicatorPin = hotPin;
	
}
